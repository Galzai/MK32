#include "esp_system.h"
#include "driver/gpio.h"
#include "key_definitions.h"
#include "keyboard_config.h"
#include "driver/pcnt.h"





int PastEncoderCount=0;

//Setting Pulse counter and encoder button
void r_encoder_setup(void){

	pcnt_config_t pcnt_config_a = {
			// Set PCNT input signal and control GPIOs
			.pulse_gpio_num = ENCODER_A_PIN,
			.ctrl_gpio_num = ENCODER_B_PIN,

			.unit = PCNT_UNIT_0,
			.channel = PCNT_CHANNEL_0,

			// What to do on the positive / negative edge of pulse input?
			.pos_mode = PCNT_COUNT_DIS,   // Count up on the positive edge
			.neg_mode = PCNT_COUNT_INC,   // Keep the counter value on the negative edge
			// What to do when control input is low or high?
			.lctrl_mode = PCNT_MODE_KEEP, // Reverse counting direction if low
			.hctrl_mode = PCNT_MODE_REVERSE,    // Keep the primary counter mode if high
			// Set the maximum and minimum limit values to watch
			.counter_h_lim = INT16_MAX,
			.counter_l_lim = INT16_MIN,
	};


	pcnt_unit_config(&pcnt_config_a);

	pcnt_set_filter_value(PCNT_UNIT_0, 1023);  // Filter Runt Pulses
	pcnt_filter_enable(PCNT_UNIT_0);

	gpio_set_direction(ENCODER_A_PIN,GPIO_MODE_INPUT);
	gpio_set_pull_mode(ENCODER_A_PIN,GPIO_FLOATING);
	gpio_set_direction(ENCODER_B_PIN,GPIO_MODE_INPUT);
	gpio_set_pull_mode(ENCODER_B_PIN,GPIO_FLOATING);

	pcnt_counter_pause(PCNT_UNIT_0); // Initial PCNT init
	pcnt_counter_clear(PCNT_UNIT_0);
	pcnt_counter_resume(PCNT_UNIT_0);

	//Encoder Button
#ifdef ENCODER_S_PIN
	gpio_pad_select_gpio(ENCODER_S_PIN);
	gpio_set_direction(ENCODER_S_PIN, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_level(ENCODER_S_PIN,0);
#endif

}

//Check encoder state, currently defined for Vol +/= and mute
uint8_t r_encoder_state(void){
	uint8_t EncoderState=0x00;
	int16_t EncoderCount;
	pcnt_get_counter_value(PCNT_UNIT_0, &EncoderCount);
	if(EncoderCount>PastEncoderCount){
		EncoderState = 0x20;
	}
	if(EncoderCount<PastEncoderCount){
		EncoderState = 0x40;
	}
#ifdef ENCODER_S_PIN
	if(gpio_get_level(ENCODER_S_PIN)==1){
		EncoderState+= 0x10;
	}
#endif
	PastEncoderCount=EncoderCount;
	return EncoderState;

}





