/*
 * TopicStats.h
 *
 * Get handler to get stats
 *
 *  Created on: 1 Oct 2021
 *      Author: jondurrant
 */

#ifndef TOPICSTATS_H_
#define TOPICSTATS_H_

#include "Topic.h"

/***
 * Topic message handler for a get topic
 */
class TopicStats : public Topic {
public:
	TopicStats();
	virtual ~TopicStats();

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

#endif /* TOPICSTATS_H_ */
