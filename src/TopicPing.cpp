/*
 * TopicPing.cpp
 *
 * Ping topic handler (respond with pong)
 *
 *  Created on: 25 Sept 2021
 *      Author: jondurrant
 */

#include "TopicPing.h"
#include <string.h>

TopicPing::TopicPing() {
	// NOP

}

TopicPing::~TopicPing() {
	// NOP
}

/***
 * handle a topic message
 * @param topicName - string name of the topic
 * @param data - string data
 * @param twin - twin interface to use for any responce or getting to state
 */
void TopicPing::handle(const char * topicName, const char * msg, TwinAbstract *twin){
	if (strcmp(topicName, "ping") == 0){
		twin->publishMsg("pong", "");
	}else if (strcmp(topicName, "pong") == 0){
		//TODO
	}
}

/***
 * handle a topic message
 * @param topicName - string name of the topic
 * @param jp - json structure
 * @param twin - twin interface to use for any responce or getting to state
 */
 void TopicPing::handle(const char * topicName, json_t const* jp, TwinAbstract *twin){
	 handle(topicName, "", twin);
 }

 /***
 * Send ping message to host
 * @param twin - twin interface to use for any responce or getting to state
 */
 void TopicPing::pingHost( TwinAbstract *twin){
	 twin->publishMsg("ping", "");
 }
