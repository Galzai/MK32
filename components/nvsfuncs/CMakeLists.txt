idf_component_register(SRCS "nvs_funcs.c" "arr_conv.c"
                    INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/main"
                    REQUIRES nvs_flash oled)
