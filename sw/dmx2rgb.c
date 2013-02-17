#include "dmx2rgb.h"
#include "pca9685.h"
#include "i2cmaster.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/setbaud.h>

volatile uint8_t dmx_buf_back[DMX_NUM_CHANNELS + 2];

uint16_t dmx_rx_cnt;    // 16-bit for 512 channels

/*
 * LED Mapping, because the wires are twisted :(
 */
uint8_t led_map[] = {6, 7, 4, 5, 2, 3, 0, 1, 9, 8, 11, 10, 13, 12, 15, 14};

#define SERIAL_FLOOR 0
#define SERIAL_HEAD1 1
#define SERIAL_DATA  2

volatile uint8_t stage = SERIAL_FLOOR;

ISR(USART_RX_vect){

    uint8_t tmp = UDR0;

    switch(stage) {
        case SERIAL_FLOOR:
            PORTB ^= 0x01;
            dmx_rx_cnt = 0;

            if(tmp == 0x55) {
                stage = SERIAL_HEAD1;
            }
            break;

        case SERIAL_HEAD1:
            if(tmp == 0xAA) {
                stage = SERIAL_DATA;
            } else {
                stage = SERIAL_FLOOR;
            }
            break;

        case SERIAL_DATA:

            dmx_buf_back[dmx_rx_cnt+1] = tmp;

            if(dmx_rx_cnt == DMX_NUM_CHANNELS) {
                stage = SERIAL_FLOOR;
            }

            dmx_rx_cnt++;

            break;

    }

}

int main (void) {


    // INIT DMX
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 1 stop bits, 8 data bits
    UCSR0B = (1<<RXEN0) | (1<<RXCIE0);               // enable transmit, receive and interrupts,

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    // INIT PORT PINS
    DDRB  = (1 << PIN0) | (1 << PIN1 ) | (1 << PIN2);
    PORTB = (1 << PIN0) | (1 << PIN1 ) | (1 << PIN2);

    // Data direction for LED ENABLE
    DDRD  |= (1 << PIN3);
    PORTD &= ~(1<<PIN3);

    sei();
    i2c_init();

    pca9685_init(PCA9685_CHIP_1, PCA9685_FREQUENCY(1200UL));
    pca9685_init(PCA9685_CHIP_2, PCA9685_FREQUENCY(950UL));
    pca9685_init(PCA9685_CHIP_3, PCA9685_FREQUENCY(650UL));

    // Switch on third LED to display init
    PORTB &= ~0x04;

    // Init led array
    uint8_t i = 0;
    for(i = 0; i < DMX_NUM_CHANNELS; i++){
        dmx_buf_back[i] = 0x00;
    }

    while(1) {  

        uint8_t led = 0;

        for(led = 0; led < 16; led++) {
            pca9685_led_pwm(PCA9685_CHIP_1, led_map[led], dmx_buf_back[led + 1]);
        }
        for(led = 0; led < 16; led++) {
            pca9685_led_pwm(PCA9685_CHIP_2, led_map[led], dmx_buf_back[led + 1 + 16]);
        }
        for(led = 0; led < 16; led++) {
            pca9685_led_pwm(PCA9685_CHIP_3, led_map[led], dmx_buf_back[led + 1 + 32]);
        }
  
    }
 
    return 0;

}
