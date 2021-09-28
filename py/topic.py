#Topic Class for handling msg from device
#Jon Durrant
#17-Sep-2021


#Topic handler superclass. Respond to a message on a particular topic channel
class Topic:
    
    
     # handle a topic message
     # @param topicName - string name of the topic
     # @param data - string data
     # @param twin - twin interface to use for any responce or getting to state
     #
    def handle(self, topicName : str, data : object, twin):
        return ;

