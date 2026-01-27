#pragma once

#include <stdint.h>

#define VN_PACKET_SIZE 36
#define VN_SYNC_BYTE   0xFA

typedef struct __attribute__((packed))
{
    uint8_t  sync; // 0xFA
    uint8_t  msg_id; // 0x01
    uint8_t  group_flags; // 0x05
    uint16_t reserved;

    float yaw;
    float pitch;
    float roll;

    float ax;
    float ay;
    float az;

    float pressure;

    uint16_t crc;
    uint8_t  pad;
} vn_packet_t;

int vn_packet_parse(const uint8_t *buf, vn_packet_t *out);

