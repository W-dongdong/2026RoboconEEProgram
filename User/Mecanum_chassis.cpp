#include "Mecanum_chassis.h"

Mecanum_Chassis::Mecanum_Chassis(float length, float width, float Wheel_Radius)
	: m_length(length),
      m_width(width),
      m_Vx(0), m_Vy(0), m_W(0),
	  m_Wheel_Radius(Wheel_Radius),
	  Wheel1(&hcan1, 0),
	  Wheel2(&hcan1, 0),
	  Wheel3(&hcan1, 0),
	  Wheel4(&hcan1, 0)
{
}

// Before you using this function, make sure that you have complete the motors's initialization
// NOTE: The unit of Vx Vy should be m/s, W should be rad/s
uint8_t Mecanum_Chassis::move(float Vx, float Vy, float W)
{
	float wheel1_rotate = (-Vx + Vy + W*(m_length + m_width))/m_Wheel_Radius;
	float wheel2_rotate = (-Vx - Vy + W*(m_length + m_width))/m_Wheel_Radius;
	float wheel3_rotate = ( Vx - Vy + W*(m_length + m_width))/m_Wheel_Radius;
	float wheel4_rotate = ( Vx + Vy + W*(m_length + m_width))/m_Wheel_Radius;
	
	uint8_t state1 = Wheel1.SpeedMode(wheel1_rotate);
	uint8_t state2 = Wheel2.SpeedMode(wheel2_rotate);
	uint8_t state3 = Wheel3.SpeedMode(wheel3_rotate);
	uint8_t state4 = Wheel4.SpeedMode(wheel4_rotate);
	
	return state1 + state2 + state3 + state4;
}
