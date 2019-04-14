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
 * Copyright 2019 Benjamin Aigner <beni@asterics-foundation.org>
 */

/** @file
 * @brief This file is a wrapper for the BLE-HID example of Espressif.
*/

#include "hal_ble.h"

#define LOG_TAG "hal_ble"

/** @brief Set a global log limit for this file */
#define LOG_LEVEL_BLE ESP_LOG_INFO

/// @brief Input queue for sending joystick reports
QueueHandle_t joystick_q;
/// @brief Input queue for sending keyboard reports
QueueHandle_t keyboard_q;
/// @brief Input queue for sending mouse reports
QueueHandle_t mouse_q;
/// @brief Input queue for sending media reports
QueueHandle_t media_q;

/** @brief Connection ID for an opened HID connection */
static uint16_t hid_conn_id = 0;
/** @brief Do we have a secure connection? */
static bool sec_conn = false;
/** @brief Callback for HID events. */
//static void hidd_event_callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param);

/** @brief Full UUID for the HID service */
static uint8_t hidd_service_uuid128[] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00,
};

/** @brief Advertising data for BLE */
static esp_ble_adv_data_t hidd_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x03c0,       //HID Generic,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(hidd_service_uuid128),
    .p_service_uuid = hidd_service_uuid128,
    .flag = 0x6,
};

/** @brief Advertising parameters */
static esp_ble_adv_params_t hidd_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x30,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};


///@brief Is Keyboard interface active?
uint8_t activateKeyboard = 0;
///@brief Is Mouse interface active?
uint8_t activateMouse = 0;
///@brief Is Joystick interface active?
uint8_t activateJoystick = 0;
///@brief Is Media interface active?
uint8_t activateMedia = 0;

uint8_t key_report[HID_KEYBOARD_IN_RPT_LEN] = {0};
uint8_t mouse_report[HID_MOUSE_IN_RPT_LEN] = {0};
uint8_t media_report[HID_CC_IN_RPT_LEN ] = {0};
uint8_t joystick_report[HID_JOYSTICK_IN_RPT_LEN] = {0};


/** @brief Callback for HID events. */
static void hidd_event_callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
{
  switch(event) {
    case ESP_HIDD_EVENT_REG_FINISH: 
      if (param->init_finish.state == ESP_HIDD_INIT_OK) {
        //esp_bd_addr_t rand_addr = {0x04,0x11,0x11,0x11,0x11,0x05};

        esp_ble_gap_set_device_name(GATTS_TAG);
        esp_ble_gap_config_adv_data(&hidd_adv_data);
      }
      break;

    case ESP_BAT_EVENT_REG: break;
    case ESP_HIDD_EVENT_DEINIT_FINISH: break;
		case ESP_HIDD_EVENT_BLE_CONNECT:
      ESP_LOGI(LOG_TAG, "ESP_HIDD_EVENT_BLE_CONNECT");
      hid_conn_id = param->connect.conn_id;
      break;
    case ESP_HIDD_EVENT_BLE_DISCONNECT:
      sec_conn = false;
      ESP_LOGI(LOG_TAG, "ESP_HIDD_EVENT_BLE_DISCONNECT");
      esp_ble_gap_start_advertising(&hidd_adv_params);
      break;
    case ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT:
      ESP_LOGI(LOG_TAG, "%s, ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT", __func__);
      ESP_LOG_BUFFER_HEX(LOG_TAG, param->vendor_write.data, param->vendor_write.length);
      break;
    default:
      break;
  }
  return;
}

