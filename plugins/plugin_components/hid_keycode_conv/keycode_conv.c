#include "keycode_conv.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "keymap.h"
#include "key_definitions.h"

#define TAG "KEY TO CHAR"

// Sizing the report for N-key rollover
uint8_t current_report[REPORT_LEN] = {0};

void enable_key_to_char(void){
	vTaskSuspend(xKeyreportTask);
	ESP_LOGI(TAG,"Suspending hid reports");

}


void disable_key_to_char(void){
	vTaskResume(xKeyreportTask);
	ESP_LOGI(TAG,"Resuming hid reports");

}

/*
 * @brief retrieve char corresponding to keycode
 */
char* keycode_to_char(void){
	char* keycode_char = "";
	return keycode_char;

}

/*
 * @brief retrieve keycode corresponding to char (or string)
 */
uint16_t char_to_keycode(void){
	uint16_t keycode = {0};
	return keycode;

}

