/*
 * espnow_split.h
 *
 *  Created on: 12 Aug 2018
 *      Author: gal
 */

#ifndef ESPNOW_RECIEVE_H_
#define ESPNOW_RECIEVE_H_

#include <freertos/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

//Queue for recieving report from slave
extern QueueHandle_t espnow_recieve_q;



void espnow_recieve(void);

#ifdef __cplusplus
}
#endif

#endif /* ESPNOW_SPLIT_H_ */
