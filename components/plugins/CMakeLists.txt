idf_component_register(SRCS "plugins.c" "layout_server.c" "testFunc.c" "plugin_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "hidkeycodeconv" "wifiman" "json")
