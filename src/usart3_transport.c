#include "usart3_transport.h"
#include "stm32h753xx.h"
#include "hardware.h"

#define RX_BUF_SIZE 128

static uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint32_t rx_head = 0;



void usart3_transport_init(void)
{
	/* Enable peripheral clock */
	RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

	/* Disable USART before config */
	USART3->CR1 &= ~USART_CR1_UE;

	/* Baud rate: 64 MHz / 115200 */
	USART3->BRR = 64000000U / 115200U;

	/* Enable TX + RX + RX interrupt */
	USART3->CR1 =
	USART_CR1_TE |
	USART_CR1_RE |
	USART_CR1_RXNEIE;

	USART3->CR2 = 0;
	USART3->CR3 = 0;

	/* Enable USART */
	USART3->CR1 |= USART_CR1_UE;

	while (!(USART3->ISR & USART_ISR_TEACK));
	while (!(USART3->ISR & USART_ISR_REACK));

	/* NVIC */
	NVIC_SetPriority(USART3_IRQn, 7);
	NVIC_EnableIRQ(USART3_IRQn);
}



void USART3_IRQHandler(void)
{
    	if (USART3->ISR & USART_ISR_RXNE_RXFNE)
    	{
		uint8_t b = USART3->RDR;

        	if (rx_head < RX_BUF_SIZE)
        	{
            		rx_buf[rx_head++] = b;
        	}
        	else
        	{
            		rx_head = 0;
        	}
    	}
}



bool usart3_transport_has_data(void)
{
    	return rx_head > 0;
}

uint32_t usart3_transport_read(uint8_t *dst, uint32_t max_len)
{
	uint32_t n;

	if (rx_head == 0)
	return 0;

	if (rx_head < max_len)
		n = rx_head;
	else
		n = max_len;

	for (uint32_t i = 0; i < n; i++)
		dst[i] = rx_buf[i];
	
	// cheap protection of receive buffer head reset
	__disable_irq();
	rx_head = 0;
	__enable_irq();

	return n;
}



void usart3_transport_write(const uint8_t *buf, uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		while (!(USART3->ISR & USART_ISR_TXE_TXFNF));
		USART3->TDR = buf[i];
	}

	while (!(USART3->ISR & USART_ISR_TC));
}

