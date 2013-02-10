#include "dmx2rgb.h"
#include "pca9685.h"
#include "i2cmaster.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


volatile uint8_t dmx_rx_complete = 0x00;
volatile uint8_t dmx_buf_back[DMX_NUM_CHANNELS + 2];

uint8_t  dmx_valid = 0;
uint16_t dmx_rx_cnt;    // 16-bit for 512 channels

/*
 * LED Mapping, because the wires are twisted :(
 */
uint8_t led_map[] = {6, 7, 4, 5, 2, 3, 0, 1, 9, 8, 11, 10, 13, 12, 15, 14};

ISR(USART_RX_vect){

    uint8_t tmp = UDR0;

    // Check if stopbit is set - new package begins
    if(UCSR0A & (1 << FE0)) {
 
        dmx_rx_cnt = 0;
        dmx_rx_complete = 0;

        dmx_buf_back[0] = tmp;

        if(tmp == 0) {
            dmx_valid = 1;
            dmx_rx_cnt++;

            // Toggle LED 1 if frame begins
            PORTB ^= 0x01;
        }else{
            dmx_valid = 0;
        }

        return;
    }

    if(dmx_valid) {

        // Check if we are in range
        if(dmx_rx_cnt >= DMX_CHANNEL && dmx_rx_cnt <= (DMX_CHANNEL + DMX_NUM_CHANNELS + 1 )){

            dmx_buf_back[dmx_rx_cnt - DMX_CHANNEL+1] = tmp;

            // If we've reached all channels we need, mark current package as completed
            if(dmx_rx_cnt == (DMX_CHANNEL + DMX_NUM_CHANNELS)) {
                dmx_rx_complete = 1;
            }
        }

        dmx_rx_cnt++;
    }

}

int main (void) {            


    // INIT DMX
    UCSR0C = (1<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00); // 2 stop bits, 8 data bits
    UCSR0B = (1<<RXEN0) | (1<<RXCIE0);               // enable transmit, receive and interrupts,
    UBRR0  = (F_CPU / (DMX_BAUD * 16L) - 1);
 
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

    while(1) {  

        if(dmx_rx_complete) {
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
  
    }                         
 
    return 0;                 

}
