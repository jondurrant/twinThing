/*
 * StateTemp.cpp
 *
 * Temperate state object. Reads Temp from PICO
 *
 *  Created on: 19 Sept 2021
 *      Author: jondurrant
 */

#include "StateTemp.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <string.h>
#include "json-maker.h"

#define TEMPSLOT 1
#define BOOTSLOT 2

/***
 * Constructor
 */
StateTemp::StateTemp() {
	//Initialise the temp censor for on chip
	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	elements=3;
	jsonHelpers[TEMPSLOT] = (StateFunc)&StateTemp::jsonTemp; // @suppress("Symbol is not resolved")
	jsonHelpers[BOOTSLOT] = (StateFunc)&StateTemp::jsonBoot; // @suppress("Symbol is not resolved")

}

/***
 * Destructor
 */
StateTemp::~StateTemp() {
	// TODO Auto-generated destructor stub
}

/***
 * Copy constructor
 * @param other
 */
StateTemp::StateTemp(const StateTemp &other) {
	temprature = other.getTemp();

}

/***
 * Update Temp from the PICO on chip sensor
 */
void StateTemp::updateTemp(){
	const float conversion_factor = 3.3f / (1 << 12);
	float v = (float)adc_read() * conversion_factor;
	float t = 27.0 - ((v - 0.706)/0.001721);
	if (t != getTemp()){
		setTemp((float)t);
	}
}

/***
 * Set temperate as provided in param
 * @param f celcius
 */
void StateTemp::setTemp(float f){
	temprature = f;
	setDirty(TEMPSLOT);
}

/***
 * Get temperateure
 * @return celcius
 */
float StateTemp::getTemp() const{
	return temprature;
}

/***
 * Get time since boot in seconds
 * @return
 */
uint32_t StateTemp::getBootTime(){
	return to_ms_since_boot(get_absolute_time ())/1000;
}


/***
 * Get temperate in JSON format
 * @param buf
 * @param len
 * @return
 */
char* StateTemp::jsonTemp(char *buf, unsigned int len){
	char *p = buf;
	size_t l = len;
    p = json_double( p, STATETEMPERATURE, (double)getTemp(), &l);
    return p;
}

/***
 * Get time since boot in json format
 * @param buf
 * @param len
 * @return
 */
char* StateTemp::jsonBoot(char *buf, unsigned int len){
	char *p = buf;
	size_t l = len;
	p = json_int( p, STATEBOOT, getBootTime(), &l);
	return p;
}


/***
* Update state data from a json structure
* @param json
*/
void StateTemp::updateFromJson(json_t const *json){
	State::updateFromJson(json);
}
