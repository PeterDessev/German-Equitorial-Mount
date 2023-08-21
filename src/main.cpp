#include <SoftwareSerial.h>
// #include <HardwareSerial.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdint.h>

#include "registers.hpp"
// #include "uart.h"

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((16000000 / (USART_BAUDRATE * 16UL))) - 1)

#define MICROSTEPS 64
#define CURRENT 2000
#define EN_PIN 7            // Enable
#define DIR_PIN 3           // Direction
#define STEP_PIN 4          // Step
#define STEP_PORT PORTD     // Step port
#define SW_RX 5            // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX 6             // TMC2208/TMC2224 SoftwareSerial transmit pin
#define DRIVER_ADDRESS 0x00 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f
#define BAUDRATE 115200
		
#define TMC2208_SYNC 0x05
#define TMC_WRITE 0x80

#define DIR 0

#define TRACKDELMS 1
#define TRACKDELUS 140
#define FASTDELUS 8
#define JUMP 1
#define STRIDE 1000

// Controller Buttons
#define UL_BUTTON_PIN PINB
#define UL_BUTTON_NUM 4
#define UR_BUTTON_PIN PINB
#define UR_BUTTON_NUM 3

GCONF_t GCONF_register = GCONF_t();
IHOLD_IRUN_t IHOLD_IRUN_register = IHOLD_IRUN_t();
TPOWERDOWN_t TPOWERDOWN_register = TPOWERDOWN_t();
CHOPCONF_t CHOPCONF_register = CHOPCONF_t();
PWMCONF_t PWMCONF_register = PWMCONF_t();
TPWMTHRS_t TPWMTHRS_register = TPWMTHRS_t();

uint16_t bytesWritten = 0;
float holdMultiplier = 0.7;

SoftwareSerial* SWSerial = nullptr;

void initRegisters() {
    GCONF_register.i_scale_analog = 1;
    GCONF_register.internal_rsense = 0; // OTP
    GCONF_register.en_spreadcycle = 0;  // OTP
    GCONF_register.multistep_filt = 1;  // OTP
    GCONF_register.shaft = 0;
    GCONF_register.index_otpw = 0; // Not sure which one to set it to, keeping the default?
    GCONF_register.index_step = 0;
    GCONF_register.pdn_disable = 1; // The DS says to set this when using the UART interface but I'm afraid of the Engrish...
    GCONF_register.mstep_reg_select = 1;
    GCONF_register.test_mode = 0;

    IHOLD_IRUN_register.iholddelay = 1; // OTP

    TPOWERDOWN_register.sr = 255;

    TPWMTHRS_register.sr = 0;

    // Investigate VACTUAL?

    // CHOPCONF_register.sr = 0x10000053;
    CHOPCONF_register.tbl = 2;
    CHOPCONF_register.hstrt = 4;
    CHOPCONF_register.hend = 0;
    // PWMCONF_register.sr = 0xC10D0024;
    PWMCONF_register.pwm_autoscale = 1;
    PWMCONF_register.pwm_autograd = 1;
    PWMCONF_register.pwm_freq = 0b01;
}

uint8_t calcCRC(uint8_t datagram[], uint8_t len) {
	uint8_t crc = 0;
	for (uint8_t i = 0; i < len; i++) {
		uint8_t currentByte = datagram[i];
		for (uint8_t j = 0; j < 8; j++) {
			if ((crc >> 7) ^ (currentByte & 0x01)) {
				crc = (crc << 1) ^ 0x07;
			} else {
				crc = (crc << 1);
			}
			crc &= 0xff;
			currentByte = currentByte >> 1;
		}
	}
	return crc;
}

void write(uint8_t addr, uint32_t regVal) {
	uint8_t len = 7;
	addr |= TMC_WRITE;
	uint8_t datagram[] = {TMC2208_SYNC, DRIVER_ADDRESS, addr, (uint8_t)(regVal>>24), (uint8_t)(regVal>>16), (uint8_t)(regVal>>8), (uint8_t)(regVal>>0), 0x00};

	datagram[len] = calcCRC(datagram, len);
    
	for(uint8_t i=0; i<=len; i++) {
		bytesWritten += SWSerial->write(datagram[i]);
	}

	_delay_ms(2);
}

void toff(uint8_t B){
    CHOPCONF_register.toff = B; 
    write(CHOPCONF_register.address, CHOPCONF_register.sr);
}

void tpwmthrs(uint32_t B){
    TPWMTHRS_register.sr = B;
    write(TPWMTHRS_register.address, TPWMTHRS_register.sr);
}

void multistepFilt(uint8_t B){
    GCONF_register.multistep_filt = B;
    write(GCONF_register.address, GCONF_register.sr);
}

void shaft(uint8_t B){
    GCONF_register.shaft = B;
    write(GCONF_register.address, GCONF_register.sr);
}

