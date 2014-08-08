#include <p32xxxx.h>
#include <plib.h>
#include "LCD.h"

char to_send = ' ';
char received = ' ';

#pragma interrupt CN_ISR ipl1 vector 26
void CN_ISR(void)
{
	int i = 0;
	IEC1CLR = 0x0001;
	if(PORTDbits.RD13 == 0)
	{
		U1ASTAbits.UTXEN = 1;
		U1ATXREG = to_send;
	}
	else if(PORTDbits.RD6 == 0)
	{
		if(to_send != 0x7F)
		{
			to_send = to_send + 1;
		}
		else
		{
			to_send = 0x20;
		}
		Display(to_send, received);
	}
	IFS1CLR = 0x0001;
	IEC1SET = 0x0001;
	for(i = 0;i<4000;i++);
}

#pragma interrupt RX_ISR ipl7 vector 24
void RX_ISR(void)
{
	received = U1ARXREG;
	if(received >= 0x20 && received <= 0x7F)
	{
		Display(to_send, received);
	}
	IFS0bits.U1RXIF = 0;
}


main()
{
    int k = 0;
    TRISDCLR = 0x17;
    PORTD = 0x0011;
	INTCONbits.MVEC = 1;
	SYSKEY = 0x0; //write invalid key to force lock
	SYSKEY = 0xAA996655; //write Key1 to SYSKEY
	SYSKEY = 0x556699AA;  //Write Key2 to SYSKEY
	OSCCONbits.PBDIV = 0b11; //PBCLK = SYSCLK/8
	SYSKEY = 0x0; //Write invalid key to force lock
	asm("di");
	MCU_init();
	LCD_init();

	TRISDbits.TRISD6 = 1;
	TRISDbits.TRISD13 = 1;
	CNCON = 0x8000;
	CNENbits.CNEN19 = 1;  //CN15(RD6), CN19(RD13)

	CNPUEbits.CNPUE19 = 1;

	CNENbits.CNEN15 = 1;  //CN15(RD6), CN19(RD13)

	CNPUEbits.CNPUE15 = 1;
	IPC6SET = 0x40000; //Set priority level = 1;
	IFS1CLR = 0x0001;
	IEC1SET = 0x0001;

	U1ABRG = 25;  //Configure baud rate to be 4800 according
				   //to the first problem
	U1AMODE = 0x0; //Enable UART1
					 //PDSEL = 0x00 8-bit data& No parity
					 //STSEL = 0 1 stop bit
	U1AMODEbits.BRGH = 1;
	U1AMODESET = 0x8000;  //Enable UART1A

        U1ASTAbits.UTXEN = 1;   //Transmit is enabled  Receive is enabled
	U1ASTAbits.URXEN = 1;
	IFS0bits.U1RXIF = 0;
	IEC0bits.U1RXIE = 1; //Enable receive interrupt
	IPC6bits.U1IP = 7;

	U1ASTAbits.URXISEL = 0;
        T1CON = 0x70;
        PR1 = 0xFFFF;
        TMR1 = 0;
        T1CONSET = 0x8000;
        asm("ei");
        while(TMR1<15)
        {
            U1ATXREG = 'w';
        }
        while(1);
}