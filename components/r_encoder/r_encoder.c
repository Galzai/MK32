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

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "key_definitions.h"
#include "keyboard_config.h"
#include "key_definitions.h"
#include "driver/pcnt.h"
#include "hal_ble.h"

int PastEncoderCount=0;

//How to process encoder activity
void r_encoder_command(uint8_t command, uint16_t encoder_commands[4]){
	uint8_t type = encoder_commands[0];
	uint8_t media_state[2] = {0};
	uint8_t mouse_state[5] = {0};
	uint8_t key_state[REPORT_LEN] = {0};

	switch(type){
	case MEDIA_ENCODER:
		if(command>=4){
			command=3;
		}
		if(encoder_commands[command] ==  KC_MEDIA_NEXT_TRACK){
			media_state[1] = 10;
		}
		if(encoder_commands[command] == KC_MEDIA_PREV_TRACK  ){
			media_state[1] = 111;
		}
		if(encoder_commands[command] == KC_MEDIA_STOP){
			media_state[1] = 12;
		}
		if(encoder_commands[command] == KC_MEDIA_PLAY_PAUSE){
			media_state[1] = 5;
		}
		if(encoder_commands[command] == KC_AUDIO_MUTE){
			media_state[1] = 1;
		}
		if(encoder_commands[command] == KC_AUDIO_VOL_UP){
			SET_BIT(media_state[0],6);
		}
		if(encoder_commands[command] == KC_AUDIO_VOL_DOWN){
			SET_BIT(media_state[0],7);
		}
		xQueueSend(media_q,(void*)&media_state, (TickType_t) 0);
		break;

	case MOUSE_ENCODER:
		mouse_state[0] = 0;
		mouse_state[1] = 0;
		mouse_state[2] = 0;
		mouse_state[3] = 0;
		for(uint8_t i = 0;i < 3; i++){
			if((CHECK_BIT(command,i)) != 0){
				switch(encoder_commands[i + 1]){
				case KC_MS_UP :
					mouse_state[2] = 15;
					break;

				case KC_MS_DOWN:
					mouse_state[2] = -15;
					break;

				case KC_MS_LEFT:
					mouse_state[1] = -15;
					break;

				case KC_MS_RIGHT:
					mouse_state[1] = 15;
					break;

				case KC_MS_BTN1:
					mouse_state[0] = 1;
					break;

				case KC_MS_BTN2:
					mouse_state[0] = 2;
					break;

				case KC_MS_WH_UP:
					mouse_state[3] = 1;
					break;
				case KC_MS_WH_DOWN:
					mouse_state[3] = -1;
					break;
				}
			}
		}
		xQueueSend(mouse_q,(void*)&mouse_state, (TickType_t) 0);
		break;

	case KEY_ENCODER:
		if(command >= 4){
			command = 3;
		}
		key_state[2] = encoder_commands[command];
		xQueueSend(keyboard_q,(void*)&key_state, (TickType_t) 0);
		break;
	}
	vTaskDelay(5 / portTICK_PERIOD_MS);
}
//Setting Pulse counter and encoder button
void r_encoder_setup(void){

	pcnt_config_t pcnt_config_a = {
			// Set PCNT input signal and control GPIOs
			.pulse_gpio_num = ENCODER_A_PIN,
			.ctrl_gpio_num = ENCODER_B_PIN,

			.unit = PCNT_UNIT_0,
			.channel = PCNT_CHANNEL_0,

			// What to do on the positive / negative edge of pulse input?
			.pos_mode = PCNT_COUNT_DIS,   // Count up on the positive edge
			.neg_mode = PCNT_COUNT_INC,   // Keep the counter value on the negative edge
			// What to do when control input is low or high?
			.lctrl_mode = PCNT_MODE_KEEP, // Reverse counting direction if low
			.hctrl_mode = PCNT_MODE_REVERSE,    // Keep the primary counter mode if high
			// Set the maximum and minimum limit values to watch
			.counter_h_lim = INT16_MAX,
			.counter_l_lim = INT16_MIN,
	};


	pcnt_unit_config(&pcnt_config_a);

	pcnt_set_filter_value(PCNT_UNIT_0, 1023);  // Filter Runt Pulses
	pcnt_filter_enable(PCNT_UNIT_0);

	gpio_set_direction(ENCODER_A_PIN,GPIO_MODE_INPUT);
	gpio_set_pull_mode(ENCODER_A_PIN,GPIO_PULLUP_ONLY);
	gpio_set_direction(ENCODER_B_PIN,GPIO_MODE_INPUT);
	gpio_set_pull_mode(ENCODER_B_PIN,GPIO_FLOATING);

	pcnt_counter_pause(PCNT_UNIT_0); // Initial PCNT init
	pcnt_counter_clear(PCNT_UNIT_0);
	pcnt_counter_resume(PCNT_UNIT_0);

	//Encoder Button
#ifdef ENCODER_S_PIN
	gpio_pad_select_gpio(ENCODER_S_PIN);
	gpio_set_direction(ENCODER_S_PIN, GPIO_MODE_INPUT);
	gpio_set_pull_mode(ENCODER_S_PIN,GPIO_PULLDOWN_ONLY);
#endif

}
//Check encoder state, currently defined for Vol +/= and mute
uint8_t r_encoder_state(void){
	uint8_t EncoderState = 0x00;
	int16_t EncoderCount;
	pcnt_get_counter_value(PCNT_UNIT_0, &EncoderCount);
	if(EncoderCount > PastEncoderCount){
		EncoderState = 1;
	}
	if(EncoderCount < PastEncoderCount){
		EncoderState = 2;
	}
#ifdef ENCODER_S_PIN
	if(gpio_get_level(ENCODER_S_PIN)==1){
		EncoderState+= 4;
	}
#endif
	PastEncoderCount = EncoderCount;
	return EncoderState;

}





