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
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "oled_tasks.h"

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"

#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include "keyboard_config.h"
#include "battery_monitor.h"

static const char *TAG = "	OLED";

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

u8g2_t u8g2; // a structure which will contain all the data for one display
uint8_t prev_led = 0;

QueueHandle_t layer_recieve_q;
QueueHandle_t led_recieve_q;

uint32_t battery_percent = 0;
uint32_t prev_battery_percent= 0;

uint8_t curr_layout = 0;
uint8_t current_led = 0;

int BATT_FLAG =0;
int DROP_H=0;

#define BT_ICON 0x5e
#define BATT_ICON 0x5b
#define LOCK_ICON 0xca


//Erasing area from oled
void erase_area(uint8_t x,uint8_t y,uint8_t w,uint8_t h){
	u8g2_SetDrawColor(&u8g2,0);
	u8g2_DrawBox(&u8g2,x,y,w,h);
	u8g2_SetDrawColor(&u8g2,1);
}

//Function for updating the OLED
void update_oled(void){
#ifdef BATT_STAT
	battery_percent = get_battery_level();
#endif

	if(xQueueReceive(layer_recieve_q,&curr_layout,(TickType_t) 0)){
		erase_area(0,7,45,7);
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
		u8g2_DrawStr(&u8g2, 0,14,layout_names[curr_layout]);
		u8g2_SendBuffer(&u8g2);
	}
	if(xQueueReceive(led_recieve_q,&current_led,(TickType_t) 0)){
		erase_area(0,24,127,8);
		if(CHECK_BIT(current_led,0)!=0){
			u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
			u8g2_DrawStr(&u8g2, 0,31,"NUM");
			u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
			u8g2_DrawGlyph(&u8g2, 16,32,LOCK_ICON);
		}

		if(CHECK_BIT(current_led,1)!=0){
			u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
			u8g2_DrawStr(&u8g2, 27,31,"CAPS");
			u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
			u8g2_DrawGlyph(&u8g2,48,32,LOCK_ICON);
		}
		if(CHECK_BIT(current_led,2)!=0){
			u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
			u8g2_DrawStr(&u8g2, 57,31,"SCROLL");
			u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
			u8g2_DrawGlyph(&u8g2,88,32,LOCK_ICON);
		}
		u8g2_SendBuffer(&u8g2);
	}

	if(battery_percent!=prev_battery_percent){
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
		char buf[sizeof(uint32_t)];
		snprintf (buf, sizeof(uint32_t), "%d", battery_percent);
		u8g2_DrawStr(&u8g2, 103,7,"%");
		if((battery_percent<100)&&(abs(battery_percent-prev_battery_percent)>=2)){
			erase_area(85,0,15,7);
			u8g2_DrawStr(&u8g2, 90,7,buf);
			u8g2_SendBuffer(&u8g2);
		}
		if((battery_percent>100)&&(BATT_FLAG = 0)){
			erase_area(85,0,15,7);
			u8g2_DrawStr(&u8g2, 85,7,"100");
			BATT_FLAG =1;
			u8g2_SendBuffer(&u8g2);
		}
		if(battery_percent==100){
			erase_area(85,0,15,7);
			u8g2_DrawStr(&u8g2, 85,7,"100");
			u8g2_SendBuffer(&u8g2);
		}
		prev_battery_percent = battery_percent;

	}
}

//oled on connection
void ble_connected_oled(void){


	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
	u8g2_DrawStr(&u8g2, 0,6,GATTS_TAG);
	u8g2_DrawStr(&u8g2, 0,14,layout_names[current_layout]);
	u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
	u8g2_DrawGlyph(&u8g2, 110,8,BATT_ICON);
	u8g2_DrawGlyph(&u8g2, 120,8,BT_ICON);

	if(CHECK_BIT(curr_led,0)!=0){
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
		u8g2_DrawStr(&u8g2, 0,31,"NUM");
		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
		u8g2_DrawGlyph(&u8g2, 16,32,LOCK_ICON);
	}

	if(CHECK_BIT(curr_led,1)!=0){
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
		u8g2_DrawStr(&u8g2, 27,31,"CAPS");
		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
		u8g2_DrawGlyph(&u8g2,48,32,LOCK_ICON);
	}
	if(CHECK_BIT(curr_led,2)!=0){
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
		u8g2_DrawStr(&u8g2, 57,31,"SCROLL");
		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
		u8g2_DrawGlyph(&u8g2,88,32,LOCK_ICON);
	}

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
	char buf[sizeof(uint32_t)];
	snprintf (buf, sizeof(uint32_t), "%d", battery_percent);
	u8g2_DrawStr(&u8g2, 103,7,"%");
	if(battery_percent<100){
		u8g2_DrawStr(&u8g2, 90,7,buf);
	}else{
		u8g2_DrawStr(&u8g2, 85,7,"100");
	}
	u8g2_SendBuffer(&u8g2);
}

