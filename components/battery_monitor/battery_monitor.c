/*
 * This battery monitoring code is based on reading the voltage
 * after after a voltage divider and checking the level on an analog pin
 *
 */


#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "keyboard_config.h"

#include "battery_monitor.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static const adc_channel_t channel = BATT_PIN;
static const adc_atten_t atten = ADC_ATTEN_DB_2_5;
static const adc_unit_t unit = ADC_UNIT_1;

uint32_t voltage = 0 ;

static esp_adc_cal_characteristics_t *adc_chars;
//check battery level

uint32_t get_battery_level(void){


    uint32_t adc_reading = 0;
    //Multisampling

    for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
    }
    adc_reading /= NO_OF_SAMPLES;

    //Convert adc_reading to voltage in mV
    voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    uint32_t battery_percent = ((voltage-Vout_min)/(Vout_max-Vout_min))*100;
    return battery_percent;

}


//initialize battery monitor pin
void init_batt_monitor(void){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(BATT_PIN ,ADC_ATTEN_DB_2_5);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

}

