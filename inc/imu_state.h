#ifndef IMU_STATE_H
#define IMU_STATE_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"


typedef struct
{
	float yaw; // deg
	float pitch; // deg
	float roll; // deg

	float ax; // m/s^2
	float ay; // m/s^2
	float az; // m/s^2

	float pressure; // kPa

	uint32_t seq; // increments on each update (for debugging)
} SimImuState;

void imu_state_init(void);

int imu_state_get(SimImuState *out);

void imu_state_set_from_cmd(const SimImuState *in);


#endif // IMU_STATE_H

