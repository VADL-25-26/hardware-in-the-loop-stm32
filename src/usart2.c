#include "usart2.h"
#include "stm32h753xx.h"

void usart2_init(void)
{
    	RCC->APB1LENR |= RCC_APB1LENR_USART2EN;

    	USART2->CR1 &= ~USART_CR1_UE;

    	USART2->BRR = 64000000U / 115200U;

    	USART2->CR1 = USART_CR1_TE;
    	USART2->CR2 = 0;
    	USART2->CR3 = 0;

    	USART2->CR1 |= USART_CR1_UE;

    	while (!(USART2->ISR & USART_ISR_TEACK));
}

void usart2_write_buffer(const uint8_t *buf, uint32_t len)
{
    	for (uint32_t i = 0; i < len; i++)
    	{
        	while (!(USART2->ISR & USART_ISR_TXE_TXFNF));
        	USART2->TDR = buf[i];
    	}

    	while (!(USART2->ISR & USART_ISR_TC));
}

