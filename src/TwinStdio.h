/*
 * TwinStdio.h
 *
 * Twin IO module communicating over STDIO
 *
 *  Created on: 24 Sept 2021
 *      Author: jondurrant
 */

#ifndef TWINSTDIO_H_
#define TWINSTDIO_H_

#include "Twin.h"

/***
 * Twin communication moduel for communicating over STDIO
 */
class TwinStdio : public Twin {
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
	TwinStdio(State *stateObj, char *readBuffer,
			unsigned int readBufferLen, char *writeBuffer,
			unsigned int writeBufferLen, char * msgBuffer, unsigned int msgBufferLen,
			json_t *jsonBuffer, unsigned int jsonBufferLen);

	/***
	 * Destuctor
	 */
	virtual ~TwinStdio();

	/***
	 * Read from source with a timeout in miliseconds
	 * @param miliseconds
	 */
	void readTimeout(unsigned int miliseconds);

protected:

	/***
	 * Write string to stdout
	 * @param buf
	 */
	void outputJson(const char * buf);
};

#endif /* TWINSTDIO_H_ */