void rms_current(uint16_t mA) {
    uint8_t CS = 32.0 * 1.41421 * mA / 1000.0 * (R_SENSE + 0.02) / 0.325;
    // UART_putn(CS);
    // If Current Scale is too low, turn on high sensitivity R_sense and
    // calculate again
    if (CS < 16) {
        CHOPCONF_register.vsense = true;
        write(CHOPCONF_register.address, CHOPCONF_register.sr);
        CS = 32.0 * 1.41421 * mA / 1000.0 * (R_SENSE + 0.02) / 0.180;
    } else { // If CS >= 16, turn off high_sense_r
        CHOPCONF_register.vsense = false;
        write(CHOPCONF_register.address, CHOPCONF_register.sr);
    }

    if (CS > 31)
        CS = 31;

    // ihold(CS * holdMultiplier);
    IHOLD_IRUN_register.ihold = CS * holdMultiplier; 
    write(IHOLD_IRUN_register.address, IHOLD_IRUN_register.sr);
    // irun(CS);
    IHOLD_IRUN_register.irun = CS; 
    write(IHOLD_IRUN_register.address, IHOLD_IRUN_register.sr);
    // val_mA = mA;
}

void mres(uint8_t B){
    CHOPCONF_register.mres = B; 
    write(CHOPCONF_register.address, CHOPCONF_register.sr);
}

void microsteps(uint16_t ms) {
  switch(ms) {
    case 256: mres(0); break;
    case 128: mres(1); break;
    case  64: mres(2); break;
    case  32: mres(3); break;
    case  16: mres(4); break;
    case   8: mres(5); break;
    case   4: mres(6); break;
    case   2: mres(7); break;
    case   0: mres(8); break;
    default: break;
  }
}

void pwm_autoscale(uint8_t B){ 
    PWMCONF_register.pwm_autoscale = B; 
    write(PWMCONF_register.address, PWMCONF_register.sr);
}

// Quickly moves the motor. The motor is stepped with a 
// delay equal to twice delay_ms.
void fastMove(int delay_us) {
    STEP_PORT |= _BV(STEP_PIN);
    _delay_us(delay_us);
    STEP_PORT &= ~_BV(STEP_PIN);
    _delay_us(delay_us);
}

void accelerate(int32_t initial_delay, int32_t final_delay, int32_t jump, uint32_t stride){
    int32_t remainder = (final_delay - initial_delay);
    int8_t sign = (remainder > 0) ? 1 : -1;
    int32_t finalInterval = remainder % jump;
    int32_t interval;

    while(remainder != finalInterval){
        remainder -= sign * jump;
        interval = final_delay - remainder;

        for(uint32_t i = 0; i < stride; i += interval){
            STEP_PORT |= _BV(STEP_PIN);
            for(uint32_t j = 0; j < interval / 200; j++)
                _delay_us(100);
            STEP_PORT &= ~_BV(STEP_PIN);
            for(uint32_t j = 0; j < interval / 200; j++)
                _delay_us(100);
        }
    }

    // fastMove(final_delay);
}

int main() {    
    initRegisters();
    // UART_init(UBRR_VALUE);
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

    // Internal LED
    DDRB |= _BV(5);

    // // Set Software Serial pins to output
    DDRD |= _BV(5);
    DDRD |= _BV(6);
 
    SWSerial = new SoftwareSerial(SW_RX, SW_TX);
    SWSerial->begin(BAUDRATE);

    // Allow in-rush Cap to charge before enabling motor
    PORTD |= _BV(EN_PIN); // Set enable to high
    _delay_ms(2000);
    PORTD &= ~_BV(EN_PIN); // Set enable to low

    // driver.toff(5);                 // Enables driver in software
    // toff(15);
    toff(5);

    // driver.rms_current(600); // Set motor RMS current
    rms_current(CURRENT);

    // driver.microsteps(0);
    microsteps(MICROSTEPS);

    // driver.pwm_autoscale(true);
    pwm_autoscale(true);

    tpwmthrs(13); // "Disable" Stealth Chop, enable Spread Cycle

    multistepFilt(1); // Try this one out, see what it does?

    shaft(DIR);

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
            // PORTB |= _BV(5);
            // while((~PINB) & _BV(3));
            // PORTB &= ~_BV(5);
            shaft(!DIR);
            // accelerate(TRACKDELMS * 500 + TRACKDELUS / 2, FASTDELUS, JUMP, STRIDE);
            while((~PINB) & _BV(UR_BUTTON_NUM))
                fastMove(FASTDELUS);
            // accelerate(FASTDELUS, TRACKDELMS * 500 + TRACKDELUS / 2, JUMP, STRIDE);
            shaft(DIR);
            break;
        case _BV(UL_BUTTON_NUM):
            // accelerate(TRACKDELMS * 500 + TRACKDELUS / 2, FASTDELUS, JUMP, STRIDE);
            while((~PINB) & _BV(UL_BUTTON_NUM))
                fastMove(FASTDELUS);
            // accelerate(FASTDELUS, TRACKDELMS * 500 + TRACKDELUS / 2, JUMP, STRIDE);
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

    return 0;
}