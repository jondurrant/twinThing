#FAKE TWIN, used for testing only
#Jon Durrant
#17-Oct-2021

from twin import Twin
import json
from serial import Serial
import logging


#===============================================================================
# Twin class to talk over a serial port
#===============================================================================
class TwinFake(Twin):
    
    #===========================================================================
    # Constructor
    #===========================================================================
    def __init__(self):
        super(TwinFake, self).__init__()
        self.logging = logging.getLogger(__name__)

        
    #===========================================================================
    # Write string to comms channel and therefore twin
    #===========================================================================
    def outputJson(self, s: str):
        logging.debug("OutputJson(%s)"%s)
        return

    #===========================================================================
    # read and process a line from the Twin
    #===========================================================================
    def readLine(self):
       return   

        