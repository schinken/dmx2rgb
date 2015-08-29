#ifndef PCA9685_HEADER
#define PCA9685_HEADER

#include <avr/io.h>
#include <avr/interrupt.h>

/* input x as Hz */
#define PCA9685_FREQUENCY(x) ((25000000UL/(4096UL*x))-1)

/* registers */
#define PCA9685_MODE1     0x00
#define PCA9685_MODE2     0x01
#define PCA9685_LED0      0x06
#define PCA9685_PRESCALER 0xFE

/* commands */
#define PCA9685_WRITE   0x00
#define PCA9685_READ    0x01

/* MODE1 bits */
#define PCA9685_RESTART 0x80
#define PCA9685_EXTCLK  0x40
#define PCA9685_AI      0x20
#define PCA9685_SLEEP   0x10
#define PCA9685_SUB1    0x08
#define PCA9685_SUB2    0x04
#define PCA9685_SUB3    0x02
#define PCA9685_ALLCALL 0x01

/* MODE2 bits */
#define PCA9685_INVRT   0x10
#define PCA9685_OCH     0x08
#define PCA9685_OUTDRV  0x04
#define PCA9685_OUTNE1  0x02
#define PCA9685_OUTNE0  0x01

void pca9685_init(uint8_t i2c_addr, uint8_t frequency);
void pca9685_led_on(uint8_t i2c_addr, uint8_t led);
void pca9685_led_off(uint8_t i2c_addr, uint8_t led);
void pca9685_led_pwm(uint8_t i2c_addr, uint8_t led, uint8_t intensity);
void pca9685_led_write(uint8_t i2c_addr, uint8_t led, uint16_t value);

void pca9685_write_register(uint8_t i2c_addr, uint8_t register_addr, uint8_t value);

#endif
