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
 * Copyright 2017 Benjamin Aigner <aignerb@technikum-wien.at,
 * beni@asterics-foundation.org>
 */

/**
 * @file
 * @brief Common data structs and IPC pointers for the firmware
 * 
 * This file contains common structs, which are used for inter-process-communication
 * within the firmware.
 * 
 * In addition, virtual button assignment is done here, depending on
 * device configuration (this firmware can be built for a FABI or a FLipMouse)
 * */

#ifndef FUNCTION_COMMON_H_
#define FUNCTION_COMMON_H_

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/portmacro.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_log.h>
#include <esp_event.h>

#include "keyboard.h"
#include "driver/rmt.h"

/** @brief Enable v2.5 compatibility
 * 
 * This define activates a few behavioural changes in the firmware
 * to be fully compatible to v2.5 GUI + firmware, although some
 * of this behaviour is inconsistent and might be changed in future releases.
 * 
 * Following changes are made by this define:
 * 
 * * AT LA initializes a slot 0 with mouse function, if called after
 * an AT DE command.
 * 
 */
#define ACTIVATE_V25_COMPAT

/** ID String to be printed on "AT ID" command.
 * @todo Change this back to new version, when GUI is compatible */
//#define IDSTRING "FLipMouse V3.0\r\n"
#define IDSTRING "Flipmouse v2.7"
//#define IDSTRING "FABI v3.0\n"

/** @brief Determine used device. Either FABI or FLipMouse.
 * 
 * @note Define one of these, otherwise it will result in a compile error.
 * */
//#define DEVICE_FABI
#define DEVICE_FLIPMOUSE

/** @brief Set count of used Neopixels
 * 
 * If LED_USE_NEOPIXEL is set, this define is used to determine the
 * count of NEOPIXEL LEDs, which are used for color output
 * */
#define LED_NEOPIXEL_COUNT  1

/** @brief Maximum length for a slot name */
#define SLOTNAME_LENGTH   32

/** @brief Maximum size of ANY virtual button config.
 * @warning If this size is set to a value smaller than a VB config, this
 * config will be truncated in flash memory!
 * @note Currently the taskKeyboardConfig_t is the biggest struct with 100Bytes 
 * @see halStorageStoreSetVBConfigs
 * @see halStorageLoadGetVBConfigs*/
#define VB_MAXIMUM_PARAMETER_SIZE 128

/** @brief maximum length for an AT command (including parameters & 'AT ', e.g., macro text) */
#define ATCMD_LENGTH   256

/** @brief Bitmask for testing if a function task should send data to USB queues */
#define DATATO_USB (1<<7)
/** @brief Bitmask for testing if a function task should send data to BLE queues */
#define DATATO_BLE (1<<6)
/** @brief Bitmask used to signal a FLipMouse (no FABI!) in CIM mode (no USB/BLE functionality) */
#define DATATO_CIM (1<<5)
/** @brief Bitmask used to signal a FLipMouse/FABI has an active Wifi server */
#define WIFI_ACTIVE (1<<4)
/** @brief Bitmask used to signal a FLipMouse/FABI has at least one connected Wifi client */
#define WIFI_CLIENT_CONNECTED (1<<3)

/** @brief Flag for signalling config updates
 * 
 * This flag is used to signal a config change.
 * If this flag is set, the system will receive a lot of AT commands
 * at once (config is saved in a .set file with AT commands).
 * Therefore, a few features can be disabled while this flag is set:
 * * Calibrating: ADC calibration can be done after config load, only one time
 * * Connection reset
 * * Debouncing: after this bit is cleared, all debouncer channels should be reset
 * * task_hid: we don't want to send out any spurious hid commands
 * * task_vb: we don't want to send out any spurious commands
 * 
 * @note Due to the limitation of FreeRTOS, which cannot block for clearing bits,
 * we use the flag SYSTEM_STABLECONFIG for the exact counterpart.
 * @see SYSTEM_STABLECONFIG
 * */
#define SYSTEM_LOADCONFIG (1<<0)

/** @brief Stable & loaded config.
 * Exact counterpart of SYSTEM_LOADCONFIG
 * @see SYSTEM_LOADCONFIG
 * */
#define SYSTEM_STABLECONFIG (1<<1)

/** @brief AT command queue is empty */
#define SYSTEM_EMPTY_CMD_QUEUE (1<<2)

