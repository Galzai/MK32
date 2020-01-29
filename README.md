# MK32 - Simple BLE keyboard for ESP32
Note: Currently tested and working on esp-idf v4.0
MK32 is a BLE keyboard firmware for ESP32 Microcontrollers,designed to be relativley easily customizable.  
Please refer to the [MK32 Wiki page](https://github.com/Galzai/MK32/wiki) for information about building your own keyboard.
Please make sure esp-idf is up-to-date before building and flashing.

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
- Basic plugin support

## To-do:
- Update esp-idf version to v4.1.
- Plugin API support(Implemented):
	-Currently only one plugin can run at a time (to modify).
- More than 2 pads with espnow (currently supports 2 pads) - Optional.
- Connection switching.
- Modify keymap via webserver:  
	* Functions for storing (and reading) AP data on flash complete.  
	* Starting web server from plugin complete.  
	* need to create proper web GUI for keymap modifications.  
- Wifi manager:   
	* Add plugin for adding AP.  
	* Functions for storing (and reading) AP from flash done.  
	* ESPNOW does not work if wifi AP is not on the same channel (Problematic for ESPNOW Based split boards).   

