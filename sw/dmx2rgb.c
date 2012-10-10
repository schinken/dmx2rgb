#include <avr/io.h>
#include <avr/interrupt.h>

#define DMX_BAUD 250000

#define DMX_BUFFERS         2
#define DMX_BUFFER_1        0
#define DMX_BUFFER_2        1

#define DMX_CHANNEL         1   // DMX Channel
#define DMX_NUM_CHANNELS    2   // How many channels? 

#define F_CPU 16000000


uint8_t dmx_rx_complete = 0x00;

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
uint16_t rx_cnt;    // 16-bit for 512 channels

ISR( USART_RX_vect ){

    uint8_t tmp = UDR0;

    PORTB ^= 0x01;

    // Check if stopbit is set - new package begins
    if ( UCSR0A & (1 << FE0) ) {
 
        rx_cnt = 0;
        dmx_rx_complete = 0;

        dmx_buf_back[0] = tmp;

		if(tmp == 0) {
			dmx_valid = 1;
            PORTB |= 0x02;
            rx_cnt++;
		}else{
			dmx_valid = 0;
            PORTB &= ~(0x02);
		}


//        dmx_switch_buffers();
        return;
    }

    if( dmx_valid ) {

        PORTB ^= 0x04;
        // Check if we are in range
        if( rx_cnt >= DMX_CHANNEL && rx_cnt <= (DMX_CHANNEL + DMX_NUM_CHANNELS) ){

            dmx_buf_back[ rx_cnt - DMX_CHANNEL ] = tmp;
        }

        // If we've reached all channels we need, mark current package as completed
        if( rx_cnt == (DMX_CHANNEL + DMX_NUM_CHANNELS)-2 ) {
            dmx_rx_complete = 1;
        }

        rx_cnt++;
    }

}

int main (void) {            


    // INIT DMX


    //UCSR0C = (1<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00); // 2 stop bits, 8 data bits
    UCSR0C = (1<<USBS0); // 2 stop bits, 8 data bits
    UCSR0B = (1<<RXEN0) | (1<<RXCIE0);               // enable transmit, receive and interrupts,
    UBRR0  = (F_CPU / (DMX_BAUD * 16L) - 1);
 //   UBRR0H = 0;
 //   UBRR0L = 3; //16 MHz->250kbps
 
    // INIT PORT PINS
    DDRB  = (1 << PIN0) | (1 << PIN1 ) | (1 << PIN2);
    PORTB = (1 << PIN0) | (1 << PIN1 ) | (1 << PIN2);

//    DDRD = 0x02;
    
    sei();

    unsigned int cnt = 0x00;

    while(1) {  

        if( dmx_rx_complete ) {
            PORTB ^= 0x02;

            if( dmx_buf_front[1] == 42 ) {
                PORTB ^= 0x04;    
            }
        }

        if( cnt == 0 ) {
 //           PORTB ^= 0x04;    
        }

        cnt++;
    }                         
 
    return 0;                 
}
