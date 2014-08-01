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
#include "server_console.h"
#include <fstream>
#include "msgcom.h"
#define MAXBUF 1024
#define MAX_DATABUF 4096

vector <info_base *> infos;
jsonhelper Jhelp;



typedef struct CLIENT {
    int fd;
    struct sockaddr_in addr;
}CLIENT;

/***************************
**server for multi-client
**PF_SETSIZE=1024
****************************/
int main(int argc, char** argv)
{
    //*******************************************
    //only for test
    info_base * info = new info_base;
    info->software_id = "chrome";
    info->user = "user1";
    info->pw = "passwordofuser1";
    info->web = "http://www.baidu.com";
    infos.push_back(info);
    //*******************************************

    pthread_t id, id1;
    int ret, ret1;
    ret=pthread_create(&id,NULL,pthread_server_console,NULL);
    if(ret!=0){
    printf ("Create pthread error!\n");
    exit (1);
    }
    ret1=pthread_create(&id1,NULL,pthread_msgcom,NULL);
    if(ret1!=0){
    printf ("Create pthread error!\n");
    exit (1);
    }
    int i,n,maxi = -1;
    int nready;
    int slisten,sockfd,maxfd=-1,connectfd;

    unsigned int myport,lisnum;

    struct sockaddr_in  my_addr,addr;
    struct timeval tv;

    socklen_t len;
    fd_set rset,allset;

    char buf[MAXBUF + 1];
    char res_buf[MAXBUF + 1];
    CLIENT client[FD_SETSIZE];

    if(argv[1])
        myport = atoi(argv[1]);
    else
        myport = 1234;

    if(argv[2])
        lisnum = atoi(argv[2]);
    else
        lisnum = FD_SETSIZE;

    if((slisten = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    //set socket NO TIME_WAIT
    int on=1;
    if((setsockopt(slisten,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)

    {
        perror("setsocketopt fail");
        std::cout<<std::endl;
        exit(1);
    }

    bzero(&my_addr,sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(myport);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(slisten, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(slisten, lisnum) == -1) {
        perror("listen");
        exit(1);
    }

    for(i=0;i<FD_SETSIZE;i++)
    {
        client[i].fd = -1;
    }

    FD_ZERO(&allset);
    FD_SET(slisten, &allset);
    maxfd = slisten;

    printf("Waiting for connections and data...\n");
    while (1)
    {
        rset = allset;

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        nready = select(maxfd + 1, &rset, NULL, NULL, &tv);

        if(nready == 0)
            continue;
        else if(nready < 0)
        {
            printf("select failed!\n");
            break;
        }
        else
        {
            if(FD_ISSET(slisten,&rset)) // new connection
            {
                len = sizeof(struct sockaddr);
                if((connectfd = accept(slisten,
                                       (struct sockaddr*)&addr,&len)) == -1)
                {
                    perror("accept() error\n");
                    continue;
                }
                for(i=0;i<FD_SETSIZE;i++)
                {
                    if(client[i].fd < 0)
                    {
                        client[i].fd = connectfd;
                        client[i].addr = addr;
                        printf("You've got a connection from %s.\n",
                               inet_ntoa(client[i].addr.sin_addr));
                        break;
                    }
                }
                if(i == FD_SETSIZE)
                    printf("too many connections");
                FD_SET(connectfd,&allset);
                if(connectfd > maxfd)
                    maxfd = connectfd;
                if(i > maxi)
                    maxi = i;
            }
            else
            {
                for(i=0;i<=maxi;i++)
                {
                    if((sockfd = client[i].fd)<0)
                        continue;
                    if(FD_ISSET(sockfd,&rset))
                    {
                        bzero(buf,MAXBUF + 1);

                        if((n = recv(sockfd,buf,MAXBUF,0)) > 0)
                        {

                            printf("received data:%s\n from %s\n",buf,inet_ntoa(client[i].addr.sin_addr));

                            std::cout<<endl;

                            //   while(1);
                            //detect the start of the instruction
                            string instruction=buf;
                            std::cout<<"finish receive instruction from client"<<std::endl;

                            //analyze the request instrucion
                            //initial the jsoncpp engine
                            strcpy(res_buf, (Jhelp.parse_msg(instruction)).c_str());
                            if(res_buf == "fail") cout << "finding info  failed!\n";
                            else
                            {
                                send(sockfd, res_buf, strlen(res_buf)+1 , 0);
                            }
                            memset(res_buf,NULL,sizeof(res_buf));
                        }
                        else
                        {
                            printf("disconnected by client!\n");
                            close(sockfd);
                            FD_CLR(sockfd,&allset);
                            client[i].fd = -1;
                        }
                    }
                }
            }
        }
    }
    close(slisten);
}
