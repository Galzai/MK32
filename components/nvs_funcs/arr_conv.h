#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <keyboard_config.h>

#ifndef ARR_CONV_H_
#define ARR_CONV_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * @convert string array to single string
 */
void str_arr_to_str(char (*layer_names)[MAX_LAYOUT_NAME_LENGTH], uint8_t layers, char **buffer);

/*
 * @convert string to string array
 */
void str_to_str_arr(char *str, uint8_t layers,char ***buffer);

#ifdef __cplusplus
}
#endif

#endif /* ARR_CONV_H_ */
