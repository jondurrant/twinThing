#Serial Digital Twin for a PICO device
#Jon Durrant
#27-Sep-2021

from twin import Twin
import json
from serial import Serial


#===============================================================================
# Twin class to talk over a serial port
#===============================================================================
class TwinSerial(Twin):
    
    #===========================================================================
    # Constructor
    #===========================================================================
    def __init__(self, serialDevice : Serial):
        super(TwinSerial, self).__init__()
        self.serialDev = serialDevice
        
    #===========================================================================
    # Write string to comms channel and therefore twin
    #===========================================================================
    def outputJson(self, s: str):
        st = s
        if (isinstance(s, dict)):
            st = json.dumps(s)
            
        b = str.encode(st+"\n")
        self.serialDev.write(b)
        self.serialDev.flush()

    #===========================================================================
    # read and process a line from the Twin
    #===========================================================================
    def readLine(self):
        b = self.serialDev.readline()
        s = b.decode("utf-8") 
        if (len(s) != 0):
            self.processJson(s)   

        