//#include <msp430.h>
#include "msp430fr2355.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>

//CONSTANTS DECLARATION
#define COL 4
#define ROW 4
#define TABLE_SIZE 4

//END CONSTANTS DECLARATION

//void debounce() {
  //  volatile unsigned int i;
    //for (i = 0; i < 50000; i++) {
      //  // Simple retardo para debounce
   // }
//}

char real_code[] = {'3','9','4','1'};

char teclado[ROW][COL] = {
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
    P2DIR |= 0x17;    // P2.0, P2.1, P2.2 y P2.4 as outputs
    P2OUT &= ~0x17;   // Initalize columns down
}

void Initialize_RGB()
{
    P6DIR |= BIT1 | BIT2 | BIT3;   // Set pins P6.1, P6.2, y P6.3 as outputs
    P6SEL1 |= BIT1 | BIT2 | BIT3;   // Selecciona la función secundaria de los pines
}

// Read digit from keypad function
char read_digit()
{
int row, col;
volatile int colorState;

    // Go through 4 columns
    for (col = 0; col < 4; col++) {
        // Put column down (active)
        P2OUT &= ~(1 << col);   // P2.0, P2.1, P2.2, P2.4
        
        // Check rows
        for (row = 0; row < 4; row++) {
            if ((P1IN & (1 << row)) == 0) 
            {  
                //Chech if the row is pressed (low)
                //debounce();  // Agregar debounce
                //if ((P1IN & (1 << fila)) == 0) {  // Confirmar que sigue presionada            
                return teclado[row][col];                   
                //}
            }
        }
        // Put the column high (desactivated)
        P2OUT |= (1 << col);
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
    volatile int colorState;
    InitializePorts_KeyPad();
    Initialize_RGB();
     
    while(true)
    {   
     counter = 0;
     key = 0;
     equal = 1;

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
            counter = 0;  // Reinicia el índice para volver a intentar
        }    
    }    
}



