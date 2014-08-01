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

    pthread_t id;
    int ret;
    ret=pthread_create(&id,NULL,pthread_client_console,NULL);
    if(ret!=0){
    printf ("Create pthread error!\n");
    exit (1);
    }
    while(1);
    return 0;

    struct msgtype buf;
    char keyboard_buffer [MESSAGE_SIZE];
    int qid;
    qid=msgget(MSGKEY,IPC_CREAT|0666);               /* MSGKEY为约定的消息队列关键字,访问控制权限为0666 */
    buf.mtype=1;                                   /* 请求进程发送消息标识为1 */
    fgets(keyboard_buffer,MESSAGE_SIZE,stdin);
    int i;
    for(i=0;i<MESSAGE_SIZE;i++)
    {
        buf.text[i]=keyboard_buffer[i];
    }
    msgsnd(qid,&buf,sizeof(buf.text), IPC_NOWAIT|0666);   /* 发送消息正文长度为buf的大小 */
    msgrcv(qid,&buf,512,11,MSG_NOERROR);  /* 指定接收mtype=pid的信息，即请求进程发送给服务器处理后的信息 */
    printf("Request received a message from server, type is: %d\, msg is %sn",buf.mtype,buf.text);
    return 0;
}
