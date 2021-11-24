# MK32 - Simple BLE keyboard for ESP32

dungphan90's fork:
- Updated to ESP-IDF v4.4.
- Migrated to CMake build system.
- Added Docker to avoid dependency mess.
- Added keyboard config and layout for "CatiONE OLKB" (https://github.com/dungphan90/OLKBBLE.git).

### Easy build process with Docker.
```
git clone https://github.com/Galzai/MK32.git
cd MK32
chmod +x ./build.sh
chmod +x ./run.sh
./build.sh
./run.sh
```
Once you're inside the container, you can run `make` to buid, `make flash` to flash the firmware, and `make monitor` to run debug.
___

MK32 is a BLE keyboard firmware for ESP32 Microcontrollers,designed to be relatively easily customizable.  
Please refer to the [MK32 Wiki page](https://github.com/Galzai/MK32/wiki) for information about building your own keyboard.
Also note that this repository is not heavily maintained.

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
- Refactor some bad coding habits (magic numbers etc, sorry I started this project when I had 0 experience!). 
- Plugin API support(Implemented):  
	* Currently only one plugin can run at a time (to modify).
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

