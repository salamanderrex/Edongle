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

};

extern vector <info_base *> infos;
#endif
