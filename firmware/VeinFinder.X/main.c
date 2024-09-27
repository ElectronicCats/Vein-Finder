/*
 * File:   main.c
 * Author: calvin
 *
 * Created on August 21, 2024, 12:21 PM
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
#include <stdint.h>

#define _XTAL_FREQ 1000000  // Frecuencia del oscilador interno (1MHz)

// Definición de pines
#define BTN_SEL PORTCbits.RC5
#define BTN_UP PORTCbits.RC0
#define BTN_DOWN PORTAbits.RA2
#define LRED PORTCbits.RC1
#define LORAN PORTCbits.RC2
#define RED PORTCbits.RC3
#define ORANGE PORTCbits.RC4

// Variables globales
uint8_t red_level = 1;
uint8_t orange_level = 1;
uint8_t current_color = 0; // 0 para rojo, 1 para naranja

void initialize() {
    // Configurar pines de entrada
    TRISCbits.TRISC5 = 1; // SEL
    TRISCbits.TRISC0 = 1; // UP
    TRISAbits.TRISA2 = 1; // DOWN
    
    // Configurar pines de salida
    TRISCbits.TRISC1 = 0; // LRED
    TRISCbits.TRISC2 = 0; // LORAN
    TRISCbits.TRISC3 = 0; // RED
    TRISCbits.TRISC4 = 0; // ORANGE
    
    // Inicializar LEDs
    LRED = 1;
    LORAN = 0;
    RED = 0;
    ORANGE = 0;
}

void update_leds() {
    if (current_color == 0) { // Modo rojo
        RED = (red_level >= 1);
        ORANGE = 0;
    } else { // Modo naranja
        RED = 0;
        ORANGE = (orange_level >= 1);
    }
}

void main(void) {
    initialize();
    
    while(1) {
        // Comprobar botón SEL
        if (BTN_SEL == 0) {
            __delay_ms(50); // Debounce
            if (BTN_SEL == 0) {
                current_color = !current_color;
                LRED = !current_color;
                LORAN = current_color;
                while (BTN_SEL == 0);
            }
        }
        
        // Comprobar botón UP
        if (BTN_UP == 0) {
            __delay_ms(50); // Debounce
            if (BTN_UP == 0) {
                if (current_color == 0 && red_level < 3) {
                    red_level++;
                } else if (current_color == 1 && orange_level < 3) {
                    orange_level++;
                }
                while (BTN_UP == 0);
            }
        }
        
        // Comprobar botón DOWN
        if (BTN_DOWN == 0) {
            __delay_ms(50); // Debounce
            if (BTN_DOWN == 0) {
                if (current_color == 0 && red_level > 1) {
                    red_level--;
                } else if (current_color == 1 && orange_level > 1) {
                    orange_level--;
                }
                while (BTN_DOWN == 0);
            }
        }
        
        update_leds();
        __delay_ms(10); // Pequeño delay para estabilidad
    }
}