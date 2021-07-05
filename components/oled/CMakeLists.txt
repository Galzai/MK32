idf_component_register(SRCS "oled_tasks.c"
                    INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/main"
                    REQUIRES "battery" "nvsfuncs")
