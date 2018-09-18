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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
# include "nvs_funcs.h"
#include <arr_conv.h>


#define NVS_TAG "NVS Storage"
#define NAMESPACE "config"
esp_err_t err;

nvs_handle keymap_handle;
#define LAYOUT_NAMES "layouts"
#define LAYOUT_NUM "num_layouts"

char **layer_names_arr;
uint8_t layers_num=0;

//add or overwrite a keymap to the nvs flash
void nvs_write_layout(uint16_t layout[MATRIX_ROWS][KEYMAP_COLS], const char* layout_name){

	ESP_LOGI(NVS_TAG,"Adding/Modifying Layout");
	uint8_t FOUND_MATCH =0;
	nvs_read_keymap_cfg();
	uint8_t cur_layers_num=layers_num;

	//check if layout exits
	for(uint8_t i=0;i<layers_num;i++){
		if(strcmp(layout_name, layer_names_arr[i])==0){
			FOUND_MATCH =1;
			break;
		}
	}

	//if layout exists
	if(FOUND_MATCH==1){
		ESP_LOGI(NVS_TAG,"Layout name previously listed: %s",layout_name);
	}

	//if layout doesn't exist
	else{
		cur_layers_num++;
		ESP_LOGI(NVS_TAG,"New layout: %s",layout_name);
		char (*curr_array)[MAX_LAYOUT_NAME_LENGTH];
		curr_array = malloc(MAX_LAYOUT_NAME_LENGTH*cur_layers_num*sizeof(char));
		for(uint8_t i=0;i<layers_num;i++){
//			curr_array[i] = malloc(MAX_LAYOUT_NAME_LENGTH*sizeof(char));
			strcpy(curr_array[i],layer_names_arr[i]);
		}
		strcpy(curr_array[layers_num],layout_name);
		nvs_write_keymap_cfg(cur_layers_num,curr_array);
	}
	free(layer_names_arr);
}
//read the what layouts are in the nvs flash
void nvs_read_keymap_cfg(void){
	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(NAMESPACE, NVS_READWRITE, &keymap_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}

	uint8_t layers=0;
	err =nvs_get_u8(keymap_handle, LAYOUT_NUM, &layers);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting layout num: %s", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting layout num");
	}
	//set string array size
	size_t str_size;
	//get string array size
	err = nvs_get_str(keymap_handle, LAYOUT_NAMES, NULL, &str_size);
	if (err != ESP_OK) {
		ESP_LOGI(NVS_TAG, "Error getting layout names size: %s", esp_err_to_name(err));
	}
	char *layer_names = (char *)malloc(str_size);
	err = nvs_get_str(keymap_handle,LAYOUT_NAMES, layer_names, &str_size);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting layout name: %s", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting layout name");
		layers_num=layers;
		str_to_str_arr(layer_names,layers,&layer_names_arr);

		for(uint8_t i=0;i<layers_num;i++){
			ESP_LOGI(NVS_TAG,"\nLayer %d:  %s",i,layer_names_arr[i]);
		}
	}


}

//write layout names to nvs flash
void nvs_write_keymap_cfg(uint8_t layers, char (*layer_names_arr)[MAX_LAYOUT_NAME_LENGTH]){

	char *layer_names;
	str_arr_to_str(layer_names_arr,layers,&layer_names);

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(NAMESPACE, NVS_READWRITE, &keymap_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successful");
	}

	err =nvs_set_u8(keymap_handle, LAYOUT_NUM, layers);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting layout num: %s", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting layout num");
	}

	err = nvs_set_str(keymap_handle,LAYOUT_NAMES, layer_names);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting layout names: %s", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting layout names");
	}
	nvs_commit(keymap_handle);
	nvs_close(keymap_handle);
	free(layer_names);
}
