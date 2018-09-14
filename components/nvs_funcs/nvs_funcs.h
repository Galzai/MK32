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
#include <config_structs.h>

#ifndef NVS_FUNCS_H_
#define NVS_FUNCS_H_

#define NVS_CONFIG_OK 1
#define NVS_CONFIG_ERR 0

#ifdef __cplusplus
extern "C" {
#endif


/*
 * @brief read keyboard configuration from nvs flash
 */
keymap_cfg_t read_nvs_keymap_cfg(void);

/*
 * @brief set new keyboard configuration in nvs flash
 */
void set_nvs_keymap_cfg(keymap_cfg_t keymap_cfg);

/*
 * @brief check if there is a saved keyboard configuration in nvs flash
 * if no configuration is found load default configuration
  */
uint8_t check_nvs_keymap_cfg(void);

#ifdef __cplusplus
}
#endif

#endif /* NVS_FUNCS_H_ */