//Slave oled display
void ble_slave_oled(void){
	battery_percent = get_battery_level();

	if(battery_percent!=prev_battery_percent){
		u8g2_ClearBuffer(&u8g2);
		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
		u8g2_DrawStr(&u8g2, 0,6,GATTS_TAG);
		u8g2_DrawStr(&u8g2, 0,14,"Slave pad 1");
		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
		u8g2_DrawGlyph(&u8g2, 110,8,BATT_ICON);
		u8g2_DrawGlyph(&u8g2, 120,8,BT_ICON);

		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
		char buf[sizeof(uint32_t)];
		snprintf (buf, sizeof(uint32_t), "%d", battery_percent);
		u8g2_DrawStr(&u8g2, 103,7,"%");
		if((battery_percent<100)&&(battery_percent-prev_battery_percent>=2)){
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawBox(&u8g2,85,90,0,7);
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawStr(&u8g2, 90,7,buf);
			u8g2_SendBuffer(&u8g2);
		}
		if((battery_percent<100)&&(battery_percent-prev_battery_percent>=2)){
			erase_area(85,0,15,7);
			u8g2_DrawStr(&u8g2, 90,7,buf);
			u8g2_SendBuffer(&u8g2);
		}
		if((battery_percent>100)&&(BATT_FLAG = 0)){
			erase_area(85,0,15,7);
			u8g2_DrawStr(&u8g2, 85,7,"100");
			BATT_FLAG =1;
			u8g2_SendBuffer(&u8g2);
		}
		if(battery_percent==100){
			erase_area(85,0,15,7);
			u8g2_DrawStr(&u8g2, 85,7,"100");
			u8g2_SendBuffer(&u8g2);
		}
		prev_battery_percent = battery_percent;
	}



}

//Waiting for connecting animation
void waiting_oled(void){
	char waiting_conn[25]="Waiting for connection";

#ifdef BATT_STAT
	battery_percent = get_battery_level();
#endif

	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
	u8g2_DrawGlyph(&u8g2, 110,8,BATT_ICON);
	u8g2_DrawGlyph(&u8g2, 120,8,BT_ICON);
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
	u8g2_DrawStr(&u8g2,0,6,GATTS_TAG);

	char buf[sizeof(uint32_t)];
	snprintf (buf, sizeof(uint32_t), "%d", battery_percent);
	u8g2_DrawStr(&u8g2, 103,7,"%");
	if((battery_percent<100)&&(battery_percent-prev_battery_percent>=2)){
		u8g2_DrawStr(&u8g2, 90,7,buf);
	}
	if(battery_percent<100){
		u8g2_DrawStr(&u8g2, 90,7,buf);
	}else{
		u8g2_DrawStr(&u8g2, 85,7,"100");
	}

	for(int i=0; i<3; i++){
		u8g2_DrawStr(&u8g2, 0,26,waiting_conn);
		u8g2_SendBuffer(&u8g2);
		vTaskDelay(100/portTICK_PERIOD_MS);
		strcat(waiting_conn,".");
	}


}

//shut down OLED
void deinit_oled(void) {
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
	u8g2_DrawStr(&u8g2,0,6,GATTS_TAG);
	u8g2_DrawStr(&u8g2, 0,26,"Going to sleep!");
	u8g2_SendBuffer(&u8g2);
	vTaskDelay(1000/portTICK_PERIOD_MS);
	u8g2_ClearDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 1);
}

//initialize oled
void init_oled(void) {
	layer_recieve_q = xQueueCreate(32,sizeof(uint8_t));
	led_recieve_q = xQueueCreate(32,sizeof(uint8_t));
	ESP_LOGI(TAG, "Setting up oled");
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda   = OLED_SDA_PIN;
	u8g2_esp32_hal.scl  = OLED_SCL_PIN;
	u8g2_esp32_hal_init(u8g2_esp32_hal);

	u8g2_Setup_ssd1306_i2c_128x64_noname_f(
			&u8g2,
			U8G2_R0,
			u8g2_esp32_i2c_byte_cb,
			u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
	u8x8_SetI2CAddress(&u8g2.u8x8,0x78);
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
	u8g2_SetPowerSave(&u8g2, 0); // wake up display

	u8g2_ClearBuffer(&u8g2);


}
