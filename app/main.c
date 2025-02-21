#include <msp430.h>
#include <stdbool.h>
#include "rgb_led.h"

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;
    init_rgb_led();

    while(true)
    {
    }
}
