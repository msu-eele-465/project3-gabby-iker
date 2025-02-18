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

// Función para inicializar los puertos
void InitializePorts_KeyPad() 
{
    // Configurar filas como entradas (con pull-up)
    P1DIR &= ~0x0F;   // P1.0, P1.1, P1.2, P1.3 como entradas
    P1REN |= 0x0F;    // Habilitar resistencias pull-up
    P1OUT |= 0x0F;    // Activar pull-up en las filas
    
    // Configurar columnas como salidas
    P2DIR |= 0x17;    // P2.0, P2.1, P2.2 y P2.4 como salidas
    P2OUT &= ~0x17;   // Inicializar columnas en bajo
}

//void InitializePorts_RGB_LED()
//{
    //P1DIR |= BIT5 | BIT6 | BIT7;    // Set LED as an output (P1.5, P1.6, P1.7)
    //P1OUT &= ~(BIT5 | BIT6 | BIT7); // Turn OFF LEDs
//}

void Initialize_RGB()
{
    P6DIR |= BIT1 | BIT2 | BIT3;   // Configura los pines P6.1, P6.2, y P6.3 como salidas
    P6SEL1 |= BIT1 | BIT2 | BIT3;   // Selecciona la función secundaria de los pines
}

void TimerConfiguration()
{
    // Configurar TimerB3 para PWM
    TB3CCR0 = 255; // Resolución de 8 bits

    // Modo PWM para los canales 1, 2 y 3
    TB3CCTL1 = OUTMOD_7; // Rojo (P6.1)
    TB3CCTL2 = OUTMOD_7; // Verde (P6.2)
    TB3CCTL3 = OUTMOD_7; // Azul (P6.3)

    TB3CTL = TBSSEL_2; // SMCLK
    TB3CTL = MC_1; // UP MODE
    TB3CTL = ID_0;

}

void setColor(unsigned int r, unsigned int g, unsigned int b) {
    // Establecer los valores PWM para RGB
    TB3CCR1 = r; // Rojo (P6.1)
    TB3CCR2 = g; // Verde (P6.2)
    TB3CCR3 = b; // Azul (P6.3)
    return;
}


// Read digit from keypad function
char read_digit()
{
int fila, col;

    // Recorrer las 4 columnas
    for (col = 0; col < 4; col++) {
        // Poner la columna en bajo (activa)
        P2OUT &= ~(1 << col);   // P2.0, P2.1, P2.2, P2.4
        
        // Comprobar las filas
        for (fila = 0; fila < 4; fila++) {
            if ((P1IN & (1 << fila)) == 0) {  // Si la fila está presionada (baja)
                //debounce();  // Agregar debounce
                //if ((P1IN & (1 << fila)) == 0) {  // Confirmar que sigue presionada
                    //P1OUT &= ~BIT5; // P1.5 OFF (LOW) - Red LED
                    //P1OUT |= BIT6;  // P1.6 ON (HIGH) - Green LED
                    setColor(196, 146, 29);
                    return teclado[fila][col];
                   
                //}
            }
        }

        // Poner la columna en alto (desactiva)
        P2OUT |= (1 << col);
    }

    return 0; // Ninguna tecla presionada
}

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;
    
    int counter, i;
    char introduced_password[TABLE_SIZE], key;
    InitializePorts_KeyPad();
    Initialize_RGB();
    TimerConfiguration();
    //InitializePorts_RGB_LED();
    //P1OUT |= BIT5;   // P1.5 en ON (HIGH) - Red LED
    setColor(196, 62, 29);
 
    while(true)
    {   

     counter=0;
     key=0;

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
     // Compara el código introducido con el código real
        int coincide = 1;
        for (i = 0; i < TABLE_SIZE; i++) {
            if (introduced_password[i] != real_code[i]) 
            {
                coincide = 0;
                break;
            }
        }

        // Verifica el código
        if (coincide==1) {
            printf("Código correcto!\n");
            counter = 0;
            //P1OUT &= ~BIT6; // P1.6 OFF (LOW) - Green LED
            //P1OUT |= BIT7;   // P1.7 ON (HIGH) - Blue LED
            setColor(29, 162, 196);
            break;  // Si el código es correcto, sale del bucle
        } else {
            printf("Código incorrecto. Intenta nuevamente.\n");
            counter = 0;  // Reinicia el índice para volver a intentar
            setColor(196, 62, 29);


            //Si no funciona tendremos algo
            //P1OUT &= ~BIT6; // P1.6 OFF (LOW) - Green LED
            //P1OUT |= BIT5;   // P1.5 en ON (HIGH) - Red LED
        }    
    }    
}



