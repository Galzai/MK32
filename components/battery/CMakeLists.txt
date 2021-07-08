idf_component_register(SRCS "battery_monitor.c"
                    INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/main"
                    REQUIRES esp_adc_cal oled)
