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

#define INC_B PORTBbits.RB0     // Asignar identificador a RB0
#define DEC_B PORTBbits.RB1     // Asignar identificador a RB1
#define TMR0_setup 246          // Asignar valor de reinicio para el TMR0 (206 para 100ms)

uint8_t valor = 0;
uint8_t selector = 0;
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t centenas = 0;
uint8_t disp0 = 0;
uint8_t disp1 = 0;
uint8_t disp2 = 0;
uint8_t valor_tabla = 0;

// Prototipo de funciones
void setup(void);
void multiplexado(uint8_t select);
void obtener_decimal(uint8_t value);
void display_7seg(uint8_t  unit, uint8_t  dec, uint8_t  cen);
void tabla_7seg(uint8_t decimal);

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
    else if (INTCONbits.T0IF){  // Evaluar bandera de interrupción del TMR0
        TMR0 = TMR0_setup;      // Reiniciar TMR0
        INTCONbits.T0IF = 0;    // Limpiar bandera de interrupción del TMR0
        //PORTC++;              // Incrementar PORTC
        selector++;             // Incremetnar variable selector
        if (selector>2)         // Reiniciar dicha variable si es mayor que 2
            selector = 0;
    }
    return;
}

// Función de configuraciones
void setup(void){
    
    ANSEL = 0;                  // Configurar I/O digitales
    ANSELH = 0;
    
    OSCCONbits.IRCF = 0b011;    // FOSC: 500kHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    TRISA = 0;                  // PORTA como salida
    PORTA = 0;                  // Limpiar PORTA
    TRISC = 0;                  // PORTC como salida
    PORTC = 0;                  // Limpiar PORTC
    TRISD = 0;                  // PORTD como salida
    PORTD = 0;                  // Limpiar PORTD
    TRISBbits.TRISB0 = 1;       // RB0 como entrada
    TRISBbits.TRISB1 = 1;       // RB1 como entrada
    OPTION_REGbits.nRBPU = 0;   // Habilitar resistencias pull-up del PORTB
    WPUBbits.WPUB0 = 1;         // Habilitar pull_up para RB0
    WPUBbits.WPUB1 = 1;         // Habilitar pull_up para RB1 
    
    OPTION_REGbits.T0CS = 0;    // Configurar reloj interno para TMR0
    OPTION_REGbits.PSA = 0;     // Asignar prescaler al TMR0
    OPTION_REGbits.PS = 0b111;  // Prescaler 1:256
    TMR0 = TMR0_setup;                 
    
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.T0IE = 1;        // Habilitar interrupción del TRM0
    INTCONbits.RBIE = 1;        // Habilitar interrupciones del PORTB
    IOCBbits.IOCB0 = 1;         // Habilitar interrpción On_change de RB0
    IOCBbits.IOCB1 = 1;         // Habilitar interrpción On_change de RB1
    INTCONbits.T0IF = 0;        // Limpiar bandera de interrupción del TRM0
    INTCONbits.RBIF = 0;        // Limpiar bandera de interrupción del PORTB
    return;
}

// Función principal
void main (void){
    setup();                        
    while(1){
        multiplexado(selector);     // Multiplexado de displays
        valor = PORTA;              // Guardar el valor del contador en PORTA
        obtener_decimal(valor);     // Obtener el decimal de dicho valor
        display_7seg(unidades, decenas, centenas);  //Configurar valor del display
    }
    return;
}

// Función de multiplexado de displays
void multiplexado(uint8_t selector){
    PORTD = 0;                  // Apagar PORTD (apagar todos los displays)
    switch(selector){           // Evaluar variable para determinar el display que debe encenderse
        case 0:
            PORTC = disp0;      // Mover el valor del display de unidades al PORTC
            PORTDbits.RD0 = 1;  // Encedecer el display de unidades
            break;
        case 1:
            PORTC = disp1;      // Mover el valor del display de decenas al PORTC
            PORTDbits.RD1 = 1;  // Encender el display de decenas
            break;
        case 2:
            PORTC = disp2;      // Mover el valor del display de centenas al PORTC
            PORTDbits.RD2 = 1;  // Encender el display de centenas
            break;
        default:
            PORTD = 0;          // Apagar todos los displays
    }
    return;
}

// Función para obtener el decimal del contador en PORTA
void obtener_decimal(uint8_t valor){
    centenas = 0;
    decenas = 0;
    unidades = 0;
    if (valor >= 100){
        centenas = valor/100;
        valor = valor%100;
    }
    if (valor >= 10){
        decenas = valor/10;
        unidades = valor%10;
    }
    if (valor < 10)
        unidades = valor;
    return;
}

// Función para configurar el valor que debe tener cada display
void display_7seg(uint8_t unidades, uint8_t decenas, uint8_t centenas){
    tabla_7seg(unidades);           // Buscar valor de unidades en la tabla de 7 seg
    disp0 = valor_tabla;            // Guardar valor equivalente en el display de unidades (0)
    
    tabla_7seg(decenas);            // Buscar valor de decenas en la tabla de 7 seg
    disp1 = valor_tabla;            // Guardar valor equivalente en el display de decenas (1)
    
    tabla_7seg(centenas);           // Buscar valor de centenas en la tabla de 7 seg
    disp2 = valor_tabla;            // Guardar valor equivalente en el display de centenas (2)
    return;
}

void tabla_7seg(uint8_t  decimal){
    switch(decimal){
        case 0:
            valor_tabla = 0b00111111;       // 0 en display 7 seg
            break;
        case 1:
            valor_tabla = 0b00000110;       // 1 en display 7 seg
            break;
        case 2:
            valor_tabla = 0b01011011;       // 2 en display 7 seg
            break;
        case 3:
            valor_tabla = 0b01001111;       // 3 en display 7 seg
            break;
        case 4:
            valor_tabla = 0b01100110;       // 4 en display 7 seg
            break;
        case 5:
            valor_tabla = 0b01101101;       // 5 en display 7 seg
            break;
        case 6:
            valor_tabla = 0b01111101;       // 6 en display 7 seg
            break;
        case 7:
            valor_tabla = 0b00000111;       // 7 en display 7 seg
            break;
        case 8:
            valor_tabla = 0b01111111;       // 8 en display 7 seg
            break;
        case 9:
            valor_tabla = 0b01101111;       // 9 en display 7 seg
            break;
        default:
            valor_tabla = 0b00000000;       // Display apagado
            break;
    }
    return;
}
    