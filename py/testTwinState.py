import unittest

from twinState import TwinState

jon = {
"name": "Jon",
"age": 13
}

ageUpdate = {
    "age": 50
}


class MyTest(unittest.TestCase):
        
        
    def testSetState(self):
        twin = TwinState()
        twin.setState(jon)
        self.assertEqual(twin.getState(), jon)       
        self.assertEqual(twin.getMeta()["age"], twin.getMeta()["name"])
        
    def testUpdateState(self):
        twin = TwinState()
        twin.setState(jon)
        self.assertEqual(twin.getState(), jon) 
        twin.updateState(ageUpdate)
        self.assertNotEqual(twin.getState(), jon)
        self.assertEqual(twin.getState()["age"], 50)
        self.assertGreater(twin.getMeta()["age"], twin.getMeta()["name"])
        
if __name__ == '__main__':
    unittest.main()