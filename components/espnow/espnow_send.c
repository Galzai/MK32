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
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"

#include "espnow_send.h"
#include "keyboard_config.h"

#include "r_encoder.h"

#define ESP_NOW_TAG "ESP-NOW"

// Queue for sending esp-now reports
QueueHandle_t espnow_matrix_send_q;
QueueHandle_t espnow_encoder_send_q;
//Mac adress of the main device (only mac address needed for ESP-NOW)
static uint8_t master_mac_adr[6]= {0x30,0xAE,0xA4,0x5D,0xBE,0x54};

static esp_now_peer_info_t Peer;
static esp_now_peer_info_t *pPeer=&Peer;

// To my understanind the you need to assign a handlfer for the WiFi loop in order to start the function.
static esp_err_t example_event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id) {
	case SYSTEM_EVENT_STA_START:;
	ESP_LOGI(ESP_NOW_TAG,"Wifi Initialized!");
	break;
	default:
		break;
	}
	return ESP_OK;
}

// Initializing WiFi
void wifi_initialize_send(void){

	ESP_LOGI(ESP_NOW_TAG,"Initialing WiFI!");

	// Setting up the Wifi.
	uint8_t slave_mac_adr[6];
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(example_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)) ; // For some reason ESP-NOW only works if all devices are in the same mode
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK( esp_wifi_set_channel(1, 0) ); // Make sure we are on the same channel
	ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,slave_mac_adr));

	//Printout the mac ID (in case we change the starting one)
	printf("DEVICE MAC ADDRESS:[");
	for(int i=0;i<6; i++)
	{
		printf("%d:", slave_mac_adr[i]);
	}
	printf("]");
}

// Callback function after sending data
void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status){


	switch(status){
	case(ESP_NOW_SEND_SUCCESS):
		ESP_LOGI(ESP_NOW_TAG,"Data Sent successfully!");
	break;

	case(ESP_NOW_SEND_FAIL):
		ESP_LOGI(ESP_NOW_TAG,"Data not Sent");
	break;

	break;
	}
}

// Function for sending state
void espnow_send_state(void *pvParameters){
	uint8_t CURRENT_MATRIX[MATRIX_ROWS][MATRIX_COLS]={0};
	while(1)
	{
		//Wait to receive update matrix state
		if(xQueueReceive(espnow_matrix_send_q,&CURRENT_MATRIX,2))
		{
			ESP_LOGI(ESP_NOW_TAG,"Sending Slave matrix state!");
			//send the report
			esp_now_send(pPeer->peer_addr,(uint8_t*)&CURRENT_MATRIX,sizeof(CURRENT_MATRIX));

		}

#ifdef R_ENCODER_SLAVE
		uint8_t CURRENT_ENCODER[1]={0};
		if(xQueueReceive(espnow_encoder_send_q,&CURRENT_ENCODER,2))
		{
			ESP_LOGI(ESP_NOW_TAG,"Sending encoder state!");
			//send the report
			esp_now_send(pPeer->peer_addr,(uint8_t*)&CURRENT_ENCODER,sizeof(CURRENT_ENCODER));

		}
#endif
	}

}
// Initialize sending via espnow
void espnow_initialize_send(void){

	ESP_LOGI(ESP_NOW_TAG,"Initialing ESP-NOW");
	esp_now_init();
	esp_now_register_send_cb(espnow_send_cb);

	//We need to assign a peer for each pad, will be improved for multiple pad functionality in the future.

	pPeer->channel=1;
	memcpy(pPeer->peer_addr,master_mac_adr,6);
	pPeer->ifidx=ESP_IF_WIFI_STA;
	pPeer->encrypt = 0;
	esp_now_add_peer(pPeer);

	//Sending matrix state
	xTaskCreate(espnow_send_state, "Send matrix changes", 4096, NULL, configMAX_PRIORITIES, NULL);

}

void espnow_send(void){

	ESP_LOGI(ESP_NOW_TAG,"Initialing ESP-NOW functions for sending data");

	uint8_t array_sample[2+MATRIX_ROWS*MATRIX_COLS];
	espnow_matrix_send_q = xQueueCreate(32,sizeof(array_sample));
#ifdef R_ENCODER_SLAVE
	r_encoder_setup();
	espnow_encoder_send_q = xQueueCreate(32,sizeof(uint8_t));
#endif

	wifi_initialize_send();
	espnow_initialize_send();
}
