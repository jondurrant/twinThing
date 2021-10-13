/*
 * Twin.cpp
 *
 *  Created on: 24 Sept 2021
 *      Author: jondurrant
 */

#include "Twin.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "json-maker.h"
#include <iterator>
#include "TwinProtocol.h"

/***
 * Constructor
 * @param stateObj - State object that twin is syncing
 * @param readBuffer - read buffer used for reading JSON, sufficient for state and msgs
 * @param readBufferLen - length of read buffer
 * @param writeBuffer - write buffer used for writting JSON, sufficient for state
 * @param writeBufferLen - write buffer length
 * @param msgBuffer  - msg buffer used for writting msg structures
 * @param msgBufferLen - msg buffer len
 * @param jsonBuffer - json Buffer for parsing the json
 * @param jsonBufferLen - number of elements in json buffer
 */
Twin::Twin(State *stateObj, char *readBuffer,
		unsigned int readBufferLen, char *writeBuffer,
		unsigned int writeBufferLen, char * msgBuffer, unsigned int msgBufferLen,
		json_t *jsonBuffer, unsigned int jsonBufferLen) {
	state = stateObj;
	readBuf = readBuffer;
	readBufLen = readBufferLen;
	writeBuf = writeBuffer;
	writeBufLen = writeBufferLen;
	jsonBuf = jsonBuffer;
	jsonBufLen = jsonBufferLen;
	msgBuf = msgBuffer;
	msgBufLen = msgBufferLen;

	state->attach(this);

	//Attach default protocol topic handlers
	addTopic("get", &get);
	addTopic("ping", &ping);
	addTopic("pong", &ping);
	addTopic("stats", &stats);

	for (unsigned int i =0 ; i < STATLEN; i++){
		statErrorsPerMin[i] = 0;
		statMSGPerMin[i] = 0;
	}
}

Twin::~Twin() {
	// NOP
}

/***
* Process a json message received
* @param str
*/
void Twin::processJson(char *str){
	json_t const* json = json_create( str, jsonBuf, jsonBufLen );
	statMsg();
	if ( !json ) {
		errorNotify("ERROR Twin::processJson json create.",str);
		statError();
		return;
	}

	json_t const* delta = json_getProperty(json, TWINDELTA);
	if (!delta){
		delta = json_getProperty(json, TWINSTATE);
	}

	if (delta){
		state->startTransaction();
		state->updateFromJson(delta);
		state->commitTransaction();
		touch();
	} else {
		delta = json_getProperty(json, TWINTOPIC);
		if (delta){
			handleMsg(json);
			touch();
		} else {
			errorNotify("ERROR Twin::processJson state/delta/topic not in JSON.", str);
			statError();
		}
	}

}

/***
 * Notify called by State object to notify of a change.
 * @param dirtyCode - used for pulling back delta of only notified change
 */
void Twin::notifyState(unsigned char dirtyCode){
	unsigned int i;

	i = state->delta(writeBuf, writeBufLen, dirtyCode);
	if (i == 0){
		errorNotify("writeBuf overrun", "Twin::notifyState");
	}
	outputJson(writeBuf);
}

/***
 * Publish a msg back to the host
 * @param topic
 * @param msg
 */
void Twin::publishMsg(const char * topic, const char * msg){
	char *p = writeBuf;
	size_t len = writeBufLen;

	p = json_objOpen( p, NULL, &len );
	p = json_nstr(p, TWINTOPIC, topic, writeBufLen - (p - writeBuf), &len);
	p = json_nstr(p, TWINDATA, msg, writeBufLen - (p - writeBuf), &len);

	p = json_objClose( p, &len );
	p = json_end( p, &len );
	if (p - writeBuf > writeBufLen){
		return;
	}
	outputJson(writeBuf);
}

/***
* Push a error message out as a topic
* @param msg
* @param data
*/
void Twin::errorNotify(const char*  msg, const char*  data){
	char *p = msgBuf;
	size_t len = msgBufLen;

	p = json_objOpen( p, NULL, &len );
	p = json_nstr(p, TOPICERROR, msg, msgBufLen - (p - writeBuf), &len);
	p = json_nstr(p, TWINDATA, data, msgBufLen - (p - writeBuf), &len);
	p = json_objClose( p, &len );
	p = json_end( p, &len );
	if (p - msgBuf > msgBufLen){
		return;
	}
	publishMsg(TOPICERROR, msgBuf);
}

