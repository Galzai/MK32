#ifndef KEYMAP_C
#define KEYMAP_C

#include "key_definitions.h"
#include "keyboard_config.h"
#include "keymap.h"

// A bit different from QMK, default returns you to the first layer, LOWER and raise increase/lower layer by order.
#define DEFAULT 0x100
#define LOWER 0x101
#define RAISE 0x102

// Keymaps are designed to be relatively interchangeable with QMK
enum custom_keycodes {
  QWERTY ,
  NUM,
//  DVORAK,
};

//Set these for each layer and use when layers are needed in a hold-to use layer
enum layer_holds {
  QWERTY_H = LAYER_HOLD_BASE_VAL ,
  NUM_H,
};


// array to hold names of layouts for oled
char default_layout_names[LAYERS][MAX_LAYOUT_NAME_LENGTH] ={
		  "QWERTY" ,
		  "NUM",
//		  "DVORAK",
};



/* select a keycode for your macro
 * important - first macro must be initialized as MACRO_BASE_VAL
 * */

#define MACROS_NUM 2
enum custom_macros {
  KC_CTRL_ALT_DELETE = MACRO_BASE_VAL ,
  KC_ALT_F4,
};

/*define what the macros do
 * important- make sure you you put the macros in the same order as the their enumeration
 */
uint16_t macros[MACROS_NUM][MACRO_LEN]={
// CTRL+ALT+DEL
{KC_LCTRL, KC_LALT,KC_DEL},
//ALT +F4
{KC_RALT, KC_LALT, KC_NO}
};


/*Encoder keys for each layer by order, and for each pad
 * First variable states what usage the encoder has
 */

uint16_t default_encoder_map[LAYERS][ENCODER_SIZE] ={
		// |VOL + | VOL - | MUTE |
		{MEDIA_ENCODER,KC_AUDIO_VOL_UP, KC_AUDIO_VOL_DOWN,KC_AUDIO_MUTE},
		// |Y+|Y-| LEFT CLICK|
		{MOUSE_ENCODER,KC_MS_UP ,KC_MS_DOWN,KC_MS_BTN1}
};


uint16_t default_slave_encoder_map[LAYERS][ENCODER_SIZE] ={
		// |VOL + | VOL - | MUTE |
		{MEDIA_ENCODER,KC_AUDIO_VOL_UP, KC_AUDIO_VOL_DOWN,KC_AUDIO_MUTE},
		// |Y+|Y-| LEFT CLICK|
		{MOUSE_ENCODER,KC_MS_RIGHT ,KC_MS_LEFT,KC_MS_BTN2}
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

			  {KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC },
			  {KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT },
			  {KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT } ,
			  {KC_LCTRL,KC_LGUI, KC_LALT, DEFAULT, NUM_H,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};

	 uint16_t _NUM[MATRIX_ROWS][KEYMAP_COLS]={

			 /* Nums
			  * ,-----------------------------------------------------------------------------------.
			  * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
			  * |------+------+------+------+------+-------------+------+------+------+------+------|
			  * | Del  |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |   -  |   =  |   [  |   ]  |  \   |
			  * |------+------+------+------+------+------|------+------+------+------+------+------|
			  * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |ISO # |ISO / |Pg Up |Pg Dn |      |
			  * |------+------+------+------+------+------+------+------+------+------+------+------|
			  * |      |      |      |      |      |             |      |      |      |      |      |
			  * `-----------------------------------------------------------------------------------'
			  */

			  {KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC },
			  {KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS },
			  {KC_LSFT, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NUHS, KC_NUBS, KC_PGUP, KC_PGDN, KC_ENT  } ,
			  {KC_LCTRL,KC_LGUI, KC_LALT, DEFAULT, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }

	};

//	 uint16_t _DVORAK[MATRIX_ROWS][KEYMAP_COLS]={
//
//			/* Dvorak
//			 * ,------------------------------------------------------------------------------------.
//			 * | Tab   |   '  |   ,  |   .  |   P  |   Y  |   F  |   G  |   C  |   R  |   L  | Bksp |
//			 * |------+-------+------+------+------+-------------+------+------+------+------+------|
//			 * | Esc   |   A  |   O  |   E  |   U  |   I  |   D  |   H  |   T  |   N  |   S  |  /   |
//			 * |------+-------+------+------+------+------|------+------+------+------+------+------|
//			 * | Shift |   ;  |   Q  |   J  |   K  |   X  |   B  |   M  |   W  |   V  |   Z  |Enter |
//			 * |------+-------+------+------+------+------+------+------+------+------+------+------|
//			 * |Defaukt| Ctrl | Alt  | GUI  |Lower |Space |Space |Raise | Left | Down |  Up  |Right |
//			 * `------------------------------------------------------------------------------------'
//			 */
//
//			  {KC_TAB,  KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,    KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_BSPC },
//			  {KC_ESC,  KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,    KC_T,    KC_N,    KC_S,    KC_SLSH },
//			  {KC_LSFT, KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_ENT  } ,
//			  {DEFAULT,  KC_LCTL, KC_LALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT }
//
//	};
// Create an array that points to the various keymaps

	 uint16_t (*default_layouts[])[MATRIX_ROWS][KEYMAP_COLS]={
			 &_QWERTY,
			 &_NUM,
//			 &_DVORAK
	 };

	 uint8_t current_layout =0;


#endif


