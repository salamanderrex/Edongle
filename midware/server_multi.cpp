#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
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
#include "usb.h"
#include "S-AES.h"
#define MAXBUF 1024
#define MAX_DATABUF 4096
extern "C"
{
#include "Handshake.h"
#include "Communicate.h"
#include "Errors.h"
}
#define PORT 4567

ws_list *l;
int port;
vector <info_base *> infos;
jsonhelper Jhelp;

/**
 * Handler to call when CTRL+C is typed. This function shuts down the server
 * in a safe way.
 */
void sigint_handler(int sig) {
    if (sig == SIGINT || sig == SIGSEGV) {
        if (l != NULL) {
            list_free(l);
            l = NULL;
        }
        (void) signal(sig, SIG_DFL);
        exit(0);
    } else if (sig == SIGPIPE) {
        (void) signal(sig, SIG_IGN);
    }
}

/**
 * Shuts down a client in a safe way. This is only used for Hybi-00.
 */
void cleanup_client(void *args) {
    ws_client *n = (ws_client *)args;
    if (n != NULL) {
        printf("Shutting client down..\n\n> ");
        fflush(stdout);
        list_remove(l, n);
    }
}

/**
 * This function listens for input from STDIN and tries to match it to a
 * pattern that will trigger different actions.
 */
void *cmdline(void *arg) {
    pthread_detach(pthread_self());
    (void) arg; char buffer[1024];

    while (1) {
        memset(buffer, '\0', 1024);
        printf("> ");
        fflush(stdout);
        fgets(buffer, 1024, stdin);

        if (strncasecmp(buffer, "users", 5) == 0 ||
                strncasecmp(buffer, "online", 6) == 0 ||
                strncasecmp(buffer, "clients", 7) == 0) {
            list_print(l);
            continue;
        } else if (strncasecmp(buffer, "exit", 4) == 0 ||
                strncasecmp(buffer, "quit", 4) == 0) {
            raise(SIGINT);
            break;
        } else if ( strncasecmp(buffer, "help", 4) == 0 ) {
            printf("------------------------ HELP ------------------------\n");
            printf("|   To display information about the online users,   |\n");
            printf("|   type: 'users', 'online', or 'clients'.           |\n");
            printf("|                                                    |\n");
            printf("|   To send a message to a specific user from the    |\n");
            printf("|   server type: 'send <IP> <SOCKET> <MESSAGE>' or   |\n");
            printf("|   'write <IP> <SOCKET> <MESSAGE>'.                 |\n");
            printf("|                                                    |\n");
            printf("|   To send a message to all users from the server   |\n");
            printf("|   type: 'sendall <MESSAGE>' or 'writeall           |\n");
            printf("|   <MESSAGE>'.                                      |\n");
            printf("|                                                    |\n");
            printf("|   To kick a user from the server and close the     |\n");
            printf("|   socket connection type: 'kick <IP> <SOCKET>'     |\n");
            printf("|   or 'close <IP> <SOCKET>'.                        |\n");
            printf("|                                                    |\n");
            printf("|   To kick all users from the server and close      |\n");
            printf("|   all socket connections type: 'kickall' or        |\n");
            printf("|   'closeall'.                                      |\n");
            printf("|                                                    |\n");
            printf("|   To quit the server type: 'quit' or 'exit'.       |\n");
            printf("------------------------------------------------------\n");
            fflush(stdout);
            continue;
        } else if ( strncasecmp(buffer, "kickall", 7) == 0 ||
                strncasecmp(buffer, "closeall", 8) == 0) {
            list_remove_all(l);
        } else if ( strncasecmp(buffer, "kick", 4) == 0 ||
                strncasecmp(buffer, "close", 5) == 0) {
            char *token = strtok(buffer, " "), *addr, *sock;

            if (token != NULL) {
                token = strtok(NULL, " ");

                if (token == NULL) {
                    printf("The command was executed without parameters. Type "
                           "'help' to see how to execute the command properly."
                           "\n");
                    fflush(stdout);
                    continue;
                } else {
                    addr = token;
                }

                token = strtok(NULL, "");

                if (token == NULL) {
                    printf("The command was executed with too few parameters. Type "
                           "'help' to see how to execute the command properly."
                           "\n");
                    fflush(stdout);
                    continue;
                } else {
                    sock = token;
                }

                ws_client *n = list_get(l, addr,
                        strtol(sock, (char **) NULL, 10));

                if (n == NULL) {
                    printf("The client that was supposed to receive the "
                           "message, was not found in the userlist.\n");
                    fflush(stdout);
                    continue;
                }

                ws_closeframe(n, CLOSE_SHUTDOWN);
            }
        } else if ( strncasecmp(buffer, "sendall", 7) == 0 ||
               strncasecmp(buffer, "writeall", 8) == 0) {
            char *token = strtok(buffer, " ");
            ws_connection_close status;

            if (token != NULL) {
                token = strtok(NULL, "");

                if (token == NULL) {
                    printf("The command was executed without parameters. Type "
                           "'help' to see how to execute the command properly."
                           "\n");
                    fflush(stdout);
                    continue;
                } else {
                    ws_message *m = message_new();
                    m->len = strlen(token);

                    char *temp = (char *)malloc( sizeof(char)*(m->len+1) );
                    if (temp == NULL) {
                        raise(SIGINT);
                        break;
                    }
                    memset(temp, '\0', (m->len+1));
                    memcpy(temp, token, m->len);
                    m->msg = temp;
                    temp = NULL;

                    if ( (status = encodeMessage(m)) != CONTINUE) {
                        message_free(m);
                        free(m);
                        raise(SIGINT);
                        break;;
                    }

                    list_multicast_all(l, m);
                    message_free(m);
                    free(m);
                }
            }
        } else if ( strncasecmp(buffer, "send", 4) == 0 ||
                strncasecmp(buffer, "write", 5) == 0) {
            char *token = strtok(buffer, " "), *addr, *sock, *msg;
            ws_connection_close status;

            if (token != NULL) {
                token = strtok(NULL, " ");

                if (token == NULL) {
                    printf("The command was executed without parameters. Type "
                           "'help' to see how to execute the command properly."
                           "\n");
                    fflush(stdout);
                    continue;
                } else {
                    addr = token;
                }

                token = strtok(NULL, " ");

                if (token == NULL) {
                    printf("The command was executed with too few parameters. Type "
                           "'help' to see how to execute the command properly."
                           "\n");
                    fflush(stdout);
                    continue;
                } else {
                    sock = token;
                }

                token = strtok(NULL, "");

                if (token == NULL) {
                    printf("The command was executed with too few parameters. Type "
                           "'help' to see how to execute the command properly."
                           "\n");
                    fflush(stdout);
                    continue;
                } else {
                    msg = token;
                }

                ws_client *n = list_get(l, addr,
                        strtol(sock, (char **) NULL, 10));

                if (n == NULL) {
                    printf("The client that was supposed to receive the "
                           "message, was not found in the userlist.\n");
                    fflush(stdout);
                    continue;
                }

                ws_message *m = message_new();
                m->len = strlen(msg);

                char *temp = (char *)malloc( sizeof(char)*(m->len+1) );
                if (temp == NULL) {
                    raise(SIGINT);
                    break;
                }
                memset(temp, '\0', (m->len+1));
                memcpy(temp, msg, m->len);
                m->msg = temp;
                temp = NULL;

                if ( (status = encodeMessage(m)) != CONTINUE) {
                    message_free(m);
                    free(m);
                    raise(SIGINT);
                    break;;
                }

                list_multicast_one(l, n, m);
                message_free(m);
                free(m);
            }
        } else {
            printf("To see functions available type: 'help'.\n");
            fflush(stdout);
            continue;
        }
    }

    pthread_exit((void *) EXIT_SUCCESS);
}

