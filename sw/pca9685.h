#ifndef PCA9685_HEADER
#define PCA9685_HEADER

#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01
#define PCA9685_LED0  0x06

void pca9685_init( uint8_t i2c_addr );
void pca9685_led_on( uint8_t i2c_addr, uint8_t led );


#endif
