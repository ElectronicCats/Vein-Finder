/*
 * File:   main.c
 * Author: calvin
 *
 * Programa blink simplificado para los LEDs LRED, LORAN, RED y ORANGE.
 */

// Configuración del PIC16F15323
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINT1  // Power-up default value for COSC bits->HFINTOSC (1MHz)
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit->CLKOUT function is disabled
#pragma config CSWEN = ON       // Clock Switch Enable bit->Writing to NOSC and NDIV is allowed
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit->Fail-Safe Clock Monitor is enabled

#pragma config MCLRE = ON       // Master Clear Enable bit->MCLR pin is Master Clear function
#pragma config PWRTE = OFF      // Power-up Timer Enable bit->Power up timer disabled
#pragma config WDTE = OFF       // Watchdog Timer Enable bits->WDT disabled
#pragma config LPBOREN = OFF    // Low-power BOR enable bit->ULPBOR disabled
#pragma config BOREN = ON       // Brown-out Reset Enable bits->Brown-out Reset enabled
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit->The PPSLOCK bit can be cleared and set only once
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a Reset

#include <xc.h>

#define _XTAL_FREQ 1000000  // Frecuencia del oscilador interno (1MHz)

// Definición de pines
#define LRED LATCbits.LATC1
#define LORAN LATCbits.LATC2
#define RED LATCbits.LATC3
#define ORANGE LATCbits.LATC4

void initialize() {
    // Configurar pines de salida para los LEDs
    TRISCbits.TRISC1 = 0; // LRED
    TRISCbits.TRISC2 = 0; // LORAN
    TRISCbits.TRISC3 = 0; // RED
    TRISCbits.TRISC4 = 0; // ORANGE
    
    // Inicializar LEDs apagados
    LRED = 0;
    LORAN = 0;
    RED = 0;
    ORANGE = 0;
}

void main(void) {
    initialize();
    
    while(1) {
        // Encender los LEDs
        LRED = 1;
        LORAN = 1;
        RED = 1;
        ORANGE = 1;
        __delay_ms(500); // Esperar 1/2 segundo
        
        // Apagar los LEDs
        LRED = 0;
        LORAN = 0;
        RED = 0;
        ORANGE = 0;
        __delay_ms(500); // Esperar 1/2 segundo
    }
}

