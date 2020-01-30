// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "hid_dev.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_log.h"

static hid_report_map_t *hid_dev_rpt_tbl;
static uint8_t hid_dev_rpt_tbl_Len;

static hid_report_map_t *hid_dev_rpt_by_id(uint8_t id, uint8_t type) {
	hid_report_map_t *rpt = hid_dev_rpt_tbl;

	for (uint8_t i = hid_dev_rpt_tbl_Len; i > 0; i--, rpt++) {
		if (rpt->id == id && rpt->type == type
				&& rpt->mode == hidProtocolMode) {
			return rpt;
		}
	}

	return NULL;
}

void hid_dev_register_reports(uint8_t num_reports, hid_report_map_t *p_report) {
	hid_dev_rpt_tbl = p_report;
	hid_dev_rpt_tbl_Len = num_reports;
	return;
}

void hid_dev_send_report(esp_gatt_if_t gatts_if, uint16_t conn_id, uint8_t id,
		uint8_t type, uint8_t length, uint8_t *data) {
	hid_report_map_t *p_rpt;

	// get att handle for report
	if ((p_rpt = hid_dev_rpt_by_id(id, type)) != NULL) {
		// if notifications are enabled
		ESP_LOGD(HID_LE_PRF_TAG, "%s(), send the report, handle = %d", __func__,
				p_rpt->handle);
		esp_ble_gatts_send_indicate(gatts_if, conn_id, p_rpt->handle, length,
				data, false);
	}

	return;
}


