#include <p32xxxx.h>
#include <plib.h>

void Timer_Initialization()
{
	INTCONbits.MVEC = 1;
	SYSKEY = 0x0; //write invalid key to force lock
	SYSKEY = 0xAA996655; //write Key1 to SYSKEY
	SYSKEY = 0x556699AA;  //Write Key2 to SYSKEY
	OSCCONbits.PBDIV = 0b00; //PBCLK = SYSCLK/1 = 4Mhz
	SYSKEY = 0x0; //Write invalid key to force lock

	T2CON = 0x70; //Timer 2 1:256 prescale
	PR2 = 0xFFFF;
	TMR2 = 0;

        T3CON = 0x70;
        PR3 = 0xFFFF;
        TMR3 = 0;
        IPC3bits.INT3IP = 6;
        IEC0bits.T3IE = 1;
        IFS0bits.T3IF = 0;
}