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

#define LRED LATCbits.LATC1
#define LORAN LATCbits.LATC2

//#define RED LATCbits.LATC3
//#define ORANGE LATCbits.LATC4

// Variables globales
uint8_t red_level = 5;
uint8_t orange_level = 5;
uint8_t current_color = 0; // 0 para rojo, 1 para naranja

void initialize() {
    
    // Desbloquear PPS para permitir cambios
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0; // PPS desbloqueado

    // Configuración de PPS para asignar las salidas PWM
    RC3PPS = 0x0B;  // Asignar PWM3 al pin RC3
    RC4PPS = 0x0C;  // Asignar PWM4 al pin RC4

    // Bloquear PPS después de configurarlo
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 1; // PPS bloqueado
    
    // Configurar pines de entrada
    TRISCbits.TRISC5 = 1; // SEL
    TRISCbits.TRISC0 = 1; // UP
    TRISAbits.TRISA2 = 1; // DOWN
    
    // Configurar pines de salida
    TRISCbits.TRISC1 = 0; // LRED
    TRISCbits.TRISC2 = 0; // LORAN
    
    TRISCbits.TRISC3 = 0; // RED
    TRISCbits.TRISC4 = 0; // ORANGE
    
    // Desactivar funciones analógicas en los pines que usan entradas digitales
    ANSELAbits.ANSA2 = 0;  // RA2 como digital
    ANSELCbits.ANSC0 = 0;  // RC0 como digital
    ANSELCbits.ANSC5 = 0;  // RC5 como digital

    // Activar resistencias pull-up para los botones
    WPUAbits.WPUA2 = 1;    // Habilitar pull-up en RA2 (BTN_DOWN)
    WPUCbits.WPUC0 = 1;    // Habilitar pull-up en RC0 (BTN_UP)
    WPUCbits.WPUC5 = 1;    // Habilitar pull-up en RC5 (BTN_SEL)
    
    // Configurar PWM
    
    // Configuración de PWM3 para el pin RC3
    PWM3CON = 0b10000000;  // Habilitar PWM3
    PWM3DCH = 0;           // Ciclo de trabajo alto inicialmente 0
    PWM3DCL = 0;           // Ciclo de trabajo bajo inicialmente 0

    // Configuración de PWM4 para el pin RC4
    PWM4CON = 0b10000000;  // Habilitar PWM4
    PWM4DCH = 0;           // Ciclo de trabajo alto inicialmente 0
    PWM4DCL = 0;           // Ciclo de trabajo bajo inicialmente 0

    // --- Configuración de Timer2 según tu solicitud ---
    T2CON = 0x40;          // Prescaler 1:1 Postcaler 1:1
    T2CLKCON = 0x01;       // Fuente de reloj Fosc/4
    T2HLT = 0x00;          // Modo normal (no modo monostable)
    T2RST = 0x00;          // Sin fuentes de reinicio
    T2PR = 0xFF;           // Periodo máximo
    T2TMR = 0x00;          // Reiniciar el contador de Timer2
    T2CONbits.TMR2ON = 1;  // Encender Timer2

    // Inicializar LEDs indicadores
    LRED = 0;
    LORAN = 0;
}

void blink() {

       // Encender los LEDs
        LRED = 1;
        LORAN = 1;
        //RED = 1;
        //ORANGE = 1;
        __delay_ms(200); // Esperar 1/2 segundo
        
        // Apagar los LEDs
        LRED = 0;
        LORAN = 0;
        //RED = 0;
        //ORANGE = 0;
        __delay_ms(200); // Esperar 1/2 segundo

}

void set_pwm_duty(uint8_t pwm, uint8_t level) {
    uint16_t duty;
    
    // Calcular el duty cycle según el nivel
    if (level == 0) {
        duty = 0;  // Apagar el LED en nivel 0
    } else {
        duty = (level * 51);  // Niveles 1 a 5: 51, 102, 153, 204, 255
    }
    
    if (pwm == 3) {
        PWM3DCH = duty >> 2;
        PWM3DCL = (duty & 0x03) << 6;
    } else if (pwm == 4) {
        PWM4DCH = duty >> 2;
        PWM4DCL = (duty & 0x03) << 6;
    }
}

void update_leds() {
    if (current_color == 0) { // Modo rojo
        set_pwm_duty(3, red_level);
    } else { // Modo naranja
        set_pwm_duty(4, orange_level);
    }
}

void main(void) {
    initialize();
    blink();
    blink();
    blink();
    
    current_color = 0; // LED rojo
    update_leds();
    current_color = 1; // LED naranja
    update_leds();
    
    LRED = 1;
    
    while(1) {
        // Comprobar botón SEL
        if (BTN_SEL == 0) {
            __delay_ms(30); // Debounce
            if (BTN_SEL == 0) {
                current_color = !current_color;
                LRED = current_color;
                LORAN = !current_color;
                while (BTN_SEL == 0); // Esperar a que el botón se suelte
            }
        }
        
        // Comprobar botón UP
        if (BTN_UP == 0) {
            __delay_ms(30); // Debounce
            if (BTN_UP == 0) {
                if (current_color == 0 && red_level < 5) {
                    red_level++;
                } else if (current_color == 1 && orange_level < 5) {
                    orange_level++;
                }
                while (BTN_UP == 0); // Esperar a que el botón se suelte
            }
        }
        
        // Comprobar botón DOWN
        if (BTN_DOWN == 0) {
            __delay_ms(30); // Debounce
            if (BTN_DOWN == 0) {
                if (current_color == 0 && red_level > 0) {
                    red_level--;
                } else if (current_color == 1 && orange_level > 0) {
                    orange_level--;
                }
                while (BTN_DOWN == 0); // Esperar a que el botón se suelte
            }
        }
        
        update_leds();
        __delay_ms(10); // Pequeño delay para estabilidad
    }
}
