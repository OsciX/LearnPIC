/* 
 * File:   main.c
 * Author: Ryan
 *
 * Created on February 26, 2023, 12:35 PM
 */

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON      // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


#define _XTAL_FREQ 4000000
#include <xc.h>




int currentDigit = 0;
int update = 0;
int state = 0;

void writeDigit(uint8_t digit, uint8_t value);

void __interrupt() timer_0() {
    if (INTCONbits.TMR0IF == 1) {
        currentDigit++;
        if (currentDigit > 3) {
            currentDigit = 0;
        }

        update = 1;
        INTCONbits.TMR0IF = 0;
    }
};
    

int main(void) {
    
    // interrupt setup
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = 1;
    
    // prescaler 1:4, 1ms interrupt (1kHz PWM frequency)
    OPTION_REG = 0x01;
    TMR0 = 155;
    
    TRISAbits.TRISA3 = 0x00;
    TRISCbits.TRISC3 = 0x00;
    TRISCbits.TRISC4 = 0x00;
    TRISCbits.TRISC5 = 0x00;
    TRISD = 0x0;

    
    // turn off 8LEDs
    PORTCbits.RC5 = 1;
    PORTD = 0xFF;
    PORTCbits.RC5 = 0;
    
    // latch enable for digit latch
    PORTCbits.RC4 = 1;
    // load digit latch
    PORTD = 0xF0;
    // lock digit latch
    PORTCbits.RC4 = 0;
    
    // output enable for latches (active low)
    PORTAbits.RA3 = 0;
    
    while(1) {
        for(int j = 0; j<4; j++) {
            for(int i = 0; i<10; i++) {
                writeDigit(j,i);
                __delay_ms(500);
            }
        }
    }
}

/*
 * writeDigit simplifies putting a value on a specified seven-segment.
 * digit specifies which 7segment display to write the bits to (0-3, right to left on the circuit board)
 * value specifies what number to put on it
 */
void writeDigit(uint8_t digit, uint8_t value) {
    
    // const unsigned int array to simplify conversion to bits
    const uint8_t nums[10] = {
        0b11000000,
        0b11111001,
        0b10100100,
        0b10110000,
        0b10011001,
        0b10010010,
        0b10000010,
        0b11111000,
        0b10000000,
        0b10011000,
    };
    
    // latch enable for digit latch
    PORTCbits.RC4 = 1;
    // load digit specifier; 7segs 0-3 are Q5-Q8 on the latch, so this is done with a shift
    PORTD = 0b1 << (4+digit);
    // lock digit latch
    PORTCbits.RC4 = 0;
    
    // segment latch high, to load bits in
    PORTCbits.RC3 = 1;
    PORTD = nums[value];
    // lock segment latch
    PORTCbits.RC3 = 0;
}