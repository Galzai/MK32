/*
 * keycode_conv.h
 *
 *  Created on: 6 Oct 2018
 *      Author: gal
 */


#ifndef KEYCODE_CONV_KEYCODE_CONV_H_
#define KEYCODE_CONV_KEYCODE_CONV_H_

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include "keyboard_config.h"

#ifdef __cplusplus
extern "C" {
#endif

//Flag to enable or disable sending reports via BLE
int BLE_EN;

/** @brief Queue for receiving keyboard input
 *  */
QueueHandle_t input_str_q;

/*
 * @pause sending key reports and use keypress to enter char
 */
void enable_key_to_char(void);

/*
 * @brief stop retrieving keypress to char and ireinit sending key reports
 */
void disable_key_to_char(void);

/*
 * @brief retrieve char corresponding to keycode
 *
 * @param keycode keycode to convert to ascii
 * @param shifted indicated if shift/caps lock is pressed
 *
 */
char keycode_to_char(uint16_t keycode, uint8_t shifted);

/*
 * @brief retrieve keycpde corresponding to char (or string)
 */
uint8_t char_to_keycode(char ascii_key);

/*
 * @convert typing to string, return string on enter key
 */
char* input_string(void);

#ifdef __cplusplus
}
#endif


#endif /* KEYCODE_CONV_KEYCODE_CONV_H_ */