/** @brief This flag group is used to determine the routing
 * of different data to either USB, BLE or both, as well as the wifi status. 
 * 
 * @see DATATO_USB
 * @see DATATO_BLE
 * @see DATATO_CIM
 * @see WIFI_ACTIVE
 * @see WIFI_CLIENT_CONNECTED
 * */
extern EventGroupHandle_t connectionRoutingStatus;

/** @brief General system status 
 * 
 * Flag group signalling the general system status.
 * 
 * @note Detailed status description is given in the flag defines!
 * @see SYSTEM_LOADCONFIG
 * @see SYSTEM_STABLECONFIG
 * @see SYSTEM_EMPTY_CMD_QUEUE
 */
extern EventGroupHandle_t systemStatus;

/** @brief Queue for sending HID commands to USB */
extern QueueHandle_t hid_usb;
/** @brief Queue for sending HID commands to BLE */
extern QueueHandle_t hid_ble;

/** @brief Queue to receive config changing commands. 
 * 
 * A string is passed to this queue with a maximum length of SLOTNAME_LENGTH.
 * 
 * Either a special command is passed or the slotname which should be loaded.
 * Possible strings to send to this queue:
 * * Name of the slot to be loaded
 * * "__NEXT" for next slot
 * * "__PREVIOUS" for previous slot
 * * "__DEFAULT" for default slot
 * * "__RESTOREFACTORY" to delete all slots & reset default 
 *    slot to factory defaults
 * * "__UPDATE" to reload tasks based on currentConfig. Used to
 *  update the configuration when changed by configuration software/GUI.
 * 
 * @see SLOTNAME_LENGTH
 * @see configSwitcherTask
 * @see currentConfig
 **/
extern QueueHandle_t config_switcher;

/** @brief Queue for sending triggered actions to the debouncer task.
 * 
 * If some input action happens (most likely either hal_io or hal_adc),
 * an element of type raw_action_t can be sent to this queue.
 * The debouncer will get these elements & start the debouncing timer
 * accordingly. If the time has passed, the event will be dispatched
 * via the system event loop.
 * @see raw_action_t
 * @see VB_EVENT
 * @see VB_PRESS_EVENT
 * @see VB_RELEASE_EVENT
 * */
extern QueueHandle_t debouncer_in;

/*++++ VIRTUAL BUTTON ASSIGNMENT ++++*/
/** this section assigns the virtual buttons to each part
 * of the FLipMouse/FABI (on a FABI device, of course each external button
 * represents one virtual button).
 * 
 * Currently used mappings:
 * Mouthpiece - Alternative mode
 * External+Internal buttons
 * 
 * Planned for future release:
 * Gesture VBs, a recorded gesture is treated like a virtual button
 * */
#ifdef DEVICE_FLIPMOUSE
/** @brief Internal button (short nose), activates Wifi on a long press */
#define VB_INTERNAL2    0
/** @brief Internal button (long button) */
#define VB_INTERNAL1    1
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL1    2
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL2    3
/** @brief Mouthpiece up (threshold mode only) */
#define VB_UP           4
/** @brief Mouthpiece down (threshold mode only) */
#define VB_DOWN         5
/** @brief Mouthpiece left (threshold mode only) */
#define VB_LEFT         6
/** @brief Mouthpiece right (threshold mode only) */
#define VB_RIGHT        7
/** @brief Sip on the mouthpiece (weak) */
#define VB_SIP          8
/** @brief Sip on the mouthpiece (strong)
 * @note If one of the VB_STRONGSIP_* actions is used, this one will not be triggered. */
#define VB_STRONGSIP    9
/** @brief Puff into the mouthpiece (weak) */
#define VB_PUFF         10
/** @brief Sip on the mouthpiece (strong)
 * @note If one of the VB_STRONGPUFF_* actions is used, this one will not be triggered. */
#define VB_STRONGPUFF   11
/** @brief Strongsip + move mouthpiece up afterwards.
 * @note If used, deactivates VB_STRONGSIP */
#define VB_STRONGSIP_UP     12
/** @brief Strongsip + move mouthpiece down afterwards.
 * @note If used, deactivates VB_STRONGSIP */
#define VB_STRONGSIP_DOWN   13
/** @brief Strongsip + move mouthpiece left afterwards.
 * @note If used, deactivates VB_STRONGSIP */
#define VB_STRONGSIP_LEFT   14
/** @brief Strongsip + move mouthpiece right afterwards.
 * @note If used, deactivates VB_STRONGSIP */
#define VB_STRONGSIP_RIGHT  15
/** @brief Strongpuff + move mouthpiece up afterwards.
 * @note If used, deactivates VB_STRONGPUFF */
