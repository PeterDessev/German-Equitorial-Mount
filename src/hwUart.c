#include <avr/io.h>
#include <stdarg.h>

#include "hwUart.h"

void usartInit(uint16_t baudRate){
    uint16_t UBBRval = (((F_CPU / (baudRate * 16UL))) - 1);
    UBRR0H = (uint8_t)(UBBRval >> 8);
    UBRR0L = (uint8_t)UBBRval;

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

void usartPrintc(uint8_t data){
    while(!(UCSR0A & _BV(UDRE0)));
    UDR0 = data;
}

uint8_t usartGetc(){
    while(!(UCSR0A & _BV(RXC0)));
    return UDR0;
}

void usartPrints(char* s){
    while(*s)
        usartPrintc(*s++);
}

void usartPrintd(int64_t n){
    if (n / 10)
        usartPrintd(n / 10);
    usartPrintc('0' + (n % 10));    
}

void usartPrintx(int64_t x){
    if (x / 0x10)
        usartPrintx(x / 0x10);
    usartPrintc(((x & 0xF) > 9 ? '7' : '0') + (x % 0x10));   
}

void usartPrintX(int64_t x){
    if (x / 0x10)
        usartPrintX(x / 0x10);
    usartPrintc(((x & 0xF) > 9 ? 'W' : '0') + (x % 0x10));   
}

void usartPrintb(int64_t b){
    if (b / 0b10)
        usartPrintb(b / 0b10);
    usartPrintc('0' + (b % 0b10));
}

void usartPrintf(char* s, ...){
    va_list args;
    va_start(args, s);
    while(*s){
        if(*s != '%'){
            usartPrintc(*s++);
            continue;
        }
        switch(*++s){
            case '%':
                usartPrintc('%');
                break;
            case 'd':
                usartPrintd(va_arg(args, int));
                break;
            case 'b':
                usartPrintb(va_arg(args, uint64_t));
                break;
            case 'x':
                usartPrintx(va_arg(args, uint64_t));
                break;
            case 'X':
                usartPrintX(va_arg(args, uint64_t));
                break;
            case 's':
                usartPrints(va_arg(args, char*));
                break;
            case 'c':
                usartPrintc(va_arg(args, uint32_t));
                break;
            default:
                usartPrints("Unknown usartPrintf format \"");
                usartPrintc(*s);
                usartPrints("\".\n");
                break;
        }
        s++;
    }
    va_end(args);
}