void *handleClient(void *args) {
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_cleanup_push(&cleanup_client, args);

    int buffer_length = 0, string_length = 1, reads = 1;

    ws_client *n = (ws_client *)args;
    n->thread_id = pthread_self();

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    char buffer[BUFFERSIZE];
    n->string = (char *) malloc(sizeof(char));

    if (n->string == NULL) {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        handshake_error("Couldn't allocate memory.", ERROR_INTERNAL, n);
        pthread_exit((void *) EXIT_FAILURE);
    }

    printf("Client connected with the following information:\n"
           "\tSocket: %d\n"
           "\tAddress: %s\n\n", n->socket_id, (char *) n->client_ip);
    printf("Checking whether client is valid ...\n\n");
    fflush(stdout);

    /**
     * Getting headers and doing reallocation if headers is bigger than our
     * allocated memory.
     */
    do {
        memset(buffer, '\0', BUFFERSIZE);
        if ((buffer_length = recv(n->socket_id, buffer, BUFFERSIZE, 0)) <= 0){
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            handshake_error("Didn't receive any headers from the client.",
                    ERROR_BAD, n);
            pthread_exit((void *) EXIT_FAILURE);
        }

        if (reads == 1 && strlen(buffer) < 14) {
            handshake_error("SSL request is not supported yet.",
                    ERROR_NOT_IMPL, n);
            pthread_exit((void *) EXIT_FAILURE);
        }
        //print receive data from client

        printf("\n---------client send: %s;",buffer);
        string_length += buffer_length;

        char *tmp = (char *)realloc(n->string, string_length);
        if (tmp == NULL) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            handshake_error("Couldn't reallocate memory.", ERROR_INTERNAL, n);
            pthread_exit((void *) EXIT_FAILURE);
        }
        n->string = tmp;
        tmp = NULL;

        memset(n->string + (string_length-buffer_length-1), '\0',
                buffer_length+1);
        memcpy(n->string + (string_length-buffer_length-1), buffer,
                buffer_length);
        reads++;
    } while( strncmp("\r\n\r\n", n->string + (string_length-5), 4) != 0
            && strncmp("\n\n", n->string + (string_length-3), 2) != 0
            && strncmp("\r\n\r\n", n->string + (string_length-8-5), 4) != 0
            && strncmp("\n\n", n->string + (string_length-8-3), 2) != 0 );

    printf("User connected with the following headers:\n%s\n\n", n->string);
    fflush(stdout);

    ws_header *h = header_new();

    if (h == NULL) {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        handshake_error("Couldn't allocate memory.", ERROR_INTERNAL, n);
        pthread_exit((void *) EXIT_FAILURE);
    }

    n->headers = h;

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if ( parseHeaders(n->string, n, port) < 0 ) {
        pthread_exit((void *) EXIT_FAILURE);
    }

    if ( sendHandshake(n) < 0 && n->headers->type != UNKNOWN ) {
        pthread_exit((void *) EXIT_FAILURE);
    }

    list_add(l, n);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    printf("Client has been validated and is now connected\n\n");
    printf("> ");
    fflush(stdout);

    uint64_t next_len = 0;
    char next[BUFFERSIZE];
    memset(next, '\0', BUFFERSIZE);

    while (1) {
        if ( communicate(n, next, next_len) != CONTINUE) {
            break;
        }

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        if (n->headers->protocol == CHAT) {
            list_multicast(l, n);
        } else if (n->headers->protocol == ECHO_1) {
            printf("\n==== I am here to ECHO_1\n");
            list_multicast_one(l, n, n->message);
        } else {

            printf("======client sends me %s",n->message->msg);
            printf("\n====I am here to RESPONSE\n");
            //=================================================
           string s_m =  Jhelp.parse_msg(n->message->msg);

                ws_message *mm = message_new();
                mm->len = strlen(s_m.c_str());

                char *ttemp = (char *)malloc( sizeof(char)*(mm->len+1) );
                if (ttemp == NULL) {
                    raise(SIGINT);
                    break;
                }
                memset(ttemp, '\0', (mm->len+1));
                memcpy(ttemp, s_m.c_str(), mm->len);
                mm->msg = ttemp;
                ttemp = NULL;
                ws_connection_close status;
                if ( (status = encodeMessage(mm)) != CONTINUE) {
                    message_free(mm);
                    free(mm);
                    raise(SIGINT);
                    break;;
                }

                printf("%s",mm->msg);
                list_multicast_one(l, n, mm);
                message_free(mm);
                free(mm);
                printf("--------send finish\n");

        //	ws_message * mm=message_new();
        //	mm->len=strlen(s_m);
        //	char ttemp= malloc (sizeof(char)* mm->len
        //	encodeMessage(n->message);
            //list_multicast_one(l, n,n->message);

            //==================================================
        }
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

        if (n->message != NULL) {
            memset(next, '\0', BUFFERSIZE);
            memcpy(next, n->message->next, n->message->next_len);
            next_len = n->message->next_len;
            message_free(n->message);
            free(n->message);
            n->message = NULL;
        }
    }

    printf("Shutting client down..\n\n");
    printf("> ");
    fflush(stdout);

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    list_remove(l, n);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    pthread_cleanup_pop(0);
    pthread_exit((void *) EXIT_SUCCESS);
}

