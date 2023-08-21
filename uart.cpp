#include "uart.h"

#include <avr/io.h>

void UART_init(uint16_t UBRR_VALUE) {
    UBRR0H = (uint8_t)(UBRR_VALUE>>8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    UCSR0B = _BV(TXEN0);
}

void UART_putc(unsigned char data) {
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void UART_puts(char* s){
    while(*s > 0) UART_putc(*s++);
}

void UART_putn(int64_t n){
    if (n < 10) {
        if(n < 0){
            UART_putc('-');
            UART_putn(n * -1);
            return;
        }
        UART_putc(n + '0');
        return;
    } 
    UART_putn(n / 10);
    UART_putc(n % 10 + '0');
}

void UART_put8(uint8_t n){
    for (int i = 0; i < 8; i++){
        UART_putc(!!(n & (1 << (7 - i))) ? '1' : '0');
    }
}

void UART_put16(uint16_t n){
    UART_put8(n >> 8);
    UART_put8(n);
}

void UART_put32(uint32_t n){
    UART_put16(n >> 16);
    UART_put16(n);
}