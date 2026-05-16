#include "task.h"

Mecanum_Chassis	R2_Chassis(0.53, 0.55, 5);
/*---------DM3519--------*/
DM3519 L_arm(&hcan1, 5);
DM3519 R_arm(&hcan1, 6);
DM3519 BaseSlider(&hcan1, 8);
DM3519 Joint(&hcan1, 7);

/*---------M2006----------*/
M2006 CatchMotor(&hcan2, 1);
M2006 GripMotor(&hcan2, 2);
M2006 BackMotor(&hcan2, 3);
M2006 SliderMotor(&hcan2, 4);

uint8_t Chassis_Init(void)
{
	uint8_t state = 0;
	/*Chassis Initialization*/
	R2_Chassis.Wheel1.m_ID = 1;
	R2_Chassis.Wheel2.m_ID = 2;
	R2_Chassis.Wheel3.m_ID = 3;
	R2_Chassis.Wheel4.m_ID = 4;
	  
	
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
    PID speed_cfg(0.08f, 0.0f, 0.0f, 4000.0f, 4000.0f);
    PID pos_cfg(0.02f, 0.0f, 0.0f, 1000.0f, 2000.0f);
    
    GripMotor.PID_Config(pos_cfg, speed_cfg);
    CatchMotor.PID_Config(pos_cfg, speed_cfg);
    BackMotor.PID_Config(pos_cfg, speed_cfg);
    SliderMotor.PID_Config(pos_cfg, speed_cfg);
    
    uint8_t stages[4] = {0, 0, 0, 0};      // 0: Processing; 1:Finished
    uint32_t lastTicks[4] = {0, 0, 0, 0};  // Start time of each stage
    
    // Make all of the motor rotate    
	motors[0] -> SpeedMode(4000);
	lastTicks[0] = HAL_GetTick();
	motors[1] -> SpeedMode(-4000);
	lastTicks[1] = HAL_GetTick();
	motors[2] -> SpeedMode(-4000);
	lastTicks[2] = HAL_GetTick();
	motors[3] -> SpeedMode(4000);
	lastTicks[3] = HAL_GetTick();

    uint8_t totalFinished = 0;

    while (totalFinished < 4) {
        uint32_t currentTick = HAL_GetTick();

        for (uint8_t i = 0; i < 4; i++) {
            // Whether the initialization is completed
            if (stages[i] == 1) {
                continue;
            }

            float absVel = (motors[i]->m_Vel >= 0) ? motors[i]->m_Vel : -motors[i]->m_Vel;

            // Find minimum boundary
            if (stages[i] == 0) {
                // Detect after 300ms, waiting for the launch of motor
                if ((currentTick - lastTicks[i] > 1000) && (absVel < 5)) {
					motors[i] -> SpeedMode(0);
                    lastTicks[i] = currentTick;
                    stages[i] = 1;
					totalFinished ++;
                }
            }
        }
        HAL_Delay(1);
    }
	/*Rotate back little bit*/
	motors[0] -> PosSpeedMode(-10, 3000);
	motors[1] -> PosSpeedMode(10, 3000);
	motors[2] -> PosSpeedMode(10, 3000);
	motors[3] -> PosSpeedMode(-10, 3000);
	
	for (uint8_t i = 0; i < 4; i++)
	{
		motors[i] -> SetZeroPoint();
		motors[i] -> SpeedMode(0);
	}
	
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
		Joint.SpeedMode(-0.3);
	}else if (DM_cmd == 2){
		Joint.SpeedMode(0.3);
	}else{
		Joint.SpeedMode(0);
	}
}

void USART2_Task(void)
{
	float x; float y; float w;
	static uint8_t mod = 0; static uint8_t last_mod = 0; uint8_t cmd;
	static uint8_t trigger_times = 0;
	
	host_Msg_parsing(&x, &y, &w, &mod, &cmd);
	UART_printf(&huart2, "x:%2f y:%2f w:%2f mod:%d cmd:%d\r\n", x, y, w, mod, cmd);

	R2_Chassis.move(x, y, w);

	/*------------------------------------------------*/
	if ((mod - last_mod) == 1) // up trigger
	{
		switch (trigger_times)
		{
			case 0: // Horizontal position
				trigger_times++;
				L_arm.PosSpeedMode(0, 1/19.2 * 10);
				R_arm.PosSpeedMode(0, 1/19.2 * 10); 
				break;
			
			case 1: // Medium position
				trigger_times++;
				L_arm.PosSpeedMode(-55*25, 1/19.2 * 5);  
				R_arm.PosSpeedMode(55*25, 1/19.2 * 5);
				break;
			
			case 2: // High position
				trigger_times = 0;
				L_arm.PosSpeedMode(-90*25, 1/19.2 * 5);  
				R_arm.PosSpeedMode(90*25, 1/19.2 * 5);
				break;
			default:
				break;
		}
	}
	last_mod = mod;
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
}
 
void CAN2_Task(void)
{
	M2006::ControlLoopUpdate(&Can2_Msg);
} 

void TIM1_Task(void)
{
	M2006::SendGroup(&hcan2, 0x200);
}

