/*
 * TopicStats.cpp
 *
 *  Created on: 25 Sept 2021
 *      Author: jondurrant
 */

#include "TopicStats.h"
#include "State.h"

TopicStats::TopicStats() {
	// NOP

}

TopicStats::~TopicStats() {
	// NOP
}


/***
 * handle a topic message
 * @param topicName - string name of the topic
 * @param data - string data
 * @param twin - twin interface to use for any response or getting to state
 */
void TopicStats::handle(const char * topicName, const char * data, TwinAbstract *twin){
	char * buf = twin->getStats();
	twin->publishMsg("stats",buf);
}

/***
 * handle a topic message
 * @param topicName - string name of the topic
 * @param jp - json structure
 * @param twin - twin interface to use for any responce or getting to state
 */
 void TopicStats::handle(const char * topicName, json_t const* jp, TwinAbstract *twin){
	 handle(topicName, "", twin);
 }
