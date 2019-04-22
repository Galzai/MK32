#include <string.h>
#include "wifi_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "nvs.h"
#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "plugins.h"

nvs_handle wifi_nvs_handle;
#define WIFI_NAMESPACE "wifi"
esp_err_t err;

#define NVS_TAG "NVS"
#define WIFI_TAG "Wi-Fi"

// store/overwrite access point config to nvs
void wifi_store_ap(wifi_sta_config_t ap_config){

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(WIFI_NAMESPACE,NVS_READWRITE, &wifi_nvs_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	//we first set the keys as their retrieval names
	char config_password[MAX_AP_KEY_LEN] = AP_PASSWORD;
	char config_scan_method[MAX_AP_KEY_LEN] = AP_SCAN_METHOD;
	char config_bssid_set[MAX_AP_KEY_LEN] = AP_BSSID_SET;
	char config_bssid[MAX_AP_KEY_LEN] = AP_BSSID;
	char config_channel[MAX_AP_KEY_LEN] = AP_CHANNEL;
	char config_listen_interval[MAX_AP_KEY_LEN] = AP_LISTEN_INTERVAL;
	char config_sort_method[MAX_AP_KEY_LEN] = AP_SORT_METHOD;
	char config_threshold[MAX_AP_KEY_LEN] = AP_THRESHOLD;

	//we then append the ssid to each retrival name
	strcat(config_password, (char *) ap_config.ssid);
	strcat(config_scan_method,(char *) ap_config.ssid);
	strcat(config_bssid_set,(char *) ap_config.ssid);
	strcat(config_bssid, (char *)ap_config.ssid);
	strcat(config_channel, (char *)ap_config.ssid);
	strcat(config_listen_interval,(char *) ap_config.ssid); //this one needs to be stored as uint16_t
	strcat(config_sort_method,(char *) ap_config.ssid);
	strcat(config_threshold, (char *)ap_config.ssid);

	//setting ssid to nvs
	err =nvs_set_blob(wifi_nvs_handle, (char *)ap_config.ssid, ap_config.ssid,sizeof(ap_config.ssid));
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s","SSID", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ","SSID");
	}

	//setting password to nvs
	err =nvs_set_blob(wifi_nvs_handle, config_password, ap_config.password,sizeof(ap_config.password));
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_PASSWORD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_PASSWORD);
	}

	//setting scan method to nvs
	err =nvs_set_u8(wifi_nvs_handle,  config_scan_method,ap_config.scan_method);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_SCAN_METHOD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_SCAN_METHOD);
	}

	//setting bssid_set to nvs
	err =nvs_set_u8(wifi_nvs_handle,  config_bssid_set,ap_config.bssid_set);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_BSSID_SET, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_BSSID_SET);
	}

	//setting bssid to nvs
	err =nvs_set_blob(wifi_nvs_handle, config_bssid,ap_config.bssid,sizeof(ap_config.bssid));
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_BSSID, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_BSSID);
	}

	//setting channel to nvs
	err =nvs_set_u8(wifi_nvs_handle, config_channel,ap_config.channel);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_CHANNEL, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_CHANNEL);
	}

	//setting listen interval to nvs
	err =nvs_set_u16(wifi_nvs_handle, config_listen_interval,ap_config.listen_interval);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_LISTEN_INTERVAL, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_LISTEN_INTERVAL);
	}

	//setting sort method interval to nvs
	err =nvs_set_u8(wifi_nvs_handle, config_sort_method,ap_config.sort_method);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_SORT_METHOD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_SORT_METHOD);
	}
	//
	//	//setting threshold to nvs
	//	err =nvs_set_u8(wifi_nvs_handle, config_threshold,ap_config.threshold);
	//	if (err != ESP_OK) {
	//		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_THRESHOLD, esp_err_to_name(err));
	//	}
	//	else{
	//		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_THRESHOLD);
	//	}

	nvs_commit(wifi_nvs_handle);
	nvs_close(wifi_nvs_handle);



}

