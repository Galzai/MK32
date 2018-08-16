# MK32 - Simple BLE keyboard for ESP32
ESP32 implementation for an HID over GATT Keyboard (Bluetooth Low Energy).  
Any tips for improvments would be greatly appriciated.

### Many thanks to:
- Benjamin Aigner for his work on implementing HID over BLE on the ESP32: https://github.com/asterics/esp32_mouse_keyboard.
- Neil Kolban for his great contributions to the ESP32 SW (in particular the Bluetooth support): https://github.com/nkolban
- QMK for their layouts and inspiration for this project: https://github.com/qmk/qmk_firmware/


## Defining your keyboard
In order to wire your desired keyboard (if handwired) I reccomend checking out:
https://geekhack.org/index.php?topic=87689.0

In order to define your keyboard:
- Modify keboard_config.h for your desired keyboard size, features, etc.
- modify matrix.c for your gpio layout
- Modify keymap.c for your desired layouts.

### For split keyboards:
- When uploading code to main board, make sure MASTER and SPLIT_MASTER are defined in keyboard_config.h.
- When uploading code to slave board, make sure SLAVE is defined in keyboard_config.h, currently you will also need to update the correct Mac adress of your main board in espnow_send.c.

## To Implement:
- more than 2 pads (currently supports 2 pads).
- macros.
- battery level indication.
- connection switching.
- potentiometer (volume etc).
- modify keymap via webserver.
- security (?).

