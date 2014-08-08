#ifndef CLIENT_CONSOLE_H
#define CLIENT_CONSOLE_H

#include <fstream>
#include <termios.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <C_R.h>
#include "msgcom.h"

using namespace std;
struct termios orig_termios;
extern vector <info_base *> infos;
extern jsonhelper Jhelp;


void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

char  getch()
{
    int r;
    char   c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

int  getchs(char * input)
{
    int n=0;
    while(1){
        input[n]=getch();   // 读入1个字符
        if (input[n]=='\n')
        {
            //printf("break\n");
            break;  // 如果是换行符，则结束循环
        }
        n++;
    }
    input[n]='\0';
}
char * get_local_ipaddress()
{

    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

void hexPrinter( unsigned char* c, int l, FILE* torrent )
{
    while( l > 0 )
    {
        fprintf( torrent, "%02x ", *c );
        l--;
        c++;
    }
    fprintf(torrent, "\n");
}

void print_menu(string username)
{
    cout<<"====================================="<<endl<<
          "torrent main panel"<<endl<<
           "====================================="<<endl<<
           "hello user : "<< username << endl<<
           "1. check current info list" << endl <<
           "2. add info to info list" << endl <<
           "3. request for password" << endl <<
           "4. store the user and password" << endl <<
           "5. exit"<<endl<<
           "====================================="<<endl;
}

void  *pthread_client_console(void *ptr)
{
    string username;
    cout << "please login:" << endl;
    cin >> username;
    //print_menu(username);
    struct msgtype buf;
    int qid;
    qid=msgget(MSGKEY,IPC_CREAT|0666);
    buf.mtype=1;
    info_base *info = new info_base;
    info->software_id = "demo";
    info->user = username;
    strcpy(buf.text, (Jhelp.generate_msg(REQUEST_INFO, info)).c_str());
    msgsnd(qid,&buf,sizeof(buf.text), IPC_NOWAIT|0666);
    msgrcv(qid,&buf,512,11,MSG_NOERROR);
    //printf("Request received a message from server, type is: %d, msg is %s\n",buf.mtype,buf.text);
    int i, j = 0;
    if(buf.text != "")
    {
        string tempstring = buf.text;
        Json::Value jroot;
        Json::Reader jreader;
        if(!jreader.parse(tempstring,jroot,false))
        {
            perror("json reader");
            exit(-1);
        }
        Json::Value parameters=jroot["parameters"];
        info->pw = parameters[j]["pw"].asString();
    }
    for(i = 0; i < infos.size(); i++)
    {
        if(info->software_id == infos.at(i)->software_id && info->user == infos.at(i)->user && info->pw == infos.at(i)->pw) break;
    }
    if(i < infos.size())
    {
        cout << "Login successful!" << endl << "Hello, user " << username << endl;
    }
    else while(1)
    {
        cout << "Login failed" << endl << "Please input the password" << endl;
        cin >> info->pw;
        for(i = 0; i < infos.size(); i++)
        {
            if(info->software_id == infos.at(i)->software_id && info->user == infos.at(i)->user && info->pw == infos.at(i)->pw) break;
        }
        if(i < infos.size()){
             cout << "Login successful!" << endl << "Hello, user " << username << endl;
             break;
        }
    }
    while(1)
    {

    // set_conio_terminal_mode();




        while (!kbhit())
        {
            /* do some work */
        }


        char instruction_id=getch();
      //  getch();
        /*
        if(instruction_id == '1')
        {
            cout << endl <<   "*************************************" << endl <<"user id\t" << "user name\t" << "user password" << endl;
            for(int i = 0; i < infos.size(); i++)
            {
                cout << i << "\t" << infos.at(i)->user << "\t" << infos.at(i)->pw << endl;
            }
            cout << "*************************************" << endl << endl;
            print_menu(username);
        }
        else if(instruction_id == '2')
        {
            info_base * info = new info_base;
            int havePassword;
            cout << "please give the pid" << endl;
            cin >> info->pid;
            cout << "please give the software id" << endl;
            cin >> info->software_id;
            cout << "please give the user name" << endl;
            cin >> info->user;
            cout << "do you know the password? (0 for unknown, 1 for known)" << endl;
            cin >> havePassword;
            if(havePassword == 1)
            {
                cout << "please give the password" << endl;
                cin >> info->pw;
            }
            cout <<"thank you!" << endl << endl;
            infos.push_back(info);
            print_menu(username);
        }
        else if(instruction_id == '3')
        {
            int id;
            cout << "please give the info id" << endl;
            cin >> id;
            if(id >= infos.size())
            {
                cout << "invalid id" << endl;
                continue;
            }
            strcpy(buf.text, (Jhelp.generate_msg(REQUEST_INFO, infos.at(id))).c_str());
            msgsnd(qid,&buf,sizeof(buf.text), IPC_NOWAIT|0666);
            msgrcv(qid,&buf,512,11,MSG_NOERROR);
            printf("Request received a message from server, type is: %d\, msg is %sn",buf.mtype,buf.text);
            string tempstring = buf.text;
            Json::Value jroot;
            Json::Reader jreader;
            if(!jreader.parse(tempstring,jroot,false))
            {
                perror("json reader");
                exit(-1);
            }
            Json::Value parameters=jroot["parameters"];
            int j = 0;
            infos.at(id)->pw = parameters[j]["pw"].asString();
            print_menu(username);
        }
        else if(instruction_id == '4')
        {
            int id;
            cout << "please give the info id" << endl;
            cin >> id;
            if(id >= infos.size())
            {
                cout << "invalid id" << endl;
                continue;
            }
            strcpy(buf.text, (Jhelp.generate_msg(REQUEST_STORE, infos.at(id))).c_str());
            msgsnd(qid,&buf,sizeof(buf.text), IPC_NOWAIT|0666);
            msgrcv(qid,&buf,512,11,MSG_NOERROR);
            printf("Request received a message from server, type is: %d\, msg is %sn",buf.mtype,buf.text);
        }
        else if(instruction_id == '5')
        {
            break;
        }

        else;*/
    }


}


#endif // CLIENT_CONSOLE_H
