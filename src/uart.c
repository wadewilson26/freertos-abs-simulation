#include "uart.h"

volatile unsigned int *const UART0_DR = (unsigned int *)0x4000C000;
volatile unsigned int *const UART0_IM = (unsigned int *)0x4000C038;
volatile unsigned int *const UART0_ICR = (unsigned int *)0x4000C044;
volatile unsigned int *const NVIC_EN0 = (unsigned int *)0xE000E100;
volatile unsigned char *const NVIC_PRI5 = (unsigned char *)0xE000E405;

void UART_PrintString(const char *str) {
  while (*str != '\0') {
    *UART0_DR = (unsigned int)(*str);
    str++;
  }
}

void UART_PrintInt(int num) {
  char buffer[10];
  int i = 0;
  if (num == 0) {
    UART_PrintString("0");
    return;
  }
  if (num < 0) {
    UART_PrintString("-");
    num = -num;
  }
  while (num > 0) {
    buffer[i++] = (num % 10) + '0';
    num /= 10;
  }
  while (i > 0) {
    i--;
    *UART0_DR = (unsigned int)(buffer[i]);
  }
}

void UART_EnableRXInterrupt(void) {

  *NVIC_PRI5 = 0xFF;

  *UART0_IM |= (1 << 4);
  *NVIC_EN0 |= (1 << 5);
}