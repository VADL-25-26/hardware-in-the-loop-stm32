#include "vn_packet.h"
#include "vn_crc.h"
#include <string.h>

// simple parser for assembly buffer to vn packet
int vn_packet_parse(const uint8_t *buf, vn_packet_t *out)
{
    	const vn_packet_t *pkt = (const vn_packet_t *)buf;

    	if (pkt->sync != VN_SYNC_BYTE)
        	return -1;

    	uint16_t crc_calc = vn_crc16(&buf[1], 32);

    	uint16_t rx_crc = ((uint16_t)buf[33] << 8) | buf[34];

    	if (crc_calc != rx_crc)
   	 	return -2;


    	memcpy(out, pkt, sizeof(vn_packet_t));
    	return 0;
}













