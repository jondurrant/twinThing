/*
 * StateTemp.h
 *
 * Useful base building block State that has temperature of PICO
 *
 *  Created on: 19 Sept 2021
 *      Author: jondurrant
 */

#ifndef STATETEMP_H_
#define STATETEMP_H_



#include "State.h"
#include "pico/stdlib.h"

/***
 * Temperature state class reading temp from PICO and holding time since boot
 * Note time since boot is only provide in state json and never in delta
 */
class StateTemp: public State {
public:
	StateTemp();
	virtual ~StateTemp();
	StateTemp(const StateTemp &other);

	/***
	 * Update temperature from PICO
	 */
	void updateTemp();
	/***
	 * Get temperature in celcius
	 * @return celcius
	 */
	float getTemp() const;
	/***
	 * Set temerature in celcius
	 * @param
	 */
	void setTemp(float);

	/***
	 * Get time since boot in seconds
	 * @return seconds
	 */
	uint32_t getBootTime();

	/***
	 * Update state data from a json structure
	 * @param json
	 */
	virtual void updateFromJson(json_t const *json);



protected:
	/***
	 * get temperature in json format
	 * @param buf
	 * @param len
	 * @return
	 */
	char* jsonTemp(char *buf, unsigned int len);
	/***
	 * get boot time in json format.
	 * @param buf
	 * @param len
	 * @return
	 */
	char* jsonBoot(char *buf, unsigned int len);

private:
	float temprature = 0.0;
	#define STATETEMPERATURE "temp"
	#define STATEBOOT "boot"

};

#endif /* STATETEMP_H_ */
