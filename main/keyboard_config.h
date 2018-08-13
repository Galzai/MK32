#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "driver/gpio.h"


#define MODULE_ID "LOLIN 32"
#define GATTS_TAG "MK32 TESTER" // The device's name
#define MAX_BT_DEVICENAME_LENGTH 40

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

//Defines for split communication, to be moved to header file later.
//#define MASTER  // undefine if you are not flashing the main controller
//#define SPLIT_MASTER	 // undefine if keyboard is not split and master
#define SLAVE	 // undefine if keyboard is master

//Define matrix
#define KEYPADS 2 // intended in order to create a Multiple keypad split boards
#define MATRIX_ROWS 4
#define MATRIX_COLS 6 // For split keyboards, define columns for one side only.
#define KEYMAP_COLS MATRIX_COLS*KEYPADS  // use this for a split keyboard, multiply MATRIX_COLS by number of keypads.

//#define KEYMAP_COLS MATRIX_COLS   // use this for a regular keyboard
#define LAYERS 2 // number of layers defined (besides default)


// Select diode direction
#define COL2ROW
//#define ROW2COL

#endif
//
