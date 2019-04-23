/*
 * keymap.h
 *
 *  Created on: 20 Sep 2018
 *      Author: gal
 */

#ifndef MAIN_KEYMAP_H_
#define MAIN_KEYMAP_H_

#include "keyboard_config.h"

extern uint16_t (*default_layouts[])[MATRIX_ROWS][KEYMAP_COLS];

extern uint16_t default_encoder_map[LAYERS][ENCODER_SIZE];
extern uint16_t default_slave_encoder_map[LAYERS][ENCODER_SIZE];

#endif /* MAIN_KEYMAP_H_ */
