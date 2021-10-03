/*
 * TwinUart.cpp
 *
 *  Created on: 24 Sept 2021
 *      Author: jondurrant
 */

#include "TwinUart.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "TwinProtocol.h"
#include "hardware/uart.h"

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
TwinUart::TwinUart(State *stateObj, char *readBuffer,
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
TwinUart::~TwinUart() {
	// NOP
}

/***
* Read from source with a timeout in miliseconds
* @param miliseconds
*/
void TwinUart::readTimeout(unsigned int miliseconds){
	int readChar;
	unsigned int timeNow = to_ms_since_boot(get_absolute_time ());
	unsigned int timeOut = timeNow + miliseconds;

	/*
	if ((timeNow - readTime) > 500){
		readBuf[0] = 0;
		readBufI=0;
	}
	*/

	if (uart_is_readable_within_us(getUart(), miliseconds)){
		while (true){
			readChar =  uart_getc(getUart());
			if ((readChar == '\n') || (readChar == '\r')){
				readBuf[readBufI] = 0;
				readBufI=0;
				readTime = to_ms_since_boot(get_absolute_time ());

				//printf("->%s\n",readBuf);

				processJson(readBuf);
				return;

			}else {
				readBuf[readBufI] = readChar;
				readBufI++;
				if (readBufI >= readBufLen){
					readBufI=0;
					readBuf[0]=0;
					errorNotify("ERROR OVERRUN",readBuf);
					return;
				}
			}
		}
	}

	timeNow = to_ms_since_boot(get_absolute_time ());
	if (quiteTimems() > getPingTimems()){
		if ((timeNow - lastPingms() ) > getPingTimems()){
			pingHost();
		}
	}

}



/***
 * Write string to stdout
 * @param buf
 */
void TwinUart::outputJson(const char * buf){
	uart_puts(getUart(), buf);
	uart_puts(getUart(), "\n");
	//printf("<-%s\n",buf);
}


/***
 * get the Uart that is being used
 * @return
 */
uart_inst_t* TwinUart::getUart() const {
	return uart;
}

/***
 * Set the uart that is being used, default is uart0
 * @param uart
 */
void TwinUart::setUart(uart_inst_t *uart) {
	this->uart = uart;
}
