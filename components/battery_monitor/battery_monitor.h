/*
 * battery_monitor.h
 *
 *  Created on: 6 Sep 2018
 *      Author: gal
 */

#ifndef BATTERY_MONITOR_BATTERY_MONITOR_H_
#define BATTERY_MONITOR_BATTERY_MONITOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *Please make sure you understand what is a voltage divider before modifying
 * */

#define R_1 47  //Pull-up resistor [kOhm]
#define R_2 22 //Pull-down resistor [kOhm]

#define Vin_max 3700 //Full Battery voltage [mV]
#define Vin_min  2300 //Minimum operating voltage for the esp32 [mV]

#define Vout_max Vin_max*R_2/(R_1+R_2) //Max voltage on analog pin [mV]
#define Vout_min Vin_min*R_2/(R_1+R_2) //Min voltage on analog pin [mV]

uint32_t get_battery_level(void);


//initialize battery monitor pin
void init_batt_monitor(void);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_BATTERY_MONITOR_BATTERY_MONITOR_H_ */
