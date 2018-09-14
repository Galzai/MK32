/*
 * config_structs.h
 *
 *  Created on: 14 Sep 2018
 *      Author: gal
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <keyboard_config.h>

#ifndef CONFIG_STRUCTS_H_
#define CONFIG_STRUCTS_H_

#ifdef __cplusplus
extern "C" {
#endif

//struct to hold the keymap config
typedef struct keymap_cfg {

	uint8_t layers;
	uint16_t (*layouts[])[MATRIX_ROWS][KEYMAP_COLS];
	uint8_t macros_num;
	uint16_t macros[macros_num][3];
	uint8_t encoder_map[LAYERS+1][4];
	uint8_t slave_encoder_map[LAYERS+1][4];


}keymap_cfg_t;

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_STRUCTS_H_ */
