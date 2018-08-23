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
#include "HID_kbdmousejoystick.h"

#define KEY_PRESS_TAG "KEY_PRESS"

/*
 * Current state of the keymap,each cell will hold the location of the key in the key report,
 *if a key is not in the report it will be set to 0
*/

uint8_t KEY_STATE[MATRIX_ROWS][KEYMAP_COLS]={0};
uint16_t led_status=0;
uint8_t modifier=0;
uint16_t keycode=0;

// Sizing the report for N-key rollover
uint8_t current_report[REPORT_LEN] = {0};
uint8_t SLAVE_MATRIX_STATE[MATRIX_ROWS][MATRIX_COLS]={0};

// Array to send when releasing macros
uint8_t macro_release[3] = {0};

// checking if a modifier key was pressed
uint16_t check_modifier( uint16_t key ){

    uint8_t cur_mod = 0;
	// these are the modifier keys
	if(( 0xE0 <= key )&&( key <= 0xE7 )){
		cur_mod = (1 << (key-0xE0));
		return cur_mod;
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

// what to do on a media key press
void media_control_send(uint16_t keycode ){

uint8_t media_state[1]={0};

	switch(keycode){

	case  KC_MEDIA_NEXT_TRACK:
		media_state[0]=0x01;
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		vTaskDelay(3);
		break;

	case KC_MEDIA_PREV_TRACK:
		media_state[0]=0x02;
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		vTaskDelay(3);
		break;

	case KC_MEDIA_STOP:
		media_state[0]=0x04;
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		vTaskDelay(3);
		break;

	case KC_MEDIA_PLAY_PAUSE:
		media_state[0]=0x08;
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		vTaskDelay(3);
		break;

	case KC_AUDIO_MUTE:
		media_state[0]=0x10;
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		vTaskDelay(3);
		break;

	case KC_AUDIO_VOL_UP:
		media_state[0]=0x20;
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		vTaskDelay(3);
		break;

	case KC_AUDIO_VOL_DOWN:
		media_state[0]=0x40;
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		vTaskDelay(3);
		break;
	}
}

void media_control_release(uint16_t keycode ){
uint8_t media_state[1]={0};
xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
vTaskDelay(3);
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

				// checking for layer adjust keycodes
				if((keycode>0xFF)&&(keycode<0x103)){
					layer_adjust(keycode);
					continue;
				}

				// checking for macros
				if(keycode>0x102){
					for(uint8_t i=0; i < 3; i++){
						uint16_t key=macros[0x103-keycode][i];
						current_report[REPORT_LEN-1-i]=key;
						modifier|=check_modifier(key);
						printf("\nmodifier:%d",modifier);
					}
					continue;
				}

				// checking for media control keycodes
				if((keycode>=0xA5)&&(keycode<=0xAE)){
					media_control_send(keycode);
				}

				// checking for system control keycodes
//				if((keycode>=0XA8)&&(keycode<=0XA7)){
//					system_control(keycode);
//					continue;
//				}

				if(current_report[report_index]==0){
					modifier|=check_modifier(keycode);
					current_report[report_index]=keycode;
				}
			}
			if(MATRIX_STATE[row][col]==0){

				//checking if macro was released
				if(keycode>0x102){
					for(uint8_t i=0; i < 3; i++){
					uint16_t key=macros[0x103-keycode][i];
					current_report[REPORT_LEN-1-i]=0;
					modifier&=~check_modifier(key);
					}
				}

				if(current_report[report_index]!=0){
					if(led_status!=0){
						led_status=0;
					}

						modifier&=~check_modifier(keycode);
						current_report[KEY_STATE[row][col]]=0;
						current_report[report_index]=0;

						// checking for media control keycodes
						if((keycode>=0xA5)&&(keycode<=0xAE)){
							media_control_release(keycode);
						}


				}

						// checking for system control keycodes
		//				if((keycode>=0XA8)&&(keycode<=0XA7)){
		//					system_control_release(keycode);
		//					continue;
		//				}


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

				//checking for layer adjust
				if(keycode>0xFF){
					printf("\nkeycode:%d",keycode);
					layer_adjust(keycode);
					continue;
				}

				// checking for macros
				if(keycode>0x102){
					for(uint8_t i=0; i < 3; i++){
						uint16_t key=macros[0x103-keycode][i];
						current_report[REPORT_LEN-1-i]=key;
						modifier|=check_modifier(key);
						printf("\nmodifier:%d",modifier);
					}
					continue;
				}

				// checking for media control keycodes
				if((keycode>=0xA5)&&(keycode<=0xAE)){
					media_control_send(keycode);
					continue;
				}

				// checking for system control keycodes
//				if((keycode>=0XA8)&&(keycode<=0XA7)){
//					system_control(keycode);
//					continue;
//				}


				if(current_report[report_index]==0){
					modifier|=check_modifier(keycode);
					current_report[report_index]=keycode;
				}
			}

			if(SLAVE_MATRIX_STATE[row][col-MATRIX_COLS]==0){

				//checking if macro was released
				if(keycode>0x102){
					for(uint8_t i=0; i < 3; i++){
					uint16_t key=macros[0x103-keycode][i];
					current_report[REPORT_LEN-1-i]=0;
					modifier&=~check_modifier(key);
					}
				}

				if(current_report[report_index]!=0){
					if(led_status!=0){
						led_status=0;
					}

					// checking for media control keycodes
					if((keycode>=0xA5)&&(keycode<=0xAE)){
						media_control_release(keycode);
					}


			}

					// checking for system control keycodes
	//				if((keycode>=0XA8)&&(keycode<=0XA7)){
	//					system_control_release(keycode);
	//					continue;
	//				}


						modifier&=~check_modifier(keycode);
						current_report[KEY_STATE[row][col]]=0;
						current_report[report_index]=0;
				}
			}
		}
	}

#endif
	current_report[0]=modifier;
	current_report[1]=led_status;
	return current_report;

}



#endif
