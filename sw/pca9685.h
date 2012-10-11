#ifndef PCA9685_HEADER
#define PCA9685_HEADER

#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01
#define PCA9685_LED0  0x06

void pca9685_init( uint8_t i2c_addr );
inline void pca9685_led_on( uint8_t i2c_addr, uint8_t led );
inline void pca9685_led_off( uint8_t i2c_addr, uint8_t led );
inline void pca9685_led_pwm( uint8_t i2c_addr, uint8_t led, uint8_t intensity );
void pca9685_led_write( uint8_t i2c_addr, uint8_t led, uint16_t value );

void pca9685_write_register( uint8_t i2c_addr, uint8_t register_addr, uint8_t value );

#endif
