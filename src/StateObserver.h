/*
 * StateObserver.h
 *
 * Abstract class receiving state change notifications from State objects
 *
 *  Created on: 21 Sept 2021
 *      Author: jondurrant
 */

#ifndef STATEOBSERVER_H_
#define STATEOBSERVER_H_


/***
 * Abstract class receiving state change notifications from State objects
 */
class StateObserver {
public:
	StateObserver();
	virtual ~StateObserver();

	/***
	 * Notification of a change of a state item with the State object.
	 * @param dirtyCode - Representation of item changed within state. Used to pull back delta
	 */
	virtual void notifyState(unsigned char dirtyCode)=0;
};

#endif /* STATEOBSERVER_H_ */
