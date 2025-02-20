#include "intrinsics.h"
#include "msp430fr2355.h"
#include <msp430.h>
#include <stdbool.h>
#include "led_bar.h"

//int key_input;
int input_bool = 1;
int i;

//------------------------------------------------------------------------------
// Begin led initialization
//------------------------------------------------------------------------------
void init_led_bar()
{
    #define ledOn 0xFF
    #define ledOff 0x0
    #define ledPattern01_init 0b10101010
    #define ledPattern02_init 0x0
    #define ledPattern03_init 0b00011000
    #define ledPattern04_init 0xFF
    #define ledPattern05_init 0b00000001
    #define ledPattern06_init 0b01111111
    #define ledPattern07_init 0b00000001

    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    
    // Setup Ports
    P3DIR |= 0xFF;              // Config Pin 3 as output
    P3OUT |= 0xFF;              // Clear  Pin 3 to start

    // Setup Timer
    TB1CTL |= TBCLR;            // Clear timers and dividers
    TB1CTL |= TBSSEL__ACLK;     // Source = ACLK
    TB1CTL |= MC__UP;           // Mode = UP
    TB1CCR0 = 32768;            // CCR0 = 32768 (1s overflow)
    // 8192 = 0.25 seconds

    // Setup Timer Compare IRQ
    TB1CCTL0 &= ~CCIFG;         //Clear CCR0 Flag
    TB1CCTL0 |= CCIE;           // Enable TB0 CCR0 Overflow IRQ
    __enable_interrupt();       // Enable Maskable IRQ

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    P3OUT = ledPattern03_init;
}
//--End LED Initialization------------------------------------------------------

//------------------------------------------------------------------------------
// Begin LED patterns
//------------------------------------------------------------------------------
void ledPatterns(int key_input) 
{
    switch(key_input)
    {
        case 'A':
            if (TB1CCR0 - 8192 > 0)
                TB1CCR0 - 8192;
            break;
        case 'B':
            if (TB1CCR0 + 8192 < 0xFFFF)
                TB1CCR0 + 8192;
            break;
        case 0:             // All LEDs off
            P3OUT = ledOff;
            break;
        case 1:             // Toggle
            if (input_bool == 1) {
                P3OUT = ledPattern01_init;
                input_bool = 0;
            } else
                P3OUT ^= 0xFF;
            break;
        case 2:             // Up counter
            if (input_bool == 1) {
                P3OUT = ledPattern02_init;
                input_bool = 0;
            } else
                P3OUT++;
            break;
        case 3:             // in and out
            for (i=0;i<3;i++)
                P3OUT = ~P3OUT & ((0xF0 & P3OUT << 1) | (0xF & P3OUT >> 1) | P3OUT << 7 | P3OUT >> 7);  // out
            for (i=0;i<3;i++)
                P3OUT = ~P3OUT & ((0xF & P3OUT << 1) | (0xF0 & P3OUT >> 1) | P3OUT << 7 | P3OUT >> 7);  // in
            break;
        case 4:             // down counter, extra credit
            if (input_bool == 1) {
                P3OUT = ledPattern04_init;
                input_bool = 0;
            } else
                P3OUT--;
            break;
        case 5:             // rotate one left, extra credit
        if (input_bool == 1) {
                P3OUT = ledPattern05_init;
                input_bool = 0;
            } else
                P3OUT = P3OUT << 1 | P3OUT >> 7;
            break;
        case 6:             // rotate 7 right, extra credit
            if (input_bool == 1) {
                P3OUT = ledPattern06_init;
                input_bool = 0;
            } else
                P3OUT = P3OUT >> 1 | P3OUT << 7;
            break;
        case 7:             // fill to the left, extra credit
            break;

    }
}


//------------------------------------------------------------------------------
// Begin Interrupt Service Routines
//------------------------------------------------------------------------------
#pragma vector = TIMER1_B0_VECTOR
__interrupt void ISR_TB1_CCR0(void)
{
    ledPatterns(2);
    TB1CCTL0 &= ~CCIFG;
}
//-- End Interrupt Service Routines --------------------------------------------