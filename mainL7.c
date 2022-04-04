/* 
 * File:   mainL7.c
 * Author: Jeferson Noj
 *
 * Created on 3 de abril de 2022, 11:37 AM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000      // Establecer frecuencia del oscilador (4MHz)
#define INC_B PORTBbits.RB0     // Asignar identificador a RB0
#define DEC_B PORTBbits.RB1     // Asignar identificador a RB1

// Prototipo de funciones
void setup(void);

// Interrupciones del PORTB
void __interrupt() isr (void){
    if(INTCONbits.RBIF){        // Evaluar bandera de interrupción del PORTB
        if(!INC_B){             // Evaluar boton de incremento
            PORTA++;            // Aumentar PORTA si el boton de incremento se presionó 
        }
        else if (!DEC_B){       // Evaluar boton de decremento (solo si no se presionó el de incrmento)
            PORTA--;            // Disminuir PORTA si el boton de decremento se presionó
        }
        INTCONbits.RBIF = 0;    // Limpiar bandera de interrupción del PORTB
    }
    return;
}

void setup(void){
    
    ANSEL = 0;                 // Configurar I/O digitales
    ANSELH = 0;
    
    OSCCONbits.IRCF = 0b0110;   // FOSC: 4MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    TRISA = 0;                  // PORTA como salida
    PORTA = 0;                  // Limpiar PORTA
    TRISBbits.TRISB0 = 1;       // RB0 como entrada
    TRISBbits.TRISB1 = 1;       // RB1 como entrada
    OPTION_REGbits.nRBPU = 0;   // Habilitar resistencias pull-up del PORTB
    WPUBbits.WPUB0 = 1;         // Habilitar pull_up para RB0
    WPUBbits.WPUB1 = 1;         // Habilitar pull_up para RB1 
    
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.RBIE = 1;        // Habilitar interrupciones del PORTB
    IOCBbits.IOCB0 = 1;         // Habilitar interrpción On_change de RB0
    IOCBbits.IOCB1 = 1;         // Habilitar interrpción On_change de RB1
    INTCONbits.RBIF = 0;        // Limpiar bandera de interrupción del PORTB
}

void main (void){
    setup();                        
    while(1){
    }
    return;
}