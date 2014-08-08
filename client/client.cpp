#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include "json/json.h"
#include "../include/C_R.h"
#include "../include/jsonHelper.h"
#include "../include/msg_types.h"
#include <fstream>
#include "msgcom.h"
#include "client_console.h"


vector <info_base *> infos;
jsonhelper Jhelp;

int main(int argc, char** argv)
{
    for(int i = 0; i < 10; i++)
    {
        info_base *info = new info_base;
        char temp[50];
        sprintf(temp, "%d", i);
        string temp1 = temp;
        info->software_id = "demo";
        info->user = "zqu" + temp1;
        info->pw = "zqu" + temp1;
        infos.push_back(info);
    }
    pthread_t id;
    int ret;
    ret=pthread_create(&id,NULL,pthread_client_console,NULL);
    if(ret!=0){
    printf ("Create pthread error!\n");
    exit (1);
    }
    while(1);
    return 0;
}
