#pragma once
#include <stdint.h>
// Initializes bidirectional asynchronous 8 bit
// 1 stop bit transmition with baudrate baudRate
// on the atmega328p usart0
void usartInit(uint16_t baudRate);

// Puts data on uart
void usartPrintc(uint8_t data);

// Halts the processor until a character is recieved
// on uart.
uint8_t usartGetc();

// Puts a string on the uart
void usartPrints(char* s);

// Puts a decimal number on uart
void usartPrintd(int64_t n);

// Puts a hexadecimal (lowercase alphanumerics)
// number on uart
void usartPrintx(int64_t x);

// Puts a Hexadecimal (uppercase alphanumerics)
// number on uart
void usartPrintX(int64_t x);

// Puts a binary number on the number
void usartPrintb(int64_t b);

// Puts a formated string with argument substitution
// on uart. Supported formats are:
// %d -> Unsgined decimal (1234)
// %x -> lowercase Hexadecimal (12ab)
// %X -> Uppercase hexadecimal (12AB)
// %b -> binary (1001)
// %s -> null-terminated string (Lorem)
// %c -> ASCII character (a)
// %% -> Prints percent literal (%)
// In the case of an unsupported format, example "r", the 
// following message (ending in a newline) is printed on uart: 
// Unknown usartPrintf format "r".
void usartPrintf(char* s, ...);