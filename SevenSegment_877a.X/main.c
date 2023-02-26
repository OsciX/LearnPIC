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

const int digits[10] = {
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


int main(void) {
    TRISAbits.TRISA3 = 0x00;
    TRISCbits.TRISC3 = 0x00;
    TRISCbits.TRISC4 = 0x00;
    TRISCbits.TRISC5 = 0x00;

    TRISD = 0x0;

    
    // turn off 8LEDs
    PORTCbits.RC5 = 1;
    PORTD = 0xFF;
    PORTCbits.RC5 = 0;
    
    
    // output enable for latches (active low)
    PORTAbits.RA3 = 0;
    
    // latch enable for digit latch
    PORTCbits.RC4 = 1;
    // load digit latch
    PORTDbits.RD4 = 1;
    PORTDbits.RD5 = 1;
    PORTDbits.RD6 = 1;
    PORTDbits.RD7 = 1;
    // lock digit latch
    PORTCbits.RC4 = 0;
    
    
    while(1) {
        for (int i = 0; i < 10; i++) {
            // segment latch high, to load bits in
            PORTCbits.RC3 = 1;
            PORTD = digits[i];
            // lock segment latch
            PORTCbits.RC3 = 0;
            __delay_ms(500);
        }
    }
}