/** @brief Callback for GAP events */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
  switch (event)
  {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
      esp_ble_gap_start_advertising(&hidd_adv_params);
      break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
      for(int i = 0; i < ESP_BD_ADDR_LEN; i++) {
        ESP_LOGD(LOG_TAG, "%x:",param->ble_security.ble_req.bd_addr[i]);
      }
      esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
      break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
      sec_conn = true;
      esp_bd_addr_t bd_addr;
      memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
      ESP_LOGI(LOG_TAG, "remote BD_ADDR: %08x%04x",\
        (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
        (bd_addr[4] << 8) + bd_addr[5]);
      ESP_LOGI(LOG_TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
      ESP_LOGI(LOG_TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
      if(!param->ble_security.auth_cmpl.success) {
          ESP_LOGE(LOG_TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
      }
      break;
    default:
        break;
  }
}

/** @brief CONTINOUS TASK - sending HID commands via BLE
 * 
 * This task is used to wait for HID commands, sent to the hid_ble
 * queue. If one command is received, it will be sent to a (possibly)
 * connected BLE device.
 */
void halBLETask_keyboard(void * params)
{

  //Empty queue if initialized (there might be something left from last connection)
  if(keyboard_q != NULL) xQueueReset(mouse_q);

  //check if queue is initialized
  if(keyboard_q != NULL){
  {
    while(1)
    {
      //pend on MQ, if timeout triggers, just wait again.
      if(xQueueReceive(keyboard_q,&key_report,portMAX_DELAY))
      {
        //if we are not connected, discard.
        if(sec_conn == false) continue;
        hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id,
        		HID_RPT_ID_KEY_IN , HID_REPORT_TYPE_INPUT, HID_KEYBOARD_IN_RPT_LEN , key_report);
      }

    }
  }
  }
  else {
    ESP_LOGE(LOG_TAG,"ble hid queue not initialized, retry in 1s");
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }

}

void halBLETask_mouse(void * params)
{

  //Empty queue if initialized (there might be something left from last connection)

  if(mouse_q != NULL) xQueueReset(keyboard_q);


  //check if queue is initialized
  if(mouse_q != NULL){
  {
    while(1)
    {

      //pend on MQ, if timeout triggers, just wait again.
      if(xQueueReceive(mouse_q,&mouse_report,portMAX_DELAY))
      {

        //if we are not connected, discard.
        if(sec_conn == false) continue;
        hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id,
          HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT, HID_MOUSE_IN_RPT_LEN, mouse_report);
      }
    }
  }
  }
  else {
    ESP_LOGE(LOG_TAG,"ble hid queue not initialized, retry in 1s");
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }

}

void halBLETask_joystick(void * params)
{
  
  //Empty queue if initialized (there might be something left from last connection)
  if(joystick_q != NULL) xQueueReset(joystick_q);

  
  //check if queue is initialized
  if(joystick_q != NULL){
  {
    while(1)
    {
      //pend on MQ, if timeout triggers, just wait again.
      if(xQueueReceive(joystick_q,&joystick_report,portMAX_DELAY))
      {

        //if we are not connected, discard.
        if(sec_conn == false) continue;
        hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id,
        		HID_RPT_ID_JOY_IN , HID_REPORT_TYPE_INPUT, HID_JOYSTICK_IN_RPT_LEN, joystick_report);
      }
    }
  }
  }
  else {
    ESP_LOGE(LOG_TAG,"ble hid queue not initialized, retry in 1s");
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }

}

void halBLETask_media(void * params)
{

  //Empty queue if initialized (there might be something left from last connection)
  if(mouse_q != NULL) xQueueReset(mouse_q);
  if(keyboard_q != NULL) xQueueReset(keyboard_q);
  if(joystick_q != NULL) xQueueReset(joystick_q);
  if(media_q != NULL) xQueueReset(media_q);

  //check if queue is initialized
  if(media_q != NULL){
  {
    while(1)
    {

      //pend on MQ, if timeout triggers, just wait again.
      if(xQueueReceive(media_q,&media_report,portMAX_DELAY))
      {

        //if we are not connected, discard.
        if(sec_conn == false) continue;
        hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id,
        		HID_RPT_ID_CC_IN, HID_REPORT_TYPE_INPUT, HID_CC_IN_RPT_LEN , media_report);
      }
    }
  }
  }
  else {
    ESP_LOGE(LOG_TAG,"ble hid queue not initialized, retry in 1s");
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }

}

/** @brief Activate/deactivate pairing mode
 * @param enable If set to != 0, pairing will be enabled. Disabled if == 0
 * @return ESP_OK on success, ESP_FAIL otherwise*/
esp_err_t halBLESetPairing(uint8_t enable);

/** @brief Get connection status
 * @return 0 if not connected, != 0 if connected */
uint8_t halBLEIsConnected(void)
{
  if(sec_conn == false) return 0;
  else return 1;
}

/** @brief En- or Disable BLE interface.
 * 
 * This method is used to enable or disable the BLE interface. Currently, the ESP32
 * cannot use WiFi and BLE simultaneously. Therefore, when enabling wifi, it is
 * necessary to disable BLE prior calling taskWebGUIEnDisable.
 * 
 * @note Calling this method prior to initializing BLE via halBLEInit will
 * result in an error!
 * @return ESP_OK on success, ESP_FAIL otherwise
 * @param onoff If != 0, switch on BLE, switch off if 0.
 * */
