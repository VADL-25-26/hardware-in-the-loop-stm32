#include "hardware.h"
#include "stm32h7xx.h"
#include "stm32h753xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hil_tasks.h"

void hardwareInit(void)
{
	// INIT TEST LED ----------------------------------------------

	// Enable clock
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;

	// Config as output
	GPIOB->MODER &= ~(3U << (0 * 2));
	GPIOB->MODER |=  (1U << (0 * 2));

	// Start high
	GPIOB->BSRR = GPIO_BSRR_BS0;

	// INIT 200Hz TIMER --------------------------------------------------
	
	RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;
	
	// Reset clock
        RCC->APB1LRSTR |= RCC_APB1LRSTR_TIM6RST;
        RCC->APB1LRSTR &= ~RCC_APB1LRSTR_TIM6RST;

        // psc, arr config
        TIM6->PSC = 6399;   // 240 MHz / 24000 = 10 kHz
        TIM6->ARR = 49;      // 10 kHz / 50 = 200 Hz

        // Generate update event
        TIM6->EGR = TIM_EGR_UG;
 
	// Explicitly wait for timer start
        TIM6->CR1 &= ~TIM_CR1_CEN;
	TIM6->DIER &= ~TIM_DIER_UIE;
	NVIC_DisableIRQ(TIM6_DAC_IRQn);

	// Priority set
        NVIC_SetPriorityGrouping(0);
	NVIC_SetPriority(TIM6_DAC_IRQn, 10);
	
	/* Enable GPIOA and GPIOC clocks */
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOCEN;
        

        // INIT PA3 FOR TEST ----------------------------------------
	
        // Output mode
        GPIOA->MODER &= ~(3U << (3 * 2));
        GPIOA->MODER |=  (1U << (3 * 2));

        // Push pull
        GPIOA->OTYPER &= ~(1U << 3);

        // Fast speed
        GPIOA->OSPEEDR &= ~(3U << (3 * 2));
        GPIOA->OSPEEDR |=  (3U << (3 * 2));

        // No pull up
        GPIOA->PUPDR &= ~(3U << (3 * 2));

        // INIT PC0 FOR TEST -------------------------------------
	
        // Output mode
        GPIOC->MODER &= ~(3U << (0 * 2));
        GPIOC->MODER |=  (1U << (0 * 2));

        // Push pull
	GPIOC->OTYPER &= ~(1U << 0);

        // High speed
        GPIOC->OSPEEDR &= ~(3U << (0 * 2));
        GPIOC->OSPEEDR |=  (3U << (0 * 2));

        // No pull up
	GPIOC->PUPDR &= ~(3U << (0 * 2));

	// USART3 PINS ----------------------------------------
	
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;

	// Set PD8 and PD9 to Alternate Function mode
	GPIOD->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
	GPIOD->MODER |=  ((2U << (8 * 2)) | (2U << (9 * 2)));

	// Select AF7 (USART3) for PD8 and PD9
	GPIOD->AFR[1] &= ~((0xFU << ((8 - 8) * 4)) | (0xFU << ((9 - 8) * 4)));
	GPIOD->AFR[1] |=  ((7U   << ((8 - 8) * 4)) | (7U   << ((9 - 8) * 4)));

	// Set high speed for PD8 and PD9
	GPIOD->OSPEEDR |= ((3U << (8 * 2)) | (3U << (9 * 2)));

	// Push pull output
	GPIOD->OTYPER &= ~((1U << 8) | (1U << 9));

	// No pull up, no pull down
	GPIOD->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));

	// USART2 PA2 TX --------------------------------------------

	GPIOA->MODER &= ~(3 << (2 * 2));
    	GPIOA->MODER |=  (2 << (2 * 2));

    	GPIOA->AFR[0] &= ~(0xF << (2 * 4));
    	GPIOA->AFR[0] |=  (7 << (2 * 4));

	// USER LEDS 2 AND 3 ---------------------------------------------
	
    	/* Enable GPIOB clock */
    	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;

    	/* PB0, PB14 → General purpose output */
    	GPIOB->MODER &= ~((3U << (0 * 2)) | (3U << (14 * 2)));
    	GPIOB->MODER |=  ((1U << (0 * 2)) | (1U << (14 * 2)));

    	// Push pull
    	GPIOB->OTYPER &= ~((1U << 0) | (1U << 14));

    	// High speed
    	GPIOB->OSPEEDR |= ((3U << (0 * 2)) | (3U << (14 * 2)));

    	/* No pull-up / pull-down */
    	GPIOB->PUPDR &= ~((3U << (0 * 2)) | (3U << (14 * 2)));

    	/* Start with LEDs OFF */
    	GPIOB->BSRR = (1U << (0 + 16)) | (1U << (14 + 16));

}

void toggleLed(void)
{
	GPIOB->ODR ^= GPIO_ODR_OD0;
}

void PA3_SetHigh(void)
{
    	GPIOA->BSRR = (1U << 3);
}

void PC0_SetHigh(void)
{
    	GPIOC->BSRR = (1U << 0);
}

void PA3_SetLow(void)
{
    	GPIOA->BSRR = (1U << (3 + 16));
}

void PC0_SetLow(void)
{
    	GPIOC->BSRR = (1U << (0 + 16));
}

void PA3_Toggle(void)
{
    	GPIOA->ODR ^= (1U << 3);
}

void PC0_Toggle(void)
{
    	GPIOC->ODR ^= (1U << 0);
}

void led2_on(void)
{
    	GPIOB->BSRR = (1U << 0);
}

void led2_off(void)
{
    	GPIOB->BSRR = (1U << (0 + 16));
}

void led3_on(void)
{
    	GPIOB->BSRR = (1U << 14);
}

void led3_off(void)
{
    	GPIOB->BSRR = (1U << (14 + 16));
}



void startOutputTimer(void) {

	TIM6->SR = 0;
	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	TIM6->CR1 |= TIM_CR1_CEN;




}


void TIM6_DAC_IRQHandler(void)
{
	
	
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (TIM6->SR & TIM_SR_UIF)
        {
                TIM6->SR &= ~TIM_SR_UIF; // clear update flag
		PA3_Toggle();

		// gate if output task not created or scheduler not running
		if (outputTaskHandle != NULL &&
		    	xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
		{
			// unblock output task
		    	vTaskNotifyGiveFromISR(outputTaskHandle, &xHigherPriorityTaskWoken);
		}

	
	           
	}

	// notify scheduler about output task being unblocked
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}





