/*
 * wifi_manager.h
 *
 *  Created on: 29 Sep 2018
 *      Author: gal
 */

#ifndef WIFI_MANAGER_WIFI_MANAGER_H_
#define WIFI_MANAGER_WIFI_MANAGER_H_

#include <keyboard_config.h>
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_event_loop.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_AP_SSID_LEN 32
#define MAX_AP_PASS_LEN 64
#define MAX_AP_BSSID_LEN 6

/*
 * names to for config data keys (for easy retrieval)
 * each key will be "retrieval name" with corresponding "ap ssid" appended at the end
 */
#define MAX_AP_KEY_LEN 64

//Retrieval names
#define AP_PASSWORD "password"
#define AP_SCAN_METHOD "scan method"
#define AP_BSSID_SET "bssid set"
#define AP_BSSID "bssid"
#define AP_CHANNEL "channel"
#define AP_LISTEN_INTERVAL "interval"
#define AP_SORT_METHOD "sort method"
#define AP_THRESHOLD "threshold"

#define NO_SSID_FOUND 0
#define SSID_FOUND 1

/**
 * @brief check if a previous wifi ap is available, if so connect to it.
 * otherwise prompt to enter connection manager
 */
void wifi_connection_init(void);

/**
 * @brief scan for availabe wifi aps and select one to connect and store to nvs
 */
void wifi_availble_ap(void);

/**
 * @brief retrieve a wifi ap config by ssid name and pass it to buffer,
 * if no ssid found return NO_SSID_FOUND otherwise returns SSID_FOUND
 */
 uint8_t wifi_retrieve_ap(uint8_t ssid[MAX_AP_SSID_LEN],wifi_sta_config_t *ap_config);

/**
 * @brief remove ap from nvs
 */
void wifi_del_ap(uint8_t ssid[MAX_AP_SSID_LEN]);

/**
 * @brief store or update a wifi ap in nvs
 */
void wifi_store_ap(wifi_sta_config_t ap_config);



#ifdef __cplusplus
}
#endif


#endif /* WIFI_MANAGER_WIFI_MANAGER_H_ */
