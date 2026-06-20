#ifndef UART_H
#define UART_H

void UART_PrintString(const char *str);
void UART_PrintInt(int num);
void UART_EnableRXInterrupt(void);

extern volatile unsigned int *const UART0_DR;
extern volatile unsigned int *const UART0_ICR;

#endif