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

#ifndef KEYPRESS_HANDLES_C
#define KEYPRESS_HANDLES_C
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "keymap.c"
#include "matrix.c"

#define KEY_PRESS_TAG "KEY_PRESS"

/*
 * Current state of the keymap,each cell will hold the location of the key in the key report,
 *if a key is not in the report it will be set to 0
*/
uint8_t KEY_STATE[MATRIX_ROWS][KEYMAP_COLS]={0};
uint16_t led_status=0;
uint16_t modifier=0;
uint16_t keycode=0;
// Sizing the report for N-key rollover
uint8_t current_report[2+MATRIX_ROWS*KEYMAP_COLS] = {0};
uint8_t SLAVE_MATRIX_STATE[MATRIX_ROWS][MATRIX_COLS]={0};
uint8_t index_holder[5] = {0};

// find the correct definition for a pressed key
//uint16_t find_key(uint16_t keymap[MATRIX_ROWS][KEYMAP_COLS]){
//
//		static uint8_t key[2];
//		// check which key was pressed
//		memcpy( key, scan_matrix(), sizeof key);
//		// return the correct definition for a key.
//		if((key[0]==MATRIX_ROWS)&&(key[1]==KEYMAP_COLS)){
//			return 0;
//		}
//		return keymap[key[0]][key[1]];
//
//
//}

// checking if a modifier key was pressed
uint16_t check_modifier( uint16_t key ){

	// these are the modifier keys
	if(( 0xE0 <= key )&&( key <= 0xE7 )){
		uint16_t modifier = key-0xE7;
		return modifier;
	}
	return 0;

}

// checking if a led status key was pressed, returning current led status
uint16_t check_led_status( uint16_t key ){

	switch(key){
		case KC_NLCK:
			return 1;
			break;

		case KC_CAPS:
			return 2;
			break;

		case KC_SLCK:
			return 3;
			break;

	}
	return 0;
}

// adjust current layer
void layer_adjust( uint16_t keycode ){

	switch(keycode){
		case DEFAULT:
			current_layout=0;
			break;

		case LOWER:
			if(current_layout==0){
				current_layout=LAYERS;
				break;
			}
			current_layout--;
			break;

		case RAISE:
			if(current_layout==LAYERS){
				current_layout=0;
				break;
			}
			current_layout++;
			break;
	}
	vTaskDelay(15);
	ESP_LOGI(KEY_PRESS_TAG,"Layer modified!, Current layer: %d ",current_layout);
}


// checking the state of each key in the matrix
uint8_t *check_key_state( uint16_t keymap[MATRIX_ROWS][KEYMAP_COLS] ){

	scan_matrix();
	for(uint8_t col=0; col < MATRIX_COLS; col++){
		for(uint8_t row=0; row <MATRIX_ROWS; row++){
			uint16_t report_index=2+col+row*KEYMAP_COLS;
			keycode=keymap[row][col];
			led_status=check_led_status(keycode);

			if(MATRIX_STATE[row][col]==1){

				if(keycode>0xFF){
					layer_adjust(keycode);
					continue;
				}

				if(current_report[report_index]==0){
					modifier|=check_modifier(keycode);
					current_report[report_index]=keycode;
				}
			}
			if(MATRIX_STATE[row][col]==0){
				if(current_report[report_index]!=0){
					if(led_status!=0){
						led_status=0;
					}
						modifier&=check_modifier(keycode);
						current_report[KEY_STATE[row][col]]=0;
						current_report[report_index]=0;


				}
			}
		}
	}


#ifdef SPLIT_MASTER

	for(uint8_t col=MATRIX_COLS; col < KEYMAP_COLS; col++){
		for(uint8_t row=0; row <MATRIX_ROWS; row++){
			uint16_t report_index=2+col+row*KEYMAP_COLS;
			keycode=keymap[row][col];
			led_status=check_led_status(keycode);

			if(SLAVE_MATRIX_STATE[row][col-MATRIX_COLS]==1){

				if(keycode>0xFF){
					printf("\nkeycode:%d",keycode);
					layer_adjust(keycode);
					continue;
				}

				if(current_report[report_index]==0){
					modifier|=check_modifier(keycode);
					current_report[report_index]=keycode;
				}
			}

			if(SLAVE_MATRIX_STATE[row][col-MATRIX_COLS]==0){
				if(current_report[report_index]!=0){
					if(led_status!=0){
						led_status=0;
					}
						modifier&=check_modifier(keycode);
						current_report[KEY_STATE[row][col]]=0;
						current_report[report_index]=0;
				}
			}
		}
	}
	current_report[0]=led_status;
	current_report[1]=modifier;
#endif
	return current_report;

}



#endif
