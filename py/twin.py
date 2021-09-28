#Twin clash to hold twin state of IOT object
#Jon Durrant
#17-Sep-2021

from twinState import TwinState
import json
import twinProtocol
from topic import Topic
from twinObserver import TwinObserver

#Twin handler superclass to matching state to twin
class Twin:
    
    #Constructor
    def __init__(self):
        self.reported : TwinState =TwinState()
        self.desired : TwinState =TwinState()
        self.declined : TwinState = TwinState()
        self.topics : dict = {}
        self.observers = []
        self.twinTouched : int = self.reported.timestamp()
        
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
        self.desired.deleteState()
        
    #delete the desired state
    def deleteState(self):
        self.deleteDesiredState()
        
    #true of the twin is upto date, false if we have a delta    
    def isUptoDate(self) -> bool:
        return (self.getDesiredMeta()["timestamp"] <= self.getReportedMeta()["timestamp"])
    
    #return a delta transaction for the target
    def getDelta(self) -> dict:
        return self.getDeltaHelper(self.getDesiredMeta(), self.getReportedMeta(), self.getDesiredState())
        
    #Local helper function to recursively work through for delta    
    def getDeltaHelper(self, desiredMeta : dict, reportedMeta : dict, desiredState: dict) -> dict:
        delta = {}
        for key in desiredMeta:
            if not key in reportedMeta.keys():
                delta[key] = desiredState[key]
            elif isinstance(desiredMeta[key], dict):
                if key in desiredState.keys():
                    d = self.getDeltaHelper(desiredMeta[key], reportedMeta[key], desiredState[key])
                    if (d != {}):
                        delta[key] = d
            elif (desiredMeta[key] >= reportedMeta[key]):
                if (key != "timestamp"):
                    delta[key] = desiredState[key]
        return delta
    
    #Local function called to update the reported state based on data from thing
    def updateFromThing(self, delta : dict):
        self.reported.updateState(delta)
        if (("trn" in delta.keys()) and ("trn" in self.desired.getState().keys())):
            if (delta["trn"] == self.desired.getState()["trn"]):
                declined = self.getDelta()
                self.declined.updateState(declined)
                self.desired.setState({})
        self.reported.touch()
        self.notify()
        
    #local function to reset the state to value from twin
    def stateFromThing(self, state: dict):
        self.reported.setState(state)
        if (("trn" in state.keys()) and ("trn" in self.desired.getState().keys())):
            if (state["trn"] == self.desired.getState()["trn"]):
                declined = self.getDelta()
                self.declined.updateState(declined)
                self.desired.setState({})
        self.reported.touch()
        
       
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
        elif twinProtocol.TWINSTATE in js.keys():
            self.stateFromThing(js["state"])
        elif twinProtocol.TWINTOPIC in js.keys():
            self.handleMsg(js)
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
        return (self.reported.timestamp() - self.twinTouched)   
        
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
            