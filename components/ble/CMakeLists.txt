idf_component_register(SRCS "keyboard.c" "hal_ble.c" "hid_dev.c" "hid_device_le_prf.c"
                       INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/main"
                       REQUIRES "bt" "battery")