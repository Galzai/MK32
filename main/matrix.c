/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Copyright 2018 Gal Zaidenstein.
 */

#ifndef MATRIX_C
#define MATRIX_C

//GPIO libraries
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "keyboard_config.h"
#include "esp_sleep.h"

/* Define pins, notice that:
 * GPIO6-11 are usually used for SPI flash
 * GPIO34-39 can only be set as input mode and do not have software pullup or pulldown functions.
 * GPIOS 0,2,4,12-15,25-27,32-39 Can be used as RTC GPIOS as well (please read about power management in ReadMe)
 */
const gpio_num_t MATRIX_ROWS_PINS[]={GPIO_NUM_0,GPIO_NUM_4,GPIO_NUM_5,GPIO_NUM_12};
const gpio_num_t MATRIX_COLS_PINS[]={GPIO_NUM_13,GPIO_NUM_14,GPIO_NUM_15,GPIO_NUM_16,GPIO_NUM_17,GPIO_NUM_18};

// matrix states
uint8_t MATRIX_STATE[MATRIX_ROWS][MATRIX_COLS]={0};
uint8_t SLAVE_MATRIX_STATE[MATRIX_ROWS][MATRIX_COLS]={0};

uint8_t (*matrix_states[])[MATRIX_ROWS][MATRIX_COLS]={
		 &MATRIX_STATE,
		 &SLAVE_MATRIX_STATE,
};

// deinitializing rtc matrix pins on  deep sleep wake up
void rtc_matrix_deinit(void){

	// Deinitializing columns
	for(uint8_t col=0; col < MATRIX_COLS; col++){

		if(rtc_gpio_is_valid_gpio(MATRIX_COLS_PINS[col])==1){
			rtc_gpio_set_level(MATRIX_COLS_PINS[col], 0);
			rtc_gpio_set_direction(MATRIX_COLS_PINS[col], RTC_GPIO_MODE_DISABLED);
			gpio_reset_pin(MATRIX_COLS_PINS[col]);
		}
	}

	// Deinitializing rows
	for(uint8_t row=0; row < MATRIX_ROWS; row++){

			if(rtc_gpio_is_valid_gpio(MATRIX_ROWS_PINS[row])==1){
				rtc_gpio_set_level(MATRIX_ROWS_PINS[row], 0);
				rtc_gpio_set_direction(MATRIX_ROWS_PINS[row], RTC_GPIO_MODE_DISABLED);
				gpio_reset_pin(MATRIX_ROWS_PINS[row]);
		}
	}
}

// Initializing rtc matrix pins for deep sleep wake up
void rtc_matrix_setup(void){
uint64_t rtc_mask = 0;

	// Initializing columns
	for(uint8_t col=0; col < MATRIX_COLS; col++){

		if(rtc_gpio_is_valid_gpio(MATRIX_COLS_PINS[col])==1){
			rtc_gpio_init((MATRIX_COLS_PINS[col]));
			rtc_gpio_set_direction(MATRIX_COLS_PINS[col], RTC_GPIO_MODE_INPUT_OUTPUT);
			rtc_gpio_set_level(MATRIX_COLS_PINS[col], 1);
			printf("\n%d is level %d",MATRIX_COLS_PINS[col],gpio_get_level(MATRIX_COLS_PINS[col]));
		}
	}

	// Initializing rows
	for(uint8_t row=0; row < MATRIX_ROWS; row++){

			if(rtc_gpio_is_valid_gpio(MATRIX_ROWS_PINS[row])==1){
				rtc_gpio_init((MATRIX_ROWS_PINS[row]));
				rtc_gpio_set_direction(MATRIX_ROWS_PINS[row], RTC_GPIO_MODE_INPUT_OUTPUT);
				rtc_gpio_set_drive_capability(MATRIX_ROWS_PINS[row],GPIO_DRIVE_CAP_0);
				rtc_gpio_set_level(MATRIX_ROWS_PINS[row], 0);
				rtc_gpio_wakeup_enable(MATRIX_ROWS_PINS[row],GPIO_INTR_HIGH_LEVEL);
				SET_BIT(rtc_mask,MATRIX_ROWS_PINS[row]);

		printf("\n%d is level %d",MATRIX_ROWS_PINS[row],gpio_get_level(MATRIX_ROWS_PINS[row]));
		}
			esp_sleep_enable_ext1_wakeup(rtc_mask,ESP_EXT1_WAKEUP_ANY_HIGH);
	}
}

