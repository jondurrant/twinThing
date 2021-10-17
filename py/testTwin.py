import unittest

from twinFake import TwinFake
import json
import copy

jon = {
"name": "Jon",
"age": 13
}

ageUpdate = {
    "age": 50
}

compound = {
    "name": "Jon",
    "age": 13,
    "kin": {
        "name": "Kevin",
        "age": 28
    }
}

compoundUpdate = {
    "age": 50,
    "kin": {
        "age": 60
    }
}

addition = {
    "uk": True
}

compoundAddition = {
    "address": {
        "street": "The Drive",
        "city": "Brighton"
    }
}


class MyTest(unittest.TestCase):
        
        
    def testSetState(self):
        twin = TwinFake()
        twin.setState(jon)
        self.assertEqual(twin.getDesiredState()["name"], jon["name"]) 
        self.assertEqual(twin.getDesiredState()["age"], jon["age"])       
        self.assertEqual(twin.getDesiredMeta()["age"], twin.getDesiredMeta()["name"])
        
    def testUpdateState(self):
        twin = TwinFake()
        twin.setState(jon)
        self.assertEqual(twin.getDesiredState()["name"], jon["name"]) 
        self.assertEqual(twin.getDesiredState()["age"], jon["age"])
        twin.updateState(ageUpdate)
        self.assertEqual(twin.getDesiredState()["name"], jon["name"]) 
        self.assertEqual(twin.getDesiredState()["age"], 50)
        self.assertGreater(twin.getDesiredMeta()["age"], twin.getDesiredMeta()["name"])
        
        
    def testDeltaForTwin(self):
        twin = TwinFake()
        twin.setState(jon)
        self.assertEqual(twin.getDesiredState()["name"], jon["name"]) 
        self.assertEqual(twin.getDesiredState()["age"], jon["age"])
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta()["name"], jon["name"]) 
        self.assertEqual(twin.getDelta()["age"], jon["age"])
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
    def testCompound(self):
        #Setup and check twin
        twin = TwinFake()
        twin.setState(compound)
        self.assertEqual(twin.getDesiredState()["name"], compound["name"]) 
        self.assertEqual(twin.getDesiredState()["age"], compound["age"])
        self.assertEqual(twin.getDesiredState()["kin"], compound["kin"])
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta()["name"], compound["name"]) 
        self.assertEqual(twin.getDelta()["age"], compound["age"])
        self.assertEqual(twin.getDelta()["kin"], compound["kin"])
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
        #Trigger Update
        twin.updateState(ageUpdate)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta()["age"], ageUpdate["age"])
        
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
        
        #Trigger Update
        twin.updateState(compoundUpdate)
        self.assertEqual(twin.isUptoDate(), False)       
        self.assertEqual(twin.getDelta()["kin"]["age"], compoundUpdate["kin"]["age"])
        
    def testAddition(self):
        #Setup and check twin
        twin = TwinFake()
        twin.setState(compound)
        self.assertEqual(twin.getDesiredState()["name"], compound["name"]) 
        self.assertEqual(twin.getDesiredState()["age"], compound["age"])
        self.assertEqual(twin.getDesiredState()["kin"], compound["kin"])
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta()["name"], compound["name"]) 
        self.assertEqual(twin.getDelta()["age"], compound["age"])
        self.assertEqual(twin.getDelta()["kin"], compound["kin"])
        
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})    
        
        #Trigger Update
        twin.updateState(addition)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta()["uk"], addition["uk"])
        
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
         #Trigger Update
        twin.updateState(compoundAddition)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta()["address"], compoundAddition["address"])
        
        
    def testFakedThing(self):
         #Setup and check twin
        twin = TwinFake()
        #print("Desired %d Reported %d"%(twin.getDesiredMeta()["timestamp"], 
        #                                twin.getReportedMeta()["timestamp"]))
        twin.stateFromThing(compound)
        #print("Desired %d Reported %d"%(twin.getDesiredMeta()["timestamp"], 
        #                                twin.getReportedMeta()["timestamp"]))
        
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getState()["name"], compound["name"])  
        self.assertEqual(twin.getState()["age"], compound["age"])  
        self.assertEqual(twin.getState()["kin"], compound["kin"])  
        
        #Update
        twin.updateState(compoundUpdate)  
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta()["age"], compoundUpdate["age"])
        self.assertEqual(twin.getDelta()["kin"]["age"], compoundUpdate["kin"]["age"])
                
        #Thing Update  
        self.assertEqual(twin.getDelta()["age"], 50)
        st = copy.deepcopy(twin.getDelta())
        st["age"]=35
        self.assertEqual(st["age"], 35)

        twin.updateFromThing(st)

        
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        self.assertEqual(twin.getState()["age"], 35)
        self.assertEqual(twin.getDeclinedState()["age"], 50)
        self.assertNotEqual(twin.getDeclinedState(), {})
        
        
    def testStateFromThingDoesNotChangeFutureDesire(self):
         #Setup and check twin
        twin = TwinFake()
        twin.setState(compound)
        self.assertEqual(twin.isUptoDate(), False)
        
        #Grab state so we can fake thing state after update
        orgState = copy.deepcopy(twin.getDelta())
        #Update twin
        twin.updateState(compoundUpdate)
        self.assertEqual(twin.isUptoDate(), False)
        
        #state from thing
        twin.stateFromThing(orgState)

        self.assertEqual(twin.isUptoDate(), False)
        self.assertNotEqual(twin.getDeclinedState, {})
        self.assertNotEqual(twin.getDelta(), {})
        
        
        
        
        
        
        
if __name__ == '__main__':
    unittest.main()