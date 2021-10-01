/*
 * Twin.h
 *
 *  Created on: 24 Sept 2021
 *      Author: jondurrant
 */

#ifndef TWIN_H_
#define TWIN_H_


#include <map>

#include "tiny-json.h"
#include "State.h"
#include "StateObserver.h"

#include "TwinAbstract.h"

//Default topic handlers
#include "Topic.h"
#include "TopicGet.h"
#include "TopicPing.h"
#include "TopicStats.h"

#define STATLEN 10




class Twin :  public TwinAbstract {
public:
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
	Twin(State *stateObj, char *readBuffer,
			unsigned int readBufferLen, char *writeBuffer,
			unsigned int writeBufferLen, char * msgBuffer, unsigned int msgBufferLen,
			json_t *jsonBuffer, unsigned int jsonBufferLen);
	virtual ~Twin();

	/***
	 * Read from source with a timeout in miliseconds
	 * @param miliseconds
	 */
	virtual void readTimeout(unsigned int miliseconds) = 0;

	/***
	 * Notify called by State object to notify of a change.
	 * @param dirtyCode - used for pulling back delta of only notified change
	 */
	 void notifyState(unsigned char dirtyCode);

	 /***
	  * Publish a msg back to the host
	  * @param topic
	  * @param msg
	  */
	 void publishMsg(const char * topic, const char * msg);

	 /***
	  * Handler called when a json message is received.
	  * @param msg
	  */
	 void handleMsg(const json_t * msg);

	 /***
	  * Add a topic handler for messages
	  * @param topicName
	  * @param topicHandler
	  */
	 void addTopic(const char * topicName, Topic *topicHandler);

	 /***
	  * return pointer to the state object
	  * @return state
	  */
	 State *getState();

	 /***
	  * get message buffer for use by Topic handlers
	  * @return
	  */
	 char * getMsgBuf();

	 /***
	  * get message buffer length
	  * @return
	  */
	 unsigned int getMsgBufLen();

	 /***
	  * Write json string back to the comms channel
	  * @param buf
	  */
	 virtual void outputJson(const char * buf) = 0;

	 /***
	  * Push a error message out as a topic
	  * @param msg
	  * @param data
	  */
	 virtual void errorNotify(const char*  msg, const char*  data);

	 /***
	  * returns amount of time with no comms before pico will send a ping message
	  * @return time in ms
	  */
	 unsigned int getPingTimems() const;

	 /***
	  * Set amount of time with no comms before pico will send a ping message
	  * @return time in ms
	  */
	 void setPingTimems(unsigned int pingTimems);

	 /***
	  * Returns ms since last communication with host
	  * @return
	  */
	 unsigned int quiteTimems();

	 /***
	  * ping the host
	  */
	 void pingHost();

	 /***
	  * Time of last ping in ms
	  * @return
	  */
	 unsigned int lastPingms();

	 /***
	  * Return json string of stats
	  * @return
	  */
	  char * getStats();

protected:

	/***
	 * Process a json message received
	 * @param str
	 */
	void processJson(char *str);

	/***
	 * Update time of last communication with host
	 */
	void touch();

	/***
	 * Add statistic for protocol error
	 */
	void statError();

	/***
	 * Add statistic for good protocol msg
	 */
	void statOK();

	unsigned int statTimeMin = 0;
	unsigned int statIndex = 0;
	unsigned int statErrorsPerMin[STATLEN];
	unsigned int statMSGPerMin[STATLEN];

	//Time of last comms with host
	unsigned int touchTime = 0;



	//Buffer structures for json string processing
	char *readBuf  = NULL;
	char *writeBuf = NULL;
	char *msgBuf = NULL;
	unsigned int readBufLen  = 0;
	unsigned int writeBufLen = 0;
	unsigned int readBufI = 0;
	unsigned int msgBufLen = 0;

	//Json parsing structure buffer
	json_t *jsonBuf = NULL;
	unsigned int jsonBufLen = 0;

	//Stage object being synced
	State *state = NULL;

	//map of topic handlers for msg handling
	std::map<std::string, Topic *> topics;


	//Default protocol topic handers
	TopicGet get;
	TopicPing ping;
	TopicStats stats;


	//ping timeout
	unsigned int pingTimems = 10000;
	unsigned int lastPing = 0;

};

#endif /* STATETWIN_H_ */
