#ifndef KEYMAP_C
#define KEYMAP_C

#include "key_definitions.h"
#include "keyboard_config.h"


// Keymaps are designed to be relatively interchangeable with QMK
enum custom_keycodes {
  QWERTY ,
  COLEMAK,
  DVORAK,
  LOWER,
  RAISE,
  ADJUST,
};

// Fillers to make layering more clear
#define _______ KC_TRNS
#define XXXXXXX KC_NO

// Each layout is represented by an array, with an array that points to all the layouts by order


	 uint16_t _QWERTY[MATRIX_ROWS][KEYMAP_COLS]={

			/* Qwerty
			 * ,-----------------------------------------------------------------------------------.
			 * | Esc  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Bksp |
			 * |------+------+------+------+------+-------------+------+------+------+------+------|
			 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  '   |
			 * |------+------+------+------+------+------|------+------+------+------+------+------|
			 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
			 * |------+------+------+------+------+------+------+------+------+------+------+------|
			 * | Ctrl | GUI  |  Alt |Adjust|Lower |Space |Space |Raise | Left | Down |  Up  |Right |
			 * `-----------------------------------------------------------------------------------'
			 */

			  {KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC },
			  {KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT },
			  {KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT } ,
			  {KC_LCTRL,KC_LGUI, KC_LALT, ADJUST,  LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};

	 uint16_t _COLEMAK[MATRIX_ROWS][KEYMAP_COLS]={

			/* Colemak
			 * ,-----------------------------------------------------------------------------------.
			 * | Tab  |   Q  |   W  |   F  |   P  |   G  |   J  |   L  |   U  |   Y  |   ;  | Bksp |
			 * |------+------+------+------+------+-------------+------+------+------+------+------|
			 * | Esc  |   A  |   R  |   S  |   T  |   D  |   H  |   N  |   E  |   I  |   O  |  '   |
			 * |------+------+------+------+------+------|------+------+------+------+------+------|
			 * | Shift|   Z  |   X  |   C  |   V  |   B  |   K  |   M  |   ,  |   .  |   /  |Enter |
			 * |------+------+------+------+------+------+------+------+------+------+------+------|
			 * |Adjust| Ctrl | Alt  | GUI  |Lower |Space |Space |Raise | Left | Down |  Up  |Right |
			 * `-----------------------------------------------------------------------------------'
			 */

			  {KC_TAB,  KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,    KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN, KC_BSPC },
			  {KC_ESC,  KC_A,    KC_R,    KC_S,    KC_T,    KC_D,    KC_H,    KC_N,    KC_E,    KC_I,    KC_O,    KC_QUOT },
			  {KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT  } ,
			  {ADJUST,  KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};

	 uint16_t _DVORAK[MATRIX_ROWS][KEYMAP_COLS]={

			/* Dvorak
			 * ,-----------------------------------------------------------------------------------.
			 * | Tab  |   '  |   ,  |   .  |   P  |   Y  |   F  |   G  |   C  |   R  |   L  | Bksp |
			 * |------+------+------+------+------+-------------+------+------+------+------+------|
			 * | Esc  |   A  |   O  |   E  |   U  |   I  |   D  |   H  |   T  |   N  |   S  |  /   |
			 * |------+------+------+------+------+------|------+------+------+------+------+------|
			 * | Shift|   ;  |   Q  |   J  |   K  |   X  |   B  |   M  |   W  |   V  |   Z  |Enter |
			 * |------+------+------+------+------+------+------+------+------+------+------+------|
			 * |Adjust| Ctrl | Alt  | GUI  |Lower |Space |Space |Raise | Left | Down |  Up  |Right |
			 * `-----------------------------------------------------------------------------------'
			 */

			  {KC_TAB,  KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,    KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_BSPC },
			  {KC_ESC,  KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,    KC_T,    KC_N,    KC_S,    KC_SLSH },
			  {KC_LSFT, KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_ENT  } ,
			  {ADJUST,  KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};


#endif


