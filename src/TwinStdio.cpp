/*
 * TwinStdio.cpp
 *
 *  Created on: 24 Sept 2021
 *      Author: jondurrant
 */

#include "TwinStdio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "TwinProtocol.h"

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
TwinStdio::TwinStdio(State *stateObj, char *readBuffer,
		unsigned int readBufferLen, char *writeBuffer,
		unsigned int writeBufferLen, char * msgBuffer, unsigned int msgBufferLen,
		json_t *jsonBuffer, unsigned int jsonBufferLen) :
			Twin(stateObj,
					readBuffer, readBufferLen, writeBuffer, writeBufferLen,
					msgBuffer, msgBufferLen,
					jsonBuffer, jsonBufferLen)   {


}


/***
 * Destructor
 */
TwinStdio::~TwinStdio() {
	// NOP
}

/***
* Read from source with a timeout in miliseconds
* @param miliseconds
*/
void TwinStdio::readTimeout(unsigned int miliseconds){
	int readChar;
	unsigned int timeNow = to_ms_since_boot(get_absolute_time ());
	unsigned int timeOut = timeNow + miliseconds;


	while (timeOut >= timeNow){

		readChar =  getchar_timeout_us(0);

		while (readChar != PICO_ERROR_TIMEOUT){
			if ((readChar == '\n') || (readChar == '\r')){
				readBuf[readBufI] = 0;
				readBufI=0;

				processJson(readBuf);

			}else {
				readBuf[readBufI] = readChar;
				readBufI++;
				if (readBufI >= readBufLen){
					readBufI=0;
					readBuf[79]=0;
					errorNotify("ERROR OVERRUN",readBuf);
				}
			}
			readChar =  getchar_timeout_us(0);
		}

		timeNow = to_ms_since_boot(get_absolute_time ());

		if (quiteTimems() > getPingTimems()){
			if ((timeNow - lastPingms() ) > getPingTimems()){
				pingHost();
			}
		}

	}

}



/***
 * Write string to stdout
 * @param buf
 */
void TwinStdio::outputJson(const char * buf){
	printf("%s\n", buf);
}
