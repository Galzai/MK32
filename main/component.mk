#
# Main Makefile. This is basically the same as a component makefile.
#
# This Makefile should, at the very least, just include $(SDK_PATH)/make/component_common.mk. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#
COMPONENT_SRCDIRS +=../plugins/plugin_components/hid_keycode_conv
COMPONENT_SRCDIRS +=../plugins/plugin_components/wifi_manager
COMPONENT_SRCDIRS +=../plugins/plugin_components
COMPONENT_SRCDIRS +=../components/nvs_funcs
COMPONENT_SRCDIRS +=../components/battery_monitor
COMPONENT_SRCDIRS +=../components/u8g2_OLED
COMPONENT_SRCDIRS +=../components/r_encoder
COMPONENT_SRCDIRS +=../components/espnow
COMPONENT_SRCDIRS +=../components/nkolban_BLE

COMPONENT_ADD_INCLUDEDIRS=.
COMPONENT_ADD_INCLUDEDIRS +=../plugins/plugin_components/hid_keycode_conv
COMPONENT_ADD_INCLUDEDIRS +=../plugins/plugin_components/wifi_manager
COMPONENT_ADD_INCLUDEDIRS +=../plugins/plugin_components
COMPONENT_ADD_INCLUDEDIRS +=../components/nvs_funcs
COMPONENT_ADD_INCLUDEDIRS +=../components/battery_monitor
COMPONENT_ADD_INCLUDEDIRS +=../components/u8g2_OLED
COMPONENT_ADD_INCLUDEDIRS +=../components/r_encoder
COMPONENT_ADD_INCLUDEDIRS +=../components/espnow
COMPONENT_ADD_INCLUDEDIRS +=../components/nkolban_BLE
# include $(IDF_PATH)/make/component_common.mk
