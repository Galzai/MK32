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

/*These are approximate values,
 * battery voltage isn't completely linear, it would be better to use a table for specific battery
 * in the future.
 */
#define Vin_max 4200 //Charging Battery voltage [mV]
#define Vin_min  3400 //Battery discharge Voltage[mV]

#define Vout_max Vin_max*R_2/(R_1+R_2) //Max voltage on analog pin [mV]
#define Vout_min Vin_min*R_2/(R_1+R_2) //Min voltage on analog pin [mV]

uint32_t get_battery_level(void);


//initialize battery monitor pin
void init_batt_monitor(void);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_BATTERY_MONITOR_BATTERY_MONITOR_H_ */
