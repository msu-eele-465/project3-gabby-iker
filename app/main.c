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

//void debounce() {
  //  volatile unsigned int i;
    //for (i = 0; i < 50000; i++) {
      //  // Simple retardo para debounce
   // }
//}

char real_code[] = {'3','9','4','1'};

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
    P2DIR |= 0x17;    // P2.0, P2.1, P2.2 y P2.4 as outputs
    P2OUT &= ~0x17;   // Initalize columns down
}

void Initialize_PinsRGB()
{       
    P1DIR |= BIT5 | BIT6 | BIT7; // Set P1.5, P1.6 y P1.7 as an output 
    P1OUT &= ~(BIT5 | BIT6 | BIT7); // Initialize outputs as low
    P1OUT |= BIT5;
    P1OUT |= BIT6;
    P1OUT |= BIT7;
}

void Initialize_Interrupts()
{
    //Set up timer 3
    TB3CTL |= TBCLR;
    TB3CTL |= TBSSEL__ACLK;
    
    //Set up timer compare 3.1 (Pull up)
    TB3CCTL0 &= ~CCIFG;
    TB3CCTL0 |= CCIE;
    TB3CCR0 |= 255;

    //Set up timer compare 3.2 (Pull down RED)
    TB3CCTL1 &= ~CCIFG;
    TB3CCTL1 |= CCIE;
    TB3CCR1 |= 196;
    TB3CCTL1 |= OUTMOD__7;

    //Set up timer compare 3.3 (Pull down GREEN)
    TB3CCTL2 &= ~CCIFG;
    TB3CCTL2 |= CCIE;
    TB3CCR2 |= 62;
    TB3CCTL2 | = OUTMOD_7;

    //Set up timer compare 3.4 (Pull down BLUE)
    TB3CCTL3 &= ~CCIFG;
    TB3CCTL3 |= CCIE;
    TB3CCR3 |= 29;
    TB3CCTL3 | = OUTMOD_7;

    TB3CTL |= MC__UP;


    _enable_interrupt();
}

// Read digit from keypad function
char read_digit()
{
int row, col;

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
                return keypad[row][col];                   
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
    Initialize_PinsRGB();
    Initialize_Interrupts();
    
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

#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_TB3_CCR0(void)
{
    P1OUT |= BIT5;
    P1OUT |= BIT6;
    P1OUT |= BIT7;
    TB3CCTL0 &= ~CCIFG;
}

#pragma vector = TIMER3_B1_VECTOR
__interrupt void ISR_TB3_CCR1(void)
{
    P1OUT &=~ BIT5;
    TB3CCTL1 &= ~CCIFG;
}

#pragma vector = TIMER3_B2_VECTOR
__interrupt void ISR_TB3_CCR2(void)
{
    P1OUT &=~ BIT6;
    TB3CCTL2 &= ~CCIFG;
}

#pragma vector = TIMER3_B3_VECTOR
__interrupt void ISR_TB3_CCR3(void)
{
    P1OUT &=~ BIT7;
    TB3CCTL3 &= ~CCIFG;
}