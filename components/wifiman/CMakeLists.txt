idf_component_register(SRCS "wifi_manager.c"
                    INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/main"
                    REQUIRES nvs_flash plugins)