// retrieve an ap config from nvs
uint8_t wifi_retrieve_ap(uint8_t ssid[MAX_AP_SSID_LEN] ,wifi_sta_config_t *ap_config){

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(WIFI_NAMESPACE,NVS_READONLY, &wifi_nvs_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}

	//we first set the keys as their retrieval names
	char config_password[MAX_AP_KEY_LEN] = AP_PASSWORD;
	char config_scan_method[MAX_AP_KEY_LEN] = AP_SCAN_METHOD;
	char config_bssid_set[MAX_AP_KEY_LEN] = AP_BSSID_SET;
	char config_bssid[MAX_AP_KEY_LEN] = AP_BSSID;
	char config_channel[MAX_AP_KEY_LEN] = AP_CHANNEL;
	char config_listen_interval[MAX_AP_KEY_LEN] = AP_LISTEN_INTERVAL;
	char config_sort_method[MAX_AP_KEY_LEN] = AP_SORT_METHOD;
	char config_threshold[MAX_AP_KEY_LEN] = AP_THRESHOLD;

	//we then append the ssid to each retrival name
	strcat(config_password, (char *)ssid);
	strcat(config_scan_method,(char *)ssid);
	strcat(config_bssid_set,(char *)ssid);
	strcat(config_bssid, (char *)ssid);
	strcat(config_channel, (char *)ssid);
	strcat(config_listen_interval, (char *)ssid); //this one needs to be stored as uint16_t
	strcat(config_sort_method, (char *)ssid);
	strcat(config_threshold, (char *)ssid);

	size_t ap_config_ssid_size =sizeof(ap_config->ssid);
	err =nvs_get_blob(wifi_nvs_handle, (char *)ssid, ap_config->ssid,&ap_config_ssid_size);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s","SSID", esp_err_to_name(err));

		return NO_SSID_FOUND;
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ","SSID");
	}

	//getting password from nvs
	size_t ap_config_pass_size = sizeof(ap_config->password);
	err =nvs_get_blob(wifi_nvs_handle, config_password, ap_config->password,&ap_config_pass_size);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_PASSWORD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_PASSWORD);
	}
	uint8_t buff;
	//getting scan method from nvs

	err = nvs_get_u8(wifi_nvs_handle,  config_scan_method,&buff);

	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_SCAN_METHOD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_SCAN_METHOD);
		ap_config->scan_method =  buff;
	}

	//getting bssid_set from nvs
	err =nvs_get_u8(wifi_nvs_handle,  config_bssid_set,&buff);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_BSSID_SET, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_BSSID_SET);
		ap_config->bssid_set =  buff;
	}

	//getting bssid from nvs
	size_t ap_config_bssid_size = sizeof(ap_config->bssid);
	err =nvs_get_blob(wifi_nvs_handle, config_bssid,ap_config->bssid,&ap_config_bssid_size);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_BSSID, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_BSSID);
	}

	//getting channel from nvs
	err =nvs_get_u8(wifi_nvs_handle, config_channel,&ap_config->channel);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_CHANNEL, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_CHANNEL);
	}

	//getting listen interval from nvs
	err =nvs_get_u16(wifi_nvs_handle, config_listen_interval,&ap_config->listen_interval);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_LISTEN_INTERVAL, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_LISTEN_INTERVAL);
	}

	//getting sort method interval from nvs
	err =nvs_get_u8(wifi_nvs_handle, config_sort_method,&buff);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_SORT_METHOD, esp_err_to_name(err));
	}
	else{
		ap_config->sort_method = buff;
		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_SORT_METHOD);
	}

	//getting threshold from nvs
	//	err =nvs_get_u8(wifi_nvs_handle, config_threshold,ap_config->threshold);
	//	if (err != ESP_OK) {
	//		ESP_LOGE(NVS_TAG, "Error getting %s: %s",AP_THRESHOLD, esp_err_to_name(err));
	//	}
	//	else{
	//		ESP_LOGI(NVS_TAG, "Success getting %s ",AP_THRESHOLD);
	//	}

	nvs_close(wifi_nvs_handle);
	return SSID_FOUND;
}

