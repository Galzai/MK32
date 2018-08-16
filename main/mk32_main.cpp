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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "driver/gpio.h"


//HID Ble functions
#include "HID_kbdmousejoystick.h"

//MK32 functions
#include "matrix.c"
#include "keypress_handles.c"
#include "keyboard_config.h"

//ESP-NOW functions
#include "espnow_recieve.h"
#include "espnow_send.h"
#define KEY_REPORT_TAG "KEY_REPORT"

static config_data_t config;
QueueHandle_t espnow_recieve_q;

extern "C" void key_reports(void *pvParameters)
{
	// Arrays for holding the report at various stages
	uint8_t past_report[2+MATRIX_ROWS*KEYMAP_COLS]={0};
	uint8_t report_state[2+MATRIX_ROWS*KEYMAP_COLS];

	while(1){
		while(HID_kbdmousejoystick_isConnected() == 0) {
			ESP_LOGI(KEY_REPORT_TAG,"Not connected, waiting for connection ");
			vTaskDelay(500);
		}

		while(HID_kbdmousejoystick_isConnected() != 0) {

			memcpy(report_state, check_key_state(*layouts[current_layout]), sizeof report_state);

			//Do not send anything if queues are uninitialized
			if(mouse_q == NULL || keyboard_q == NULL || joystick_q == NULL)
			{
				ESP_LOGE(KEY_REPORT_TAG,"queues not initialized");
			}
			else{
				if(memcmp(past_report, report_state, sizeof past_report)!=0){
					memcpy(past_report,report_state, sizeof past_report );
					xQueueSend(keyboard_q,(void*)&report_state, (TickType_t) 0);
					vTaskDelay(3);
;

				}
			}

		}


	}

}

//Function for sending out the modified matrix
extern "C" void slave_scan(void *pvParameters){

	uint8_t PAST_MATRIX[MATRIX_ROWS][MATRIX_COLS]={0};

while(1){
	scan_matrix();
	if(memcmp(&PAST_MATRIX, &MATRIX_STATE, sizeof MATRIX_STATE)!=0){
		memcpy(&PAST_MATRIX, &MATRIX_STATE, sizeof MATRIX_STATE );

		xQueueSend(espnow_send_q,(void*)&MATRIX_STATE, (TickType_t) 0);
		vTaskDelay(1);
	 }
	}
}

//Update the matrix state via reports recieved by espnow
extern "C" void espnow_update_matrix(void *pvParameters){


	uint8_t CURRENT_MATRIX[MATRIX_ROWS][MATRIX_COLS]={0};
	while(1){
		if(xQueueReceive(espnow_recieve_q,&CURRENT_MATRIX,10000))
		{
			memcpy(&SLAVE_MATRIX_STATE, &CURRENT_MATRIX, sizeof CURRENT_MATRIX );
		}
	}
}


extern "C" void app_main()
{
	matrix_setup();
	esp_err_t ret;

	// Initialize NVS.
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );

	// Read config
	nvs_handle my_handle;
	ESP_LOGI("MAIN","loading configuration from NVS");
	ret = nvs_open("config_c", NVS_READWRITE, &my_handle);
	if(ret != ESP_OK) ESP_LOGE("MAIN","error opening NVS");
	size_t available_size = MAX_BT_DEVICENAME_LENGTH;
	strcpy(config.bt_device_name, GATTS_TAG);
	nvs_get_str (my_handle, "btname", config.bt_device_name, &available_size);
	if(ret != ESP_OK)
	{
		ESP_LOGE("MAIN","error reading NVS - bt name, setting to default");
		strcpy(config.bt_device_name, GATTS_TAG);
	} else ESP_LOGI("MAIN","bt device name is: %s",config.bt_device_name);

//If the device is a slave initialize sending reports to master
#ifdef SLAVE
	xTaskCreatePinnedToCore(slave_scan, "Scan changes for slave", 4096, NULL, configMAX_PRIORITIES, NULL,1);
	espnow_send();
#endif


//If the device is a master for split board initialize receiving reports from slave
#ifdef SPLIT_MASTER

	uint8_t array_sample[2+MATRIX_ROWS*MATRIX_COLS];
	espnow_recieve_q = xQueueCreate(32,sizeof(array_sample));
	espnow_recieve();
	xTaskCreatePinnedToCore(espnow_update_matrix, "ESP-NOW slave matrix state", 4096, NULL, configMAX_PRIORITIES, NULL,1);
#endif

#ifdef MASTER
	//activate keyboard BT stack
	HID_kbdmousejoystick_init(1,1,0,0,config.bt_device_name);
	ESP_LOGI("HIDD","MAIN finished...");

	esp_log_level_set("*", ESP_LOG_INFO);

	// Start the keyboard Tasks
	// Create the key scanning task on core 1 (otherwise it will crash)
	xTaskCreatePinnedToCore(key_reports, "key reports", 4096, NULL, configMAX_PRIORITIES, NULL,1);
#endif


}
