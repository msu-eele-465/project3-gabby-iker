#include "intrinsics.h"
#include "msp430fr2355.h"
#include <msp430.h>
#include <stdbool.h>
#include "led_bar.h"

int new_input_bool = true;
int pattern3_out = true;
int pattern1_cur;
int pattern2_cur;
int pattern3_cur;
int pattern4_cur;
int pattern5_cur;
int pattern6_cur;
int pattern_prev = 0;
char key_cur;
char key_prev = '\0';
bool pattern1_start = false;
bool pattern2_start = false;
bool pattern3_start = false;
bool pattern4_start = false;
bool pattern5_start = false;
bool pattern6_start = false;

//------------------------------------------------------------------------------
// Begin led initialization
//------------------------------------------------------------------------------
void init_led_bar()
{
    #define ledOn 0xFF
    #define ledOff 0
    #define ledPattern01_init 0b10101010
    #define ledPattern02_init 0
    #define ledPattern03_init 0b00011000
    #define ledPattern04_init 0xFF
    #define ledPattern05_init 0b00000001
    #define ledPattern06_init 0b01111111
    #define ledPattern07_init 0b00000001

    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    
    // Setup Ports
    P3OUT &= ~0xFF;             // Clear  Pin 3 to start
    P3DIR |= 0xFF;              // Config Pin 3 as output

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
}
//--End LED Initialization------------------------------------------------------

//------------------------------------------------------------------------------
// Begin LED patterns
//------------------------------------------------------------------------------
void led_patterns(char key_cur) 
{
    switch(key_cur)
    {
        case 'A':
            if (TB1CCR0 - 8192 > 0)
                TB1CCR0 = TB1CCR0 - 8192;
            break;
        case 'B':
            if (TB1CCR0 + 8192 < 0xFFFF)
                TB1CCR0 = TB1CCR0 + 8192;
            break;
        case '0':           // Static state
            P3OUT = ledPattern01_init;
            break;
        case '1':           // Toggle
            if (new_input_bool == true) {
                if (key_cur == key_prev | pattern1_start == false)
                {
                    P3OUT = ledPattern01_init;
                    pattern1_start = true;
                }
                else
                    P3OUT = pattern1_cur;
                new_input_bool = false;
            } else
                pattern1_cur = (P3OUT ^= 0xFF);
            break;
        case '2':             // Up counter
            if (new_input_bool == true) {
                if (key_cur == key_prev | pattern2_start == false)
                {
                    P3OUT = ledPattern02_init;
                    pattern2_start = true;
                }
                else
                    P3OUT = pattern2_cur;
                new_input_bool = false;
            } else
                pattern2_cur = (P3OUT++);
            break;
        case '3':             // in and out
            if (new_input_bool == true) {
                if (key_cur == key_prev | pattern3_start == false)
                {
                    P3OUT = ledPattern03_init;
                    pattern3_start = true;
                }
                else
                    P3OUT = pattern3_cur;
                new_input_bool = false;
            }
            else if ((pattern3_out == true & P3OUT != 0b10000001) | (pattern3_out == false & P3OUT == 0b00011000))  // out
            {
                pattern3_cur = (P3OUT = ~P3OUT & ((0xF0 & P3OUT << 1) | (0xF & P3OUT >> 1) | P3OUT << 7 | P3OUT >> 7));
                pattern3_out = true;
            }
            else if ((pattern3_out == false & P3OUT != 0b00011000) | (pattern3_out == true & P3OUT == 0b10000001))  // in
            {
                pattern3_cur = (P3OUT = ~P3OUT & ((0xF & P3OUT << 1) | (0xF0 & P3OUT >> 1) | P3OUT << 7 | P3OUT >> 7));
                pattern3_out = false;
            }
            break;
        case '4':             // down counter, extra credit
            if (new_input_bool == true) {
                if (key_cur == key_prev | pattern4_start == false)
                {
                    P3OUT = ledPattern04_init;
                    pattern4_start = true;
                }
                else
                    P3OUT = pattern4_cur;
                new_input_bool = false;
            } else
                pattern4_cur = (P3OUT--);
            break;
        case '5':             // rotate one left, extra credit
        if (new_input_bool == true) {
            if (key_cur == key_prev | pattern5_start == false)
                {
                    P3OUT = ledPattern05_init;
                    pattern5_start = true;
                }
            else
                P3OUT = pattern5_cur;
                new_input_bool = false;
            } else
                pattern5_cur = (P3OUT = P3OUT << 1 | P3OUT >> 7);
            break;
        case '6':             // rotate 7 right, extra credit
            if (new_input_bool == true) {
                if (key_cur == key_prev | pattern6_start == false)
                {
                    P3OUT = ledPattern06_init;
                    pattern6_start = true;
                }
                else
                    P3OUT = pattern6_cur;
                new_input_bool = false;
            } else
                pattern6_cur = (P3OUT = P3OUT >> 1 | P3OUT << 7);
            break;
        case 7:             // fill to the left, extra credit
            break;
        default:
            P3OUT = ledOff;
            break;
    }
}

void set_led_bar(char key_input)
{
    key_prev = key_cur;
    key_cur = key_input;       // Update global key_cur for ISR
    new_input_bool = true;
    led_patterns(key_cur);
}


//------------------------------------------------------------------------------
// Begin Interrupt Service Routines
//------------------------------------------------------------------------------
#pragma vector = TIMER1_B0_VECTOR
__interrupt void ISR_TB1_CCR0(void)
{
    led_patterns(key_cur);
    TB1CCTL0 &= ~CCIFG;
}
//-- End Interrupt Service Routines --------------------------------------------
