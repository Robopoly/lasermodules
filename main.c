/*
 * LaserModule
 *
 * Andrew Watson - 2010
 *
 * Laser beam detector
 * Runs at 1 kHz to filter ambient light, outputs logic level for beam status
 *
 */

#include <avr/io.h>
#include "config.h"

#include <util/delay.h>
#include "uart.h"

/*
 * Debug pins 
 */
#define DEBUG_DDR       DDRA
#define DEBUG_PORT      PORTA
#define DEBUG_PINS      PINA

/*
 * Port where lasers are connected
 */
#define LASER_PORT      PORTB
#define LASER_DDR       DDRB
#define LASER_MASK      0x0F

/*
 * Input pins for photodetectors
 */
#define DETECTOR_PINS   PINB
#define DETECTOR_DDR    DDRB
#define DETECTOR_MASK   0xF0

/*
 * Logic output to rest of application
 */
#define OUTPUT_PORT     PORTC
#define OUTPUT_DDR      DDRC
#define OUTPUT_FIRST    0   //first pin for output

/*
 * Available logic states
 */
#define ON  1
#define OFF 0

/*
 * Some macros
 */
#define laser_on(n)     LASER_PORT |= _BV(n)
#define laser_off(n)    LASER_PORT &= ~_BV(n)
#define laser_toggle(n) LASER_PORT ^= _BV(n)

/*
 * Number of lasers to control (1-4)
 */
#define N_LASERS        4
uint8_t laser[N_LASERS];

/*
 * Min number of values before state change
 */
#define THRESH          10

/*
 * Module enable/disable
 */
#define ENABLE_LETTER   C 
#define ENABLE_PIN      5
#define ENABLE_PINS     PINC
#define pin_value(reg,pin)  ((reg & (1<<(pin))) >> (pin))

/*
 * Indicator LED on C2
 */
#define led_on()    PORTC |= 0x04
#define led_off()   PORTC &= ~0x04

uint8_t is_enabled(void);
void set_output(uint8_t number, uint8_t value);

void init_ports(void);

int main(void)
{
    // init i/o
    DEBUG_PORT = 0x00;
    DEBUG_DDR = 0xFF;

    init_ports();

    uint8_t i;

    uint8_t output_mask = 1 << OUTPUT_FIRST;
    for(i=0; i<N_LASERS; i++)
    {
        output_mask |= output_mask << 1;
        output_mask <<= 1;
    }

    /* uart_init(0,0,0,0,0);
    uart_transmit_byte_block('\n');
    uart_transmit_byte_block('\r'); */

    int state = ON;

    for(;;)
    {
        // if module is disabled, turn off all outputs
        while(!is_enabled())
        {
            OUTPUT_PORT &= ~output_mask;
            led_off();
            _delay_ms(10);
        }
        // indicator led
        led_on();
       
        // turn all lasers to state X
        for(i=0; i<N_LASERS; i++)
        {
            if(state == ON)
                laser_on(i);
            else
                laser_off(i);
        }
        // wait T ms
        _delay_ms(10);
        
        // is input at state X?
        uint8_t inputs = 0;

        // read and parse photodetector inputs
        inputs = DETECTOR_PINS & DETECTOR_MASK;
        for(i=0; i<N_LASERS; i++)
        {
            if(pin_value(inputs, i+5) == state) 
            {
                laser[i] += 1;
                if(laser[i] > THRESH)
                {
                    laser[i] = THRESH;
                    set_output(i, ON);
                }
                else
                {
                    set_output(i, OFF);
                }
            }
            else
            {
                if(laser[i] > 0)
                    /*
                     * TODO : should maybe consider setting it to zero directly
                     */
                    laser[i] -= 1;
                // No hysteresis implemented yet.
                if(laser[i] < THRESH)
                    set_output(i, OFF);
            }
        }

        // change state
        state ^= 1;
    }

    // main

    return 0;
    
}

void init_ports(void)
{
    LASER_PORT &= ~LASER_MASK;
    LASER_DDR  |= LASER_MASK;

    DETECTOR_DDR &= ~DETECTOR_MASK;

    DDRC &= ~_BV(5); // enable pin

    DDRC |= _BV(2); // enable indicator LED
}

/*
 * Set output N to state V
 */
void set_output(uint8_t number, uint8_t value)
{
    // set ones
    OUTPUT_PORT |= value << (number + OUTPUT_FIRST);
    // set zeroes
    OUTPUT_PORT &= ~(value << (number + OUTPUT_FIRST));
}

/*
 * Check enable pin to decide whether to turn on lasers
 */
uint8_t is_enabled(void)
{
    if (ENABLE_PINS & _BV(ENABLE_PIN))
        return 1;
    else
        return 0;
} 
