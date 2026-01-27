#ifndef USART3_H
#define USART3_H

#include <stdint.h>

// Initialize USART3 for 115200 8N1 (TX enabled, RX optional)
void usart3_init(void);

// Blocking transmit helpers
void usart3_write_byte(uint8_t byte);
void usart3_write_buffer(const uint8_t *buf, uint32_t len);

#endif // USART3_H

