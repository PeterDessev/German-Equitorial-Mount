#pragma once
#include <stdint.h>

void UART_init(uint16_t UBRR_VALUE);

void UART_putc(unsigned char data);

void UART_puts(char* s);

void UART_putn(int64_t n);

void UART_put8(uint8_t n);

void UART_put16(uint16_t n);

void UART_put32(uint32_t n);