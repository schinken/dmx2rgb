#include <avr/io.h>
#include <avr/interrupt.h>

#define DMX_BAUD 250000

#define DMX_BUFFERS         2
#define DMX_BUFFER_1        0
#define DMX_BUFFER_2        1

#define DMX_CHANNEL         1   // DMX Channel
#define DMX_NUM_CHANNELS    10   // How many channels? 

#define F_CPU 16000000


volatile uint8_t dmx_rx_complete = 0x00;

uint8_t dmx_buffers[ DMX_BUFFERS ][ DMX_NUM_CHANNELS + 2 ];

uint8_t* dmx_buf_back  = dmx_buffers[ DMX_BUFFER_1 ];
uint8_t* dmx_buf_front = dmx_buffers[ DMX_BUFFER_2 ];

inline void dmx_switch_buffers() {

    // Check if current backbuffer is buffer 1, if yes, switch
    if( dmx_buf_back == dmx_buf_back + DMX_BUFFER_1 ) {
        dmx_buf_back  = dmx_buffers[ DMX_BUFFER_1 ];
        dmx_buf_front = dmx_buffers[ DMX_BUFFER_2 ];
    } else {
        dmx_buf_back  = dmx_buffers[ DMX_BUFFER_2 ];
        dmx_buf_front = dmx_buffers[ DMX_BUFFER_1 ];
    }
}

uint8_t  dmx_valid = 0;
uint16_t dmx_rx_cnt;    // 16-bit for 512 channels

ISR( USART_RX_vect ){

    uint8_t tmp = UDR0;

    // Check if stopbit is set - new package begins
    if ( UCSR0A & (1 << FE0) ) {
 
        dmx_rx_cnt = 0;
        dmx_rx_complete = 0;

        dmx_buf_back[0] = tmp;

		if(tmp == 0) {
			dmx_valid = 1;
            dmx_rx_cnt++;
		}else{
			dmx_valid = 0;
		}


//        dmx_switch_buffers();
        return;
    }

    if( dmx_valid ) {

        // Check if we are in range
        if( dmx_rx_cnt >= DMX_CHANNEL && dmx_rx_cnt <= (DMX_CHANNEL + DMX_NUM_CHANNELS + 1 ) ){

            dmx_buf_back[ dmx_rx_cnt - DMX_CHANNEL +1 ] = tmp;

            // If we've reached all channels we need, mark current package as completed
            if( dmx_rx_cnt == (DMX_CHANNEL + DMX_NUM_CHANNELS) ) {
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

    sei();

    while(1) {  

        if( dmx_rx_complete ) {

            if( dmx_buf_back[1] == 42 ) {
                PORTB &= ~0x01;    
            } else {
                PORTB |= 0x01;
            }

            if( dmx_buf_back[2] == 42 ) {
                PORTB &= ~0x02;    
            } else {
                PORTB |= 0x02;
            }

            if( dmx_buf_back[3] == 42 ) {
                PORTB &= ~0x04;    
            } else {
                PORTB |= 0x04;
            }
        }
    }                         
 
    return 0;                 
}
