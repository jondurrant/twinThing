/*
 * TwinUart.h
 *
 * Twin IO module communicating over STDIO
 *
 *  Created on: 24 Sept 2021
 *      Author: jondurrant
 */

#ifndef TWINUART_H_
#define TWINUART_H_

#include "hardware/uart.h"

#include "Twin.h"

/***
 * Twin communication moduel for communicating over STDIO
 */
class TwinUart : public Twin {
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
	TwinUart(State *stateObj, char *readBuffer,
			unsigned int readBufferLen, char *writeBuffer,
			unsigned int writeBufferLen, char * msgBuffer, unsigned int msgBufferLen,
			json_t *jsonBuffer, unsigned int jsonBufferLen);

	/***
	 * Destuctor
	 */
	virtual ~TwinUart();

	/***
	 * Read from source with a timeout in miliseconds
	 * @param miliseconds
	 */
	virtual void readTimeout(unsigned int miliseconds);

	/***
	 * get the Uart that is being used
	 * @return
	 */
	uart_inst_t* getUart() const;

	/***
	 * Set the uart that is being used, default is uart0
	 * @param uart
	 */
	void setUart(uart_inst_t *uart);

protected:

	/***
	 * Write string to stdout
	 * @param buf
	 */
	virtual void outputJson(const char * buf);

	//Uart being used
	uart_inst_t *uart = uart0;

	//last read
	unsigned int readTime = 0;
};

#endif /* TWINUART_H_ */