// delete ap from nvs
void wifi_del_ap(uint8_t ssid[MAX_AP_SSID_LEN]){

	ESP_LOGI(NVS_TAG,"Opening NVS handle");
	err = nvs_open(WIFI_NAMESPACE,NVS_READWRITE, &wifi_nvs_handle);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		ESP_LOGI(NVS_TAG,"NVS Handle opened successfully");
	}
	//we first set the keys as their retrieval names
	char config_password[MAX_AP_KEY_LEN] = AP_PASSWORD;
	char config_scan_method[MAX_AP_KEY_LEN] = AP_SCAN_METHOD;
	char config_bssid_set[MAX_AP_KEY_LEN] = AP_BSSID_SET;
	char config_bssid[MAX_AP_KEY_LEN] = AP_BSSID;
	char config_channel[MAX_AP_KEY_LEN] = AP_CHANNEL;
	char config_listen_interval[MAX_AP_KEY_LEN] = AP_LISTEN_INTERVAL;
	char config_sort_method[MAX_AP_KEY_LEN] = AP_SORT_METHOD;
	char config_threshold[MAX_AP_KEY_LEN] = AP_THRESHOLD;

	//we then append the ssid to each retrival name
	strcat(config_password,(char*) ssid);
	strcat(config_scan_method,(char*) ssid);
	strcat(config_bssid_set,(char*) ssid);
	strcat(config_bssid,(char*) ssid);
	strcat(config_channel,(char*) ssid);
	strcat(config_listen_interval,(char*) ssid); //this one needs to be stored as uint16_t
	strcat(config_sort_method,(char*) ssid);
	strcat(config_threshold,(char*) ssid);

	//erasing ssid from nvs
	err =nvs_erase_key(wifi_nvs_handle,(char*)ssid);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s","SSID", esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ","SSID");
	}

	//erasing password from nvs
	err =nvs_erase_key(wifi_nvs_handle, config_password);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error setting %s: %s",AP_PASSWORD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success setting %s ",AP_PASSWORD);
	}

	//erasing scan method from nvs
	err =nvs_erase_key(wifi_nvs_handle,  config_scan_method);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error erasing %s: %s",AP_SCAN_METHOD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success erasing %s ",AP_SCAN_METHOD);
	}

	//erasing bssid_set from nvs
	err =nvs_erase_key(wifi_nvs_handle,  config_bssid_set);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error erasing %s: %s",AP_BSSID_SET, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success erasing %s ",AP_BSSID_SET);
	}

	//erasing bssid from nvs
	err =nvs_erase_key(wifi_nvs_handle, config_bssid);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error erasing %s: %s",AP_BSSID, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success erasing %s ",AP_BSSID);
	}

	//erasing channel from nvs
	err =nvs_erase_key(wifi_nvs_handle, config_channel);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error erasing %s: %s",AP_CHANNEL, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success erasing %s ",AP_CHANNEL);
	}

	//erasing listen interval from nvs
	err =nvs_erase_key(wifi_nvs_handle, config_listen_interval);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error erasing %s: %s",AP_LISTEN_INTERVAL, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success erasing %s ",AP_LISTEN_INTERVAL);
	}

	//erasing sort method interval from nvs
	err =nvs_erase_key(wifi_nvs_handle, config_sort_method);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error erasing %s: %s",AP_SORT_METHOD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success erasing %s ",AP_SORT_METHOD);
	}

	//erasing threshold from nvs
	err =nvs_erase_key(wifi_nvs_handle, config_threshold);
	if (err != ESP_OK) {
		ESP_LOGE(NVS_TAG, "Error erasing %s: %s",AP_THRESHOLD, esp_err_to_name(err));
	}
	else{
		ESP_LOGI(NVS_TAG, "Success erasing %s ",AP_THRESHOLD);
	}

	nvs_commit(wifi_nvs_handle);
	nvs_close(wifi_nvs_handle);

}
#define EXAMPLE_ESP_MAXIMUM_RETRY  100
static EventGroupHandle_t s_wifi_event_group;
/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "wifi station";

static int s_retry_num = 0;
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id) {
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		ESP_LOGI(TAG, "got ip:%s",
				ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
		s_retry_num = 0;
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
	{
		if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
			s_retry_num++;
			ESP_LOGI(TAG,"retry to connect to the AP");
		}
		ESP_LOGI(TAG,"connect to the AP fail\n");
		break;
	}
	default:
		break;
	}
	return ESP_OK;
}

