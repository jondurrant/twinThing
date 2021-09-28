#TwinState clash to hold twin state of IOT object
#Jon Durrant
#17-Sep-2021

import copy
import time

#===============================================================================
# Twin State to represent either the desired or reported state of the twin
#===============================================================================
class TwinState:
    
    #===========================================================================
    # Constructor
    #===========================================================================
    def __init__(self):
        self.deleteState()
        
    #===========================================================================
    # set the state
    #@state - Dictionary of the state
    #===========================================================================
    def setState(self, state: dict):
        if isinstance(state, dict):
            self.state = copy.deepcopy(state)
            self.buildMeta(self.state, self.meta)
            self.meta["timestamp"] = self.timestamp()
        else:
            raise Exception("state is not a dictionary")
    
    #===========================================================================
    # local function to build meta data  for state, recursive
    #@state: Dictionary
    #@meta: Dictionary
    #===========================================================================
    def buildMeta(self, state: dict, meta : dict):
        ts = self.timestamp()
        for key in state:
            if isinstance(state[key], dict):
                meta[key] = {}
                self.buildMeta(state[key], meta[key])
            else:
                meta[key] = ts
                        
    #===========================================================================
    # return a dictionaryof the state
    #===========================================================================
    def getState(self) -> dict:
        return self.state
    
    #===========================================================================
    # return meta data giving timestamp on each member item
    #===========================================================================
    def getMeta(self) -> dict:
        return self.meta
        
    #=======================================================================
    # update state with the given delta
    #=======================================================================
    def updateState(self, state: dict):
        self.updateStateHelper(state, self.state, self.meta)
        self.meta["timestamp"] = self.timestamp()
            
    #===========================================================================
    # local helper recursive function to update stae and metadata
    #===========================================================================
    def updateStateHelper(self, srcState: dict, destState: dict, meta: dict):
        ts = self.timestamp()
        for key in srcState:
            if isinstance(srcState[key], dict):
                if not key in destState.keys():
                    destState[key] = {}
                    meta[key] = {}
                self.updateStateHelper(srcState[key], destState[key], meta[key])
            else:
                destState[key] = srcState[key]
                meta[key] = ts
        
    #===========================================================================
    # delete the state
    #===========================================================================
    def deleteState(self):
        self.state = {}
        self.meta = {
            "timestamp": self.timestamp()
        }
        
    #===========================================================================
    # Update the master timestamp on the state
    #===========================================================================
    def touch(self):
        self.meta["timestamp"] = self.timestamp()
        
    #===========================================================================
    # local function to generate tiestamps
    #===========================================================================
    def timestamp(self):
        return time.time_ns()
        