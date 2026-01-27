#include "vn_crc.h"

// matches VN100 IMU
uint16_t vn_crc16(const uint8_t *data, uint32_t length)
{
    	uint16_t crc = 0;

    	for (uint32_t i = 0; i < length; i++)
    	{
        	crc = (crc >> 8) | (crc << 8);
        	crc ^= data[i];
        	crc ^= (crc & 0xFF) >> 4;
        	crc ^= crc << 12;
        	crc ^= (crc & 0xFF) << 5;
    	}

    	return crc;
}

