#ifndef __TASK_H
#define __TASK_H


#include "main.h"
#include "DM3519_Lib.h"
#include "Mecanum_Chassis.h"
#include "host_parsing.h"
#include "M2006_Lib.h"
#include "usart_drv.h"

void Motor_Init(void);
void ManualMode(void);
void CAN1_Task(void);
void CAN2_Task(void);
void TIM1_Task(void);

void Test_Function(void);


extern Mecanum_Chassis R2_Chassis;
/*---------DM3519--------*/
extern DM3519 L_arm;
extern DM3519 R_arm;
extern DM3519 BaseSlider;
extern DM3519 Joint;

/*---------M2006----------*/
extern M2006 CatchMotor;
extern M2006 GripMotor;
extern M2006 BackMotor;
extern M2006 SliderMotor;

#endif
