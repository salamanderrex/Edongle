#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSGKEY 5113709257
#define MESSAGE_SIZE 1024
#define SERVER_RESPONSE_TYPE
struct msgtype{
	long mtype;
	char text [MESSAGE_SIZE];
	int id;
};