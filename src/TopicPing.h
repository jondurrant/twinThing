/*
 * TopicPing.h
 *
 * Ping topic handler (respond with pong)
 *
 *  Created on: 25 Sept 2021
 *      Author: jondurrant
 */

#ifndef TOPICPING_H_
#define TOPICPING_H_

#include "Topic.h"

class TopicPing : public Topic{
public:
	TopicPing();
	virtual ~TopicPing();

	/***
	 * handle a topic message
	 * @param topicName - string name of the topic
	 * @param data - string data
	 * @param twin - twin interface to use for any responce or getting to state
	 */
	void handle(const char * topicName, const char * data, TwinAbstract *twin);

	/***
	 * handle a topic message
	 * @param topicName - string name of the topic
	 * @param jp - json structure
	 * @param twin - twin interface to use for any responce or getting to state
	 */
	 void handle(const char * topicName, json_t const* jp, TwinAbstract *twin);

	 /***
	 * Send ping messahe to host
	 * @param twin - twin interface to use for any responce or getting to state
	 */
	 void pingHost( TwinAbstract *twin);
};

#endif /* TOPICPING_H_ */
