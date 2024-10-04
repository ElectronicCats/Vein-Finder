/*
 * File:   main.c
 * Author: calvin
 *
 * Created on October 4, 2024, 10:01 PM
 */

// Configuración del PIC16F15323
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits->HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1
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

#define _XTAL_FREQ 32000000  // Frecuencia del oscilador interno (32MHz)

// Definición de pines
#define BTN_SEL PORTCbits.RC5
#define BTN_UP PORTCbits.RC0
#define BTN_DOWN PORTAbits.RA2

#define LRED PORTCbits.RC1
#define LORAN PORTCbits.RC2

// Variables globales
uint8_t red_level = 1;
uint8_t orange_level = 1;
uint8_t current_color = 0; // 0 para rojo, 1 para naranja

void initialize() {
    // Configurar pines de entrada
    TRISCbits.TRISC5 = 1; // SEL
    TRISCbits.TRISC0 = 1; // UP
    TRISAbits.TRISA2 = 1; // DOWN
    
    // Activar resistencias de pull-up
    WPUCbits.WPUC5 = 1; // Pull-up para SEL
    WPUCbits.WPUC0 = 1; // Pull-up para UP
    WPUAbits.WPUA2 = 1; // Pull-up para DOWN
    
    // Configurar pines de salida
    TRISCbits.TRISC1 = 0; // LRED
    TRISCbits.TRISC2 = 0; // LORAN
    TRISCbits.TRISC3 = 0; // RED (PWM)
    TRISCbits.TRISC4 = 0; // ORANGE (PWM)
    
    // Inicializar LEDs
    LRED = 1;
    LORAN = 0;

    // Configurar PWM
    // PWM3 en RC3 para LED rojo
    PWM3CON = 0x80;  // PWM enabled
    PWM3DCH = 0;     // Duty cycle inicial
    PWM3DCL = 0;

    // PWM4 en RC4 para LED naranja
    PWM4CON = 0x80;  // PWM enabled
    PWM4DCH = 0;     // Duty cycle inicial
    PWM4DCL = 0;

    // Configurar Timer2 para PWM
    T2CON = 0b00000100;  // Timer2 ON, prescaler 1:1
    PR2 = 255;  // Periodo PWM

    // Configurar PPS para PWM
    RC3PPS = 0x0E;  // PWM3 en RC3
    RC4PPS = 0x0F;  // PWM4 en RC4
}

void set_pwm_duty(uint8_t pwm, uint8_t duty) {
    if (pwm == 3) {
        PWM3DCH = duty;
        PWM3DCL = 0;
    } else if (pwm == 4) {
        PWM4DCH = duty;
        PWM4DCL = 0;
    }
}

void update_leds() {
    if (current_color == 0) { // Modo rojo
        LRED = 1;
        LORAN = 0;
        set_pwm_duty(3, (red_level - 1) * 85);  // 0, 85, 170 para niveles 1, 2, 3
        set_pwm_duty(4, 0);
    } else { // Modo naranja
        LRED = 0;
        LORAN = 1;
        set_pwm_duty(3, 0);
        set_pwm_duty(4, (orange_level - 1) * 85);  // 0, 85, 170 para niveles 1, 2, 3
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
