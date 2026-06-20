extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;
extern unsigned int _estack;

void Reset_Handler(void);
void Default_Handler(void);

extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void UART0_Handler(void);
extern int main(void);

__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[])(void) = {
    (void (*)(void))&_estack,
    Reset_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    0,
    0,
    0,
    0,
    vPortSVCHandler,
    Default_Handler,
    0,
    xPortPendSVHandler,
    xPortSysTickHandler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    UART0_Handler};

void Reset_Handler(void) {
  unsigned int *pSrc = &_etext;
  unsigned int *pDst = &_data;

  while (pDst < &_edata) {
    *pDst++ = *pSrc++;
  }

  pDst = &_bss;
  while (pDst < &_ebss) {
    *pDst++ = 0;
  }

  main();
  while (1)
    ;
}

void Default_Handler(void) {
  while (1)
    ;
}