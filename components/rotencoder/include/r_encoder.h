/*
 * r_encoder.h
 *
 *  Created on: 18 Aug 2018
 *      Author: gal
 */

#ifndef R_ENCODER_H_
#define R_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Setup the rotary encoder
 * */
void r_encoder_setup(void);

/** @brief check rotary encoder status
 * */
uint8_t r_encoder_state(void);

void r_encoder_command(uint8_t command, uint16_t encoder_commands[4]);

#ifdef __cplusplus
}
#endif

#endif /* R_ENCODER_H_ */
