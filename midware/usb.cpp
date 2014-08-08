#include <iostream>
#include <stdio.h>
#include "usb.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

extern "C"
{
#include "S-AES.h"
}

using namespace std;


#define FALSE  -1
#define TRUE   0

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,
                            19200,  9600, 4800, 2400, 1200,  300, };
int unlock_flag = 0;


extern int operation;
extern FILE *source_file;
extern FILE *dest_file;

void set_speed(int fd, int speed){
         int   i;
         int   status;
         struct termios   Opt;
         tcgetattr(fd, &Opt);
         for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {
                   if  (speed == name_arr[i]) {
                            tcflush(fd, TCIOFLUSH);
                            cfsetispeed(&Opt, speed_arr[i]);
                            cfsetospeed(&Opt, speed_arr[i]);
                            status = tcsetattr(fd, TCSANOW, &Opt);
                            if  (status != 0) {
                                     perror("tcsetattr fd");
                                     return;
                            }
                            tcflush(fd,TCIOFLUSH);
                   }
         }
}

int set_Parity(int fd,int databits,int stopbits,int parity)
{
         struct termios options;
         if  ( tcgetattr( fd,&options)  !=  0) {
                   perror("SetupSerial 1");
                   return(FALSE);
         }
         options.c_cflag &= ~CSIZE;
         options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
         options.c_oflag  &= ~OPOST;   /*Output*/

         switch (databits)
         {
         case 7:
                   options.c_cflag |= CS7;
                   break;
         case 8:
                   options.c_cflag |= CS8;
                   break;
         default:
                   fprintf(stderr,"Unsupported data size/n"); return (FALSE);
         }
switch (parity)
{
         case 'n':
         case 'N':
                   options.c_cflag &= ~PARENB;   /* Clear parity enable */
                   options.c_iflag &= ~INPCK;     /* Enable parity checking */
                   break;
         case 'o':
         case 'O':
                   options.c_cflag |= (PARODD | PARENB);
                   options.c_iflag |= INPCK;             /* Disnable parity checking */
                   break;
         case 'e':
         case 'E':
                   options.c_cflag |= PARENB;
                   options.c_cflag &= ~PARODD;
                   options.c_iflag |= INPCK;
                   break;
         case 'S':
         case 's':  /*as no parity*/
             options.c_cflag &= ~PARENB;
                   options.c_cflag &= ~CSTOPB;break;
         default:
                   fprintf(stderr,"Unsupported parity/n");
                   return (FALSE);
         }
/* Set stop bits*/
switch (stopbits)
{
         case 1:
                   options.c_cflag &= ~CSTOPB;
                   break;
         case 2:
                   options.c_cflag |= CSTOPB;
            break;
         default:
                    fprintf(stderr,"Unsupported stop bits/n");
                    return (FALSE);
}
/* Set input parity option */
if (parity != 'n')
         options.c_iflag |= INPCK;
tcflush(fd,TCIFLUSH);
options.c_cc[VTIME] = 0;
options.c_cc[VMIN] = 13;
if (tcsetattr(fd,TCSANOW,&options) != 0)
{
         perror("SetupSerial 3");
         return (FALSE);
}
return (TRUE);
}

int OpenDev(char *Dev)
{
         int     fd = open( Dev, O_RDWR );
        //| O_NOCTTY | O_NDELAY
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return -1;
    }
    else
    return fd;
}

