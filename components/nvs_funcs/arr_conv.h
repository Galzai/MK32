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
 * @brief convert blob to keymap matrix, copy the data to the buffer
 */
void blob_to_key_mat(uint16_t layout_arr[MATRIX_ROWS*KEYMAP_COLS],uint16_t buffer[MATRIX_ROWS][KEYMAP_COLS]);

/*
 * @brief convert keymap matrix to blob, copy the data to the buffer
 */
void key_mat_to_blob(uint16_t layout[MATRIX_ROWS][KEYMAP_COLS],uint16_t *buffer);

/*
 * @brief convert string array to single string, copy the data to the buffer
 */
void str_arr_to_str(char (*layer_names)[MAX_LAYOUT_NAME_LENGTH], uint8_t layers, char **buffer);
/*
 * @brief convert string to string array, copy the data to the buffer
 */
void str_to_str_arr(char *str, uint8_t layers,char ***buffer);

#ifdef __cplusplus
}
#endif

#endif /* ARR_CONV_H_ */
