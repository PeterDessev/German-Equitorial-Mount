#include <avr/delay.h>
#include <stdarg.h>
#include <avr/io.h>

#include "softwareUart.h"

void baudDelay();

void halfBaudDelay();

softwareUart* initSoftUart(volatile uint8_t* txDDR, volatile uint8_t* txPORT, uint8_t txBit, volatile uint8_t* rxDDR, volatile uint8_t* rxPIN, uint8_t rxBit){
    softwareUart* ret = malloc(sizeof(*ret));

    ret->txDDR = txDDR;
    ret->txPORT = txPORT;
    ret->txBit = txBit;

    ret->rxDDR = rxDDR;
    ret->rxPIN = rxPIN;
    ret->rxBit = rxBit;
    
    // Sets tx pin to output and rx pin to input
    *ret->txDDR |= _BV(ret->txBit);
    *ret->rxDDR &= ~_BV(ret->rxBit);

    // Pull tx high
    *ret->txPORT |= _BV(ret->txBit);
    return ret;
}

void removeSoftUart(softwareUart** uart){
    free((*uart));
    (*uart) = 0;
}

void softUartPrintc(softwareUart* s, uint8_t c){
    uint8_t portRemainder = *s->txPORT & ~_BV(s->txBit);

    // Start bit
    *s->txPORT &= ~_BV(s->txBit);
    baudDelay();

    // Data frame
    for(uint8_t i = 0; i < 8; i++){
        *s->txPORT = portRemainder + (c & 1) * _BV(s->txBit);
        c >>= 1;
        baudDelay();
    }

    // Stop bit
    *s->txPORT |= _BV(s->txBit);
    baudDelay();
}

uint8_t softUartGetc(softwareUart* s){
    uint8_t byte = 0;
    while(*s->rxPIN & _BV(s->rxBit));

    halfBaudDelay();

    for(uint8_t i = 0; i < 8; i++){
        byte += (*s->rxPIN & _BV(s->rxBit)) ? _BV(7) : 0;
        byte >>= 1;
        baudDelay();
    }
    return byte;
}

void softUartPrints(softwareUart* s, char* str){
    while(*str)
        softUartPrintc(s, *str++);
}

void softUartPrintd(softwareUart* s, int64_t n){
    if (n / 10)
        softUartPrintd(s, n / 10);
    softUartPrintc(s, '0' + (n % 10)); 
}

void softUartPrintx(softwareUart* s, int64_t x){
    if (x / 0x10)
        softUartPrintx(s, x / 0x10);
    softUartPrintc(s, ((x & 0xF) > 9 ? '7' : '0') + (x % 0x10));   
}

void softUartPrintX(softwareUart* s, int64_t x){
    if (x / 0x10)
        softUartPrintX(s, x / 0x10);
    softUartPrintc(s, ((x & 0xF) > 9 ? 'W' : '0') + (x % 0x10)); 
}

void softUartPrintb(softwareUart* s, int64_t b){
    if (b / 0b10)
        softUartPrintb(s, b / 0b10);
    softUartPrintc(s, '0' + (b % 0b10));
}

void softUartPrintf(softwareUart* s, char* str, ...){
    va_list args;
    va_start(args, str);
    while(*str){
        if(*str != '%'){
            softUartPrintc(s, *str++);
            continue;
        }
        switch(*++str){
            case '%':
                softUartPrintc(s, '%');
                break;
            case 'd':
                softUartPrintd(s, va_arg(args, int));
                break;
            case 'b':
                softUartPrintb(s, va_arg(args, uint64_t));
                break;
            case 'x':
                softUartPrintx(s, va_arg(args, uint64_t));
                break;
            case 'X':
                softUartPrintX(s, va_arg(args, uint64_t));
                break;
            case 's':
                softUartPrints(s, va_arg(args, char*));
                break;
            case 'c':
                softUartPrintc(s, va_arg(args, uint32_t));
                break;
            default:
                softUartPrints(s, "Unknown usartPrintf format \"");
                softUartPrints(s, *str);
                softUartPrints(s, "\".\n");
                break;
        }
        str++;
    }
    va_end(args);
}

void baudDelay(){
    _delay_us(1000000.0f / SOFT_UART_BAUDRATE);
}

void halfBaudDelay(){
    _delay_us(500000.0f / SOFT_UART_BAUDRATE);
}