void run_usb()
{
    int fd;
    int write_flag = 0;
    int end_flag = 0;
    int twelve_flag = 0;
    int next_flag = 0;
    int receive_flag = 0;
    int former_flag = 0;
    int op_num = 0;
    int now_reading = 0;
    int request_signal_sent = 0;
    int file_request_sent = 0;

    bool username_flag = true;

    const BYTE Key[2] = {1};

    char request_signal = 10;
    char send_start_signal = -7;
    char send_end_flag = -7;
    char file_send_signal = -4;
    char encrypted_request_signal = -5;
    char plain_request_signal = -6;


    char tempbuff;
    char result[50] = {0};
    char file[100];
    char *dev  = "/dev/ttyUSB0";
    fd = OpenDev(dev);
    set_speed(fd,4800);
    if (set_Parity(fd,8,1,'N') == FALSE)
    {
        fprintf(stderr,"%s","Set Parity Error");
    }

    int ip = 0;
    if(operation != 0)
    {
        while(!feof(source_file))
        {
            file[ip] = fgetc(source_file);
            ip++;
        }
        fclose(source_file);
    }

        while (1)
        {
            //printf("%d\n",operation);
            if(operation == 1)
            {
                receive_flag = 1;
                write_flag = 1;
            }
            else if(operation == 2)
            {
                receive_flag = 1;
                write_flag = 1;
            }
            else if(operation == 0)
            {
                for(int i = 0; i < infos.size(); i++)
                {
                    //printf("The size of infos is %d\n",infos.size());
                    //sleep(1000);
                    switch(infos[i]->flag)
                    {
                    case -1:
                        receive_flag = 0;
                        write_flag = 1;
                        op_num = i;
                        break;
                    case -2:
                        if(now_reading != 1)
                        {
                            receive_flag = 1;
                            write_flag = 0;
                            op_num = i;
                            infos[i]->user.clear();
                            infos[i]->pw.clear();
                            now_reading = 1;
                            request_signal_sent = 0;
                        }
                        break;
                    }
                }
            }
            if(write_flag == 1 && unlock_flag == 1)
            {
                if(operation == 1 )
                {
                    write(fd, &file_send_signal, sizeof(char));
                    int ip = 0;
                    while(file[ip] != NULL)
                    {
                        write(fd, &file[ip], sizeof(char));
                        ip++;
                    }
                    write(fd, &encrypted_request_signal, sizeof(char));
                }
                else if(operation == 2)
                {
                    write(fd, &file_send_signal, sizeof(char));
                    int ip = 0;
                    while(file[ip] != NULL)
                    {
                        write(fd, &file[ip], sizeof(char));
                        ip++;
                    }
                    write(fd, &plain_request_signal, sizeof(char));
                }
                else if(operation == 0)
                {
                    //cout<<"here starts sending"<<endl;
                    //cout<<infos.at(op_num)->user<<endl;
                    //cout<<infos.at(op_num)->pw<<endl;
                    //infos.at(op_num)->user_length = infos.at(op_num)->user.length();
                    //infos.at(op_num)->pw_length = infos.at(op_num)->pw.length();

                    write(fd, &send_start_signal, sizeof(char));
                    for(int i = 0; i < infos[op_num]->user.length(); i++)
                    {
                        char n = infos[op_num]-> user[i];
                        write(fd, &n, sizeof(char));
                        usleep(50*1000);
                    }
                    write(fd, &send_start_signal, sizeof(char));
                    for(int i = 0; i < infos[op_num]->pw.length(); i++)
                    {
                        char n = infos[op_num]-> pw[i];
                        write(fd, &n, sizeof(char));
                        usleep(50*1000);
                    }
                    write(fd, &send_end_flag, sizeof(char));
                    infos[op_num]->flag = 0;
                    write_flag = 0;
                }
            }
            if(receive_flag == 1 || unlock_flag == 0)
            {
                if(unlock_flag == 0)
                {
                    if(read(fd, &tempbuff, sizeof(char))>0)
                    {
                        //printf("%c\n",tempbuff);
                        if(tempbuff == 11)
                        {
                            unlock_flag = 1;
                            printf("%s\n","unlock success");
                        }
                    }
                }
                if(unlock_flag == 1 && receive_flag == 1)
                {
                    if(request_signal_sent == 0 && operation == 0)
                    {
                        write(fd, &request_signal, sizeof(char));  //send 10 to usb requiring username and password
                        request_signal_sent = 1;
                        //printf("%s\n","Sending too many 10?");
                    }
                    if(read(fd, &tempbuff, sizeof(char))>0)
                    {
                       //printf("%c\n",tempbuff);
                    }
                    switch(tempbuff)
                    {
                    case 11:
                        unlock_flag = 1;
                    break;
                    case -2:
                        if(twelve_flag == 0)
                        {
                            //printf("%s\n","got -2 here");
                            next_flag = next_flag+1;
                            end_flag = 0;
                            twelve_flag = 1;
                        }
                    break;
                    case -3:
                        if(end_flag != 1)
                        {
                            //printf("%s\n","got -3 here");
                            end_flag = 1;
                            next_flag = 0;
                            twelve_flag = 0;
                            username_flag = !username_flag;
                            if(username_flag == true)
                            {
                                //printf("%s\n","not here?");
                                infos[op_num]->flag = 0;
                                now_reading = 0;
                            }
                        }
                    break;
                    default:
                        if(next_flag != 0 && end_flag != 1 && next_flag != former_flag && tempbuff != '\n')
                        {
                            if(operation == 0)
                            {
                                result[next_flag] = tempbuff;
                                //printf("%c\n",tempbuff);
                                if(username_flag == true)
                                {
                                    infos[op_num]->user+= tempbuff;
                                }
                                else
                                    infos[op_num]->pw += tempbuff;
                            }
                            else
                            {
                                //printf("%c\n",tempbuff);
                                if(operation == 2)
                                {
                                    if(tempbuff >= 0x20 && tempbuff <= 0x7F)
                                    fputc(tempbuff, dest_file);
                                }
                                else
                                {
                                    if(tempbuff != -1)
                                    fputc(tempbuff, dest_file);
                                }
                            }
                        }
                        twelve_flag = 0;
                        former_flag = next_flag;
                    }
                }
            }
        }
}


void *pthread_usb(void *ptr)
{
    run_usb();
}
