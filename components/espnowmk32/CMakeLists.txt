idf_component_register(SRCS "espnow_receive.c" "espnow_send.c"
                    INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/main"
                    REQUIRES "nvs_flash" "ble" "rotencoder" "nvsfuncs" "esp_wifi" "freertos")
