#include "ServoMotor_Lib.h"

ServoMotor::ServoMotor(uint16_t max_angle_dgr, uint16_t min_pulse_us, uint16_t max_pulse_us)
	: m_max_angle_dgr(max_angle_dgr),
      m_min_pulse_us(min_pulse_us),
      m_max_pulse_us(max_pulse_us),
      m_angle_dgr(0)
{
	m_step = (float)(m_max_pulse_us - m_min_pulse_us) / (float)(m_max_angle_dgr);
}

float ServoMotor::GetAnlge(void)
{
	return m_angle_dgr;
}
void ServoMotor::SetAngle(float angle_dgr)
{
	if (angle_dgr < 0){
		angle_dgr = 0;
	} else if (angle_dgr > m_max_angle_dgr) {
		angle_dgr = m_max_angle_dgr;
	}
	uint16_t add_pulse_us = (uint16_t)(m_step * angle_dgr);
	uint16_t load_pulse_us = m_min_pulse_us + add_pulse_us;
	
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, load_pulse_us);
}

