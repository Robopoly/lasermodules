/*File Robopoly_uart.h, written by C. Auvigne, October 09*/

#ifndef ROBOPOLY_UART_H
#define ROBOPOLY_UART_h

enum mode
{
	LITTLE_ENDIAN = 0,
	BIG_ENDIAN
};

typedef void (*callback_rx) (unsigned char last);
typedef void (*callback_tx)(void);

/*Init all uart parameters, doc in .c*/
void uart_init(callback_rx ,unsigned char*,unsigned char,callback_tx,unsigned char*);

/* Transmit one byte, blocking function*/
void uart_transmit_byte_block(unsigned char);
/* Transmit one word, blocking function*/
void uart_transmit_word_block(int,unsigned char);
/*Transmit one string, blocking function*/
void uart_transmit_string_block(char*);
/*Start a trame transmission, doc in .c*/
void uart_start_transmission(unsigned char);
/*Send a int in ASCII format, between 0 and 999*/
void uart_send_dec(int);

/* Enable reception uart interrupt*/
void uart_enable_rxie(void);

/* Receive a byte, blocking function*/
unsigned char uart_receive_byte_block(void);


#endif
