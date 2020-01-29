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
#include "matrix.h"
#include "hal_ble.h"
#include "oled_tasks.h"
#include "nvs_keymaps.h"
#include "plugin_manager.h"

#define KEY_PRESS_TAG "KEY_PRESS"

/*
 * Current state of the keymap,each cell will hold the location of the key in the key report,
 *if a key is not in the report it will be set to 0
 */

uint8_t KEY_STATE[MATRIX_ROWS][KEYMAP_COLS] = { 0 };
uint16_t led_status = 0;
uint8_t modifier = 0;
uint16_t keycode = 0;

// Sizing the report for N-key rollover
uint8_t current_report[REPORT_LEN] = { 0 };

// Array to send when releasing macros
uint8_t macro_release[3] = { 0 };

// Flag in order to know when to ignore layer change on layer hold
uint8_t layer_hold_flag = 0;
uint8_t prev_layout = 0;

// checking if a modifier key was pressed
uint16_t check_modifier(uint16_t key) {

	uint8_t cur_mod = 0;
	// these are the modifier keys
	if ((KC_LCTRL <= key) && (key <= KC_RGUI)) {
		cur_mod = (1 << (key - KC_LCTRL));
		return cur_mod;
	}
	return 0;

}

// checking if a led status key was pressed, returning current led status
uint16_t check_led_status(uint16_t key) {

	switch (key) {
	case KC_NLCK:
		return 1;

	case KC_CAPS:
		return 2;

	case KC_SLCK:
		return 3;

	}
	return 0;
}

// what to do on a media key press
void media_control_send(uint16_t keycode) {

	uint8_t media_state[2] = { 0 };
	if (keycode == KC_MEDIA_NEXT_TRACK) {
		media_state[1] = 10;
	}
	if (keycode == KC_MEDIA_PREV_TRACK) {
		media_state[1] = 111;
	}
	if (keycode == KC_MEDIA_STOP) {
		media_state[1] = 12;
	}
	if (keycode == KC_MEDIA_PLAY_PAUSE) {
		media_state[1] = 5;
	}
	if (keycode == KC_AUDIO_MUTE) {
		media_state[1] = 1;
	}
	if (keycode == KC_AUDIO_VOL_UP) {
		SET_BIT(media_state[0], 6);
	}
	if (keycode == KC_AUDIO_VOL_DOWN) {
		SET_BIT(media_state[0], 7);
	}

	xQueueSend(media_q, (void*) &media_state, (TickType_t) 0);
}

void media_control_release(uint16_t keycode) {
	uint8_t media_state[2] = { 0 };
	xQueueSend(media_q, (void*) &media_state, (TickType_t) 0);
}

//used for debouncing
static uint32_t millis() {
	return esp_timer_get_time() / 1000;
}

uint32_t prev_time = 0;
// adjust current layer
void layer_adjust(uint16_t keycode) {
	uint32_t cur_time = millis();
	if (cur_time - prev_time > DEBOUNCE) {
		if (layer_hold_flag == 0) {
			switch (keycode) {
			case DEFAULT:
				current_layout = 0;
				break;

			case LOWER:
				if (current_layout == 0) {
					current_layout = MAX_LAYER;
					break;
				}
				current_layout--;
				break;

			case RAISE:
				if (current_layout == MAX_LAYER) {
					current_layout = 0;
					break;
				}
				current_layout++;
				break;
			}
#ifdef OLED_ENABLE
			xQueueSend(layer_recieve_q, &current_layout, (TickType_t) 0);
#endif
			ESP_LOGI(KEY_PRESS_TAG, "Layer modified!, Current layer: %d ",
					current_layout);
		}
	}
	prev_time = cur_time;
}

