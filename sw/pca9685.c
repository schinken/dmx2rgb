#include "dmx2rgb.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "pca9685.h"
#include "i2cmaster.h"

/**
 * CIEL Lookup table, copied from https://github.com/brunnels/PCA9685/blob/master/PCA9685.cpp
 *
 * 12 bits PWM to CIE Luminance conversion
 * L* = 116(Y/Yn)^1/3 - 16 , Y/Yn > 0.008856
 * L* = 903.3(Y/Yn), Y/Yn <= 0.008856
 * 
*/

const uint16_t CIEL12[] PROGMEM = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 18, 20, 21, 23, 25, 27, 28, 30,
  32, 34, 36, 37, 39, 41, 43, 45, 47, 49, 52, 54, 56, 59, 61, 64, 66, 69, 72, 75, 77, 80, 83, 87,
  90, 93, 97, 100, 103, 107, 111, 115, 118, 122, 126, 131, 135, 139, 144, 148, 153, 157, 162, 167,
  172, 177, 182, 187, 193, 198, 204, 209, 215, 221, 227, 233, 239, 246, 252, 259, 265, 272, 279,
  286, 293, 300, 308, 315, 323, 330, 338, 346, 354, 362, 371, 379, 388, 396, 405, 414, 423, 432,
  442, 451, 461, 471, 480, 490, 501, 511, 521, 532, 543, 554, 565, 576, 587, 599, 610, 622, 634,
  646, 658, 670, 683, 696, 708, 721, 734, 748, 761, 775, 789, 802, 817, 831, 845, 860, 875, 890,
  905, 920, 935, 951, 967, 983, 999, 1015, 1032, 1048, 1065, 1082, 1099, 1117, 1134, 1152, 1170,
  1188, 1206, 1225, 1243, 1262, 1281, 1301, 1320, 1340, 1359, 1379, 1400, 1420, 1441, 1461, 1482,
  1504, 1525, 1547, 1568, 1590, 1613, 1635, 1658, 1681, 1704, 1727, 1750, 1774, 1798, 1822, 1846,
  1871, 1896, 1921, 1946, 1971, 1997, 2023, 2049, 2075, 2101, 2128, 2155, 2182, 2210, 2237, 2265,
  2293, 2322, 2350, 2379, 2408, 2437, 2467, 2497, 2527, 2557, 2587, 2618, 2649, 2680, 2712, 2743,
  2775, 2807, 2840, 2872, 2905, 2938, 2972, 3006, 3039, 3074, 3108, 3143, 3178, 3213, 3248, 3284,
  3320, 3356, 3393, 3430, 3467, 3504, 3542, 3579, 3617, 3656, 3694, 3733, 3773, 3812, 3852, 3892,
  3932, 3973, 4013, 4055, 4095};

#define PWM_MAX 4095
#define PWM_TO_CIEL(x) (pgm_read_word(&CIEL12[x]))
#define HIGHBYTE(x) ( (uint8_t) (x >> 8 ))
#define LOWBYTE(x)  ( (uint8_t) (x & 0xFF) )



/**
 * Initialize and Reset PCA9685 Chip
 */

void pca9685_init(uint8_t i2c_addr, uint8_t frequency) {
 
	/* set frequency */
	pca9685_write_register(i2c_addr, PCA9685_MODE1, PCA9685_SLEEP);
	pca9685_write_register(i2c_addr, PCA9685_PRESCALER, frequency);
    
    _delay_ms(1);
    pca9685_write_register(i2c_addr, PCA9685_MODE1, PCA9685_SUB2 | PCA9685_SUB3 /* 0x06 */);
    _delay_ms(1);

    pca9685_write_register(i2c_addr, PCA9685_MODE1, PCA9685_AI | PCA9685_ALLCALL /* 0x21 */);
    
    
}

/**
 * Switch LED On
 */

inline void pca9685_led_on(uint8_t i2c_addr, uint8_t led) {
    pca9685_led_pwm(i2c_addr, led, 255);   
}

/**
 * Switch LED Off
 */

inline void pca9685_led_off(uint8_t i2c_addr, uint8_t led) {
    pca9685_led_pwm(i2c_addr, led, 0);  
}

/**
 * Set LED with an intensity between 0 - 255
 */

inline void pca9685_led_pwm(uint8_t i2c_addr, uint8_t led, uint8_t intensity) {
    pca9685_led_write(i2c_addr, led, PWM_TO_CIEL(intensity));
}

/**
 * Write LED Registers
 */

void pca9685_led_write(uint8_t i2c_addr, uint8_t led, uint16_t value) {
 
    // Toggle second LED on i2c write
    PORTB ^= 0x02;

    i2c_start(i2c_addr + PCA9685_WRITE);
    i2c_write(PCA9685_LED0 + 4*led);

    uint16_t phase = (PWM_MAX-value)-1;


    i2c_write(0x00);
    i2c_write(0x00);

    // Write LED Off
    i2c_write(LOWBYTE(value));
    i2c_write(HIGHBYTE(value));

    i2c_stop();
}

/**
 * Write PCA9685 Register
 */

void pca9685_write_register(uint8_t i2c_addr, uint8_t register_addr, uint8_t value) {
    i2c_start(i2c_addr + PCA9685_WRITE);
    i2c_write(register_addr);
    i2c_write(value);
    i2c_stop();
}
