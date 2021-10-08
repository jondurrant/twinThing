/*
 * State.cpp
 *
 * State superclass to hold the state of a Twin Thing object.
 *
 *  Created on: 19 Sept 2021
 *      Author: jondurrant
 */

#include "State.h"
#include "json-maker.h"
#include <stdio.h>
#include "TwinProtocol.h"


#define TRNSLOT 0

/***
 * Constructor
 */
State::State() {
	for (unsigned char i = 0 ; i < elements; i++){
		jsonHelpers[i] = NULL;
	}
	elements=1;
	jsonHelpers[TRNSLOT] = (StateFunc)&State::jsonTRN;
}

/***
 * Destructor
 */
State::~State() {
	// NOP
}

/***
 * Copy constuctor - no items so blank
 * @param other - to copy from
 */
State::State(const State &other) {
	// TODO Auto-generated constructor stub

}


/***
 * Pull back a delta description in JSON format.
 * @param buf - Buffer to write it to
 * @param len - Len of buffer
 * @return len of json. If 0 then has overwritten end.
 */
unsigned int State::delta(char *buf, unsigned int len) {
	int i = delta(buf, len, dirty);
	return i;
}

/***
 *Pull back a delta of state in JSON format
 * @param buf - buffer to write JSON to
 * @param len - length of buffer
 * @param dirtyCode - code representing the items changed
 * @return length of json or 0 if we exceeded buffer
 */
unsigned int State::delta(char *buf, unsigned int len, unsigned char dirtyCode){
	char *p = buf;
	size_t l = len;

	if (len <10){
		return 0; // It will never fit
	}

	p = json_objOpen( p, NULL, &l );
	p = json_objOpen( p, TWINDELTA, &l );

	unsigned char m = 0;

	for (unsigned char i = 0 ; i < elements; i++){
		m = 1 << i;
		if (m & dirtyCode){
			if (jsonHelpers[i] != NULL) {
				p = (this->*jsonHelpers[i])(p, len - (p - buf));
			}
			if (p - buf > len){
				return 0;
			}
		}
	}
	p = json_objClose( p, &l );
	p = json_objClose( p, &l);
	p = json_end( p, &l );
	if (p - buf > len){
		return 0;
	}
	return p - buf;
}

/***
 * Return state of object as json
 * @param buf - buffer to write json to
 * @param len - length of buffer
 * @return length of json or zero if we ran out of space
 */
unsigned int State::state(char *buf, unsigned int len) {
	char *p = buf;
	size_t l = len;

	if (len < 10){
		return 0; //It will never fit
	}
	p = json_objOpen( p, NULL, &l );
	p = json_objOpen( p, TWINSTATE, &l );

	for (unsigned char i = 0 ; i < elements; i++){
		if (jsonHelpers[i] != NULL) {
			p = (this->*jsonHelpers[i])(p, len - (p - buf));
		}
		if (p - buf > len){
			return 0;
		}
	}
	p = json_objClose( p, &l );
	p = json_objClose( p, &l );
	p = json_end( p, &l );
	if (p - buf > len){
		return 0;
	}
	return p - buf;
}

/***
 * Set state as clean so no delta available
 */
void State::setClean(){
	dirty = 0;
}

/***
 * Is state dirty since last notification was sent.
 * Will only be true in a transaction
 * @return bool
 */
bool State::isDirty() const{
	return (dirty != 0);
}

/***
 * Set the given element to be dirty.
 * Only 8 slots available
 * @param element
 */
void State::setDirty(unsigned char element){
	if (element < 8){
		unsigned char m = 1 << element;
		dirty = dirty | m;
		if (! transaction){
			notifyObservers();
		}
	} else {
		printf("setDirty Too High 0x%x\n", element);
	}
}

/***
 * Attach an observer which will be notified of changes
 * @param observer
 */
void State::attach(StateObserver *observer){
	observers.push_back(observer);
}

/***
 * Detach observer
 * @param observer
 */
void State::detach(StateObserver *observer){
	observers.remove(observer);
}

/***
 * Send notifications of change to all observers
 */
void State::notifyObservers(){
	std::list<StateObserver *>::iterator iterator = observers.begin();
	while (iterator != observers.end()) {
	  (*iterator)->notifyState(dirty);
	  ++iterator;
	}
	setClean();
}

/***
 * Start a transaction and supress notifications until transaction is complete
 */
void State::startTransaction(){
	transaction = true;
}

/***
 * Complete transactions and notify all observers
 */
void State::commitTransaction(){
	transaction = false;
	if (isDirty()){
		notifyObservers();
	}
}


void State::updateFromJson(json_t const *json){
	json_t const *jp;

	jp = json_getProperty(json, STATETRN);
	if (jp){
		if (JSON_INTEGER == json_getType(jp)){
			int i = json_getInteger(jp);
			if (i > 0){
				setTransaction((unsigned int)i);
			}
		}
	}
}

/***
 * Set transaction number. Used by twin client on a host to confirm updates
 * have been processed by pico
 * @param newTRN
 */
void State::setTransaction(unsigned int newTrn){
	trn = newTrn;
	setDirty(TRNSLOT);
}

/***
 * get transaction code. Used by twin client on a host to confirm updates
 * have been processed by pico
 * @return trn
 */
unsigned int State::getTransaction(){
	return trn;
}

/***
 * Retrieve transaction number in JSON format
 * @param buf
 * @param len
 * @return
 */
char* State::jsonTRN(char *buf, unsigned int len){
	char *p = buf;
	size_t l = len;
	p = json_uint( p, STATETRN, getTransaction(), &l);
	return p;
}
