# Twin Observer, superclass - abstract
#Jon Durrant
#27-Sep-2021

#===============================================================================
# TwinObserver superclass, abstract
#===============================================================================
class TwinObserver:
    def notify(self, twin):
        return 
    
    def twinOffline(self, twin):
         return
     
    def twinOnline(self, twin):
        return 
    
    #Called if twin is quiet for more than the quietTimeoutMs
    def twinQuiet(self, twin):
        return
    
    #Called folloing a twinQuiet alarm when the twin recovers and first talks
    def twinChatting(self, twin):
        return