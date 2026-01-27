#pragma once
#include <stdint.h>
#include <stdbool.h>


void usart3_transport_init(void);

bool usart3_transport_has_data(void);
uint32_t usart3_transport_read(uint8_t *dst, uint32_t max_len);
void usart3_transport_write(const uint8_t *buf, uint32_t len);

