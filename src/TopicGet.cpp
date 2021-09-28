/*
 * TopicGet.cpp
 *
 *  Created on: 25 Sept 2021
 *      Author: jondurrant
 */

#include "TopicGet.h"
#include "State.h"

TopicGet::TopicGet() {
	// NOP

}

TopicGet::~TopicGet() {
	// NOP
}


/***
 * handle a topic message
 * @param topicName - string name of the topic
 * @param data - string data
 * @param twin - twin interface to use for any response or getting to state
 */
void TopicGet::handle(const char * topicName, const char * data, TwinAbstract *twin){
	State * state = twin->getState();
	char * buf = twin->getMsgBuf();
	unsigned int len = twin->getMsgBufLen();

	state->state(buf, len);
	twin->outputJson(buf);
}

/***
 * handle a topic message
 * @param topicName - string name of the topic
 * @param jp - json structure
 * @param twin - twin interface to use for any responce or getting to state
 */
 void TopicGet::handle(const char * topicName, json_t const* jp, TwinAbstract *twin){
	 handle(topicName, "", twin);
 }
