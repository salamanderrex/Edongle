
/************************************************************************
* LCD.c
************************************************************************/
#include "LCD.h"
#include <plib.h>
#include <string.h>

const uchar startStr3[] = "VE373 Final Proj";
/* main driver */
void Display_voltage(int result, char gate_code)
{
	double b = result/1024.0*5.0;
	int ones = b;
	int tenth = b*10.0-ones*10;
	int hundredth = b*100.0-tenth*10-ones*100;
	char onesd = ones+48;
	char tenthd = tenth+48;
	char hundredthd = hundredth + 48;
	uchar startStr[] = "0.00V 0         ";
	int i = 0;
	startStr[0] = onesd;
	startStr[1] = '.';
	startStr[2] = tenthd;
	startStr[3] = hundredthd;
	startStr[6] = gate_code;
	LCD_goto(0x00);
	DelayMsec(1);
	LCD_puts(startStr);
	DelayMsec(1);
	LCD_goto(0x40);
	LCD_puts(startStr3);
}

void Display_message(const uchar *message)
{
	LCD_goto(0x00);
	DelayMsec(1);
	LCD_puts(message);
	DelayMsec(1);
	LCD_goto(0x40);
	LCD_puts(startStr3);
}

void Display_uart(char send, char received) //For debug use
{
	uchar startStr[] = "To send:  ";
	uchar startStr3[] = "Received:  ";
	startStr[9] = send;
	startStr3[10] = received;
	LCD_goto(0x00);
	DelayMsec(1);
	LCD_puts(startStr);
	DelayMsec(1);
	LCD_goto(0x40);
	LCD_puts(startStr3);
}

/* initialize the PIC32 MCU */
void MCU_init() {
/* setup I/O ports to connect to the LCD module */
TRISDbits.TRISD7 = 0;
TRISDbits.TRISD4 = 0;
TRISEbits.TRISE0 = 0;
TRISEbits.TRISE1 = 0;
TRISEbits.TRISE2 = 0;
TRISEbits.TRISE3 = 0;
/* setup Timer to count for 1 us and 1 ms */
T1CON = 0x0;

}
/* initialize the LCD module */

void LCD_init() {
DelayMsec(15);    //wait for 15 ms
RS = 0;      //send command

//E = 1;
Data = LCD_IDLE;
//E = 0;
DelayMsec(5);    //wait for 5 ms

//E = 1;
Data = LCD_IDLE;
//E = 0;
DelayUsec(100);    //wait for 100 us

//E = 1;
Data = LCD_IDLE;
//E = 0;
DelayMsec(5);

//Why we need one more LCD_IDLE?
//E = 1;
Data = LCD_IDLE;
//E = 0;
DelayUsec(100);


LCD_putchar(LCD_2_LINE_4_BITS); //4-bits mode
DelayUsec(40);
LCD_putchar(LCD_DSP_CSR); //Display on, cursor off, blink off
DelayUsec(40);
LCD_putchar(LCD_CLR_DSP); //Clear Display
DelayMsec(5);
LCD_putchar(LCD_CSR_INC); //Increment mode, without display shift
}
/* Send one byte c (instruction or data) to the LCD */
void LCD_putchar(uchar c)
{
E = 1;
Data = c>>4;
E = 0;

E = 1;
Data = c;
E = 0;
}

/* Display a string of characters *s by continuously calling LCD_putchar() */
void LCD_puts(const uchar *s)
{
	int i = 0;
	RS = 1;
	while(*(s+i)!=NULL)
	{
		LCD_putchar(s[i]);
		i++;
		DelayUsec(40);
	}
	RS = 0;
}

void LCD_goto(uchar addr)
{
	addr = addr + 0x80;
	LCD_putchar(addr);
	DelayUsec(40);
}

void DelayUsec(uchar num)
{
	TMR1 = 0;
	T1CONSET = 0x8000;
	//This is for 8Mhz CPU clock
	//We don't use interrupt handler here because the CPU really needs pending
	while(TMR1 < 18*num);
	T1CON = 0x0;
}

void GenMsec()
{
	TMR1 = 0;
	T1CONSET = 0x8000;
	while(TMR1 < 0xFA0);
	T1CON = 0x0;
}

void DelayMsec(uchar num)
{
	uchar i;
	for (i=0; i<num; i++)
	{
		GenMsec();
	}
}

void LCD_Initialization()
{
	MCU_init();
	LCD_init();       //Do Initialization for LCD display
}

/*************end of LCD.c**************/
