/*
  Created by Raul Vargas @ Electronic Cats 2024
 
  This code is designed for a Vein Finder device, which uses red and orange LEDs
  to enhance the contrast of veins beneath the skin. By adjusting the brightness levels 
  of the LEDs through PWM, the device optimizes the illumination for better 
  visibility of veins, aiding in procedures like venipuncture. 
  The alternating control between the red and orange LEDs helps in achieving
  the best contrast for different skin tones and vein depths.

  The code controls the PWM output on a PIC16F15323 microcontroller, allowing adjustment 
  across multiple brightness levels via three physical buttons. It also alternates between 
  red and orange LEDs to independently manage their intensity.
  
  This code is beerware; if you see me (or any other Electronic Cats
  member) at the local, and you've found our code helpful,
  please buy us a round!
  Distributed as-is; no warranty is given.

  Created on August 21, 2024, 12:21 PM
 */

// PIC16F15323 Configuration
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINT1  // Power-up default value for COSC bits->HFINTOSC (1MHz)
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit->CLKOUT function is disabled
#pragma config CSWEN = ON       // Clock Switch Enable bit->Writing to NOSC and NDIV is allowed
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit->Fail-Safe Clock Monitor is enabled

#pragma config MCLRE = ON       // Master Clear Enable bit->MCLR pin is Master Clear function
#pragma config PWRTE = OFF      // Power-up Timer Enable bit->Power-up timer disabled
#pragma config WDTE = OFF       // Watchdog Timer Enable bits->WDT disabled
#pragma config LPBOREN = OFF    // Low-power BOR enable bit->ULPBOR disabled
#pragma config BOREN = ON       // Brown-out Reset Enable bits->Brown-out Reset enabled
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit->The PPSLOCK bit can be cleared and set only once
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a Reset

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 1000000  // Internal oscillator frequency (1MHz)

// Pin definitions
#define BTN_SEL PORTCbits.RC5
#define BTN_UP PORTCbits.RC0
#define BTN_DOWN PORTAbits.RA2

#define LRED LATCbits.LATC1
#define LORAN LATCbits.LATC2

//#define RED LATCbits.LATC3
//#define ORANGE LATCbits.LATC4

// Global variables
uint8_t red_level = 5;
uint8_t orange_level = 5;
uint8_t current_color = 0; // 0 for red, 1 for orange

void initialize() {
    
    // Unlock PPS to allow changes
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0; // PPS unlocked

    // PPS configuration to assign PWM outputs
    RC3PPS = 0x0B;  // Assign PWM3 to RC3 pin
    RC4PPS = 0x0C;  // Assign PWM4 to RC4 pin

    // Lock PPS after configuring
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 1; // PPS locked
    
    // Configure input pins
    TRISCbits.TRISC5 = 1; // SEL
    TRISCbits.TRISC0 = 1; // UP
    TRISAbits.TRISA2 = 1; // DOWN
    
    // Configure output pins
    TRISCbits.TRISC1 = 0; // LRED
    TRISCbits.TRISC2 = 0; // LORAN
    
    TRISCbits.TRISC3 = 0; // RED
    TRISCbits.TRISC4 = 0; // ORANGE
    
    // Disable analog functions on pins using digital inputs
    ANSELAbits.ANSA2 = 0;  // RA2 as digital
    ANSELCbits.ANSC0 = 0;  // RC0 as digital
    ANSELCbits.ANSC5 = 0;  // RC5 as digital

    // Enable pull-up resistors for the buttons
    WPUAbits.WPUA2 = 1;    // Enable pull-up on RA2 (BTN_DOWN)
    WPUCbits.WPUC0 = 1;    // Enable pull-up on RC0 (BTN_UP)
    WPUCbits.WPUC5 = 1;    // Enable pull-up on RC5 (BTN_SEL)
    
    // Configure PWM
    
    // PWM3 configuration for RC3 pin
    PWM3CON = 0b10000000;  // Enable PWM3
    PWM3DCH = 0;           // High duty cycle initially 0
    PWM3DCL = 0;           // Low duty cycle initially 0

    // PWM4 configuration for RC4 pin
    PWM4CON = 0b10000000;  // Enable PWM4
    PWM4DCH = 0;           // High duty cycle initially 0
    PWM4DCL = 0;           // Low duty cycle initially 0

    // --- Timer2 Configuration ---
    T2CON = 0x40;          // Prescaler 1:1 Postcaler 1:1
    T2CLKCON = 0x01;       // Clock source Fosc/4
    T2HLT = 0x00;          // Normal mode (no monostable mode)
    T2RST = 0x00;          // No reset sources
    T2PR = 0xFF;           // Maximum period
    T2TMR = 0x00;          // Reset Timer2 counter
    T2CONbits.TMR2ON = 1;  // Turn on Timer2

    // Initialize indicator LEDs
    LRED = 0;
    LORAN = 0;
}

void blink() {

    // Turn on the LEDs
    LRED = 1;
    LORAN = 1;
    __delay_ms(200); // Wait 1/5 second
    
    // Turn off the LEDs
    LRED = 0;
    LORAN = 0;
    //RED = 0;
    //ORANGE = 0;
    __delay_ms(200); // Wait 1/5 second

}

void set_pwm_duty(uint8_t pwm, uint8_t level) {
    uint16_t duty;
    
    // Calculate the duty cycle according to the level
    if (level == 0) {
        duty = 0;  // Turn off the LED at level 0
    } else {
        duty = (level * 51);  // Levels 1 to 5: 51, 102, 153, 204, 255
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
    if (current_color == 0) { // Red mode
        set_pwm_duty(3, red_level);
    } else { // Orange mode
        set_pwm_duty(4, orange_level);
    }
}

void main(void) {
    initialize();
    blink();
    blink();
    blink();
    
    current_color = 0; // Red LED
    update_leds();
    current_color = 1; // Orange LED
    update_leds();
    
    LRED = 1;
    
    while(1) {
        // Check SEL button
        if (BTN_SEL == 0) {
            __delay_ms(30); // Debounce
            if (BTN_SEL == 0) {
                current_color = !current_color;
                LRED = current_color;
                LORAN = !current_color;
                while (BTN_SEL == 0); // Wait for button release
            }
        }
        
        // Check UP button
        if (BTN_UP == 0) {
            __delay_ms(30); // Debounce
            if (BTN_UP == 0) {
                if (current_color == 0 && red_level < 5) {
                    red_level++;
                } else if (current_color == 1 && orange_level < 5) {
                    orange_level++;
                }
                while (BTN_UP == 0); // Wait for button release
            }
        }
        
        // Check DOWN button
        if (BTN_DOWN == 0) {
            __delay_ms(30); // Debounce
            if (BTN_DOWN == 0) {
                if (current_color == 0 && red_level > 0) {
                    red_level--;
                } else if (current_color == 1 && orange_level > 0) {
                    orange_level--;
                }
                while (BTN_DOWN == 0); // Wait for button release
            }
        }
        
        update_leds();
        __delay_ms(10); // Small delay for stability
    }
}
