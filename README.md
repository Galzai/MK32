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
- Modify keboard_config.h for your desired keyboard size, features, gpio pins etc.
- Modify keymap.c for your desired layouts (multiple layers in progress).

## To Implement:
- Layers
- Split functionlity (with multiple pad selection).