// checking the state of each key in the matrix
uint8_t *check_key_state(uint16_t **keymap) {

	scan_matrix();
	for (uint8_t pad = 0; pad < KEYPADS; pad++) {

		uint8_t matrix_state[MATRIX_ROWS][MATRIX_COLS] = { 0 };
		memcpy(matrix_state, matrix_states[pad], sizeof(matrix_state));

		for (uint8_t col = (MATRIX_COLS * pad);
				col < ((pad + 1) * (MATRIX_COLS)); col++) {
			for (uint8_t row = 0; row < MATRIX_ROWS; row++) {

				uint16_t report_index = (2 + col + row * KEYMAP_COLS);
				keycode = keymap[row][col];

				//checking if the keycode is transparent
				if (keycode == KC_TRNS) {
					if (current_layout == 0) {
						keycode = *default_layouts[MAX_LAYER][row][col];
					} else {
						keycode =
								*default_layouts[current_layout - 1][row][col];
					}
				}

				led_status = check_led_status(keycode);
				if (matrix_state[row][col - MATRIX_COLS * pad] == 1) {


					//checking for function
					if (keycode >= PLUGIN_BASE_VAL){
						plugin_launcher(keycode);
						continue;
					}
					//checking for layer hold
					if ((keycode >= LAYER_HOLD_BASE_VAL)
							&& (keycode <= LAYER_HOLD_MAX_VAL)) {
						if (layer_hold_flag == 0) {
							prev_layout = current_layout;
							current_layout = (keycode - LAYER_HOLD_BASE_VAL);
							layer_hold_flag = 1;
#ifdef OLED_ENABLE
						xQueueSend(layer_recieve_q, &current_layout,
								(TickType_t) 0);
#endif
						ESP_LOGI(KEY_PRESS_TAG,
								"Layer modified!, Current layer: %d ",
								current_layout);
						}

						continue;

					}

					// checking for layer adjust keycodes
					if ((keycode >= LAYERS_BASE_VAL)
							&& (keycode < MACRO_BASE_VAL)) {
						layer_adjust(keycode);
						continue;
					}

					// checking for macros
					if ((keycode >= MACRO_BASE_VAL)
							&& (keycode <= LAYER_HOLD_BASE_VAL)) {
						for (uint8_t i = 0; i < 3; i++) {
							uint16_t key = macros[MACRO_BASE_VAL - keycode][i];
							current_report[REPORT_LEN - 1 - i] = key;
							modifier |= check_modifier(key);
							printf("\nmodifier:%d", modifier);
						}
						continue;
					}

					// checking for media control keycodes
					if ((keycode >= KC_MEDIA_NEXT_TRACK)
							&& (keycode <= KC_AUDIO_VOL_DOWN)) {
						media_control_send(keycode);
					}

					// checking for system control keycodes
					//				if((keycode>=0XA8)&&(keycode<=0XA7)){
					//					system_control(keycode);
					//					continue;
					//				}

					if (current_report[report_index] == 0) {
						modifier |= check_modifier(keycode);
						current_report[report_index] = keycode;

					}
				}
				if (matrix_state[row][col - MATRIX_COLS * pad] == 0) {

					//checking for layer hold release
					if ((layouts[prev_layout][row][col] >= LAYER_HOLD_BASE_VAL)
							&& (keycode <= LAYER_HOLD_MAX_VAL)
							&& (layer_hold_flag == 1)) {
						current_layout = 0;
						layer_hold_flag = 0;
#ifdef OLED_ENABLE
						xQueueSend(layer_recieve_q, &current_layout,
								(TickType_t) 0);
#endif
						ESP_LOGI(KEY_PRESS_TAG,
								"Layer modified!, Current layer: %d ",
								current_layout);
					}

					//checking if macro was released
					if ((keycode >= MACRO_BASE_VAL)
							&& (keycode <= LAYER_HOLD_BASE_VAL)) {
						for (uint8_t i = 0; i < 3; i++) {
							uint16_t key = macros[MACRO_BASE_VAL - keycode][i];
							current_report[REPORT_LEN - 1 - i] = 0;
							modifier &= ~check_modifier(key);
						}
					}

					if (current_report[report_index] != 0) {
						if (led_status != 0) {
							led_status = 0;
						}

						modifier &= ~check_modifier(keycode);
						current_report[KEY_STATE[row][col]] = 0;
						current_report[report_index] = 0;

						// checking for media control keycodes
						if ((keycode >= KC_MEDIA_NEXT_TRACK)
								&& (keycode <= KC_AUDIO_VOL_DOWN)) {
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
	}
	current_report[0] = modifier;
	current_report[1] = led_status;
	return current_report;

}

#endif
