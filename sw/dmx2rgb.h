
#define DMX_BAUD 250000

#define DMX_BUFFERS         2
#define DMX_BUFFER_1        0
#define DMX_BUFFER_2        1

#define DMX_CHANNEL         1   // DMX Channel
#define DMX_NUM_CHANNELS    48   // How many channels? 

#define F_CPU 16000000

#define PCA9685_CHIP_1 0b10000010
#define PCA9685_CHIP_2 0b10000110
#define PCA9685_CHIP_3 0b10001110

#define LED_ENABLE()  PORTB &= ~(1 << PIN3)
#define LED_DISABLE() PORTB |= (1 << PIN3)
