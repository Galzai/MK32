/*
 * plugin_manager.c
 *
 *  Created on: 21 Apr 2019
 *      Author: gal
 */
#include "esp_log.h"
#include "plugin_manager.h"

TaskHandle_t pluginHandle;
#define PLUGIN_LOG "Plugin Manager"

int taskFlag = 0;
int wifiFlag = 0;
/*
 * @brief start a plugin FreeRTOS task
 */
void start_plugin_task(func_t plugin, int wifi_enable, int block_ble_report){
	uint8_t no_err = 1;
	if(taskFlag == 1){
		ESP_LOGE(PLUGIN_LOG, "Plugin already running!");
		return;
	}
	else{
		if(wifi_enable == 1){
			no_err = wifi_connection_init();
			wifiFlag = 1;
		}
		if(block_ble_report == 1){
			BLE_EN = 0;
		}
	}
	//create the task
	if(no_err == 1){
		ESP_LOGI(PLUGIN_LOG, "Starting plugin");
		pluginEn = 1;
		xTaskCreatePinnedToCore(plugin, "Plugin task", 4096, NULL,configMAX_PRIORITIES,&pluginHandle, 1);
		taskFlag = 1;
	}
	if(no_err == 0){
		ESP_LOGE(PLUGIN_LOG, "Error initializing plugin");
	}
}


/*
 * @brief close the plugin that is currently running
 */
void close_plugin_task(void){
	if(taskFlag != 1){
		ESP_LOGE(PLUGIN_LOG, "No plugin running!");
	}
	else{

		taskFlag = 0;
	//	vTaskSuspend(pluginHandle);

		if(wifiFlag == 1){
			wifi_connection_deinit();
			wifiFlag = 0;
		}
		if(BLE_EN == 0){
			BLE_EN = 1;
		}

		vTaskDelete(pluginHandle);
		ESP_LOGI(PLUGIN_LOG, "Plugin closed");
	}

}

