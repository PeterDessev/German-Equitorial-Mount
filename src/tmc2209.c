#include <stdbool.h>
#include <stdlib.h>
#include <avr/delay.h>

#define TMC2209_INTERNAL
#include "tmc2209.h"

uint16_t bytesWritten = 0;

uint8_t calcCRC(uint8_t datagram[], uint8_t len);

void write(tmc2209* driver, uint8_t addr, uint32_t regval);

void toff(tmc2209* driver, uint8_t B){
    driver->chopconf.toff = B; 
    write(driver, CHOPCONF_ADDRESS, driver->chopconf.sr);
}

void tpwmthrs(tmc2209* driver, uint32_t B){
    driver->tpwmthrs.sr = B;
    write(driver, TPWMTHRS_ADDRESS, driver->tpwmthrs.sr);
}

void multistepFilt(tmc2209* driver, uint8_t B){
    driver->gconf.multistep_filt = B;
    write(driver, GCONF_ADDRESS, driver->gconf.sr);
}

void shaft(tmc2209* driver, uint8_t B){
    driver->gconf.shaft = B;
    write(driver, GCONF_ADDRESS, driver->gconf.sr);
}

void rms_current(tmc2209* driver, uint16_t mA) {
    uint8_t CS = 32.0 * 1.41421 * mA / 1000.0 * (driver->rsense + 0.02) / 0.325;
    // If Current Scale is too low, turn on high sensitivity R_sense and
    // calculate again
    if (CS < 16) {
        driver->chopconf.vsense = true;
        write(driver, CHOPCONF_ADDRESS, driver->chopconf.sr);
        CS = 32.0 * 1.41421 * mA / 1000.0 * (driver->rsense + 0.02) / 0.180;
    } else { // If CS >= 16, turn off high_sense_r
        driver->chopconf.vsense = false;
        write(driver, CHOPCONF_ADDRESS, driver->chopconf.sr);
    }

    if (CS > 31)
        CS = 31;

    // ihold(CS * holdMultiplier);
    driver->ihold_irun.ihold = CS * driver->holdMultiplier; 
    write(driver, IHOLDIRUN_ADDRESS, driver->ihold_irun.sr);
    // irun(CS);
    driver->ihold_irun.irun = CS; 
    write(driver, IHOLDIRUN_ADDRESS, driver->ihold_irun.sr);
    // val_mA = mA;
}

void mres(tmc2209* driver, uint8_t B){
    driver->chopconf.mres = B; 
    write(driver, CHOPCONF_ADDRESS, driver->chopconf.sr);
}

void microsteps(tmc2209* driver, uint16_t ms) {
  switch(ms) {
    case 256: mres(driver, 0); break;
    case 128: mres(driver, 1); break;
    case  64: mres(driver, 2); break;
    case  32: mres(driver, 3); break;
    case  16: mres(driver, 4); break;
    case   8: mres(driver, 5); break;
    case   4: mres(driver, 6); break;
    case   2: mres(driver, 7); break;
    case   0: mres(driver, 8); break;
    default: break;
  }
}

void pwm_autoscale(tmc2209* driver, uint8_t B){ 
    driver->pwmconf.pwm_autoscale = B; 
    write(driver, PWMCONF_ADDRESS, driver->pwmconf.sr);
}

tmc2209* createDriver(float rSesnse, uint8_t uartAddress, softwareUart* uart){
    tmc2209* ret = malloc(sizeof(*ret));
    ret->rsense = rSesnse;
    
    ret->tmc2208Sync = TMC2208_SYNC;
    ret->tmcWrite = TMC_WRITE;
    ret->address = uartAddress;

    ret->uart = uart;

    defaultSettings(ret);
    return ret;
}

void defaultSettings(tmc2209* driver){
    driver->holdMultiplier = DEFAULT_HOLD_MULT;
    driver->gconf.i_scale_analog = 1;
    driver->gconf.internal_rsense = 0; // OTP
    driver->gconf.en_spreadcycle = 0;  // OTP
    driver->gconf.multistep_filt = 1;  // OTP
    driver->gconf.shaft = 0;
    driver->gconf.index_otpw = 0; // Not sure which one to set it to, keeping the default?
    driver->gconf.index_step = 0;
    driver->gconf.pdn_disable = 1; // The DS says to set this when using the UART interface but I'm afraid of the Engrish...
    driver->gconf.mstep_reg_select = 1;
    driver->gconf.test_mode = 0;

    driver->ihold_irun.iholddelay = 1; // OTP

    driver->tpowerdown.sr = 255;

    driver->tpwmthrs.sr = 1;

    // Investigate VACTUAL?

    // CHOPCONF_register.sr = 0x10000053;
    driver->chopconf.tbl = 2;
    driver->chopconf.hstrt = 4;
    driver->chopconf.hend = 0;
    // PWMCONF_register.sr = 0xC10D0024;
    driver->pwmconf.pwm_autoscale = 1;
    driver->pwmconf.pwm_autograd = 1;
    driver->pwmconf.pwm_freq = 0b01;
}

void removeDriver(tmc2209** driver){
    free((*driver));
    (*driver) = 0;
}

void write(tmc2209* driver, uint8_t addr, uint32_t regVal) {
	uint8_t len = 7;
	addr |= driver->tmcWrite;
	uint8_t datagram[] = {driver->tmc2208Sync, driver->address, addr, (uint8_t)(regVal>>24), (uint8_t)(regVal>>16), (uint8_t)(regVal>>8), (uint8_t)(regVal>>0), 0x00};

	datagram[len] = calcCRC(datagram, len);
    
	for(uint8_t i=0; i<=len; i++) {
		bytesWritten++;
        softUartPrintc(driver->uart, datagram[i]);
	}

	_delay_ms(2);
}

uint32_t read(tmc2209* driver, uint8_t addr){
    uint8_t receiveLen = 8;
    uint8_t receiveDatagram[] = {
        0x00, // Sync + reserved
        0x00, // Master address 
        0x00, // RW + 7 bit register address
        0x00, 0x00, 0x00, 0x00, // Data
        0x00 // CRC
    };
    uint32_t data = 0;

	uint8_t transmitLen = 4;
	uint8_t transmitDatagram[] = {
        driver->tmc2208Sync, 
        driver->address, 
        addr | driver->tmcWrite, 
        0x00
    };

	transmitDatagram[transmitLen - 1] = calcCRC(transmitDatagram, transmitLen);
    
	for(uint8_t i = 0; i < transmitLen; i++) {
		bytesWritten++;
        softUartPrintc(driver->uart, transmitDatagram[i]);
	}
    
    for(int i = 0; i < receiveLen; i++){
        receiveDatagram[i] = softUartGetc(driver->uart);
    }

    data += receiveDatagram[3] << 8*3;
    data += receiveDatagram[4] << 8*2;
    data += receiveDatagram[5] << 8*1;
    data += receiveDatagram[6] << 8*0;
    
	_delay_ms(2);
    return data;
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