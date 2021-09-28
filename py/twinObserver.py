# Twin Observer, superclass - abstract
#Jon Durrant
#27-Sep-2021

#===============================================================================
# TwinObserver superclass, abstract
#===============================================================================
class TwinObserver:
    def notify(self, twin):
        raise Exception("Abstraction function - twinObserver.notify")