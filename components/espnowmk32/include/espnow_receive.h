/*
 * espnow_split.h
 *
 *  Created on: 12 Aug 2018
 *      Author: gal
 */

#ifndef ESPNOW_RECEIVE_H_
#define ESPNOW_RECEIVE_H_

#include <freertos/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

//Queue for receiving report from slave
extern QueueHandle_t espnow_receive_q;

/** @Setup the function for receiving data via espnow
 * */
void espnow_receive(void);

#ifdef __cplusplus
}
#endif

#endif /* ESPNOW_SPLIT_H_ */
