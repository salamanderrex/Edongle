
#include "ADC.h"
#include "UART.h"
#include "Timer.h"

const uchar startStr4[] = "VE373 Final Proj";

void Status_LED_Initialization()
{
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD1 = 0;
}

void Initialization(void)
{
	Status_LED_Initialization();
	Timer_Initialization();
	LCD_Initialization();
	ADC_Initialization();
	asm("ei");
}

void Unlock()
{
	
	const uchar unlock[] = "Unlock gesture";
        Display_message(unlock);
        AD1CON1SET = 0x8000;
}

main()
{
	Initialization();
	Unlock();
	while(1);
}