esp_err_t halBLEEnDisable(int onoff) {
  return ESP_OK;
}


/** @brief Main init function to start HID interface (C interface)
 * @see hid_ble */
esp_err_t halBLEInit(uint8_t enableKeyboard,uint8_t enableMedia, uint8_t enableMouse, uint8_t enableJoystick)
{
  activateKeyboard = enableKeyboard;
  activateMedia = enableMedia;
  activateMouse = enableMouse;
  activateJoystick = enableJoystick;

	//initialise queues, even if they might not be used.
	mouse_q = xQueueCreate(32,HID_MOUSE_IN_RPT_LEN*sizeof(uint8_t));
	keyboard_q = xQueueCreate(32,REPORT_LEN*sizeof(uint8_t));
	joystick_q = xQueueCreate(32,HID_JOYSTICK_IN_RPT_LEN*sizeof(uint8_t));
	media_q = xQueueCreate(32,HID_CC_IN_RPT_LEN*sizeof(uint8_t));
    
  // Initialize NVS.
  esp_err_t ret = nvs_flash_init();
  
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(LOG_TAG, "%s initialize controller failed\n", __func__);
    return ESP_FAIL;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (ret) {
    ESP_LOGE(LOG_TAG, "%s enable controller failed\n", __func__);
    return ESP_FAIL;
  }

  ret = esp_bluedroid_init();
  if (ret) {
    ESP_LOGE(LOG_TAG, "%s init bluedroid failed\n", __func__);
    return ESP_FAIL;
  }

  ret = esp_bluedroid_enable();
  if (ret) {
    ESP_LOGE(LOG_TAG, "%s init bluedroid failed\n", __func__);
    return ESP_FAIL;
  }
  
  if(!hidd_le_env.enabled) {
    memset(&hidd_le_env, 0, sizeof(hidd_le_env_t));
    hidd_le_env.enabled = true;
  }
  
  esp_ble_gap_register_callback(gap_event_handler);
  
  hidd_le_env.hidd_cb = hidd_event_callback;
  if(hidd_register_cb() != ESP_OK) {
    ESP_LOGE(LOG_TAG,"register CB failed");
    return ESP_FAIL;
  }
  esp_ble_gatts_app_register(BATTRAY_APP_ID);
  if(esp_ble_gatts_app_register(HIDD_APP_ID) != ESP_OK) {
    ESP_LOGE(LOG_TAG,"Register App failed");
    return ESP_FAIL;
  }

  /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
  esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND;     //bonding with peer device after authentication
  esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
  uint8_t key_size = 16;      //the key size should be 7~16 bytes
  uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
  /* If your BLE device act as a Slave, the init_key means you hope which types of key of the master should distribut to you,
  and the response key means which key you can distribut to the Master;
  If your BLE device act as a master, the response key means you hope which types of key of the slave should distribut to you, 
  and the init key means which key you can distribut to the slave. */
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
  
  //create BLE task
  //xTaskCreate(&halBLETask, "ble_task", TASK_BLE_STACKSIZE, NULL, HAL_BLE_TASK_PRIORITY_BASE, NULL);
  TaskHandle_t xBLETask_keyboard;
  TaskHandle_t xBLETask_mouse;
  TaskHandle_t xBLETask_media;
  TaskHandle_t xBLETask_joystick;

  xTaskCreatePinnedToCore(halBLETask_keyboard, "ble_task_keyboard", TASK_BLE_STACKSIZE, NULL, configMAX_PRIORITIES, &xBLETask_keyboard, 0);
  xTaskCreatePinnedToCore(halBLETask_mouse, "ble_task_mouse", TASK_BLE_STACKSIZE, NULL, configMAX_PRIORITIES, &xBLETask_mouse, 0);
  xTaskCreatePinnedToCore(halBLETask_media, "ble_task_media", TASK_BLE_STACKSIZE, NULL, configMAX_PRIORITIES, &xBLETask_media, 0);
  xTaskCreatePinnedToCore(halBLETask_joystick, "ble_task_joystick", TASK_BLE_STACKSIZE, NULL, configMAX_PRIORITIES, &xBLETask_joystick, 0);
  
  //set log level according to define
  esp_log_level_set(HID_LE_PRF_TAG,LOG_LEVEL_BLE);
  esp_log_level_set(LOG_TAG,LOG_LEVEL_BLE);
  
  return ESP_OK;
}
