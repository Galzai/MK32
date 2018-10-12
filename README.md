# MK32 - Simple BLE keyboard for ESP32
MK32 is a BLE keyboard firmware for ESP32 Microcontrollers,designed to be relativley easily customizable.  
Please refer to the [MK32 Wiki page](https://github.com/Galzai/MK32/wiki) for information about building your own keyboard.

### Many thanks to:
- Benjamin Aigner for his work on implementing HID over BLE on the ESP32: https://github.com/asterics/esp32_mouse_keyboard.
- Neil Kolban for his great contributions to the ESP32 SW (in particular the Bluetooth support): https://github.com/nkolban
- QMK for their layouts and inspiration for this project: https://github.com/qmk/qmk_firmware/

## Features:
- Split Keyboards
- I2C/SPI displays
- Customizable layouts
- Macros
- Rotary Encoders (With customizable encoder commands)
- Battery Monitoring
- Power Management

## To-do:
- Plugin API support (in progress).
- Proper debouncing (Temporerly using delays).
- More than 2 pads (currently supports 2 pads).
- Connection switching.
- Modify keymap via webserver (in progress) .
- Security (?)