#define VB_STRONGPUFF_UP    16
/** @brief Strongpuff + move mouthpiece down afterwards.
 * @note If used, deactivates VB_STRONGPUFF */
#define VB_STRONGPUFF_DOWN  17
/** @brief Strongpuff + move mouthpiece left afterwards.
 * @note If used, deactivates VB_STRONGPUFF */
#define VB_STRONGPUFF_LEFT  18
/** @brief Strongpuff + move mouthpiece right afterwards.
 * @note If used, deactivates VB_STRONGPUFF */
#define VB_STRONGPUFF_RIGHT 19
/** @brief Limiter value, used for checking&accessing any arrays
 * @note Set this value accordingly! */
#define VB_MAX          20
#endif

#ifdef DEVICE_FABI
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL1    0
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL2    1
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL3    2
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL4    3
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL5    4
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL6    5
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL7    6
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL8    7
/** @brief External button (connected via a 3.5mm jack plug) */
#define VB_EXTERNAL9    8
/** @brief Internal button (long nose), activates Wifi on a long press */
#define VB_INTERNAL1    9
/** @brief Sip on the mouthpiece (weak) */
#define VB_SIP          10
/** @brief Puff into the mouthpiece (weak) */
#define VB_PUFF         11
/** @brief Sip on the mouthpiece (strong) */
#define VB_STRONGSIP    12
/** @brief Puff into the mouthpiece (strong) */
#define VB_STRONGPUFF   13
/** @brief Limiter value, used for checking&accessing any arrays
 * @note Set this value accordingly! */
#define VB_MAX          14
#endif

/** @brief Special virtual button, which is used to trigger an action
 * immediately, instead of attaching it to a VB. */
#define VB_SINGLESHOT   32

/** @brief Event ID for the event loop of press/release events for VBs
 * 
 * If there is a button pressed, the sip/puff is triggered,..., an event
 * will be posted (by the debouncer) to the system event queue.
 * Any task can subscribe to these events, currently these are task_vb
 * and task_hid.
 * @note This enum is used for sending an action to the debouncer as well.*/
typedef enum {
	/** Press event issued */
	VB_PRESS_EVENT,
	/** Release event issued */
	VB_RELEASE_EVENT
} vb_event_t;

/** @brief Declaring a new event base for VB actions */
ESP_EVENT_DECLARE_BASE (VB_EVENT);

/*++++ TASK PRIORITY ASSIGNMENT ++++*/
/** @brief ADC task priority. Not high. */
#define HAL_ADC_TASK_PRIORITY     (tskIDLE_PRIORITY + 2)
/** @brief Debouncer task priority. Highest priority (for short response time) */
#define DEBOUNCER_TASK_PRIORITY  (configMAX_PRIORITIES)
/** @brief BLE task priority. Not high. */
#define HAL_BLE_TASK_PRIORITY_BASE  (tskIDLE_PRIORITY + 2)
/** @brief Config switcher task priority. Higher than basic tasks. */
#define HAL_CONFIG_TASK_PRIORITY  (tskIDLE_PRIORITY + 5)
/** @brief Command parser task priority. Higher than basic tasks. */
#define TASK_COMMANDS_PRIORITY  (tskIDLE_PRIORITY + 6)

/*++++ MAIN CONFIG STRUCT ++++*/

/**
 * @brief Mode of operation for the mouthpiece
 * @see VB_UP
 * @see VB_DOWN
 * @see VB_LEFT
 * @see VB_RIGHT
 * */
typedef enum mouthpiece_mode {
	/** Do not do anything with the mouthpiece */
	NONE,
	/** Mouthpiece controls mouse cursor, <br>
	 *            Used parameters: <br>
	 *              * max_speed
	 *              * acceleration
	 *              * deadzone_x/y
	 *              * sensitivity_x/y
	 */
	MOUSE,
	/** Mouthpiece controls two joystick axis <br>
	 *            Used parameters <br>
	 *              * deadzone_x/y
	 *              * sensitivity_x/y
	 *              * axis
	 */
	JOYSTICK,
	/** Mouthpiece triggers virtual buttons <br>
	 *            Used parameters <br>
	 *              * deadzone_x/y
	 *              * threshold_x/y
	 */
	THRESHOLD
} mouthpiece_mode_t;

