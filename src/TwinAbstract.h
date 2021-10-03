/*
 * TwinAbstract.h
 *
 * Abstract Twin class used to manage interface to Topics
 *
 *  Created on: 28 Sept 2021
 *      Author: jondurrant
 */

#ifndef TWINABSTRACT_H_
#define TWINABSTRACT_H_

#include "tiny-json.h"
#include "State.h"
#include "StateObserver.h"


class TwinAbstract: public StateObserver {
public:
	TwinAbstract();
	virtual ~TwinAbstract();

	/***
	 * Notify called by State object to notify of a change.
	 * @param dirtyCode - used for pulling back delta of only notified change
	 */
	 virtual void notifyState(unsigned char dirtyCode)=0;

	 /***
	  * Publish a msg back to the host
	  * @param topic
	  * @param msg
	  */
	 virtual void publishMsg(const char * topic, const char * msg)=0;

	 /***
	  * Handler called when a json message is received.
	  * @param msg
	  */
	 virtual void handleMsg(const json_t * msg)=0;

	 /***
	  * return pointer to the state object
	  * @return state
	  */
	 virtual State *getState()=0;

	 /***
	  * get message buffer for use by Topic handlers
	  * @return
	  */
	 virtual char * getMsgBuf()=0;

	 /***
	  * get message buffer length
	  * @return
	  */
	 virtual unsigned int getMsgBufLen()=0;

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
	 virtual void errorNotify(const char*  msg, const char*  data)=0;

	 /***
	  * returns amount of time with no comms before pico will send a ping message
	  * @return time in ms
	  */
	 virtual unsigned int getPingTimems() const=0;

	 /***
	  * Set amount of time with no comms before pico will send a ping message
	  * @return time in ms
	  */
	 virtual void setPingTimems(unsigned int pingTimems) =0;

	 /***
	  * Returns ms since last communication with host
	  * @return
	  */
	 virtual unsigned int quiteTimems() =0;

	 /***
	  * ping the host
	  */
	 virtual void pingHost() =0;

	 /***
	  * Time of last ping in ms
	  * @return
	  */
	 virtual unsigned int lastPingms() =0;


	 /***
	  * Return json string of stats
	  * @return
	  */
	  virtual char * getStats() = 0;

	 /***
	  * Add statistic for protocol error
	  */
	  virtual void statError() = 0;

	 /***
	  * Add statistic for msg received
	  */
	 virtual void statMsg() = 0;


};

#endif /* TWINABSTRACT_H_ */
