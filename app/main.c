#include "intrinsics.h"
#include "msp430fr2355.h"
#include <msp430.h>
#include <stdbool.h>
#include "heartbeat.h"
#include "led_bar.h"

void init()
{
    //init_heartbeat();
    init_led_bar();
}

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    init();
    

    while(true)
    {
    }
    return 0;
}


