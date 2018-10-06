/*
 * keycode_conv.h
 *
 *  Created on: 6 Oct 2018
 *      Author: gal
 */


#ifndef KEYCODE_CONV_KEYCODE_CONV_H_
#define KEYCODE_CONV_KEYCODE_CONV_H_

#include "keyboard_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @pause sending key reports and use keypress to enter char
 */
void enable_key_to_char(void);

/*
 * @brief stop retrieving keypress to char and ireinit sending key reports
 */
void disable_key_to_char(void);

/*
 * @brief retrieve char corresponding to keycode
 */
char* keycode_to_char(void);

/*
 * @brief retrieve keycpde corresponding to char (or string)
 */
uint16_t char_to_keycode(void);

;

#ifdef __cplusplus
}
#endif


#endif /* KEYCODE_CONV_KEYCODE_CONV_H_ */
