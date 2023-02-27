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

uint32_t tcount = 0;
uint8_t diag = 0;

void setup();
void writeDigit(uint8_t digit, uint8_t value);


void __interrupt() timer_0() {
    // preload timer for 1kHz frequency
    TMR0 = 10;
    if (INTCONbits.TMR0IF == 1) {
        tcount++;
        diag = !diag;
        INTCONbits.TMR0IF = 0;
    }
};
    

int main(void) {
    setup();

    uint8_t places[4] = {0,0,0,0};          // holds digits to write to display
    
    uint16_t disp = 0;                      // count to display
    uint16_t cal = 0;                       // temp variable for filling places from disp
    uint8_t ldu = 0;                        // last digit updated
    uint32_t pcount = tcount;               // used for counting upwards every 1 sec
    
    while(1) {
        
        if (ldu != tcount%4) {
            // update ldu and write digit
            ldu = tcount%4;
            writeDigit(ldu, places[ldu]);    

            // if ldu has changed, check if we need to count up
            if (tcount - pcount >= 1000) {
                pcount = tcount;
                disp++;
                
                cal = disp;
                for (int i = 0; i < 4; i++) {
                    places[i] = cal % 10;
                    cal /= 10;
                }
            }
            
            // 500Hz signal on RB7, for checking interrupt timing
            PORTBbits.RB7 = diag;  
        }
        
       
    }
}

// Initialize registers and interrupts
void setup() { 
    // interrupt setup
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = 1;
    
    // prescaler 1:4, 1ms interrupt (1kHz PWM frequency)
    OPTION_REG = 0x01;
    TMR0 = 10;
    
    TRISAbits.TRISA3 = 0;
    TRISBbits.TRISB7 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 0;
    TRISD = 0x00;

    
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
    
    
    // output enable for latches (active low)
    PORTAbits.RA3 = 1;  
    
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
    
    // output enable for latches (active low)
    PORTAbits.RA3 = 0;  

}

