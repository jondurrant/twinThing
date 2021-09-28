#Topic Class for handling ping and pong responses
#Jon Durrant
#17-Sep-2021

from topic import Topic
import twinProtocol


#Handler for the ping topic. Will also handle to pong response
class TopicPing(Topic):
    
    
     # handle a topic message
     # @param topicName - string name of the topic
     # @param data - string data
     # @param twin - twin interface to use for any responce or getting to state
     #
    def handle(self, topicName : str, data: object, twin):
        if (topicName == twinProtocol.TOPICPING):
            twin.publishMsg(twinProtocol.TOPICPONG, "");
            twin.touch()
        if (topicName == twinProtocol.TOPICPONG):
            twin.touch()
