#Twin clash to hold twin state of IOT object
#Jon Durrant
#17-Sep-2021

from twinState import TwinState
import json
import twinProtocol
from topic import Topic
from twinObserver import TwinObserver
from dotmap import DotMap

import threading
import time
import logging



#Twin handler superclass to matching state to twin
class Twin(threading.Thread):

    #Constructor
    def __init__(self, options: dict = {}):
        threading.Thread.__init__(self)
        self.logging = logging.getLogger(__name__)
        self.lock = threading.Lock()
        self.reported : TwinState =TwinState()
        self.reported.setState({"trn": 0})
        self.desired : TwinState =TwinState()
        self.desired.setState({"trn": 0})
        self.declined : TwinState = TwinState()
        self.topics : dict = {}
        self.observers = []
        self.options = DotMap()
        self.twinTouched : int = self.reported.timestamp()
        self.connected = False
        self.setOptions(options)
        self.quietTimeout = DotMap({
            'timer': None,
            'alarm': False
            })
        
    #Set options
    def setOptions(self, options: dict):
        self.options.quietTimeoutMs = options.get('quietTimeoutMs', -1)
        if (self.options.quietTimeoutMs > 0):
            if (self.quietTimeout.timer):
                self.quietTimeout.timer.cancel()
            self.quietCheck()
        return
    
    #Internal function to check for quite client and raise alarm
    def quietCheck(self):
        if (self.timeSinceConversation() > self.options.quietTimeoutMs):
            if (not self.quietTimeout.alarm):
                self.quietTimeout.alarm = True
                for obs in self.observers:
                    obs.twinQuiet(self)
        else:
            if (self.quietTimeout.alarm):
                self.quietTimeout.alarm = False
                for obs in self.observers:
                    obs.twinChatting(self)
        if (self.options.quietTimeoutMs > 0):
            #Check 10 times in the timeout period
            t = self.options.quietTimeoutMs/1000.0/10.0
            self.quietTimeout.timer = threading.Timer(t, self.quietCheck)
            self.quietTimeout.timer.start()
        
    # Terturns true if quiet alarm has activiated
    def getQuietAlarm(self) -> bool:
        return self.quietTimeout.alarm
    
    #get reported state of the twin
    def getReportedState(self) -> dict:
        return self.reported.getState();
    
    #get current desired state, if no changes then will be empty
    def getDesiredState(self) -> dict:
        return self.desired.getState();
    
    #get declined state, so any state members that the twin has not updated 
    #on a transaction change
    def getDeclinedState(self) -> dict:
        return self.declined.getState();
    
    #Metadata giving the change time of each state item
    def getReportedMeta(self) -> dict:
        return self.reported.getMeta();
    
    #Metadata giving the change time of each state item
    def getDesiredMeta(self) -> dict:
        return self.desired.getMeta(); 
    
    #Metadata giving the change time of each state item
    def getDeclinedMeta(self) -> dict:
        return self.declined.getMeta(); 
    
    #Set the desired state of the object, will trigger a transaction to the twin
    def setDesiredState(self, state : dict):
        with self.lock:
            self.desired.setState(state)
            self.desired.updateState({
                twinProtocol.TWINTRN:  self.nextTrn()
            })
        delta = { 
            twinProtocol.TWINSTATE : self.getDelta()
        }
        self.outputJson(delta)
    
    #alias for setDesiredState
    def setState(self, state : dict):
        self.setDesiredState(state)
        
    #alias for getReportedState    
    def getState(self) -> dict:
        return self.getReportedState()
    
    #Update desired state, without overwriting any other requests
    #Will trigger a transaction to the Twin
    def updateDesiredState(self, state : dict):
        with self.lock:
            self.desired.updateState(state)
            self.desired.updateState({
                twinProtocol.TWINTRN:  self.nextTrn()
            })
        delta = { 
            twinProtocol.TWINDELTA: self.getDelta()
        }
        #print("->%s"%json.dumps(delta))
        self.outputJson(delta)
        
    #alias for updateDesiredState    
    def updateState(self, state : dict):
        self.updateDesiredState(state)
        
        
    #delete the desired state
    def deleteDesiredState(self):
        with self.lock:
            self.desired.deleteState()
        
    #delete the desired state
    def deleteState(self):
        self.deleteDesiredState()
        
    #true of the twin is upto date, false if we have a delta    
    def isUptoDate(self) -> bool:
        #return (self.getDesiredMeta()["timestamp"] <= self.getReportedMeta()["timestamp"])
        return (self.getReportedState().get(twinProtocol.TWINTRN, -1) == self.getDesiredState().get(twinProtocol.TWINTRN, -1))
    
    #return a delta transaction for the target
    def getDelta(self) -> dict:
        return self.getDeltaHelper(self.getDesiredMeta(), self.getReportedMeta(), self.getDesiredState(), self.getReportedState())
        
    #Local helper function to recursively work through for delta    
    def getDeltaHelper(self, desiredMeta : dict, reportedMeta : dict, desiredState: dict, reportedState : dict) -> dict:
        delta = {}
        for key in desiredMeta:
            if not key == 'timestamp':
                if not key in reportedMeta.keys():
                    delta[key] = desiredState[key]
                elif not key in reportedState.keys():
                    delta[key] = desiredState[key]
                elif isinstance(desiredMeta[key], dict):
                    if key in desiredState.keys():
                        d = self.getDeltaHelper(desiredMeta[key], reportedMeta[key], desiredState[key], reportedState[key])
                        if (d != {}):
                            delta[key] = d
                        
                elif (key in desiredState.keys()):
                    if (reportedState[key] != desiredState[key]):
                        delta[key] = desiredState[key]
        return delta
    
    #Local function called to update the reported state based on data from thing
    def updateFromThing(self, delta : dict):
        with self.lock:
            self.reported.updateState(delta)
            
            if (not twinProtocol.TWINTRN in delta.keys()):
                d = {}
                if (len(self.getDelta().keys()) <= 1):
                    d[twinProtocol.TWINTRN] = self.desired.getState()[twinProtocol.TWINTRN]
                else:
                    d[twinProtocol.TWINTRN] = self.desired.getState()[twinProtocol.TWINTRN] - 1      
                self.reported.updateState(d)
                
            if ((twinProtocol.TWINTRN in delta.keys()) and (twinProtocol.TWINTRN in self.desired.getState().keys())):
                if (delta[twinProtocol.TWINTRN] == self.desired.getState()[twinProtocol.TWINTRN]):
                    declined = self.getDelta()
                    self.declined.updateState(declined)
                    d = {}
                    d[twinProtocol.TWINTRN] = self.getReportedState()[twinProtocol.TWINTRN]
                    self.desired.setState(d)
            self.reported.touch()
        self.notify()
        
    #local function to reset the state to value from twin
    def stateFromThing(self, state: dict):
        with self.lock:
            self.reported.setState(state)
            
            if (not twinProtocol.TWINTRN in state.keys()):
                d = {}
                if (len(self.getDelta().keys()) <= 1):
                    d[twinProtocol.TWINTRN] = self.desired.getState()[twinProtocol.TWINTRN]
                else:
                    d[twinProtocol.TWINTRN] = self.desired.getState()[twinProtocol.TWINTRN] - 1      
                self.reported.updateState(d)
                
            if ((twinProtocol.TWINTRN in state.keys()) and (twinProtocol.TWINTRN in self.desired.getState().keys())):
                if (state[twinProtocol.TWINTRN] >= self.desired.getState()[twinProtocol.TWINTRN]):
                    declined = self.getDelta()
                    self.declined.updateState(declined)
                    self.desired.setState({})
            self.reported.touch()
        self.notify()
        
       
    #Publish a message to the twin
    #@topic - topic name
    #@data - any serialisable object
    def publishMsg(self, topic: str, data: object): 
        js={}
        js[twinProtocol.TWINTOPIC] = topic
        js[twinProtocol.TWINDATA] = data
        s = json.dumps(js)
        self.outputJson(s)
    
    #handle msg from twin {topic="xxx", data={}}
    #@jsonMsg - dict of the message structure
    def handleMsg(self, jsonMsg : dict):
        topic = jsonMsg[twinProtocol.TWINTOPIC]
        data = ""
        if (twinProtocol.TWINDATA in jsonMsg):
            data = jsonMsg[twinProtocol.TWINDATA]
        if (topic in self.topics.keys()):
            self.topics[topic].handle(topic, data, self)
    
    
    #===========================================================================
    # add a topic handler to the function
    #===========================================================================
    def addTopic(self, topicName: str, topicHandler : Topic):
        self.topics[topicName] = topicHandler

    #===========================================================================
    # output the json or string to the comms channel for the twin
    #===========================================================================
    def outputJson(self, s):
        raise Exception("Virtual Function Twin.outputJson")
    
    #===========================================================================
    # Read a Line and handle the content
    #===========================================================================
    def readLine(self):
        raise Exception("Virtual Function Twin.outputJson")
    
    #===========================================================================
    # process the json received from the twin
    # @s string of the json
    #===========================================================================
    def processJson(self, s: str):
        js = {}
        
        if (isinstance(s, str)):
            try:
                js = json.loads(s)
            except json.JSONDecodeError as ex:
                print(ex)
                print("ERROR Not JSON: %s"%s)
        if (isinstance(s, dict)):
            js = s
                        
        if twinProtocol.TWINDELTA in js.keys():
            self.updateFromThing(js["delta"])
            self.touch()
        elif twinProtocol.TWINSTATE in js.keys():
            self.stateFromThing(js["state"])
            self.touch()
        elif twinProtocol.TWINTOPIC in js.keys():
            self.handleMsg(js)
            self.touch()
        else:
            print("Unknown data %s"%s)

            
   #====================================================================
   # update a timestamp of the last update with the Twin
   #====================================================================
    def touch(self):
        self.twinTouched = self.reported.timestamp()
            
        
    #===========================================================================
    # How long since the last conversation with the twin
    # return time in miliseconds
    #===========================================================================
    def timeSinceConversation(self) -> int:
        return (self.reported.timestamp() - self.twinTouched)/1000000   
        
    #=======================================================================
    # attach a change listener
    #=======================================================================
    def attach(self, observer : TwinObserver) :
        self.observers.append(observer)

    #===========================================================================
    # detach a twin observer
    #===========================================================================
    def detach(self, observer: TwinObserver):
        self.observers.remove(observer)
        
    #===========================================================================
    # Notify all of the twin observers of a change of state
    #===========================================================================
    def notify(self):
        for obs in self.observers:
            obs.notify(self)
            
    #===========================================================================
    # returns the next transaction code to sync between the devices
    #===========================================================================
    def nextTrn(self) -> int:
        dtrn = 0
        rtrn = 0
        
        if (twinProtocol.TWINTRN in self.getDesiredState()):
            dtrn = self.getDesiredState()[twinProtocol.TWINTRN]
        if (twinProtocol.TWINTRN in self.getReportedState()):
            rtrn = self.getReportedState()[twinProtocol.TWINTRN]
        trn = max(dtrn, rtrn) + 1
        if (trn > 0xFFFFFFFFFFFFFFFF):
            trn = 1
        return trn
    
    #===========================================================================
    # Launch as threaded process
    #===========================================================================
    def run(self):
        while True:
            self.readLine()
            time.sleep(0.005)
            
    #===========================================================================
    # Connect to twin - use with child to manage connections
    #===========================================================================
    def connect(self):
        self.connected = True
        for obs in self.observers:
            obs.twinOnline(self)
        
    #===========================================================================
    # Disconenct from twin - use on child to manage connections
    #===========================================================================
    def disconnect(self):
        self.connected = False
        for obs in self.observers:
            obs.twinOffline(self)
        
    #===========================================================================
    # Check if twin is connected to it's physical counterpart
    #===========================================================================
    def isConnected(self):
        return self.connected
    
    
    #===========================================================================
    # Request full state be sent from thing
    #===========================================================================
    def requestGet(self):
        self.publishMsg(twinProtocol.TOPICGET, "")
        
    #=======================================================================
    # Send delta to thing
    #=======================================================================
    def sendDelta(self):
        delta = { 
            twinProtocol.TWINDELTA : self.getDelta()
        }
        self.outputJson(delta)
        
        
        
        
        
            