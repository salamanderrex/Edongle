#ifndef SERVER_CONSOLE_H
#define SERVER_CONSOLE_H

#include <fstream>
#include <termios.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <C_R.h>
using namespace std;
struct termios orig_termios;
extern vector <info_base *> infos;


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
           "1. check current info list"<<endl<<
           "2. exit"<<endl<<
           "====================================="<<endl;
}

void  *pthread_server_console(void *ptr)
{
    string username;
    cout << "please login:" << endl;
    cin >> username;
    print_menu(username);

    while(1)
    {

    // set_conio_terminal_mode();




        while (!kbhit())
        {
            /* do some work */
        }


        char instruction_id=getch();
      //  getch();
        if(instruction_id == '1')
        {
            cout << endl <<   "*************************************" << endl <<"user id\t" << "user name\t" << "user password" << endl;
            for(int i = 0; i < infos.size(); i++)
            {
                cout << i << "\t" << infos.at(i)->user << "\t" << infos.at(i)->pw << endl <<  "*************************************" << endl;
            }
            print_menu(username);
        }
        else if(instruction_id == '2')
        {
            break;
        }
        else;

    }


}


#endif // SERVER
