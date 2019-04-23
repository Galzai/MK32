#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define MODULE_ID "LOLIN 32"
#define GATTS_TAG "MK32 V3.0" // The device's name
#define MAX_BT_DEVICENAME_LENGTH 40

#define MASTER  // undefine if you are not flashing the main controller
#define SPLIT_MASTER	 // undefine if keyboard is not split and master
//#define SLAVE	 // undefine if keyboard is master

#define DEBOUNCE 5 //debounce time in ms

//Define matrix
#define KEYPADS 2 // intended in order to create a Multiple keypad split boards
#define MATRIX_ROWS 4
#define MATRIX_COLS 6 // For split keyboards, define columns for one side only.

#define LAYERS 2 // number of layers defined

// Select diode direction
#define COL2ROW
//#define ROW2COL

//Encoder definitions
#define R_ENCODER // undefine if no rotary encoder is used
//#define R_ENCODER_SLAVE // undefine if no rotary encoder is used on slave pad
#define ENCODER_A_PIN GPIO_NUM_32 // encoder phase A pin
#define ENCODER_B_PIN GPIO_NUM_33// encoder phase B pin
#define ENCODER_S_PIN GPIO_NUM_27// encoder switch pin

//OLED Parameters
#define OLED_ENABLE //undefine if no oled is used
#define OLED_SDA_PIN GPIO_NUM_23
#define OLED_SCL_PIN GPIO_NUM_22

/*Battery monitoring
 * Please read check battery_monitor.h for resistor values before applying
 * use ADC1 only,  */

#define BATT_STAT //define to enable battery monitoring
#define BATT_PIN ADC1_CHANNEL_7 //gpio pin 35, refer to the esp32 before modifying

//deep sleep parameters, mind that reconnecting after deep sleep might take a minute or two
#define SLEEP_MINS 5 // undefine if you do not need deep sleep, otherwise define number of minutes for deepsleep

/*
 *---------------------------- Everything below here should not be modified for standard usage----------------------
 *
 * */
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var,pos) (var |= 1UL << pos);

#define MAX_LAYER (LAYERS-1)
#define MOD_LED_BYTES 2 //bytes for led status and modifiers
#define MACRO_LEN 3 //keys for macros
#define KEYMAP_COLS MATRIX_COLS*KEYPADS  // used for a symmetrical split keyboard
#define REPORT_LEN (MOD_LED_BYTES+MACRO_LEN+MATRIX_ROWS*KEYMAP_COLS) //size of hid reports with NKRO and room for 3 key macro
#define REPORT_COUNT_BYTES (MATRIX_ROWS*KEYMAP_COLS+MACRO_LEN)

#define LAYER_HOLD_MAX_VAL 0x134
#define LAYER_HOLD_BASE_VAL 0x123
#define MACRO_BASE_VAL 0x103
#define LAYERS_BASE_VAL 0xFF

#define ENCODER_SIZE 4
#define MEDIA_ENCODER 0
#define MOUSE_ENCODER 1
#define KEY_ENCODER 2

typedef struct config_data {
	char bt_device_name[MAX_BT_DEVICENAME_LENGTH];
} config_data_t;

typedef struct joystick_data {
	uint16_t X;
	uint16_t Y;
	uint16_t Z;
	uint16_t Z_rotate;
	uint16_t slider_left;
	uint16_t slider_right;
	uint16_t buttons1;
	uint16_t buttons2;
	int16_t hat;
} joystick_data_t;

extern uint8_t current_layout;
extern uint8_t curr_led;

extern uint16_t default_encoder_map[LAYERS][ENCODER_SIZE];
extern uint16_t default_slave_encoder_map[LAYERS][ENCODER_SIZE];

#define MAX_LAYOUT_NAME_LENGTH 15
// array to hold names of layouts for oled
extern char default_layout_names[LAYERS][MAX_LAYOUT_NAME_LENGTH];

extern TaskHandle_t xKeyreportTask;

#endif
//
