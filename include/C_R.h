#ifndef C_R_H
#define C_R_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class info_base
{
public:
    string software_id;
    string user;
    string web;
	string pw;
    string extra;
    int pid;
    int flag;//-1:write_to_PIC32; -2:read_from_PIC32; -3:read_from_PIC32_failed; 0:idle
};

extern vector <info_base *> infos;
#endif
