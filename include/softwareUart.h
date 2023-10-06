#pragma once
#include <stdint.h>

#define SOFT_UART_BAUDRATE 9600

typedef struct softwareUart_t {
    volatile uint8_t* txDDR;
    volatile uint8_t* txPORT;
    uint8_t txBit;

    volatile uint8_t* rxDDR;
    volatile uint8_t* rxPIN;
    uint8_t rxBit;
} softwareUart;

softwareUart* initSoftUart(volatile uint8_t* txDDR, volatile uint8_t* txPORT, uint8_t txBit, volatile uint8_t* rxDDR, volatile uint8_t* rxPIN, uint8_t rxBit);

void removeSoftUart(softwareUart** uart);

// Prints a character on uart.
void softUartPrintc(softwareUart* s, uint8_t c);

// Halts the processor until a character is recieved
// on uart.
uint8_t softUartGetc(softwareUart* s);

// Puts a string on the uart
void softUartPrints(softwareUart* s, char* str);

// Puts a decimal number on uart
void softUartPrintd(softwareUart* s, int64_t n);

// Puts a hexadecimal (lowercase alphanumerics)
// number on uart
void softUartPrintx(softwareUart* s, int64_t x);

// Puts a Hexadecimal (uppercase alphanumerics)
// number on uart
void softUartPrintX(softwareUart* s, int64_t x);

// Puts a binary number on the number
void softUartPrintb(softwareUart* s, int64_t b);

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
// Unknown softUartPrintf format "r".
void softUartPrintf(softwareUart* s, char* str, ...);