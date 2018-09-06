/*
 * oled_tasks.h
 *
 *  Created on: 01 Sep 2018
 *      Author: gal
 */

#ifndef OLED_TASKS_H_
#define OLED_TASKS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Setup the oled screen
 * */
void init_oled(void );

/** @brief deinitialize the oled screen
 * */

void deinit_oled(void);

/** @brief connecting waiting animation
 * */
void waiting_oled(void);

/** @brief connected graphic
 * */
void ble_connected_oled(void);

/** @brief connected graphic
 * */
void ble_slave_oled(void);

/** @brief running oled task
 * */
void update_oled(void);


#ifdef __cplusplus
}
#endif

#endif /* OLED_TASKS_H_ */
