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

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>


#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"

#include "HID_kbdmousejoystick.h"
#include "espnow_recieve.h"
#include "keyboard_config.h"
#include "r_encoder.h"

//ESP-now tag for reports
#define ESP_NOW_TAG "ESP-NOW"

// Queue for received esp-now report

static const uint8_t channel=1;
//uint8_t master_mac_adr[6]= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // Will be used in the future for setting a mac address manually


// To my understanding the you need to assign a handler for the WiFi loop in order to start the function.
static esp_err_t example_event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id) {
	case SYSTEM_EVENT_STA_START:;
	ESP_LOGI(ESP_NOW_TAG,"WiFi Initialized");
	break;
	default:
		break;
	}
	return ESP_OK;
}

// Initializing WiFi
static void wifi_initialize_recieve(void){

	ESP_LOGI(ESP_NOW_TAG,"Initialing WiFi");
	uint8_t slave_mac_adr[6];

	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(example_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)) ; // For some reason ESP-NOW only works if all devices are in the same mode
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	//esp_wifi_set_mac(ESP_IF_WIFI_STA, master_mac_adr);
	ESP_ERROR_CHECK(esp_wifi_start());
	esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE); // Make sure we are on the same channel
	ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,slave_mac_adr));


	//Printout the mac ID (in case we change the starting one)
	printf("DEVICE MAC ADDRESS:[");
	for(int i=0;i<6; i++)
	{
		printf("%d:", slave_mac_adr[i]);
	}
	printf("]");
}

//ESP-NOW callback upon receiving data
static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len){
	ESP_LOGI(ESP_NOW_TAG,"Data received!");
	uint8_t CURRENT_ENCODER[1]={0};
	uint8_t CURRENT_MATRIX[MATRIX_ROWS][MATRIX_COLS]={0};

	// for key reports
	if(data_len == MATRIX_ROWS*MATRIX_COLS){
		memcpy(CURRENT_MATRIX, data, sizeof(CURRENT_MATRIX) );
		xQueueSend(espnow_recieve_q,(void*)&CURRENT_MATRIX, (TickType_t) 0);
	}

	// currently for encoder reports
	if(data_len==1){
		memcpy(CURRENT_ENCODER, data, sizeof(CURRENT_ENCODER) );
		r_encoder_command(CURRENT_ENCODER[0], slave_encoder_map[current_layout]);
		xQueueSend(media_q,(void*)&CURRENT_ENCODER, (TickType_t) 0);

	}

}


//Initialize receiving via ESP-NOW
static void espnow_initialize_recieve(void){
	ESP_LOGI(ESP_NOW_TAG,"Initialing ESP-NOW");
	esp_now_init();
	esp_now_register_recv_cb(espnow_recv_cb);
}

void espnow_recieve(void){
	ESP_LOGI(ESP_NOW_TAG,"Initialing ESP-NOW functions for receiving data");

	wifi_initialize_recieve();
	espnow_initialize_recieve();
}
