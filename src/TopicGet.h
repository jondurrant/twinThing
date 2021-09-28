/*
 * TopicGet.h
 *
 * Get handler to get state
 *
 *  Created on: 25 Sept 2021
 *      Author: jondurrant
 */

#ifndef TOPICGET_H_
#define TOPICGET_H_

#include "Topic.h"

/***
 * Topic message handler for a get topic
 */
class TopicGet : public Topic {
public:
	TopicGet();
	virtual ~TopicGet();

	/***
	 * handle a topic message
	 * @param topicName - string name of the topic
	 * @param data - string data
	 * @param twin - twin interface to use for any response or getting to state
	 */
	void handle(const char * topicName, const char * data, TwinAbstract *twin);

	/***
	 * handle a topic message
	 * @param topicName - string name of the topic
	 * @param jp - json structure
	 * @param twin - twin interface to use for any responce or getting to state
	 */
	 void handle(const char * topicName, json_t const* jp, TwinAbstract *twin);
};

#endif /* TOPICGET_H_ */
