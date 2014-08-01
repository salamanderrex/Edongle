#include "msgcom.h"
#include <stdio.h>
#include <stdlib.h>

main()

{                                                      /* 请求进程 */

struct msgtype buf;

char keyboard_buffer [MESSAGE_SIZE];

int qid,pid;

qid=msgget(MSGKEY,IPC_CREAT|0666);               /* MSGKEY为约定的消息队列关键字,访问控制权限为0666 */

buf.mtype=1;                                   /* 请求进程发送消息标识为1 */
//buf.id=pid=getpid();                   /* 请求进程发送消息内容为进程标识 */

 

fgets(keyboard_buffer,MESSAGE_SIZE,stdin);

int i;

for(i=0;i<MESSAGE_SIZE;i++)

{

buf.text[i]=keyboard_buffer[i];


}

//printf("\n%s",keyboard_buffer);

msgsnd(qid,&buf,sizeof(buf.text), IPC_NOWAIT|0666);   /* 发送消息正文长度为buf的大小 */

msgrcv(qid,&buf,512,11,MSG_NOERROR);  /* 指定接收mtype=pid的信息，即请求进程发送给服务器处理后的信息 */

printf("Request received a massags from server, type is: %d\, msg is %sn",buf.mtype,buf.text);

return 0;

}
