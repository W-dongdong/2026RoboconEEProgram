#ifndef __MECANUM_CHASSIS_H
#define __MECANUM_CHASSIS_H

#include "math_vector.h"
#include "M3508_Lib.h"
#include "DM3519_Lib.h"

/*
		Wheel_2			Wheel_1
		|	\\			//		^	-|
		V	\\			//		|	 |
									 |
									 |	Length = a (m)
									 |
		Wheel_3			Wheel_4		 |
		|	//			\\		^	 |
		V	//			\\		|	-|
			
			|____________|	Width = b (m)
*/

class Mecanum_Chassis{
private:
	float m_length;		// In meter
	float m_width;		// In meter


public:
	float m_Vx;
	float m_Vy;
	float m_W;
	float m_Wheel_Radius;// In meter

	DM3519 Wheel1;
	DM3519 Wheel2;
	DM3519 Wheel3;
	DM3519 Wheel4;

	Mecanum_Chassis(float length, float width, float Wheel_Radius);
	
	uint8_t move(float Vx, float Vy, float W);
	
};


#endif
