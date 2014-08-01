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

#endif // MSGCOM_H
