idf_component_register(SRCS "testMK32.cpp" "keymap.c" "matrix.c" "keypress_handles.c"
                    INCLUDE_DIRS "."
                    REQUIRES "battery" "ble" "oled" "nvsfuncs" "plugins" "espnowmk32" "rotencoder")
