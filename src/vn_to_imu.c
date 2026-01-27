#include "vn_to_imu.h"

// convert packet to state object
void vn_packet_to_imu_state(const vn_packet_t *pkt, SimImuState *state)
{
    	state->yaw = pkt->yaw;
    	state->pitch = pkt->pitch;
    	state->roll = pkt->roll;

    	state->ax = pkt->ax;
    	state->ay = pkt->ay;
    	state->az = pkt->az;

    	state->pressure = pkt->pressure;
}