// connect to a wifi AP
void wifi_connection_init(void){

	uint16_t num_records = 0;
	wifi_ap_record_t* ap_records;
	wifi_sta_config_t sta_config;
	tcpip_adapter_ip_info_t ip_info;


	;


	// if the keyboards does not use esp now we need to init wifi
#ifndef SPLIT_MASTER
	tcpip_adapter_init();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)) ; //
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	//esp_wifi_set_mac(ESP_IF_WIFI_STA, master_mac_adr);
	ESP_ERROR_CHECK(esp_wifi_start());
#endif
	//try to connect to the default ap
	s_wifi_event_group = xEventGroupCreate();

	// start scanning for available aps
	wifi_scan_config_t scan_config = {0};
	err = esp_wifi_scan_start(&scan_config, 1);

	if(err != ESP_OK){

		ESP_LOGE(WIFI_TAG, "Error scanning for AP : %s", esp_err_to_name(err));
	}else{

		ESP_LOGI(WIFI_TAG, "Success scanning for AP");
		err = esp_wifi_scan_get_ap_num(&num_records);
		if(err != ESP_OK){

			ESP_LOGE(WIFI_TAG, "Error scanning for AP records number : %s", esp_err_to_name(err));

		}else{

			ESP_LOGI(WIFI_TAG, "Success scanning for AP records number");
			ap_records = malloc(num_records*sizeof(wifi_ap_record_t));
			err = esp_wifi_scan_get_ap_records(&num_records, ap_records);

			if(err != ESP_OK){
				ESP_LOGE(WIFI_TAG, "Error scanning for AP : %s", esp_err_to_name(err));
			}else{
				ESP_LOGI(WIFI_TAG, "Success scanning for AP");
				// check if one of the APs exists in flash and try to connect to it,
				// if it does exist but cannot be connected to continue to try other APs in flash
				for(uint8_t ap_record = 0; ap_record < num_records; ap_record++){

					ESP_LOGI(WIFI_TAG, "Found AP %d: %s",ap_record ,ap_records[ap_record].ssid);
					if(strcmp((char*)ap_records[ap_record].ssid, DEFAULT_SSID) == 0){

						wifi_config_t ap_config ={
								.sta = {
										.ssid = DEFAULT_SSID,
										.password = DEFAULT_PASSWORD
								},
						};
						ESP_ERROR_CHECK(esp_wifi_stop());
						ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );
						err = esp_wifi_set_config(ESP_IF_WIFI_STA,&ap_config);
						ESP_LOGI(WIFI_TAG, "Found default AP %d: %s",ap_record ,ap_records[ap_record].ssid);
						ESP_ERROR_CHECK(esp_wifi_start());
						break;
					}
					err = wifi_retrieve_ap(ap_records[ap_record].ssid, &sta_config);
					if(err == SSID_FOUND){
						wifi_config_t ap_config ={
								.sta = sta_config
						};
						ESP_ERROR_CHECK(esp_wifi_stop());
						ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );
						err = esp_wifi_set_config(ESP_IF_WIFI_STA,&ap_config);
						if(err != ESP_OK){
							ESP_LOGE(WIFI_TAG, "Error setting AP %s config: %s", sta_config.ssid, esp_err_to_name(err));
						}else{
							ESP_LOGI(WIFI_TAG, "Success setting AP %s config ",sta_config.ssid);
							ESP_ERROR_CHECK(esp_wifi_start());
							if(err != ESP_OK){
								ESP_LOGE(WIFI_TAG, "Error connecting to %s AP: %s", sta_config.ssid, esp_err_to_name(err));
							}else{
								ESP_LOGI(WIFI_TAG, "Success connecting to %s AP config",sta_config.ssid);
								ESP_LOGI(WIFI_TAG, "IP Address: %s ,Subnet mask: %s Subnet mask: %s",ip4addr_ntoa(&ip_info.ip),ip4addr_ntoa(&ip_info.netmask),ip4addr_ntoa(&ip_info.gw));
								break;
							}
						}
					}
				}
			}
		}
	}


}
