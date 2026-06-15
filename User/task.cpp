#include "task.h"

Mecanum_Chassis	R2_Chassis(0.53, 0.55, 5);
/*---------DM3519--------*/
DM3519 L_arm(&hcan1, 5, 0x700);
DM3519 R_arm(&hcan1, 6, 0x700); // 这里的接收需要改成对应ID
DM3519 BaseSlider(&hcan1, 8, 0x788);
DM3519 Joint(&hcan1, 7, 0x788);

/*---------M2006----------*/
M2006 CatchMotor(&hcan2, 1);
M2006 GripMotor(&hcan2, 2);
M2006 BackMotor(&hcan2, 3);
M2006 SliderMotor(&hcan2, 4);

uint8_t Chassis_Init(void)
{
	uint8_t state = 0;
	  
	
	state += R2_Chassis.Wheel1.SetMotorState(enable);
	state += R2_Chassis.Wheel2.SetMotorState(enable);
	state += R2_Chassis.Wheel3.SetMotorState(enable);
	state += R2_Chassis.Wheel4.SetMotorState(enable);
	state += L_arm.SetMotorState(enable);
	state += R_arm.SetMotorState(enable);
	if (state == 6){
		UART_printf(&huart2, "Chassis Initialization completed\r\n");
		return 1;
	} else{
		UART_printf(&huart2, "Chassis Initialization failed\r\n");
		return 0;
	}
}

M2006* motors[4] = {&GripMotor, &CatchMotor, &BackMotor, &SliderMotor};

uint8_t M2006_Init(void)
{
    PID speed_cfg(0.08f, 0.0f, 0.01f, 4000.0f, 4000.0f);
    PID pos_cfg(0.02f, 0.0f, 0.0f, 1000.0f, 2000.0f);
    
    GripMotor.PID_Config(pos_cfg, speed_cfg);
    CatchMotor.PID_Config(pos_cfg, speed_cfg);
    BackMotor.PID_Config(pos_cfg, speed_cfg);
    SliderMotor.PID_Config(pos_cfg, speed_cfg);
	

	UART_printf(&huart2, "M2006 initialized in parallel.\r\n");
	return 1;
}

uint8_t DM3519_Init(void)
{
	uint8_t state = 0;
	state += Chassis_Init();
	state += BaseSlider.SetMotorState(enable);
	state += Joint.SetMotorState(enable);
	if (state == 3){
		UART_printf(&huart2, "DM3519 Initialization completed\r\n");
		return 1;
	} else{
		UART_printf(&huart2, "DM3519 Initialization failed\r\n");
		return 0;
	}
}

void Motor_Init(void)
{
	uint8_t state = 0;
	state += DM3519_Init();
	state += M2006_Init();
	
	if (state == 2) {
		UART_printf(&huart2, "Motor Initialization completed\r\n");
	} else {
		UART_printf(&huart2, "Motor Initialization failed\r\n");
		while(1);
	}
}

/*This is the register of motor*/
//M2006* motors[4] = {&GripMotor, &CatchMotor, &BackMotor, &SliderMotor};
float targetPositions[4] = {-360 * 6.0f, 360 * 4.0f, 360 * 4.0f, -360 * 4.0f};
uint16_t targetSpeeds[4] = {8000, 5000, 5000, 5000};
void Command_parsing(uint8_t cmd)
{
	for (uint8_t i = 0; i < 4; i++) 
    {
        if ((cmd >> i) & 0x01) {
            motors[i]->PosSpeedMode(targetPositions[i], targetSpeeds[i]);
        } else {
            motors[i]->PosSpeedMode(0, targetSpeeds[i]);
        }
    }
	
	uint8_t DM_cmd = (cmd >> 4)&0x3;
	if (DM_cmd == 1){
		BaseSlider.SpeedMode(-0.5); 
	}else if (DM_cmd == 2){
		BaseSlider.SpeedMode(0.5);
	}else{
		BaseSlider.SpeedMode(0);
	}
	
	DM_cmd = (cmd >> 6);
	if (DM_cmd == 1){
		Joint.PosSpeedMode(3250, 0.5);
	}else if (DM_cmd == 2){
		Joint.PosSpeedMode(400, 0.3);
	}else{
		Joint.PosSpeedMode(0, 0.5);
	}
}

void ManualMode(void)
{
	float x; float y; float w;
	static uint8_t mod = 0; static uint8_t last_mod = 0; uint8_t cmd = 0;
	static uint8_t trigger_times = 0;
	
	host_Msg_parsing(&x, &y, &w, &mod, &cmd);
	UART_DMA_printf(&huart2, "x:%2f y:%2f w:%2f mod:%d cmd:%d\r\n", x, y, w, mod, cmd);

	R2_Chassis.move(x, y, w);

	/*------------------------------------------------*/
//	if ((mod - last_mod) == 1) // up trigger
//	{
		switch (mod)
		{
			case 0: // Horizontal position
				trigger_times++;
				L_arm.PosSpeedMode(0, 1/19.2 * 10);
				R_arm.PosSpeedMode(0, 1/19.2 * 10); 
				break;
			
			case 1: // Get spear position
				trigger_times++;
				L_arm.PosSpeedMode(-70*25, 1/19.2 * 5);  
				R_arm.PosSpeedMode(70*25, 1/19.2 * 5);
				break;
			
			case 2: // Match position
				trigger_times = 0;
				L_arm.PosSpeedMode(-31*25, 1/19.2 * 5);  
				R_arm.PosSpeedMode(31*25, 1/19.2 * 5);
				break;
			default:
				break;
		}
//	}
//	last_mod = mod;
	/*------------------------------------------------*/
	Command_parsing(cmd);
}

void Test_Function(void)
{
 
}

void CAN1_Task(void) 
{
	/*ATTENTION!!!printf is time-costy, do not use it in practical control, especially for position control*/
	/*Remember the optimization is ENABLE (on level 1)*/
//	UART_printf(&huart2, "BasePos:%f\r\n", BaseSlider.m_Pos); 
	if (BaseSlider.ParseFeedback(&Can1_Msg))
	{
//		UART_DMA_printf(&huart2, "Base: %2f\r\n", BaseSlider.m_Pos);
	}
}

void CAN2_Task(void)
{
	M2006::ControlLoopUpdate(&Can2_Msg);
}

uint8_t DM_times = 0;
void TIM1_Task(void)
{
	M2006::SendGroup(&hcan2, 0x200);
	if (DM_times == 50)
	{
//		BaseSlider.SetMotorState(enable);
		DM_times = 0;
	}
	DM_times ++;
}

