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

// Prototipo de la función para configurar PWM
void setup_PWM(void);

// Prototipo para actualizar el nivel de PWM
void update_PWM(unsigned char level);

int main(void) {
    // Variable de nivel para el brillo (valor de 0 a 255)
    unsigned char level = 128; // Inicialmente 50% del brillo máximo
    
    // Configuración de PWM
    setup_PWM();
    
    while (1) {
        // Aquí podrías modificar el valor de `level` para cambiar el brillo
        // Ejemplo simple de cambiar el brillo (ajusta según tus necesidades)
        for (level = 0; level < 255; level++) {
            update_PWM(level);
            __delay_ms(10); // Retardo para visualizar el cambio de brillo
        }
        
        for (level = 255; level > 0; level--) {
            update_PWM(level);
            __delay_ms(10); // Retardo para visualizar el cambio de brillo
        }
    }
    
    return 0;
}

void setup_PWM(void) {
    // Configuración del oscilador interno a 1 MHz
    OSCCON1bits.NOSC = 0b110;   // Oscilador HFINTOSC seleccionado
    OSCCON1bits.NDIV = 0b0011;  // Divisor de 8 para reducir la frecuencia a 1 MHz
    OSCFRQbits.HFFRQ = 0b000;   // Oscilador interno configurado a 1 MHz
    
    // Configuración de PPS para asignar las salidas PWM
    RC3PPS = 0x0B;  // Asignar PWM3 al pin RC3
    RC4PPS = 0x0C;  // Asignar PWM4 al pin RC4

    // Configuramos RC3 y RC4 como salidas para PWM
    TRISCbits.TRISC3 = 0;  // RC3 como salida
    TRISCbits.TRISC4 = 0;  // RC4 como salida

    // Configuración de PWM3 para el pin RC3
    PWM3CON = 0b10000000;  // Habilitar PWM3
    PWM3DCH = 0;           // Ciclo de trabajo alto inicialmente 0
    PWM3DCL = 0;           // Ciclo de trabajo bajo inicialmente 0

    // Configuración de PWM4 para el pin RC4
    PWM4CON = 0b10000000;  // Habilitar PWM4
    PWM4DCH = 0;           // Ciclo de trabajo alto inicialmente 0
    PWM4DCL = 0;           // Ciclo de trabajo bajo inicialmente 0

    // --- Configuración de Timer2 según tu solicitud ---
    T2CON = 0x60;          // Postcaler 1:16, prescaler 1:1
    T2CLKCON = 0x01;       // Fuente de reloj Fosc/4
    T2HLT = 0x00;          // Modo normal (no modo monostable)
    T2RST = 0x00;          // Sin fuentes de reinicio
    T2PR = 0xFF;           // Periodo máximo
    T2TMR = 0x00;          // Reiniciar el contador de Timer2
    T2CONbits.TMR2ON = 1;  // Encender Timer2

}

void update_PWM(unsigned char level) {
    // Ajustar el ciclo de trabajo en RC3 (PWM3)
    PWM3DCH = level >> 2;        // Los 8 bits más significativos
    PWM3DCL = (level & 0x03) << 6; // Los 2 bits menos significativos

    // Ajustar el ciclo de trabajo en RC4 (PWM4)
    PWM4DCH = level >> 2;        // Los 8 bits más significativos
    PWM4DCL = (level & 0x03) << 6; // Los 2 bits menos significativos
}
