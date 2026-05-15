#include "usart.h"
#include "host_parsing.h"


void host_Msg_parsing(float* Vx, float* Vy, float* W, uint8_t *mod, uint8_t *cmd)
{
	uint32_t x = process_area[4] << 24 | process_area[3] << 16 | process_area[2] << 8 | process_area[1] << 0;
	uint32_t y = process_area[8] << 24 | process_area[7] << 16 | process_area[6] << 8 | process_area[5] << 0;
	uint32_t w = process_area[12] << 24 | process_area[11] << 16 | process_area[10] << 8 | process_area[9] << 0;
	*mod = process_area[13];
	*cmd = process_area[14];
	
	memcpy(Vx, &x, 4);
	memcpy(Vy, &y, 4);
	memcpy(W, &w, 4);
}


