#include "timer_drv.h"


HAL_StatusTypeDef TIM_Start(TIM_HandleTypeDef *htim)
{
	return HAL_TIM_Base_Start_IT(htim);
}

void TIM_SetCompare(TIM_HandleTypeDef *htim, uint16_t value)
{
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, value);
}

uint8_t TIM1_Flag = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
		TIM1_Flag = 1;
    }
}
