/*
 * plugins.h
 *
 *  Created on: 21 Apr 2019
 *      Author: gal
 */

#ifndef PLUGINS_PLUGINS_H_
#define PLUGINS_PLUGINS_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "plugin_manager.h"


#define DEFAULT_SSID "SSID" //default SSID for WiFi access point
#define DEFAULT_PASSWORD "PASS" //default password for WiFi access point

/*
 *@brief Add the headers for your plugins here, make sure the
 *  function to be called is of type void and only takes argument void * pvParameters
 */
#include "layout_server.h"
#include "testFunc.h"

//Set the keycodes that indicate plugins here, make sure to set the first key code as PLUGIN_BASE_VAL
enum plugin_keycodes {
	PN_CLOSE = PLUGIN_BASE_VAL,
	PN_LAYOUT,
	PN_TEST
};

/*
 *@We create tasks for the plugins depending on the keycode pressed
 *  edit this functing in plugins.c
 */
void plugin_launcher(uint16_t keycode);

/** @brief Queue for sending mouse reports
 * @see mouse_command_t */


#ifdef __cplusplus
}
#endif

#endif /* PLUGINS_PLUGINS_H_ */
