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
# include "keymap.h"
#include <arr_conv.h>

#define NVS_TAG "NVS Storage"

#define KEYMAP_NAMESPACE "keymap_config"
#define ENCODER_NAMESPACE "encoder_config"
#define SLAVE_ENCODER_NAMESPACE "slave_encoder_config"

esp_err_t err;

nvs_handle keymap_handle;
nvs_handle encoder_handle;
nvs_handle slave_encoder_handle;

#define LAYOUT_NAMES "layouts"
#define LAYOUT_NUM "num_layouts"

char **layer_names_arr;
uint8_t layers_num=0;
uint16_t ***layouts;
uint16_t **encoder_map;
uint16_t **slave_encoder_map;

//read a layout from nvs
void nvs_read_layout(const char* layout_name,uint16_t buffer[MATRIX_ROWS][KEYMAP_COLS]){
	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	uint16_t layout[MATRIX_ROWS][KEYMAP_COLS] = {0};
	err = nvs_open(KEYMAP_NAMESPACE, NVS_READWRITE, &keymap_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	uint16_t layout_arr[MATRIX_ROWS*KEYMAP_COLS] = {0};
	size_t arr_size;
	//get blob array size
	err = nvs_get_blob(keymap_handle, layout_name, NULL, &arr_size);
	err = nvs_get_blob(keymap_handle,layout_name,layout_arr,&arr_size);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting layout: %s", esp_err_to_name(err));

	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting layout");
		blob_to_key_mat(layout_arr,layout);
	}
	memcpy(buffer, layout, sizeof(layout) );
	ESP_LOGI(NVS_TAG, "Layout copied to buffer");
	nvs_close(keymap_handle);
}

//read encoder layout
void nvs_read_encoder_layout(const char* layout_name,uint16_t buffer[ENCODER_SIZE]){
	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	uint16_t layout[ENCODER_SIZE] = {0};
	err = nvs_open(ENCODER_NAMESPACE, NVS_READWRITE, &encoder_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	size_t arr_size;
	//get blob array size
	err = nvs_get_blob(keymap_handle, layout_name, NULL, &arr_size);
	err = nvs_get_blob(keymap_handle,layout_name,layout,&arr_size);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting layout: %s", esp_err_to_name(err));

	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting layout");
	}
	memcpy(buffer, layout, sizeof(layout) );
	ESP_LOGI(NVS_TAG, "Layout copied to buffer");
	nvs_close(encoder_handle);
}

//add or overwrite an encoder layout to the nvs
void nvs_write_encoder_layout(uint16_t encoder_layout_arr[ENCODER_SIZE], const char* encoder_layout_name){

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(SLAVE_ENCODER_NAMESPACE, NVS_READWRITE, &slave_encoder_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	err = nvs_set_blob(keymap_handle,encoder_layout_name, encoder_layout_arr,sizeof(*encoder_layout_arr));
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error writing layout: %s", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success writing layout");
	}

	nvs_close(slave_encoder_handle);
}

//read slave encoder layout
void nvs_read_slave_encoder_layout(const char* layout_name,uint16_t buffer[ENCODER_SIZE]){
	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	uint16_t layout[ENCODER_SIZE] = {0};
	err = nvs_open(SLAVE_ENCODER_NAMESPACE, NVS_READWRITE, &slave_encoder_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	size_t arr_size;
	//get blob array size
	err = nvs_get_blob(keymap_handle, layout_name, NULL, &arr_size);
	err = nvs_get_blob(keymap_handle,layout_name,layout,&arr_size);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting layout: %s", esp_err_to_name(err));

	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting layout");
	}
	memcpy(buffer, layout, sizeof(layout) );
	ESP_LOGI(NVS_TAG, "Layout copied to buffer");
	nvs_close(slave_encoder_handle);
}

//add or overwrite a slave encoder layout to the nvs
void nvs_write_slave_encoderlayout_(uint16_t encoder_layout_arr[ENCODER_SIZE], const char* encoder_layout_name){

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(ENCODER_NAMESPACE, NVS_READWRITE, &encoder_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	err = nvs_set_blob(keymap_handle,encoder_layout_name, encoder_layout_arr,sizeof(*encoder_layout_arr));
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error writing layout: %s", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success writing layout");
	}

	nvs_close(encoder_handle);
}

//add or overwrite a keymap to the nvs
void nvs_write_layout_matrix(uint16_t layout[MATRIX_ROWS][KEYMAP_COLS], const char* layout_name){

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(KEYMAP_NAMESPACE, NVS_READWRITE, &keymap_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	uint16_t layout_arr[MATRIX_ROWS*KEYMAP_COLS] = {0};
	key_mat_to_blob(layout,layout_arr);

	err = nvs_set_blob(keymap_handle,layout_name, layout_arr,sizeof(layout_arr));
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error writing layout: %s", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success writing layout");
	}

	nvs_close(keymap_handle);
}

//add or overwrite a keymap to the nvs
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
			strcpy(curr_array[i],layer_names_arr[i]);
		}
		strcpy(curr_array[layers_num],layout_name);
		nvs_write_keymap_cfg(cur_layers_num,curr_array);

	}
	nvs_write_layout_matrix(layout,layout_name);

	free(layer_names_arr);
}

//read the what layouts are in the nvs
void nvs_read_keymap_cfg(void){
	free(layer_names_arr);
	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(KEYMAP_NAMESPACE, NVS_READWRITE, &keymap_handle);
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
	nvs_close(keymap_handle);


}

