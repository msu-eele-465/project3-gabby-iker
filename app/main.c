#include <msp430.h>
#include "msp430fr2355.h"
#include "intrinsics.h"
#include <stdbool.h>
#include <stdio.h>


//CONSTANTS DECLARATION
#define COL 4
#define ROW 4
#define TABLE_SIZE 4

//END CONSTANTS DECLARATION

void debounce() {
    volatile unsigned int i;
    for (i = 0; i < 20000; i++) {
    }
}

char real_code[] = {'3','9','4','D'};

char keypad[ROW][COL] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

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



int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;
    
    int counter, i, equal;
    char introduced_password[TABLE_SIZE], key; 

    InitializePorts_KeyPad();
      
    while(true)
    {   

     counter = 0;
     key = 0;
    
     printf("Introduce the code\n");

     while (counter < TABLE_SIZE)
     {
        key = read_digit();
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
            break;  // If the code is correct, get out of the loop
        } 
        else 
        {
            printf("Incorrect code. Try again.\n");
            counter = 0;  // Reinitiate counter to try again
        }    
    }    
}
