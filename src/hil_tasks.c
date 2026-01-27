#include "FreeRTOS.h"
#include "task.h"
#include "stm32h753xx.h"
#include "hardware.h"
#include "hil_tasks.h"
#include "imu_state.h"
#include <string.h>
#include "pwm_feedback.h"
#include <stdio.h>
#include "vn_crc.h"
#include "vn_packet.h"
#include "vn_to_imu.h"
#include "usart2.h"
#include "usart3_transport.h"


#define VN_PACKET_SIZE 36

TaskHandle_t outputTaskHandle = NULL;
TaskHandle_t feedbackTaskHandle = NULL;
TaskHandle_t pcRxTaskHandle = NULL;

// output task that blocks until 200 Hz timer interrupt
void outputTask(void *argument)
{
	// declare packet for sending, object for storing from state read
	uint8_t packet[VN_PACKET_SIZE];
        SimImuState state;

	// yield upon first run, start timer
	taskYIELD();
	startOutputTimer();
	
	for (;;)
	{

		// block until 200 Hz timer overflows
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		PC0_SetHigh();

		// get latest state; non-blocking to maintain determinism,
		// if mutex is already acquired uses last latest
		imu_state_get(&state);

		// build packet
		packet[0] = 0xFA; // sync
        	packet[1] = 0x01; // msg id
        	packet[2] = 0x05; // group flags
        	packet[3] = 0x00;
        	packet[4] = 0x00;
		
		// copy from grabbed state to packet
        	memcpy(&packet[5], &state.yaw, 4);
        	memcpy(&packet[9], &state.pitch, 4);
        	memcpy(&packet[13], &state.roll, 4);
        	memcpy(&packet[17], &state.ax, 4);
        	memcpy(&packet[21], &state.ay, 4);
        	memcpy(&packet[25], &state.az, 4);
        	memcpy(&packet[29], &state.pressure, 4);

        	// crc
		uint16_t crc = vn_crc16(&packet[1], 32);

        	packet[33] = (crc >> 8) & 0xFF;
        	packet[34] = crc & 0xFF;

        	// byte 35 pads
        	packet[35] = 0x00;
		
		// write over usart2
        	usart2_write_buffer(packet, VN_PACKET_SIZE);

		PC0_SetLow();
		
	}
}


// feedback task for reporting changes in pulse width in ACS actuator signal
void feedbackTask(void *argument)
{
    	uint32_t last_width = 0;
    	uint8_t have_baseline = 0;

    	// if difference exceeds this, latch
    	const uint32_t WIDTH_THRESHOLD = 1500;

    	for (;;)
    	{
	
 		uint32_t diff;
        	if (pwm_new_measurement)
        	{
	
			
            		pwm_new_measurement = 0;

            		uint32_t current = pwm_width_ticks;
		
	   	
	    		if (!have_baseline)
	    		{
		    		last_width = current;
		    		have_baseline = 1;
	    		}
	    		else
	    		{

				// handle varying over and underflows
            			if (current > last_width)
				{
					diff = current - last_width;
				}
				else 
				{
					diff = last_width - current;
				}

				if (diff > WIDTH_THRESHOLD)

				{
                			actuator_moved_flag = 1;

					// send flag to pc
					const char msg[] = "ACS_PWM_CHANGED\n";
					usart3_transport_write((const uint8_t *)msg, sizeof(msg) - 1);
					led2_on();
						
           			}
	    

            			last_width = current;
	    		}
        	}
	
		// repeat at 1 kHz
        	vTaskDelay(pdMS_TO_TICKS(1));
    	}
}



// receive task from pc
void pcRxTask(void *argument)
{
	// declare receive buffer, assembly buffer
	uint8_t rx_tmp[64];
	static uint8_t assemble_buf[128];
	static uint32_t assemble_len = 0;

	// define packet object, state object to update latest state
	vn_packet_t pkt;
	SimImuState new_state;

	for (;;)
	{

		if (usart3_transport_has_data())
		{
			uint32_t n = usart3_transport_read(rx_tmp, sizeof(rx_tmp));

			if (assemble_len + n <= sizeof(assemble_buf))
			{
				// keep adding from receive buffer to assembly buffer
				memcpy(&assemble_buf[assemble_len], rx_tmp, n);
				assemble_len += n;
			}
			else
			{
				// overflow recovery
				assemble_len = 0;
			}
		}

		// parse the packet using the assembly buffer
		while (assemble_len >= VN_PACKET_SIZE)
		{
		    	if (vn_packet_parse(assemble_buf, &pkt) == 0)
		    	{
				PC0_SetHigh();
				
				// convert packet to state object and update latest state
				vn_packet_to_imu_state(&pkt, &new_state);
				imu_state_set_from_cmd(&new_state);

				PC0_SetLow();
				
				// resync by sliding window
				memmove(
				    	assemble_buf,
				    	assemble_buf + VN_PACKET_SIZE,
				    	assemble_len - VN_PACKET_SIZE
				);
				assemble_len -= VN_PACKET_SIZE;
		    	}
		    	else
		    	{
				// resync by sliding byte by byte since parsing failed
				memmove(
			    		assemble_buf,
			    		assemble_buf + 1,
			    		assemble_len - 1
				);
				assemble_len -= 1;
		    	}
		}

		vTaskDelay(pdMS_TO_TICKS(1));
	}
}


void createTasks(void)
{
	xTaskCreate(
	        outputTask,
                "Output Task",
                512,
        	NULL,
        	tskIDLE_PRIORITY + 3,
        	&outputTaskHandle
        );

	xTaskCreate(
    		feedbackTask,
    		"Feedback Task",
    		256,
    		NULL,
   		tskIDLE_PRIORITY + 2,
    		&feedbackTaskHandle
	);

	xTaskCreate(
    		pcRxTask,
    		"PC Rx Task",
   		512,
    		NULL,
    		tskIDLE_PRIORITY + 1,
    		&pcRxTaskHandle
	);
}

