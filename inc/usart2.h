#pragma once
#include <stdint.h>

void usart2_init(void);
void usart2_write_buffer(const uint8_t *buf, uint32_t len);

