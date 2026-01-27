#include "pwm_feedback.h"
#include "stm32h753xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware.h"

volatile uint32_t pwm_width_ticks = 0;
volatile uint8_t pwm_new_measurement = 0;

uint8_t actuator_moved_flag = 0;
void pwm_feedback_init(void)
{
    /* Enable GPIOA and TIM2 clocks */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;

    /* PA0 → AF1 (TIM2_CH1) */
    GPIOA->MODER &= ~(3U << (0 * 2));
    GPIOA->MODER |=  (2U << (0 * 2));

    GPIOA->AFR[0] &= ~(0xFU << (0 * 4));
    GPIOA->AFR[0] |=  (1U << (0 * 4));   // AF1 = TIM2

    /* Timer clock: 1 MHz (assuming 64 MHz APB1 timer clock) */
    TIM2->PSC = 64 - 1;     // 64 MHz / 64 = 1 MHz
    TIM2->ARR = 0xFFFFFFFF;
    

    /*
     * Input capture:
     * CH1 = rising edge
     * CH2 = falling edge (mapped to same pin)
     */
    
    TIM2->CCMR1 =
        (1U << TIM_CCMR1_CC1S_Pos) |  // CC1 mapped to TI1
        (2U << TIM_CCMR1_CC2S_Pos);   // CC2 mapped to TI1
    
    
    /* Rising edge on CH1, falling edge on CH2 */
    
    TIM2->CCER =
        TIM_CCER_CC1E |
        TIM_CCER_CC2E | TIM_CCER_CC2P;
   
    /* Enable interrupt on CC2 (falling edge = pulse width complete) */
    TIM2->DIER |= TIM_DIER_CC2IE;
    
    /* Enable TIM2 IRQ */
    NVIC_SetPriority(TIM2_IRQn, 6);
    NVIC_EnableIRQ(TIM2_IRQn);

    // Reset on rising edge
    TIM2->SMCR = 	(5 << TIM_SMCR_TS_Pos) | (4 << TIM_SMCR_SMS_Pos);
    /* Start timer */
    TIM2->CR1 |= TIM_CR1_CEN;
}


uint8_t pwm_actuator_moved(void)
{
    return actuator_moved_flag;
}

void pwm_clear_actuator_moved(void)
{
    actuator_moved_flag = 0;
}


void TIM2_IRQHandler(void)
{
	static uint8_t debounce = 0;
    if (TIM2->SR & TIM_SR_CC2IF)
	    
    {
	//toggleLed();

        //uint32_t rise = TIM2->CCR1;

	if (debounce > 6) {

        	uint32_t fall = TIM2->CCR2;

        	pwm_width_ticks = fall;
		pwm_new_measurement = 1;
	} else {
		++debounce;
	}
	
        TIM2->SR &= ~TIM_SR_CC2IF;
	
    }
}

