#ifndef PWM_FEEDBACK_H
#define PWM_FEEDBACK_H

#include <stdint.h>
extern volatile uint32_t pwm_width_ticks;
extern volatile uint8_t pwm_new_measurement;
extern uint8_t actuator_moved_flag;


void pwm_feedback_init(void);



// never implemented
uint8_t pwm_actuator_moved(void);
void pwm_clear_actuator_moved(void);

#endif