/***
* Handler called when a json message is received.
* @param msg
*/
void Twin::handleMsg(const json_t * msg){
	const char * topic = NULL;
	const char * msgText = NULL;
	json_t const* jp = json_getProperty(msg, TWINTOPIC);

	if (jp){
		if (JSON_TEXT == json_getType(jp)){
			topic =  json_getValue(jp);
			jp = json_getProperty(msg, TWINDATA);
			if (jp){
				auto it = topics.find(topic);
				if(it == topics.end()){
					errorNotify("Unknown topic", topic);
					return;
				}
				if (JSON_TEXT == json_getType(jp)){
					msgText =  json_getValue(jp);
					it->second->handle(topic, msgText, this);
				}
				if (JSON_OBJ == json_getType(jp)){
					it->second->handle(topic, jp, this);
				}
			}
		}
	}

}

/***
* Add a topic handler for messages
* @param topicName
* @param topicHandler
*/
void Twin::addTopic(const char * topicName, Topic *topicHandler){
	topics.insert(std::make_pair(topicName, topicHandler));
}

/***
* return pointer to the state object
* @return state
*/
State * Twin::getState(){
	return state;
}

/***
* get message buffer for use by Topic handlers
* @return
*/
char * Twin::getMsgBuf(){
	return msgBuf;
}


/***
 * return msg buffer length for use by topic handlers
 * @return
 */
unsigned int Twin::getMsgBufLen(){
	return msgBufLen;
}

/***
* returns amount of time with no comms before pico will send a ping message
* @return time in ms
*/
unsigned int Twin::getPingTimems() const {
	return pingTimems;
}

/***
 * Set amount of time with no comms before pico will send a ping message
 * @return time in ms
 */
void Twin::setPingTimems(unsigned int pingTimems) {
	this->pingTimems = pingTimems;
}

/***
* Returns ms since last communication with host
* @return
*/
unsigned int Twin::quiteTimems(){
	return (to_ms_since_boot(get_absolute_time ()) - touchTime);
}

/***
 * Update time of last communication with host
 */
void Twin::touch(){
	touchTime = to_ms_since_boot(get_absolute_time ());
}

/***
 * ping the host
 */
void Twin::pingHost(){
	ping.pingHost(this);
	lastPing = to_ms_since_boot(get_absolute_time ());
}

/***
* Time of last ping in ms
* @return
*/
unsigned int Twin::lastPingms(){
	return lastPing;
}


/***
 * Add statistic for protocol error
 */
void Twin::statError(){
	unsigned int min = to_ms_since_boot(get_absolute_time ()) / 60000;
	if (min != statTimeMin){
		statTimeMin = min;
		statIndex++;
		if (statIndex >= STATLEN){
			statIndex = 0;
		}
		statErrorsPerMin[statIndex] = 0;
		statMSGPerMin[statIndex] = 0;
	}
	statErrorsPerMin[statIndex]++;
}

/***
 * Add statistic for protocol msg received
 */
void Twin::statMsg(){
	unsigned int min = to_ms_since_boot(get_absolute_time ()) / 60000;
	if (min != statTimeMin){
		statTimeMin = min;
		statIndex++;
		if (statIndex >= STATLEN){
			statIndex = 0;
		}
		statErrorsPerMin[statIndex] = 0;
		statMSGPerMin[statIndex] = 0;
	}
	statMSGPerMin[statIndex]++;
}

/***
 * Return json string of stats
 * @return
 */
char * Twin::getStats(){
	int i;
	char *p = msgBuf;
	size_t len  = msgBufLen;


	p = json_objOpen( p, NULL, &len );
	p = json_arrOpen( p, "error", &len);
	for (i = statIndex; i >= 0 ; i--){
		p = json_uint( p, NULL, statErrorsPerMin[i], &len );
	}
	for (i = STATLEN-1; i > statIndex; i--){
		p = json_uint( p, NULL, statErrorsPerMin[i], &len );
	}
	p = json_arrClose( p, &len);
	p = json_arrOpen( p, "msg", &len);
	for (i = statIndex; i >= 0 ; i--){
		p = json_uint( p, NULL, statMSGPerMin[i], &len );
	}
	for (i = STATLEN-1; i > statIndex; i--){
		p = json_uint( p, NULL, statMSGPerMin[i], &len );
	}
	p = json_arrClose( p, &len);
	p = json_objClose( p, &len );
	return msgBuf;
}
