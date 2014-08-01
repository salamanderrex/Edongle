#include "msgcom.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

main()

{                                             /* 服务器进程 */

struct msgtype buf;

int qid;

if((qid=msgget(MSGKEY,IPC_CREAT|0666))== -1)       

return(-1);                                       /* 出错处理 */

while(1){

msgrcv(qid,&buf,512,1,MSG_NOERROR);             /* 接收所有请求进程发送的消息 */

printf("Server receive a request from process %s\n",buf.text);

buf.mtype=11;                                 /* 将请求进程的标识数作为mtype的值，以便于请求进程识别 */

 

 

strcpy(buf.text,"Hello client!!\n");

msgsnd(qid,&buf,sizeof(buf.text),IPC_NOWAIT|04000);                     /* 将消息发送给请求进程 */

 

}

}
