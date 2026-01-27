#include <stdint.h>

/* Required by CMSIS */
uint32_t SystemCoreClock = 64000000;

/* Called from Reset_Handler */
void SystemInit(void)
{
    /* Do nothing for now */
}

/* Required by STM32H7 startup (even on single-core parts) */
void ExitRun0Mode(void)
{
    /* Do nothing */
}

