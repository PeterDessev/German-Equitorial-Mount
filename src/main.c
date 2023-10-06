#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "registers.h"
#include "tmc2209.h"
#include "hwUart.h"
#include "softwareUart.h"

#define MICROSTEPS 32
#define CURRENT 2000
#define EN_PIN 7            // Enable
#define DIR_PIN 3           // Direction
#define STEP_PIN 4          // Step
#define STEP_PORT PORTD     // Step port
#define BAUDRATE 9600
#define DIR 0

#define TRACKDELMS 3
#define TRACKDELUS 70
#define FASTDELUS 8

// Controller Buttons
#define UL_BUTTON_PIN PINB
#define UL_BUTTON_NUM 4
#define UR_BUTTON_PIN PINB
#define UR_BUTTON_NUM 3

// Quickly moves the motor. The motor is stepped with a 
// delay equal to twice delay_ms.
extern inline void fastMove() {
    STEP_PORT |= _BV(STEP_PIN);
    _delay_us(FASTDELUS);
    STEP_PORT &= ~_BV(STEP_PIN);
    _delay_us(FASTDELUS);
}

int main() {
    usartInit(BAUDRATE);

    // Clear DDRD and set all pins to input
    DDRD = 0;

    // Set ports 3, 4, 5, 7 to output
    DDRD |= _BV(DIR_PIN);  // Dir pin output
    DDRD |= _BV(STEP_PIN); // Step pin output
    DDRB |= _BV(5);        // LED pin output
    DDRD |= _BV(EN_PIN);   // Enable pin output

    // Set ports 11, 12 to input
    DDRB &= ~_BV(3); // Controller Button
    DDRB &= ~_BV(4); // Controller Button

    PORTD |= _BV(3); // Set Dir pin to high

    // // Set Software Serial pins to output
    DDRD |= _BV(5);
    DDRD |= _BV(6);

    // Allow in-rush Cap to charge before enabling motor
    PORTD |= _BV(EN_PIN); // Set enable to high
    _delay_ms(2000);
    PORTD &= ~_BV(EN_PIN); // Set enable to low

    softwareUart* raDriverUart = initSoftUart(&DDRD, &PORTD, 5, &DDRD, &PIND, 6);
    tmc2209* raDriver = createDriver(0.11f, 0, raDriverUart);
    
    toff(raDriver, 5);
    rms_current(raDriver, CURRENT); // Set motor RMS current
    microsteps(raDriver, MICROSTEPS);
    pwm_autoscale(raDriver, true);
    tpwmthrs(raDriver, 13); // "Disable" Stealth Chop, enable Spread Cycle
    multistepFilt(raDriver, 1); // Try this one out, see what it does?
    shaft(raDriver, DIR);

    // Automatic Tuning
    // PWM_OFS_AUTO
    // Trigger Run Current
    // STEP_PORT |= _BV(STEP_PIN);
    // _delay_ms(2);
    // STEP_PORT &= ~_BV(STEP_PIN);
    // _delay_ms(2);
    // _delay_ms(140);

    // // PWM_GRAD_AUTO
    // uint16_t rpm = 100;
    // uint16_t gradDel = 300 / rpm;
    // for(uint16_t i = 0; i < 400; i++){
    //     STEP_PORT |= _BV(STEP_PIN);
    //     _delay_ms(gradDel);
    //     STEP_PORT &= ~_BV(STEP_PIN);
    //     _delay_ms(gradDel);
    // }


    while (1) {
        int buttonStat = (~PINB) & (_BV(UL_BUTTON_NUM) | _BV(UR_BUTTON_NUM)); // Get inputs on buttons 

        switch (buttonStat) {
        case _BV(UR_BUTTON_NUM):
            shaft(raDriver, !DIR);
            while((~PINB) & _BV(UR_BUTTON_NUM))
                fastMove();
            shaft(raDriver, DIR);
            break;
        case _BV(UL_BUTTON_NUM):
            while((~PINB) & _BV(UL_BUTTON_NUM))
                fastMove();
            break;
        default: // Move tracker normally
            // PORTB |= _BV(5);
            STEP_PORT |= _BV(STEP_PIN);
            _delay_us(TRACKDELMS * 500);
            _delay_us(TRACKDELUS / 2);
            STEP_PORT &= ~_BV(STEP_PIN);
            // PORTB &= ~_BV(5);
            _delay_us(TRACKDELMS * 500);
            _delay_us(TRACKDELUS / 2);
            break;
        }
    }
    removeDriver(&raDriver);
    removeSoftUart(&raDriverUart);
    return 0;
}