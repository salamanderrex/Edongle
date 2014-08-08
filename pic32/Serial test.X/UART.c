#include "UART.h"
#include "S-AES.h"

char *username;
char *password;
char received;
char file[100] = {0};
int i = 0;
char result[100] = {0};
BYTE Key[2] = {91,0};

#pragma interrupt RX_ISR ipl7 vector 24
void RX_ISR(void)
{
    received = U1ARXREG;
    PORTD = 2;
    if(received == 10)
    {
        i = 0;
        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -2;
        }
        TMR2 = 0;
        while(username[i] != NULL)
        {
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = username[i];
            }
            i++;
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = -2;
            }
        }
        DelayMsec(5000);

        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -3;
        }
        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -2;
        }
        i = 0;
        while(password[i] != NULL)
        {
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = password[i];
            }
            i++;
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = -2;
            }
        }
        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -3;
        }

    }

    else if(received == -7)
    {
        if(receive_user == -1)
            receive_user = 1;
        else if(receive_user == 1)
            receive_user = 0;
        else
            receive_user = -1;
        i = 0;
    }

    else if(received == -4)
    {
        receive_file = 1;
        file_pt = 0;
    }

    else if(received == -5)
    {
        receive_file = 0;
        file_pt = 0;
        //BYTE Key[2] = {'1','2'};
	BYTE RoundKey[6];
	Extend_Key(Key,RoundKey);
	while(file[file_pt] != NULL)
	{
		BYTE temp[2];
		BYTE temp_result[2];
		temp[0] = file[file_pt];
		temp[1] = file[file_pt+1];
		EncryptBlock(temp,RoundKey,temp_result);
		result[file_pt] = temp_result[0];
		result[file_pt+1] = temp_result[1];
		file_pt = file_pt + 2;
	}
        file_pt = 0;
        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -2;
        }
        while(result[file_pt] != NULL)
        {
            char to_send = result[file_pt];
            file_pt++;
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = to_send;
            }
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = -2;
            }
        }
        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -3;
        }
        file_pt = 0;
    }

    else if(received == -6)
    {
        receive_file = 0;
        file_pt = 0;
        //BYTE Key[2] = {'1','2'};
	BYTE RoundKey[6];
	Extend_Key(Key,RoundKey);
	while(file[file_pt] != NULL)
	{
		BYTE temp[2];
		BYTE temp_result[2];
		temp_result[0] = file[file_pt];
		temp_result[1] = file[file_pt+1];
		DecryptBlock(temp_result,RoundKey,temp);
		file[file_pt] = temp[0];
		file[file_pt+1] = temp[1];
		file_pt = file_pt + 2;
	}
        file_pt = 0;
        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -2;
        }
        while(file[file_pt] != NULL)
        {
            char to_send = file[file_pt];
            file_pt++;
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = to_send;
            }
            TMR2 = 0;
            while(TMR2 < 400)
            {
                U1ATXREG = -2;
            }
        }
        TMR2 = 0;
        while(TMR2 < 400)
        {
            U1ATXREG = -3;
        }
        file_pt = 0;
    }

    if(receive_file == 1 && received != -4 && received != -5)
    {
        file[file_pt] = received;
        file_pt ++;
    }

    if(receive_user == 1 && received != -7)
    {
        username[i] = received;
        i++;
    }

    else if(receive_user == 0 && received != -7)
    {
        password[i] = received;
        i++;
    }
    PORTDINV = 2;
    IFS0bits.U1RXIF = 0;
}


void UART_Initialization()
{
    username = (char*)malloc(sizeof(char)*50);
    password = (char*)malloc(sizeof(char)*50);
    int i;
    IPC6SET = 0x40000; //Set priority level = 1;
    IFS1CLR = 0x0001;
    IEC1SET = 0x0001;

    U1ABRG = 207;  //Configure baud rate to be 4800
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

    for(i = 0; i < 50; i++)
    {
        username[i] = 0;
    }

    for(i = 0; i < 50; i++)
    {
        password[i] = 0;
    }


    //f = fopen("wenjian","wb");
} 