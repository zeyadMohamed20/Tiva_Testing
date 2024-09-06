/************************************************* 
*  FILE  DESCRIPTION
*------------------------------------------------
*     File: main.c
*     Description: Contains Test functions
*
****************************************************/
#include "Std_Types.h"
#include "DIO.h"

int main(void)
{
	uint8 pin_value = 0;
	Port_Config mode = { PORT_F, PIN_MODE_GPIO, PIN_OUTPUT, PULL_DOWN_RES, DRIVE_CURRENT_2M, OPEN_DRAIN_INACTIVE,  1 };
	Pin_Init(PIN0, &mode);
	Pin_Init(PIN1, &mode);
	pin_value = Pin_Read(PORT_F, PIN1);
	while(1)
		{
			for(int i = 0; i < 90000; i++);
			pin_value ^= (1<<1);
			Write_Pin(PORT_F, PIN1, pin_value); /* Toggle the Red LED at PF1 */
		}
}

void GPIO_Test(void)
{
	uint8 pin_value = 0;
	uint8 port_value = 0;

	Port_Config mode = { PORT_A, PIN_MODE_GPIO, PIN_INPUT, PULL_DOWN_RES, DRIVE_CURRENT_2M, OPEN_DRAIN_INACTIVE,  1 };
	Pin_Init(PIN0, &mode);
	
	Write_Pin(PORT_A, PIN0, 1 );
	
	Port_Write(PORT_A, 0x0E );
	Pin_Flip(PORT_A, PIN0);
	
	pin_value = Pin_Read(PORT_A, PIN0);
	port_value = Port_Read(PORT_A);
}