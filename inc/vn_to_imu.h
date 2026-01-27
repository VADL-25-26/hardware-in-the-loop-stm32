#pragma once
#include "vn_packet.h"
#include "imu_state.h"

void vn_packet_to_imu_state(const vn_packet_t *pkt, SimImuState *state);

