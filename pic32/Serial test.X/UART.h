#ifndef _UART_H
#define _UART_H

#include <p32xxxx.h>
#include "LCD.h"
#include <plib.h>

char to_send = ' ';
char received = ' ';

int receive_user = -1;
int receive_file = 0;


int file_pt;

void UART_Initialization();




#pragma interrupt clear_ignore ipl6 vector 12
void clear_ignore(void)
{
    T3CONCLR = 0x8000;
    TMR3 = 0;
    IFS0bits.T3IF = 0;
}

#endif

