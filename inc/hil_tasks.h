#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "stm32h753xx.h"


extern TaskHandle_t outputTaskHandle;
extern TaskHandle_t feedbackTaskHandle;
extern TaskHandle_t pcRxTaskHandle;
void outputTask(void* arg);
void feedbackTask(void* arg);
void pcRxTask(void* arg);
void createTasks(void);




