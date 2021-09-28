/*
 * TwinProtocol.h
 *
 * Definitions for the Twin Protocol, within JSON structure
 *
 *  Created on: 26 Sept 2021
 *      Author: jondurrant
 */

#ifndef TWINPROTOCOL_H_
#define TWINPROTOCOL_H_

/***
 * Contrainer Types
 */

//state of Twin, {"state"={}}
#define TWINSTATE "state"

//delta of Twin change, {"delta"={}}
#define TWINDELTA "delta"


/***
 * Message structure - MSG
 */

//TOPIC, {"topic"="....", "data"="..."}
#define TWINTOPIC "topic"

//DATA, {"topic"="....", "data"="..."}
#define TWINDATA "data"

/***
 * Default TOPICS
 */

//ERROR - when something goes wrong on the PICO
// {"topic"="error", "data"="..."}
#define TOPICERROR "error"

//GET - request to GET current status from PICO
// {"topic"="get", "data"=""}
#define TOPICGET "get"







#endif /* TWINPROTOCOL_H_ */
