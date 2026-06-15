#include "AutoAction.h"

float x; float y; float w;
static uint8_t mod = 0; static uint8_t last_mod = 0; uint8_t cmd = 0;
//PID V_Chassis(0.2, 0.001, 0.1, );

void UpStairs(float Des)
{
//	switch (mod)
//		{
//			case 0: // Horizontal position
//				L_arm.PosSpeedMode(0, 1/19.2 * 10);
//				R_arm.PosSpeedMode(0, 1/19.2 * 10); 
//				break;
//			
//			case 1: // Medium position
//				L_arm.PosSpeedMode(-55*25, 1/19.2 * 5);  
//				R_arm.PosSpeedMode(55*25, 1/19.2 * 5);
//				break;
//			
//			case 2: // High position
//				L_arm.PosSpeedMode(-90*25, 1/19.2 * 5);  
//				R_arm.PosSpeedMode(90*25, 1/19.2 * 5);
//				break;
//			default:
//				break;
//		}
	/* Stand Up */
	L_arm.PosSpeedMode(-55*25, 1/19.2 * 5);  
	R_arm.PosSpeedMode(55*25, 1/19.2 * 5);
	while (L_arm.m_Vel || R_arm.m_Vel)
	{
		L_arm.PosSpeedMode(-55*25, 1/19.2 * 5);  
		R_arm.PosSpeedMode(55*25, 1/19.2 * 5);
	}
	
	/* Approach to the stair */
	while (Des != 0)
	{
		R2_Chassis.move(x, y, w);
	}
	
	L_arm.PosSpeedMode(0, 1/19.2 * 10);
	R_arm.PosSpeedMode(0, 1/19.2 * 10);
	
	while (L_arm.m_Vel || R_arm.m_Vel)
	{
		L_arm.PosSpeedMode(0, 1/19.2 * 10);
		R_arm.PosSpeedMode(0, 1/19.2 * 10);
	}
}

void CollectBox()
{
	
}

void PutBox()
{
	
}

void AutoMode(void)
{
	float x; float y; float w;
	static uint8_t mod = 0; static uint8_t last_mod = 0; uint8_t cmd = 0;
	
	host_Msg_parsing(&x, &y, &w, &mod, &cmd);
//	UART_printf(&huart2, "x:%2f y:%2f w:%2f mod:%d cmd:%d\r\n", x, y, w, mod, cmd);
	R2_Chassis.move(x, y, w);
}
