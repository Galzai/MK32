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

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "keyboard_config.h"
#include <arr_conv.h>

//convert blob to keymap matrix
void blob_to_key_mat(uint16_t layout_arr[MATRIX_ROWS*KEYMAP_COLS],uint16_t buffer[MATRIX_ROWS][KEYMAP_COLS]){

	for(uint8_t row=0; row<MATRIX_ROWS;row++){
		for(uint8_t col=0; col<KEYMAP_COLS;col++){
			buffer[row][col]=layout_arr[row*KEYMAP_COLS+col];
		}
	}
}

//convert keymap matrix to blob
void key_mat_to_blob(uint16_t layout[MATRIX_ROWS][KEYMAP_COLS],uint16_t *buffer){

	uint16_t layout_arr[MATRIX_ROWS*KEYMAP_COLS] = {0};
	for(uint8_t row=0; row<MATRIX_ROWS;row++){
		for(uint8_t col=0; col<KEYMAP_COLS;col++){
			layout_arr[row*KEYMAP_COLS+col]=layout[row][col];
		}

	}
	memcpy(buffer,layout_arr,sizeof(layout_arr));
}

//convert string array to a single string
void str_arr_to_str(char (*layer_names)[MAX_LAYOUT_NAME_LENGTH], uint8_t layers, char **buffer){

	char str[(layers+(layers*MAX_LAYOUT_NAME_LENGTH))*sizeof(char)];
	memset( str, 0, (layers+(layers*MAX_LAYOUT_NAME_LENGTH))*sizeof(char) );
	for (uint8_t i=0;i<layers;i++){
		strcat(str,layer_names[i]);
		strcat(str,",");
	}
	*buffer = malloc(sizeof(str));
	strcpy(*buffer,str);
}

//convert string to string array
void str_to_str_arr(char *str, uint8_t layers,char ***buffer){

	char (*layer_names)[MAX_LAYOUT_NAME_LENGTH] =malloc(layers*MAX_LAYOUT_NAME_LENGTH*sizeof(char));
	char *to= (char*) malloc(MAX_LAYOUT_NAME_LENGTH);
	strncpy(to, str, MAX_LAYOUT_NAME_LENGTH);

	const char s[2]=",";
	char *token;
	//	   get the first token
	token = strtok(str, s);
	//	   walk through other tokens and copy them to array
	uint8_t i=0;
	while( token != NULL ) {
		strcpy(layer_names[i],token);
		token = strtok(NULL, s);
		i++;
	}
	*buffer = malloc(sizeof(layer_names));
	for(uint8_t i =0; i<layers;i++){
		(*buffer)[i] = malloc(sizeof(layer_names[i]));
		strcpy((*buffer)[i],layer_names[i]);
	}

}
