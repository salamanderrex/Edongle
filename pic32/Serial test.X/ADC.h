#include "LCD.h"
#include <plib.h>
#include "UART.h"

int result[10];
char gate = '0';
char gate_code[9] = "000000000";
char gate_c = '0';
int gate_count = 0;
char msg[] = "Unlock gesture";
const char true_code[9] = "adehi0000";


#pragma interrupt ADC_READ ipl3 vector 27
void ADC_READ(void)
{
	result[0] = ADC1BUF0;
	if(result[0] > 600)
	{
		gate = 'b';
	}
	result[1] = ADC1BUF1;
	if(result[1] > 600)
	{
		gate = 'c';
	}
	result[2] = ADC1BUF2;
	if(result[2] > 600)
	{
		gate = 'd';
	}
	result[3] = ADC1BUF3;
	if(result[3] > 600)
	{
		gate = 'e';
	}
	result[4] = ADC1BUF4;
	if(result[4] > 600)
	{
		gate = 'f';
	}
	result[5] = ADC1BUF5;
	if(result[5] > 600)
	{
		gate = 'a';
	}
	result[6] = ADC1BUF6;
	if(result[6] > 600)
	{
		gate = 'g';
	}
	result[7] = ADC1BUF7;
	if(result[7] > 600)
	{
		gate = 'h';
	}
	result[8] = ADC1BUF8;
	if(result[8] > 600)
	{
		gate = 'i';
	}
        //if(result[9] > 600)
        //{
        //    gate = '0';
        //}

	if(gate != gate_c && gate != '0')
	{
		gate_code[gate_count] = gate;
		gate_count  ++;
		DelayMsec(300);
		if(gate_count == 5)
		{
			if(strcmp(gate_code,true_code) == 0)
			{
				const uchar unlock_success[] = "Unlock success";
				Display_message(unlock_success);
                                //msg = unlock_success;
                                UART_Initialization();
                                PORTD = 1;
				TMR2 = 0;
				T2CONSET = 0x8000;
                                while(TMR2 < 400)
				{
					U1ATXREG = 11; //Send 11 to inform successful unlock
				}
                                //AD1CON1CLR = 0x8000;
                                DelayMsec(2000);
                                gate_count = 0;
                                gate_code[0] = '0';
                                gate_code[1] = '0';
                                gate_code[2] = '0';
                                gate_code[3] = '0';
                                gate_code[4] = '0';
                                gate_code[5] = '0';
                                gate_code[6] = '0';
                                gate_code[7] = '0';
                                gate_code[8] = '0';
				const uchar enjoy[] = "Enjoy Edongle ";
				Display_message(enjoy);
			}
			else
			{
				gate_count = 0;
                                gate_code[0] = '0';
                                gate_code[1] = '0';
                                gate_code[2] = '0';
                                gate_code[3] = '0';
                                gate_code[4] = '0';
                                gate_code[5] = '0';
                                gate_code[6] = '0';
                                gate_code[7] = '0';
                                gate_code[8] = '0';
				const uchar unlock_fail[] = "Wrong gesture ";
				Display_message(unlock_fail);
			}
		}
	}
	//Display_voltage(result[1],gate_code[gate_count-1]);
        gate_c = gate;
	gate = '0';
	IFS1bits.AD1IF = 0;
}



void ADC_Initialization()
{
	asm("di");
	INTCONbits.MVEC = 1;
	TRISBbits.TRISB10 = 1;
	TRISBbits.TRISB1 = 1;
	TRISBbits.TRISB2 = 1;
	TRISBbits.TRISB3 = 1;
	TRISBbits.TRISB4 = 1;
	TRISBbits.TRISB5 = 1;
	TRISBbits.TRISB11 = 1;
	TRISBbits.TRISB12 = 1;
	TRISBbits.TRISB13 = 1;  
       // TRISBbits.TRISB14 = 1;  //Configure AN10, AN1, AN2, AN3, AN4, AN5, AN11, AN12, AN13, AN14 as input

	AD1PCFGbits.PCFG10 = 0;
	AD1PCFGbits.PCFG1 = 0;
	AD1PCFGbits.PCFG2 = 0;
	AD1PCFGbits.PCFG3 = 0;
	AD1PCFGbits.PCFG4 = 0;
	AD1PCFGbits.PCFG5 = 0;
	AD1PCFGbits.PCFG11 = 0;
	AD1PCFGbits.PCFG12 = 0;
	AD1PCFGbits.PCFG13 = 0;
       // AD1PCFGbits.PCFG14 = 0; //Configure AN10, AN1, AN2, AN3, AN4, AN5, AN11, AN12, AN13 as analog

	AD1CHS = 0;           //Clear all bits of AD1CHS to do further configuration

	//AD1CHSbits.CH0SA = 1;  //For debug use

	AD1CHSbits.CH0NA = 0;	//Channel 0 negative input is VR-

	AD1CSSL = 0x3C3E;          // Enable input scanning for AN10, AN1, AN2, AN3, AN4, AN5, AN11, AN12, AN13, AN14

	AD1CON1bits.ASAM = 1;         // ASAM = 1 Auto sampling
	AD1CON1bits.SSRC = 7;			             //FORM = 000 Integer 16-bit
				             //SSRC = 111
	AD1CON2 = 0;         //Clear all bits of AD1CON2 to do further cofiguration
							 //ALTS = 0 Always use MUX A
							 //BUFM = 0 One 16-bit buffer
					     //SMPI = 0000 Interrupt at the completion of every convert
							 //CSCNA = 0 Do not scan inputs
	AD1CON2bits.VCFG = 0x3;   //VCFG = 011 means using External VREF+ and VREF- as reference
	AD1CON2bits.CSCNA = 1;
	AD1CON3 = 0x0010;        //ADCS = 00000010 128*Tpb = Tad
	AD1CON3bits.SAMC = 9;

	IFS1bits.AD1IF = 0;
	IEC1bits.AD1IE = 1;
	IPC6bits.AD1IP = 3;
	AD1CON2bits.SMPI = 9;
	//asm("ei");

	//AD1CON1SET = 0x8000;     //Turn on the ADC module
}
