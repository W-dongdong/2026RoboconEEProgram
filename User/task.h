#ifndef __TASK_H
#define __TASK_H


#include "main.h"
#include "DM3519_Lib.h"
#include "Mecanum_Chassis.h"
#include "host_parsing.h"
#include "M2006_Lib.h"
#include "usart_drv.h"

void Motor_Init(void);
void USART2_Task(void);
void CAN1_Task(void);
void CAN2_Task(void);
void TIM1_Task(void);

void Test_Function(void);

#endif
