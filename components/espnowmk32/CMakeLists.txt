idf_component_register(SRCS "espnow_receive.c" "espnow_send.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "nvs_flash" "ble" "rotencoder" "nvsfuncs")
