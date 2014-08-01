#ifndef JSON_HELPER_H
#define JSON_HELPER_H


#include "json/json.h"
#include "C_R.h"
#include <iostream>
#include <string>
#include "msg_types.h"
using namespace std;

extern vector <info_base *> infos;

class jsonhelper
{
public:
	Json::FastWriter jwriter;
    Json::Reader jreader;

    string generate_msg(int type, info_base * info)
    {
        string returnstr;
    	Json::Value root;
        Json::Value parameter;
        Json::Value parameters;
        if(type == REQUEST_INFO)
        {
            parameter["pid"] = info->pid;
            parameter["user"] = info->user;
            parameter["software_id"] = info->software_id;
            parameter["extra"] = "";
            parameters.append(parameter);
        }
        else if(type == RESPONSE_INFO)
        {
            parameter["pid"] = info->pid;
            parameter["user"] = info->user;
            parameter["pw"] = info->pw;
            parameter["software_id"] = info->software_id;
            parameter["extra"] = "";
            parameters.append(parameter);
        }
        else if(type == REQUEST_STORE)
        {
            parameter["pid"] = info->pid;
            parameter["user"] = info->user;
            parameter["pw"] = info->pw;
            parameter["software_id"] = info->software_id;
            parameter["extra"] = "";
            parameters.append(parameter);
        }
        else if(type == RESPONSE_STORE)
        {
            parameter["user"] = info->user;
            parameter["software_id"] = info->software_id;
            parameter["pid"] = info->pid;
            parameter["extra"] = info->extra;
            parameters.append(parameter);
        }
        else if(type == WEB_RESPONSE_INFO)
        {
            parameter["user"] = info->user;
            parameter["pw"] = info->pw;
            parameter["software_id"] = info->software_id;
            parameter["web"] = info->web;
            parameters.append(parameter);
        }
        else if(type == WEB_RESPONSE_STORE)
        {
            parameter["user"] = info->user;
            parameter["software_id"] = info->software_id;
            parameter["web"] = info->web;
            parameter["extra"] = info->extra;
            parameters.append(parameter);
        }
        else
        {
            returnstr = "invalid type";
        }

		root["type"]=type;
		root["parameters"]=parameters;
        returnstr= jwriter.write(root);
        return returnstr;
    }

    string parse_msg(string input_string)
    {
        Json::Value jroot;
        int type=0;
        if(!jreader.parse(input_string,jroot,false))
        {
            perror("json reader");
            exit(-1);
        }
        type=jroot["type"].asInt();
        std::cout<<jroot["type"]<<std::endl;

        Json::Value parameters=jroot["parameters"];
        int j = 0;
        if(type == REQUEST_INFO)
        {
            int i;
            for(i = 0; i < infos.size(); i++)
            {
                if((infos.at(i))->software_id == parameters[j]["software_id"].asString() && (infos.at(i))->pid == parameters[j]["pid"].asInt() && (infos.at(i))->user == parameters[j]["user"].asString()) break;
            }
            if(i == infos.size()) return "fail";
            else return generate_msg(RESPONSE_INFO, infos.at(i));
        }
        else if(type == REQUEST_STORE)
        {
            info_base * info = new info_base;
            info->software_id = parameters[j]["software_id"].asString();
            info->user = parameters[j]["user"].asString();
            info->pw = parameters[j]["pw"].asString();
            info->pid = parameters[j]["pid"].asInt();
            info->extra = "1";
            infos.push_back(info);
            return generate_msg(RESPONSE_STORE, info);
        }
        else if(type == WEB_REQUEST_INFO)
        {
            int i;
            for(i = 0; i < infos.size(); i++)
            {
                if((infos.at(i))->software_id == parameters[j]["software_id"].asString() && (infos.at(i))->web == parameters[j]["web"].asString() && (infos.at(i))->user == parameters[j]["user"].asString()) break;
            }
            if(i == infos.size()) return "fail";
            else return generate_msg(WEB_RESPONSE_INFO, infos.at(i));
        }

        else if(type == WEB_LAZY_REQUEST_INFO)
        {
            int i;
            for(i = 0; i < infos.size(); i++)
            {
                if((infos.at(i))->software_id == parameters[j]["software_id"].asString() && (infos.at(i))->web == parameters[j]["web"].asString()) break;
            }
            if(i == infos.size()) return "fail";
            else return generate_msg(WEB_RESPONSE_INFO, infos.at(i));
        }

        else if(type == WEB_REQUEST_STORE)
        {
            info_base * info = new info_base;
            info->software_id = parameters[j]["software_id"].asString();
            info->user = parameters[j]["user"].asString();
            info->pw = parameters[j]["pw"].asString();
            info->web = parameters[j]["web"].asString();
            info->extra = "1";
            infos.push_back(info);
            return generate_msg(WEB_RESPONSE_STORE, info);
        }

        else
        {
            cout << "invalid type" << endl;
            return "invalid type";
        }
    	


    }

};

#endif
