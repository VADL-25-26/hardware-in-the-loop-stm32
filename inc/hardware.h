#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

void hardwareInit(void);
void toggleLed(void);
void startOutputTimer(void);
/* PA3 control */
void PA3_SetHigh(void);
void PA3_SetLow(void);
void PA3_Toggle(void);

/* PC0 control */
void PC0_SetHigh(void);
void PC0_SetLow(void);
void PC0_Toggle(void);

// LED2
void led2_on(void);
void led2_off(void);

// LED3
void led3_on(void);
void led3_off(void);
#endif // HARDWARE_INIT_H

