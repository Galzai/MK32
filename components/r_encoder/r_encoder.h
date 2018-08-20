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

#ifdef __cplusplus
}
#endif

#endif /* R_ENCODER_H_ */
