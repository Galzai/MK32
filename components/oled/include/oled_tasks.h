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
#include "u8g2.h"

#ifdef __cplusplus
extern "C" {
#endif

//rotation defines
#define LANDSCAPE U8G2_R0
#define DEG90 U8G2_R1
#define DEG180 U8G2_R2
#define DEG270 U8G2_R3

/** @brief Setup the oled screen
 * @param rotation - Set rotation
 * */
void init_oled(const u8g2_cb_t *rotation);

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

/** @brief Queue for sending layer to oled
 **/
extern QueueHandle_t layer_recieve_q;

/** @brief Queue for sending led status to oled
 **/
extern QueueHandle_t led_recieve_q;

#ifdef __cplusplus
}
#endif

#endif /* OLED_TASKS_H_ */