//write layout names to nvs
void nvs_write_keymap_cfg(uint8_t layers, char (*layer_names_arr)[MAX_LAYOUT_NAME_LENGTH]){

	char *layer_names;
	str_arr_to_str(layer_names_arr,layers,&layer_names);

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(KEYMAP_NAMESPACE, NVS_READWRITE, &keymap_handle);
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

//load the layouts from nvs
void nvs_load_layouts(void){

	ESP_LOGI(NVS_TAG,"Loading layouts");
	nvs_read_keymap_cfg();

	if(layers_num!=0){

		// set keymap layouts
		ESP_LOGI(NVS_TAG,"Layouts found on NVS, loading layouts");
		layouts = malloc(layers_num*sizeof(uint16_t**));
		for(uint8_t i = 0;i < layers_num; i++){
			uint16_t layout_buff[MATRIX_ROWS][KEYMAP_COLS] = {0};
			nvs_read_layout(layer_names_arr[i],layout_buff);
			layouts[i] = malloc(sizeof(layout_buff));
			ESP_LOGI(NVS_TAG,"malloc");
			for(uint8_t row=0; row<MATRIX_ROWS;row++){
				layouts[i][row] = malloc(sizeof(layout_buff[row]));
				for(uint8_t col=0; col<KEYMAP_COLS;col++){
					layouts[i][row][col] = layout_buff[row][col];
				}
			}
		}
	}else{

		ESP_LOGI(NVS_TAG,"Layouts not found on NVS, loading default layouts");
		free(layer_names_arr);
		layouts = malloc(LAYERS*sizeof(uint16_t**));
		layer_names_arr = malloc(sizeof(default_layout_names));
		for(uint8_t i = 0;i < LAYERS; i++){
			layouts[i] = malloc(sizeof( (*default_layouts)[i]));
			layer_names_arr[i] = malloc(sizeof(default_layout_names[i]));
			strcpy(layer_names_arr[i],default_layout_names[i]);
			for(uint8_t row=0; row<MATRIX_ROWS;row++){
				layouts[i][row] = malloc(sizeof( (*default_layouts[i])[row]));
				for(uint8_t col=0; col<KEYMAP_COLS;col++){
					layouts[i][row][col] = (*default_layouts[i])[row][col];
				}
			}
		}
	}

	//set encoder layouts
	if(layers_num!=0){

		ESP_LOGI(NVS_TAG,"Encoder layouts found on NVS, loading layouts");
		encoder_map = malloc(layers_num*sizeof(uint16_t*));
		for(uint8_t i = 0;i < layers_num; i++){
			uint16_t encoder_layout_buff[ENCODER_SIZE] = {0};
			nvs_read_encoder_layout(layer_names_arr[i],encoder_layout_buff);
			encoder_map[i] = malloc(sizeof(encoder_layout_buff));
			ESP_LOGI(NVS_TAG,"malloc");
			for(uint8_t key=0; key<ENCODER_SIZE;key++){
				encoder_map[i][key] = encoder_layout_buff[key];
			}
		}
	}else{

		ESP_LOGI(NVS_TAG,"Encoder layouts not found on NVS, loading default layouts");
		free(layer_names_arr);
		encoder_map = malloc(LAYERS*sizeof(uint16_t*));
		layer_names_arr = malloc(sizeof(default_layout_names));
		for(uint8_t i = 0;i < LAYERS; i++){
			encoder_map[i] = malloc(sizeof((default_encoder_map)[i]));
			layer_names_arr[i] = malloc(sizeof(default_layout_names[i]));
			strcpy(layer_names_arr[i],default_layout_names[i]);
			for(uint8_t key=0; key<ENCODER_SIZE;key++){
				encoder_map[i][key] = default_encoder_map[i][key];
			}
		}
	}

	//set slave encoder layouts
	if(layers_num!=0){
		ESP_LOGI(NVS_TAG,"Slave Encoder layouts found on NVS, loading layouts");
		slave_encoder_map = malloc(layers_num*sizeof(uint16_t*));
		for(uint8_t i = 0;i < layers_num; i++){
			uint16_t encoder_layout_buff[ENCODER_SIZE] = {0};
			nvs_read_slave_encoder_layout(layer_names_arr[i],encoder_layout_buff);
			encoder_map[i] = malloc(sizeof(encoder_layout_buff));
			ESP_LOGI(NVS_TAG,"malloc");
			for(uint8_t key=0; key<ENCODER_SIZE;key++){
				slave_encoder_map[i][key] = encoder_layout_buff[key];
			}
		}
	}else{

		ESP_LOGI(NVS_TAG,"Slave encoder layouts not found on NVS, loading default layouts");
		free(layer_names_arr);
		slave_encoder_map = malloc(LAYERS*sizeof(uint16_t*));
		layer_names_arr = malloc(sizeof(default_layout_names));
		for(uint8_t i = 0;i < LAYERS; i++){
			slave_encoder_map[i] = malloc(sizeof((default_encoder_map)[i]));
			layer_names_arr[i] = malloc(sizeof(default_layout_names[i]));
			strcpy(layer_names_arr[i],default_layout_names[i]);
			for(uint8_t key=0; key<ENCODER_SIZE;key++){
				slave_encoder_map[i][key] = default_slave_encoder_map[i][key];
			}
		}
	}

}
