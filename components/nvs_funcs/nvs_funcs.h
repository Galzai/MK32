/*
 * nvs_funcs.h
 *
 *  Created on: 13 Sep 2018
 *      Author: gal
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <keyboard_config.h>

#ifndef NVS_FUNCS_H_
#define NVS_FUNCS_H_

#define NVS_CONFIG_OK 1
#define NVS_CONFIG_ERR 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @load the layouts from nvs
 */
void nvs_load_layouts(void);

/*
 * @read a layout from nvs
 */
void nvs_read_layout(const char* layout_name,uint16_t buffer[MATRIX_ROWS][KEYMAP_COLS]);

/*
 * @add a layout to nvs or overwrite existing one
 */
void nvs_write_layout(uint16_t layout[MATRIX_ROWS][KEYMAP_COLS],const char* layout_name);

/*
 * @brief read keyboard configuration from nvs
 */
void nvs_read_keymap_cfg(void);

/*
 * @brief write keyboard configuration to nvs (without keymaps)
 */
void nvs_write_keymap_cfg(uint8_t layers, char (*layer_names)[MAX_LAYOUT_NAME_LENGTH]);



#ifdef __cplusplus
}
#endif

#endif /* NVS_FUNCS_H_ */
