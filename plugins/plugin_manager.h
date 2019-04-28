/*
 * plugin_manager.h
 *
 *  Created on: 21 Apr 2019
 *      Author: gal
 */

#ifndef PLUGINS_PLUGIN_MANAGER_H_
#define PLUGINS_PLUGIN_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "keycode_conv.h"
#include "wifi_manager.h"

uint8_t pluginEn;
typedef void(*func_t)( void * pvParameters); //just to make things easier
/*
 * @brief start a plugin FreeRTOS task
 * @param plugin pass a function to be opened as the plugin
 * @param wifi_enable set 0 if function does not require wiFi
 * @param block_ble_report set 0 if you do not wish to block keyboard reports
 */
void start_plugin_task(func_t plugin, int wifi_enable, int block_ble_report);


/*
 * @@brief close the plugin that is currently running
 */
void close_plugin_task(void);

#ifdef __cplusplus
}
#endif



#endif /* PLUGINS_PLUGIN_MANAGER_H_ */
