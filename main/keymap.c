#ifndef KEYMAP_C
#define KEYMAP_C

#include "key_definitions.h"
#include "keyboard_config.h"

// A bit different from QMK, default return you to the first layer, LOWER and raise increase/lower layer by order.
#define DEFAULT 0x100
#define LOWER 0x101
#define RAISE 0x102

// Keymaps are designed to be relatively interchangeable with QMK
enum custom_keycodes {
  QWERTY ,
  COLEMAK,
  DVORAK,
};


/* select a keycode for your macro
 * important - first macro must be initialized as 0x103
 * */

#define MACROS_NUM 2
enum custom_macros {
  KC_CTRL_ALT_DELETE = 0x103 ,
  KC_ALT_F4,
};

/*define what the macros do
 * important- make sure you you put the macros in the same order as the their enumeration
 */
uint16_t macros[MACROS_NUM][3]={
// CTRL+ALT+DEL
{KC_LCTRL, KC_LALT,KC_DEL},
//ALT +F4
{KC_RALT, KC_LALT, KC_NO}
};

// Fillers to make layering more clear
#define _______ KC_TRNS
#define XXXXXXX KC_NO

// Each keymap is represented by an array, with an array that points to all the keymaps  by order
	 uint16_t _QWERTY[MATRIX_ROWS][KEYMAP_COLS]={

			/* Qwerty
			 * ,------------------------------------------------------------------------------------.
			 * | Esc  |   Q  |   W  |   E   |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Bksp |
			 * |------+------+------+-------+------+-------------+------+------+------+------+------|
			 * | Tab  |   A  |   S  |   D   |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  '   |
			 * |------+------+------+-------+------+------|------+------+------+------+------+------|
			 * | Shift|   Z  |   X  |   C   |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
			 * |------+------+------+-------+------+------+------+------+------+------+------+------|
			 * | Ctrl | GUI  |  Alt |Default|Lower |Space |Space |Raise | Left | Down |  Up  |Right |
			 * `------------------------------------------------------------------------------------'
			 */

			  {KC_ESC,  KC_AUDIO_MUTE,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC },
			  {KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT },
			  {KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT } ,
			  {KC_LCTRL,KC_LGUI, KC_LALT, DEFAULT,  LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};

	 uint16_t _COLEMAK[MATRIX_ROWS][KEYMAP_COLS]={

			/* Colemak
			 * ,------------------------------------------------------------------------------------.
			 * | Tab   |   Q  |   W  |   F  |   P  |   G  |   J  |   L  |   U  |   Y  |   ;  | Bksp |
			 * |------+------+------+------+------+-------------+------+------+------+-------+------|
			 * | Esc   |   A  |   R  |   S  |   T  |   D  |   H  |   N  |   E  |   I  |   O  |  '   |
			 * |------+------+------+------+------+------|------+------+------+------+-------+------|
			 * | Shift |   Z  |   X  |   C  |   V  |   B  |   K  |   M  |   ,  |   .  |   /  |Enter |
			 * |------+------+------+------+------+------+------+------+------+------+-------+------|
			 * |Default| Ctrl | Alt  | GUI  |Lower |Space |Space |Raise | Left | Down |  Up  |Right |
			 * `------------------------------------------------------------------------------------'
			 */

			  {KC_TAB,  KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,    KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN, KC_BSPC },
			  {KC_ESC,  KC_A,    KC_R,    KC_S,    KC_T,    KC_D,    KC_H,    KC_N,    KC_E,    KC_I,    KC_O,    KC_QUOT },
			  {KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT  } ,
			  {DEFAULT,  KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};

	 uint16_t _DVORAK[MATRIX_ROWS][KEYMAP_COLS]={

			/* Dvorak
			 * ,------------------------------------------------------------------------------------.
			 * | Tab   |   '  |   ,  |   .  |   P  |   Y  |   F  |   G  |   C  |   R  |   L  | Bksp |
			 * |------+-------+------+------+------+-------------+------+------+------+------+------|
			 * | Esc   |   A  |   O  |   E  |   U  |   I  |   D  |   H  |   T  |   N  |   S  |  /   |
			 * |------+-------+------+------+------+------|------+------+------+------+------+------|
			 * | Shift |   ;  |   Q  |   J  |   K  |   X  |   B  |   M  |   W  |   V  |   Z  |Enter |
			 * |------+-------+------+------+------+------+------+------+------+------+------+------|
			 * |Defaukt| Ctrl | Alt  | GUI  |Lower |Space |Space |Raise | Left | Down |  Up  |Right |
			 * `------------------------------------------------------------------------------------'
			 */

			  {KC_TAB,  KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,    KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_BSPC },
			  {KC_ESC,  KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,    KC_T,    KC_N,    KC_S,    KC_SLSH },
			  {KC_LSFT, KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_ENT  } ,
			  {DEFAULT,  KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};
// Create an array that points to the various keymaps

	 uint16_t (*layouts[])[MATRIX_ROWS][KEYMAP_COLS]={
			 &_QWERTY,
			 &_COLEMAK,
			 &_DVORAK
	 };

	 uint8_t current_layout =0;


#endif