typedef struct CLIENT {
    int fd;
    struct sockaddr_in addr;
}CLIENT;

/***************************
**server for multi-client
**PF_SETSIZE=1024
****************************/
int operation = 0;
FILE *source_file;
FILE *dest_file;


int main(int argc, char** argv)
{
    //*******************************************
    //only for test

    info_base * info = new info_base;
    info->software_id = "chrome";
    info->user = "ff";              //your id
    info->pw = "pwpwpwppwpw";           //your password
    info->web = "http://umji.sjtu.edu.cn";
    info->flag = 0;
    infos.push_back(info);

    for(int i = 0; i < 5; i++)
    {
        info_base *info1 = new info_base;
        char temp[50];
        sprintf(temp, "%d", i);
        string temp1 = temp;
        info1->software_id = "demo";
        info1->user = "zqu" + temp1;
        info1->pw = "zqu" + temp1;
        info1->flag = 0;
        infos.push_back(info1);
    }
    //*******************************************

    if(argc == 4)
        {
            char c = *argv[3];
            operation = c-48;
            source_file = fopen(argv[1],"rb");
            if(source_file == NULL)
                cout<<"error opening source file"<<endl;
            dest_file = fopen(argv[2],"wb");
            if(dest_file == NULL)
                cout<<"error creating destination file"<<endl;
        }
        else
        {
            operation = 0;
        }

    pthread_t id, id1,id_usb;
    int ret, ret1,ret_usb;
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
    ret_usb=pthread_create(&id_usb,NULL,pthread_usb,NULL);
    if(ret_usb!=0){
    printf ("Create pthread error!\n");
    exit (1);
    }
    int server_socket, client_socket, on = 1;

        struct sockaddr_in server_addr, client_addr;
        socklen_t client_length;
        pthread_t pthread_id;
        pthread_attr_t pthread_attr;

        /**
         * Creating new lists, l is supposed to contain the connected users.
         */
        l = list_new();

        /**
         * Listens for CTRL-C and Segmentation faults.
         */
        (void) signal(SIGINT, &sigint_handler);
        (void) signal(SIGSEGV, &sigint_handler);
        (void) signal(SIGPIPE, &sigint_handler);


        printf("Server: \t\tStarted\n");
        fflush(stdout);

        /**
         * Assigning port value.
         */
        if (argc == 2) {
            port = strtol(argv[1], (char **) NULL, 10);

            if (port <= 1024 || port >= 65565) {
                port = PORT;
            }

        } else {
            port = PORT;
        }

        printf("Port: \t\t\t%d\n", port);
        fflush(stdout);

        /**
         * Opening server socket.
         */
        if ( (server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
            server_error(strerror(errno), server_socket, l);
        }

        printf("Socket: \t\tInitialized\n");
        fflush(stdout);

        /**
         * Allow reuse of address, when the server shuts down.
         */
        if ( (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on,
                        sizeof(on))) < 0 ){
            server_error(strerror(errno), server_socket, l);
        }

        printf("Reuse Port %d: \tEnabled\n", port);
        fflush(stdout);

        memset((char *) &server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);

        printf("Ip Address: \t\t%s\n", inet_ntoa(server_addr.sin_addr));
        fflush(stdout);

        /**
         * Bind address.
         */
        if ( (bind(server_socket, (struct sockaddr *) &server_addr,
                sizeof(server_addr))) < 0 ) {
            server_error(strerror(errno), server_socket, l);
        }

        printf("Binding: \t\tSuccess\n");
        fflush(stdout);

        /**
         * Listen on the server socket for connections
         */
        if ( (listen(server_socket, 10)) < 0) {
            server_error(strerror(errno), server_socket, l);
        }

        printf("Listen: \t\tSuccess\n\n");
        fflush(stdout);

        /**
         * Attributes for the threads we will create when a new client connects.
         */
        pthread_attr_init(&pthread_attr);
        pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
        pthread_attr_setstacksize(&pthread_attr, 524288);

        printf("Server is now waiting for clients to connect ...\n\n");
        fflush(stdout);

        /**
         * Create commandline, such that we can do simple commands on the server.
         */
        /*
        if ( (pthread_create(&pthread_id, &pthread_attr, cmdline, NULL)) < 0 ){
            server_error(strerror(errno), server_socket, l);
        }
        */
        /**
         * Do not wait for the thread to terminate.
         */
        //pthread_detach(pthread_id);

        while (1) {
            client_length = sizeof(client_addr);

            /**
             * If a client connects, we observe it here.
             */
            if ( (client_socket = accept(server_socket,
                    (struct sockaddr *) &client_addr,
                    &client_length)) < 0) {
                server_error(strerror(errno), server_socket, l);
            }

            /**
             * Save some information about the client, which we will
             * later use to identify him with.
             */
            char *temp = (char *) inet_ntoa(client_addr.sin_addr);
            char *addr = (char *) malloc( sizeof(char)*(strlen(temp)+1) );
            if (addr == NULL) {
                server_error(strerror(errno), server_socket, l);
                break;
            }
            memset(addr, '\0', strlen(temp)+1);
            memcpy(addr, temp, strlen(temp));

            ws_client *n = client_new(client_socket, addr);

            /**
             * Create client thread, which will take care of handshake and all
             * communication with the client.
             */
            if ( (pthread_create(&pthread_id, &pthread_attr, handleClient,
                            (void *) n)) < 0 ){
                server_error(strerror(errno), server_socket, l);
            }

            pthread_detach(pthread_id);
        }

        list_free(l);
        l = NULL;
        close(server_socket);
        pthread_attr_destroy(&pthread_attr);
        return EXIT_SUCCESS;
    }
