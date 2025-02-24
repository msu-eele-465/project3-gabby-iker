#include <msp430.h>
#include "msp430fr2355.h"
#include "intrinsics.h"
#include <stdbool.h>
#include <stdio.h>
#include "rgb_led.h"
#include "led_bar.h"
#include "heartbeat.h"


//CONSTANTS DECLARATION
#define COL 4
#define ROW 4
#define TABLE_SIZE 4

//END CONSTANTS DECLARATION

void debounce() {
    volatile unsigned int i;
    for (i = 20000; i > 0; i--) {}
}

char real_code[] = {'3','9','4','D'};

char keypad[ROW][COL] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

int lockState = 3;

// Function ton initialaize the ports
void InitializePorts_KeyPad() 
{
    // Set rows as inputs (with pull-up)
    P1DIR &= ~0x0F;   // P1.0, P1.1, P1.2, P1.3 as inputs
    P1REN |= 0x0F;    // Activate pull-up
    P1OUT |= 0x0F;    // Activar pull-up in rows
    
    // Set columns as outputs
    P5DIR |= BIT0 | BIT1 | BIT2 | BIT3; // Set P5.0, P5.1, P5.2 y P5.3 as outputs:
    P5OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);  // Set down the pins P5.0, P5.1, P5.2 y P5.3:
}

// Read digit from keypad function
char read_digit()
{
int row, col;

    // Go through 4 columns
    for (col = 0; col < 4; col++) {
        // Put column down (active)
        P5OUT &= ~(1 << col);   // P2.0, P2.1, P2.2, P2.4
         __delay_cycles(1000);  // Little stop to stabilize the signal

        // Recorre las 4 filas
        for (row = 0; row < 4; row++) {
            if ((P1IN & (1 << row)) == 0) {  // We detect that the row is low

                debounce();  // Wait to filter the bouncing
                if ((P1IN & (1 << row)) == 0) {  // Confirm that the key is pressed
                    char key = keypad[row][col];
                    // Wait until the key is released to avoid multiple readings 
                    while ((P1IN & (1 << row)) == 0);
                    // Deactivate the column before returning
                    P5OUT |= (1 << col);                    
                    return key;
                }
            }
        }
        // Put the column high (desactivated)
        P5OUT |= (1 << col);
    }

    return 0; // No key pressed
}

char System_Unlocking(void)
{
    int row, col;

    // Go through 4 columns
    for (col = 0; col < 4; col++) {
        // Put column down (active)
        P5OUT &= ~(1 << col);   // P2.0, P2.1, P2.2, P2.4
         __delay_cycles(1000);  // Little stop to stabilize the signal

        // Go through 4 rows
        for (row = 0; row < 4; row++) {
            if ((P1IN & (1 << row)) == 0) {  // We detect that the row is low
                debounce();  // Wait to filter the bouncing
                if ((P1IN & (1 << row)) == 0) {  // Confirm that the key is pressed
                    continue_rgb_led(0);                // Set LED to yellow
                    char key = keypad[row][col];
                    // Wait until the key is released to avoid multiple readings 
                    while ((P1IN & (1 << row)) == 0);
                    // Deactivate the column before returning
                    P5OUT |= (1 << col);                    
                    return key;
                }
            }
        }
        // Put the column high (desactivated)
        P5OUT |= (1 << col);
    }

    return 0; // No key pressed
}

char System_Unlocked(void)
{
    char key_unlocked = '\0';

    // Continuously poll until 'D' is pressed
    while (key_unlocked != 'D') {
        int row, col;

        for (col = 0; col < 4; col++) {
            // Activate column
            P5OUT &= ~(1 << col);
            __delay_cycles(1000);

            // Check all rows
            for (row = 0; row < 4; row++) {
                if ((P1IN & (1 << row)) == 0) {
                    debounce();
                    if ((P1IN & (1 << row)) == 0) {
                        key_unlocked = keypad[row][col];
                        if (key_unlocked != 'D') {
                            set_led_bar(key_unlocked);
                        }
                        // Wait for key release
                        while ((P1IN & (1 << row)) == 0);
                        P5OUT |= (1 << col);

                        if (key_unlocked == 'D') {
                            continue_rgb_led(3);  // Set LED to red when 'D' is pressed
                            set_led_bar('D');
                            return key_unlocked;
                        }
                    }
                }
            }
            // Deactivate column
            P5OUT |= (1 << col);
        }
    }

    return key_unlocked;
}



int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;
    
    int counter, i, equal;
    char introduced_password[TABLE_SIZE], key, save_digit, key_unlocked; 

    InitializePorts_KeyPad();
    init_heartbeat();
    init_rgb_led();
    init_led_bar();
      
    while(true)
    {   

     counter = 0;
     key = 0;
    
     printf("Introduce the code\n");

     while (counter < TABLE_SIZE)
     {
        key = System_Unlocking();
        if(key!=0)
        {
            introduced_password [counter] = key;
            counter++;
        }        
     }

     //Compare the introduced code with the real code   
     equal = 1;   
     for (i = 0; i < TABLE_SIZE; i++) {
        if (introduced_password[i] != real_code[i]) 
        {
            equal = 0;
            break;
        }
    }

        // Verify the code
        if (equal==1) 
        {
            printf("Correct Code!\n");
            counter = 0;
            continue_rgb_led(1);            // Set LED to blue
            for (i = 0; i < TABLE_SIZE; i++) 
            {
                introduced_password[i] = 0;        
            }
            System_Unlocked();  // This now handles polling until 'D' is pressed


        } 
        else 
        {
            printf("Incorrect code. Try again.\n");
            counter = 0;  // Reinitiate counter to try again
            continue_rgb_led(3);            // Set LED to red
            led_patterns('\0');
            for (i = 0; i < TABLE_SIZE; i++) 
            {
                introduced_password[i] = 0;        
            }
        }    
    }
}
