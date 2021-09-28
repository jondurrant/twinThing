import unittest

from twin import Twin
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
        twin = Twin()
        twin.setState(jon)
        self.assertEqual(twin.getDesiredState(), jon)       
        self.assertEqual(twin.getDesiredMeta()["age"], twin.getDesiredMeta()["name"])
        
    def testUpdateState(self):
        twin = Twin()
        twin.setState(jon)
        self.assertEqual(twin.getDesiredState(), jon)   
        twin.updateState(ageUpdate)
        self.assertNotEqual(twin.getDesiredState(), jon)
        self.assertEqual(twin.getDesiredState()["age"], 50)
        self.assertGreater(twin.getDesiredMeta()["age"], twin.getDesiredMeta()["name"])
        
        
    def testDeltaForTwin(self):
        twin = Twin()
        twin.setState(jon)
        self.assertEqual(twin.getDesiredState(), jon)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta(), jon)
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
    def testCompound(self):
        #Setup and check twin
        twin = Twin()
        twin.setState(compound)
        self.assertEqual(twin.getDesiredState(), compound)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta(), compound)
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
        #Trigger Update
        twin.updateState(ageUpdate)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta(), ageUpdate)
        
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
        
        #Trigger Update
        twin.updateState(compoundUpdate)
        self.assertEqual(twin.isUptoDate(), False)       
        self.assertEqual(twin.getDelta(), compoundUpdate)
        
    def testAddition(self):
        #Setup and check twin
        twin = Twin()
        twin.setState(compound)
        self.assertEqual(twin.getDesiredState(), compound)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta(), compound)
        
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})    
        
        #Trigger Update
        twin.updateState(addition)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta(), addition)
        
        #Force reported update and check delta
        twin.reported.updateState(twin.getDelta())
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        
         #Trigger Update
        twin.updateState(compoundAddition)
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta(), compoundAddition)
        
        
    def testFakedThing(self):
         #Setup and check twin
        twin = Twin()
        #print("Desired %d Reported %d"%(twin.getDesiredMeta()["timestamp"], 
        #                                twin.getReportedMeta()["timestamp"]))
        twin.stateFromThing(compound)
        #print("Desired %d Reported %d"%(twin.getDesiredMeta()["timestamp"], 
        #                                twin.getReportedMeta()["timestamp"]))
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getState(), compound)  
        
        #Update
        twin.updateState(compoundUpdate)  
        self.assertEqual(twin.isUptoDate(), False)
        self.assertEqual(twin.getDelta(), compoundUpdate)
        
        #Add TRN to allow decline to work
        trn=481
        trnUpdate = {"trn": trn}
        twin.updateState(trnUpdate)
        
        #Thing Update  
        ageTrnUpdate = copy.deepcopy(ageUpdate)
        ageTrnUpdate["trn"]=trn
        twin.updateFromThing(ageTrnUpdate)
        self.assertEqual(twin.isUptoDate(), True)
        self.assertEqual(twin.getDelta(), {})
        print(json.dumps(twin.getDeclinedState()))
        self.assertNotEqual(twin.getDeclinedState, {})
        
        
        
if __name__ == '__main__':
    unittest.main()