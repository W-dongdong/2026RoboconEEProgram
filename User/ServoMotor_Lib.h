#ifndef __SERVOMOTOR_LIB_H
#define __SERVOMOTOR_LIB_H

#include "timer_drv.h"

class ServoMotor{
private:
	uint16_t m_max_angle_dgr;
    uint16_t m_min_pulse_us;
    uint16_t m_max_pulse_us;
    float    m_angle_dgr;
	float	 m_step;

public:
	ServoMotor(uint16_t max_angle, uint16_t min_pulse_us, uint16_t max_pulse_us);
	float GetAnlge(void);
	void SetAngle(float Angle);
};

#endif