// Initializing matrix pins
void matrix_setup(void){

#ifdef COL2ROW
	// Initializing columns
	for(uint8_t col=0; col < MATRIX_COLS; col++){

		gpio_pad_select_gpio(MATRIX_COLS_PINS[col]);
		gpio_set_direction(MATRIX_COLS_PINS[col], GPIO_MODE_INPUT_OUTPUT);
		gpio_set_level(MATRIX_COLS_PINS[col], 0);
		printf("\n%d is level %d",MATRIX_COLS_PINS[col],gpio_get_level(MATRIX_COLS_PINS[col]));
	}

	// Initializing rows
	for(uint8_t row=0; row < MATRIX_ROWS; row++){

		gpio_pad_select_gpio(MATRIX_ROWS_PINS[row]);
		gpio_set_direction(MATRIX_ROWS_PINS[row], GPIO_MODE_INPUT_OUTPUT);
		gpio_set_drive_capability(MATRIX_ROWS_PINS[row],GPIO_DRIVE_CAP_0);
		gpio_set_level(MATRIX_ROWS_PINS[row], 0);

		printf("\n%d is level %d",MATRIX_ROWS_PINS[row],gpio_get_level(MATRIX_ROWS_PINS[row]));
	}
#endif
#ifdef ROW2COL
	// Initializing rows
	for(uint8_t row=0; row < MATRIX_ROWS; row++){

		gpio_pad_select_gpio(MATRIX_ROWS_PINS[row]);
		gpio_set_direction(MATRIX_ROWS_PINS[row], GPIO_MODE_INPUT_OUTPUT);
		gpio_set_level(MATRIX_ROWS_PINS[row], 0);
		printf("\n%d is level %d",MATRIX_ROWS_PINS[row],gpio_get_level(MATRIX_ROWS_PINS[row]));
	}

	// Initializing columns
	for(uint8_t col=0; col < MATRIX_ROWS; col++){

		gpio_pad_select_gpio(MATRIX_COLS_PINS[col]);
		gpio_set_direction(MATRIX_COLS_PINS[col], GPIO_MODE_INPUT_OUTPUT);
		gpio_set_drive_capability(MATRIX_COLS_PINS[col],GPIO_DRIVE_CAP_0);
		gpio_set_level(MATRIX_COLS_PINS[col], 0);

		printf("\n%d is level %d",MATRIX_COLS_PINS[col],gpio_get_level(MATRIX_COLS_PINS[col]));
	}
#endif
}


// Scanning the matrix for input
void scan_matrix(void){
#ifdef COL2ROW
	// Setting column pin as low, and checking if the input of a row pin changes.
	for(uint8_t col=0; col < MATRIX_COLS; col++){
		gpio_set_level(MATRIX_COLS_PINS[col], 1);
		for(uint8_t row=0; row <MATRIX_ROWS; row++){

			MATRIX_STATE[row][col]=gpio_get_level(MATRIX_ROWS_PINS[row]) ;
		}
		gpio_set_level(MATRIX_COLS_PINS[col], 0);


	}


#endif
#ifdef ROW2COL
	// Setting row pin as low, and checking if the input of a column pin changes.
	for(uint8_t row=0; row < MATRIX_ROWS; row++){
		gpio_set_level(MATRIX_ROWS_PINS[row], 0);

		for(uint8_t col=0; col <MATRIX_COLS; col++){
			MATRIX_STATE[row][col]=gpio_get_level(MATRIX_COLS_PINS[col]) ;
		}
		gpio_set_level(MATRIX_ROWS_PINS[row], 0);
	}
#endif


}

#endif
