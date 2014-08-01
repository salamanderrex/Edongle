#ifndef MSGCOM_H
#define MSGCOM_H
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include "jsonHelper.h"

#define MSGKEY 5113709262
#define MESSAGE_SIZE 1024
#define SERVER_RESPONSE_TYPE

extern jsonhelper Jhelp;

struct msgtype{
    long mtype;
    char text [MESSAGE_SIZE];
    int id;
};

void  *pthread_msgcom(void *ptr)
{
    struct msgtype buf;
    int qid;
    qid = msgget(MSGKEY,IPC_CREAT|0666);
    while(1){
        msgrcv(qid,&buf,512,1,MSG_NOERROR);             /* 接收所有请求进程发送的消息 */
        printf("Server receive a request from process %s\n",buf.text);
        buf.mtype=11;                                 /* 将请求进程的标识数作为mtype的值，以便于请求进程识别 */
        string tempstring = buf.text;
        strcpy(buf.text,  (Jhelp.parse_msg(tempstring)).c_str());
        msgsnd(qid,&buf,sizeof(buf.text),IPC_NOWAIT|04000);                     /* 将消息发送给请求进程 */
    }
}
#endif // MSGCOM_H
