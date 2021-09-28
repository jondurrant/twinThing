#Twin protocol definitions
#Jon Durrant
#27-Sep-2021


#//state of Twin, {"state"={}}
TWINSTATE = "state"

##delta of Twin change, {"delta"={}}
TWINDELTA = "delta"


###
###Message structure - MSG
###

#//TOPIC, {"topic"="....", "data"="..."}
TWINTOPIC = "topic"

#//DATA, {"topic"="....", "data"="..."}
TWINDATA = "data"

####
### Default TOPICS
###

#ERROR - when something goes wrong on the PICO
# {"topic"="error", "data"="..."}
TOPICERROR = "error"

#GET - request to GET current status from PICO
# {"topic"="get", "data"=""}
TOPICGET = "get"


#PING - request PONG response
# {"topic"="ping", "data"=""}
TOPICPING = "ping"

#PONG - response to PING
# {"topic"="pong", "data"=""}
TOPICPONG = "pong"


#trn - transaction state member, used to sync twins
TWINTRN = "trn"
