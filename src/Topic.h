/*
 * Topic.h
 *
 * Topic message handler superclass
 *
 *  Created on: 25 Sept 2021
 *      Author: jondurrant
 */

#ifndef TOPIC_H_
#define TOPIC_H_

/***
 * Superclass for TOPIC message handlers
 * handler could handle multiple topicName if required
 */
class Topic;

#include "TwinAbstract.h"
#include <tiny-json.h>

class Topic {
public:

	/***
	 * handle a topic message
	 * @param topicName - string name of the topic
	 * @param data - string data
	 * @param twin - twin interface to use for any responce or getting to state
	 */
	virtual void handle(const char * topicName, const char * data, TwinAbstract *twin) = 0;

	/***
	 * handle a topic message
	 * @param topicName - string name of the topic
	 * @param jp - json structure
	 * @param twin - twin interface to use for any responce or getting to state
	 */
	virtual void handle(const char * topicName, json_t const* jp, TwinAbstract *twin) = 0;

protected:
	/***
	 * Constructor
	 */
	Topic();

	/***
	 * Destructor
	 */
	virtual ~Topic();
};

#endif /* TOPIC_H_ */
