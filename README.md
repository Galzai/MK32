# MK32 - Simple BLE keyboard for ESP32
ESP32 implementation for an HID over GATT Keyboard (Bluetooth Low Energy).  
Any tips for improvments would be greatly appriciated.

### Many thanks to:
- Benjamin Aigner for his work on implementing HID over BLE on the ESP32: https://github.com/asterics/esp32_mouse_keyboard.
- Neil Kolban for his great contributions to the ESP32 SW (in particular the Bluetooth support): https://github.com/nkolban
- QMK for their layouts and inspiration for this project: https://github.com/qmk/qmk_firmware/

### General Notes:
- For information regarding flashing ESP32 Firmware please visit: https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html
- In menuconfig make sure that : Components->FreeRTOS->TickRate is set to 1000Hz

## Defining your keyboard
In order to wire your desired keyboard (if handwired) I reccomend checking out:
https://geekhack.org/index.php?topic=87689.0

In order to define your keyboard:
- Modify keyboard_config.h for your desired keyboard size, features, etc.
- modify matrix.c for your gpio layout
- Modify keymap.c for your desired layouts and macros.

### For split keyboards:
- When uploading code to main board, make sure MASTER and SPLIT_MASTER are defined in keyboard_config.h.
- When uploading code to slave board, make sure SLAVE is defined in keyboard_config.h, currently you will also need to update the correct Mac adress of your main board in espnow_send.c.

### OLED Display:
- You can add an OLED display to your keyboard by defining ENABLE_OLED in keyboard_config.h and setting the correct i2c pins.
- The graphics are currently configured for an SSD1306 128x64 OLED but can be modified by changing your display in oled_tasks.c. please refer to the u8g2 wiki for additional information: https://github.com/olikraus/u8g2/wiki
- The oled currently responds relativley slowly (probably an issue with i2c over 100khz on the esp32).

### Battery Monitoring:
- Battery level indication is by approximate values (you can look at LiPo voltage discharge graphs and adjust using an appropriate voltage-discharge capacity table)
- You can add battery monitoring by selecting an adc pin and using a voltage divider.
- See battery_monitor.h for resistor values. 
- I have not tested the accuracy of the battery monitoring as of yet.


### For rotary encoders (for volume knob etc):
- Define R_ENCODER or (R_ENCODER_SLAVE if on slave pad) and set encoder pins in keyboard_config.h (undefine ENCODER_S_PIN if your encoder does not have a switch).
- You can modify encoder actions in keymap.c (set it as a mouse, consumer device, ordinary keyboard keys etc).

### Power consumption and deep sleep:
- For better battery usage you can define the number of inactive minutes you would like your device to wait before sleeping in keyboard_config.h. 
- Notice that waking from deep sleep on keypress is only possible on keys which connect rtc gpio pins (so make sure to have at least 1 rtc row pin, and 1 rtc col pin 
  for wake up keys of your choice), if you do not want to enable deep sleep simply undefine SLEEP_MINS.
- Notice that on windows reconnection after waking from deep sleep might take a couple of minutes, no problem on linux and android.
 (on windows you can rescan for bluetooth devices and it should recconect without waiting).
- Tested current (hopefully will improve in future): 
 40 mA for BLE (without split functionality). 
 100 mA with split mode enabled (possible to reduce if I change from ESP-NOW to BLE).  
 <1 mA while in deep sleep.


## To Implement:
- Proper debouncing (Temporerly using delays).
- More than 2 pads (currently supports 2 pads).
- Connection switching.
- Modify keymap via webserver.
- Security (?).