/**
 * @brief Config for the ADC task & the analog mode of operation
 * 
 * @todo describe all parameters of this struct.
 * 
 * @todo Remove report raw from here & create new "volatile" config struct -> no need to save, but no need to overwrite on slot change
 * @see mouthpiece_mode_t
 * @see VB_SIP
 * @see VB_PUFF
 * @see VB_STRONGSIP
 * @see VB_STRONGPUFF
 * */
typedef struct adc_config {
	/** mode setting for mouthpiece, @see mouthpiece_mode_t */
	mouthpiece_mode_t mode;
	/** acceleration for x & y axis (mouse & joystick mode) */
	uint8_t acceleration;
	/** max speed for x & y axis (mouse & joystick mode) */
	uint8_t max_speed;
	/** deadzone values for x & y axis (mouse & joystick & threshold mode) */
	uint8_t deadzone_x;
	uint8_t deadzone_y;
	/** sensitivity values for x & y axis (mouse & joystick mode) */
	uint8_t sensitivity_x;
	uint8_t sensitivity_y;
	/** pressure sensor, sip threshold */
	uint16_t threshold_sip;
	/** pressure sensor, puff threshold */
	uint16_t threshold_puff;
	/** pressure sensor, strongsip threshold */
	uint16_t threshold_strongsip;
	/** pressure sensor, strongpuff threshold */
	uint16_t threshold_strongpuff;
	/** Enable report RAW values (!=0), values are sent via halSerialSendUSBSerial */
	uint8_t reportraw;
	/** joystick axis assignment TBD: assign axis to numbers*/
	uint8_t axis;
	/** FLipMouse orientation, 0,90,180 or 270° */
	uint16_t orientation;
	/** On-the-fly calibration, count of idle events before triggering calibration */
	uint8_t otf_count;
	/** On-the-fly calibration, level of detecting idle (all raw values need to change less
	 * than this value to be detected as idle) */
	uint8_t otf_idle;
} adc_config_t;

/** @brief Type of VB command
 * @see vb_cmd_t */
typedef enum {
	T_CONFIGCHANGE = 1, /** @brief Config change request */
	T_CALIBRATE, /** @brief Calibrationrequest */
	T_SENDIR, /** @brief Send an IR command */
	T_MACRO /** @brief Trigger macro execution */
} vb_cmd_type_t;

/** @brief Complete configuration for the current settings.
 * 
 * This struct contains all necessary settings for a complete device
 * setup. It will be loaded on a slot change (or startup).
 * 
 * @note This struct contains only settings which will be stored/loaded from flash.
 */
typedef struct generalConfig {
	/** @brief Versionnumber for this slot.
	 * @note Currently unused */
	uint32_t slotversion;
	/** @brief ADC related config */
	adc_config_t adc;
	/** @brief Flag for active BLE-HID */
	uint8_t ble_active;
	/** @brief Flag for active USB-HID */
	uint8_t usb_active;
	/** @brief Mouse wheel: stepsize */
	uint8_t wheel_stepsize;
	/** @brief keyboard locale to be used by BLE&USB(serial) HID */
	uint8_t locale;
	/** @brief Timeout between IR edges before command is declared as finished */
	uint8_t irtimeout;
	/** @brief Global anti-tremor time for press */
	uint16_t debounce_press;
	/** @brief Global anti-tremor time for release */
	uint16_t debounce_release;
	/** @brief Global anti-tremor time for idle */
	uint16_t debounce_idle;
	/** @brief Enable/disable button learning mode
	 * @todo Move to "volatile" storage, independent from slot change */
	uint8_t button_learn;
	/** @brief Feedback mode.
	 * 
	 * * 0 disables LED and buzzer
	 * * 1 disables buzzer, but LED is on
	 * * 2 disables LED, but buzzer gives output
	 * * 3 gives LED and buzzer feedback
	 * */
	uint8_t feedback;
	/** @brief Anti-tremor (debounce) time for press of each VB */
	uint16_t debounce_press_vb[VB_MAX];
	/** @brief Anti-tremor (debounce) time for release of each VB */
	uint16_t debounce_release_vb[VB_MAX];
	/** @brief Anti-tremor (debounce) time for idle of each VB */
	uint16_t debounce_idle_vb[VB_MAX];
	/** @brief Slotname of this config */
	char slotName[SLOTNAME_LENGTH];
} generalConfig_t;

/** @brief One VB command (not HID), maybe an element of a command chain 
 * 
 * This struct is used for VB commands, which are not HID commands (e.g.
 * calibrate, IR recv/send)

 * @see task_vb_addCmd
 * @see task_vb_clearCmds
 * @see task_vb_getCmdChain
 * @see task_vb_setCmdChain */
