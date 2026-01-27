#include "FreeRTOS.h"
#include "task.h"
#include "hil_tasks.h"
#include "hardware.h"
#include "stm32h753xx.h"
#include "usart3_transport.h"
#include "imu_state.h"
#include "pwm_feedback.h"

#include "usart2.h"

// delay function for testing
static void delayMs(volatile uint32_t ms)
{
    	for (volatile uint32_t i = 0; i < ms; i++) {
        	for (volatile uint32_t j = 0; j < 8000; j++) {
           		__NOP();
        	}
    	}
}





int main(void)
{
	// init usart pins, timer
    	hardwareInit();

	// init usart2, which is used for output tx (PA2)
	usart2_init();

	// init usart3, which is used for input from/feedback to pc sim
	usart3_transport_init();

	// init imu state, which is a critical section that stores latest state
	imu_state_init();

	// test message
	const char hello[] = "HELLO\r\n";
	usart3_transport_write((const uint8_t *)hello, sizeof(hello) - 1);

	// init pwm change detection
	pwm_feedback_init();

    	createTasks();
    	vTaskStartScheduler();
   
 
     

 
    
	// this should never get reached
    	while (1) {
        	GPIOB->ODR ^= GPIO_ODR_OD0;
        	delayMs(1000);
    	}
}



// function defs to satisfy bare metal dependencies

void vApplicationStackOverflowHook(struct tskTaskControlBlock *xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    for (;;);
}

void vApplicationMallocFailedHook(void)
{
    for (;;);
}
























