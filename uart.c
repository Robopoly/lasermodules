#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "config.h"

#define TRIG_CHAR '\n'


/* Internal global variable */
unsigned char rx_byte=0,tx_byte=0,flag_interrupt_tx = 0;
unsigned char* buffer_rx= 0,*buffer_tx = 0;
callback_rx rx_cb;
callback_tx tx_cb;

/*Init uart module with baudrate set in config.h
cb_rx must be null to disable rx_interrupt
cb_tx must be null to disable  tx_interrupt
_buffer_rx: buffer to receive data, must be >=RX_BUFFER_SIZE defined in config.h
_buffer_tx: buffer to transmit data, must be >=TX_BUFFER_SIZE defined in config.h
nb_byte_rx: Number of byte attempt to receive before executing callback cb_rx*/
void uart_init(callback_rx cb_rx,unsigned char * _buffer_rx,unsigned char nb_byte_rx ,callback_tx cb_tx,unsigned char * _buffer_tx)
{
		buffer_rx = _buffer_rx;
		buffer_tx = _buffer_tx;

		#ifndef BAUD
		# warning "BAUD is not defined, automatically set to 9600baud"
		#define	BAUD 9600
		#endif

		#include <util/setbaud.h>

		UBRRH = UBRRH_VALUE;
		UBRRL = UBRRL_VALUE;
		#if USE_2X
		UCSRA |= (1 << U2X);
		#else
		UCSRA &= ~(1 << U2X);
		#endif
		UCSRB |= 0x18;

		if(cb_rx)
		{
			UCSRB|=(1<<RXCIE);
			rx_byte = nb_byte_rx;
			rx_cb = cb_rx;
		}
		else
			UCSRB &= ~(1<<RXCIE);
		
		if(cb_tx)
		{
			UCSRB|= (1<<TXCIE);
			tx_cb = cb_tx;
		}

		else
			UCSRB &= ~(1<<TXCIE);
			
}


/*To Transmit only one byte, block function,txie interrupt
will automatically  be disabled for the transmission*/
void uart_transmit_byte_block(unsigned char data)
{
	UCSRB &= ~(1<<TXCIE);
	while ( !( UCSRA & (1<<UDRE)));
	UDR = data;
	UCSRA|=(1<<TXC);
	flag_interrupt_tx = 0;
	UCSRB |= (1<<TXCIE);
}

void uart_transmit_string_block(char *text)
{
	unsigned char i;
	for(i=0; text[i] != '\0'; i++)
	{
		uart_transmit_byte_block((unsigned char)text[i]);
	}
}

void uart_start_transmission(unsigned char nb_byte)
{
	UDR = *buffer_tx;
	tx_byte = nb_byte;
}

/* To receive only one byte,block function; rxie interrupt 
will automatically be disabled uart_enable_rxie must be call to reenable
reception interrupt*/
unsigned char uart_receive_byte_block(void)
{
	UCSRB &= ~(1<<RXCIE);
	// Wait for data to be received 
	while ( !(UCSRA & (1<<RXC)) );
	// Get and return received data from buffer 
	return UDR;
}

void uart_enable_rxie(void)
{
	UCSRB |= (1<<RXCIE);
}

/* Data: word to transmit
	mode: must be BIG_ENDIAN or LITTE_ENDIAN*/
void uart_transmit_word_block(int data, unsigned char mode)
{

	if(mode == BIG_ENDIAN)
	{
		while ( !( UCSRA & (1<<UDRE)));
		UDR = data&0xFF;
		while ( !( UCSRA & (1<<UDRE)));
		UDR = (data>>8);
	}

	else if(mode == LITTLE_ENDIAN)
	{
		while ( !( UCSRA & (1<<UDRE)));
		UDR = (data>>8);
		while ( !( UCSRA & (1<<UDRE)));
		UDR = data&0xFF;

	}
}


/* Send the value of the argument in ASCII code 
arg must be set between 0 and 999*/	
void uart_send_dec(int arg)
{
	unsigned char centaine = 0,dizaine = 0;
	while(arg>=100)
	{
		arg-=100;
		centaine++;
	}
	while(arg>=10)
	{
		arg-=10;
		dizaine++;
	}
	uart_transmit_byte_block(centaine+48);
	uart_transmit_byte_block(dizaine+48);
	uart_transmit_byte_block(arg+48);
}	



ISR(USART_RX_vect)
{
	PORTC|=(1<<2);
	static unsigned char no_byte;
	if(no_byte<(rx_byte-1) && no_byte<(RX_BUFFER_SIZE-1))
	{
		*(buffer_rx+no_byte) = UDR;
		no_byte++;
#ifdef TRIG_CHAR
                if(UDR == TRIG_CHAR)
                {
                    no_byte = 0;
                    rx_cb(UDR);
                }
#endif
	}
	else
	{
		*(buffer_rx+no_byte) = UDR;
		no_byte = 0;
		rx_cb(UDR);
	}
}


ISR(USART_TX_vect)
{
	static unsigned char no_byte = 1;
	
	
	if(flag_interrupt_tx) //Avoid enable TXIE interrupt if 0
	{
		if(no_byte<tx_byte && no_byte<TX_BUFFER_SIZE)
		{
			UDR = *(buffer_tx+no_byte);
			no_byte++;
		}

		else
		{
			no_byte = 1;
			if(tx_cb)
				tx_cb();
		}
	}

	flag_interrupt_tx = 1;

}