typedef struct vb_cmd {
	/** @brief Number of virtual button. MSB signals if this is a press or
	 * release action:
	 * * If it is set (mask: 0x80), it is a press action
	 * * If it is cleared, it is a release action 
	 * */
	uint8_t vb;
	/** @brief Type of command */
	vb_cmd_type_t cmd;
	/** @brief Original AT command string, might be NULL if not used.
	 * @note This value is used to store a command. If it is NULL,
	 * this command cannot be stored. */
	char *atoriginal;
	/** @brief Parameter string, e.g. for slot names. Might be NULL if not necessary */
	char *cmdparam;
	/** @brief Pointer to next VB command element, might be NULL. 
	 * @note This pointer is set to NULL as long as it is not added to the command chain. */
	struct vb_cmd *next;
} vb_cmd_t;

/** @brief  One HID command, maybe an element of a command chain
 *
 * This struct is used either as one element to be passed to hal_serial
 * or the BLE class OR it is used to form a chained list of all HID
 * commands, which are currently active. task_hid takes care
 * of maintaining a list of all active VBs, if one gets triggered (via
 * task_debouncer), the HID task walks through this list for one or more
 * (in case of multiple press/release actions) HID commands, which are
 * sent to the BLE/USB HAL.
 * @see task_hid_addCmd
 * @see task_hid_clearCmds
 * @see task_hid_getCmdChain
 * @see task_hid_setCmdChain */
typedef struct hid_cmd {
	/** @brief Number of virtual button. MSB signals if this is a press or
	 * release action:
	 * * If it is set (mask: 0x80), it is a press action
	 * * If it is cleared, it is a release action 
	 * */
	uint8_t vb;
	/** @brief Command to be sent, see HID_kbdmousejoystick.cpp or the
	 * usb_bridge for explanations. */
	uint8_t cmd[3];
	/** @brief Original AT command string, might be NULL if not used.
	 * @note This value is used to store a command. If it is NULL,
	 * this command cannot be stored. */
	char *atoriginal;
	/** @brief Pointer to next HID command element, might be NULL. 
	 * @note This pointer is set to NULL as long as it is not added to the command chain. */
	struct hid_cmd *next;
} hid_cmd_t;

/** @brief State of IR receiver
 * @see TASK_HAL_IR_RECEV_MINIMUM_EDGES
 * @see TASK_HAL_IR_RECV_MAXIMUM_EDGES*/
typedef enum irstate {
	/** @brief Nothing is done, this halIOIR_t struct is not used for receiving */
	IR_IDLE,
	/** @brief Receiver is active and storing */
	IR_RECEIVING,
	/** @brief If timeout was triggered and not enough edges are detected */
	IR_TOOSHORT,
	/** @brief If timeout was triggered and enough edges were stored */
	IR_FINISHED,
	/** @brief Too many edges were detected, could not store */
	IR_OVERFLOW
} irstate_t;

/** @brief Struct for sending/receiving an IR command */
typedef struct halIOIR {
	/** @brief Buffer for IR signal
	 * @warning Do not free this buffer! It will be freed by transmitting function
	 * @note In case of receiving, this buffer can be freed. */
	rmt_item32_t *buffer;
	/** @brief Count of rmt_item32_t items */
	uint16_t count;
	/** @brief Status of receiver */
	irstate_t status;
} halIOIR_t;

/** @brief RAW VB action type, sent to debouncer_in queue.
 * @see debouncer_in
 */
typedef struct raw_action {
	/** @brief VB number
	 * @note We don't use the MSB for signalling press/release here. */
	uint32_t vb;
	/** @brief Type of event */
	vb_event_t type;
	/** @brief Any additional payload.
	 * @note Currently unused. Might be used in future versions */
	void *payload;
} raw_action_t;

/** @brief Strips away \\r\\t and \\n */
void strip(char *s);

/** @brief NVS key for wifi password */
#define NVS_WIFIPW  "nvswifipw"

/** @brief Minutes between last client disconnected and WiFi is switched off */
#define WIFI_OFF_TIME 5

/** @brief printf pattern for 8bit binary output.
 * @note Stackoverflow: https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
 * @note Usage: printf("Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte)); OR
 * printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
 BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
 */
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c" 

/** @brief printf parameter for 8bit binary output.
 * @note Stackoverflow: https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
 * printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
 BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
 */
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#endif /*FUNCTION_COMMON_H_*/
