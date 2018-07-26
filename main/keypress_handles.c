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

#include "keymap.c"
#include "matrix.c"


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
uint8_t corrected_report[7] = {0};
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


// checking the state of each key in the matrix
uint8_t *check_key_state( uint16_t keymap[MATRIX_ROWS][KEYMAP_COLS] ){

	scan_matrix();
	for(uint8_t col=0; col < MATRIX_COLS; col++){
		for(uint8_t row=0; row <MATRIX_ROWS; row++){
			uint16_t report_index=2+col+row*KEYMAP_COLS;
			keycode=keymap[row][col];
			led_status=check_led_status(keycode);

			if(MATRIX_STATE[row][col]==1){
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
//	for(int i=0; i<10;i++){
//	printf("\n%d",current_report[i]);
//	}
#endif
	return current_report;

}

// resizing current report status for proper buffer size (8 Bit until i figure out how to increase the buffer)
uint8_t *check_key_state_corrected( uint16_t keymap[MATRIX_ROWS][KEYMAP_COLS] ){

	check_key_state(keymap);
	// Adding modifiers and led status
	memcpy(corrected_report,current_report,2);

	// Adding keys to the report
	for(uint8_t i=2; i< sizeof(corrected_report); i++){
		if(corrected_report[i]== 0){
			for(uint8_t j=2; j<2+MATRIX_ROWS*KEYMAP_COLS;j++)
			{
				// Checking if key is still pressed (to not add it twice)
				for(uint8_t y=2; y< sizeof(corrected_report); y++){
					if((current_report[j]==corrected_report[y])&&(current_report[j]!= 0)){
						break;
					}
					// If the key has been recently pressed add it to the report
					else{
					corrected_report[i]=current_report[j];
					index_holder[i-2]=j;
							break;
					}
				}

			}
		}
		// releasing keys from the report
		if(corrected_report[i]!= 0){
				if(current_report[index_holder[i-2]]!=corrected_report[i]){
					corrected_report[i]=0;

			}
		}

	}
	// releasing keys from the report





return corrected_report;
}



#endif
