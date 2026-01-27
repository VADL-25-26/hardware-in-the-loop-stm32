#include "usart3.h"
#include "stm32h753xx.h"

void usart3_init(void)
{
    // Enable USART3 peripheral clock
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

    // Disable USART before configuration
    USART3->CR1 &= ~USART_CR1_UE;

    /*
     * Baud rate:
     * Assuming APB1 peripheral clock = 64 MHz
     * Baud = 115200
     */
    USART3->BRR = 64000000U / 115200U;

    /*
     * CR1 configuration:
     * - 8 data bits
     * - No parity
     * - TX enable
     * - RX disabled for now (can be enabled later)
     */
    USART3->CR1 =
        USART_CR1_TE;

    // 1 stop bit
    USART3->CR2 = 0;

    // No flow control, no DMA yet
    USART3->CR3 = 0;

    // Enable USART
    USART3->CR1 |= USART_CR1_UE;

    // Wait for transmitter to be ready
    while (!(USART3->ISR & USART_ISR_TEACK));
}

void usart3_write_byte(uint8_t byte)
{
    // Wait until TX buffer has space
    while (!(USART3->ISR & USART_ISR_TXE_TXFNF));
    USART3->TDR = byte;
}

void usart3_write_buffer(const uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        usart3_write_byte(buf[i]);
    }

    // Wait until transmission complete
    while (!(USART3->ISR & USART_ISR_TC));
}

