/*
 * oled_tasks.h
 *
 *  Created on: 01 Sep 2018
 *      Author: gal
 */

#ifndef OLED_TASKS_H_
#define OLED_TASKS_H_

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <esp_log.h>

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

/** @brief Queue for sending mouse reports
 **/
extern QueueHandle_t layer_recieve_q;
extern QueueHandle_t led_recieve_q;

#ifdef __cplusplus
}
#endif

#endif /* OLED_TASKS_H_ */
