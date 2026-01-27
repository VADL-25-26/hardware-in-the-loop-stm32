#include "imu_state.h"

static SimImuState latest_state;
static SemaphoreHandle_t state_mutex;

void imu_state_init(void)
{
	// protect critical section with mutex
        state_mutex = xSemaphoreCreateMutex();

        // dummy vals 
        latest_state.yaw = 10.0f;
        latest_state.pitch = 1.0f;
        latest_state.roll = -2.0f;

        latest_state.ax = 0.0f;
        latest_state.ay = 0.0f;
        latest_state.az = 9.80665f;

        latest_state.pressure = 101.325f;
        latest_state.seq = 0;
}

// doesn't block in order to preserve hard real-time for output task,
// if mutex is already acquired output just uses last state
int imu_state_get(SimImuState *out)
{
    	if (xSemaphoreTake(state_mutex, 0) == pdTRUE)
    	{
        	*out = latest_state;
        	xSemaphoreGive(state_mutex);
        	return 1;
    	}

    	return 0;
}

// blocks if output task has mutex for read
void imu_state_set_from_cmd(const SimImuState *in)
{
    	xSemaphoreTake(state_mutex, portMAX_DELAY);
    	latest_state = *in;
    	latest_state.seq++;
    	xSemaphoreGive(state_mutex);
}

