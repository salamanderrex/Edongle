#include "json/json.h"
#include "C_R.h"
#include <iostream>
#include <string>
using namespace std;
class jsonhelper
{
public:
	Json::FastWriter jwriter;
    Json::Reader jreader;

    string generate_msg(int type, int pid,string software_id, string user, string pw)
    {
    	Json::Value root;
        Json::Value parameter;
        Json::Value parameters;


		parameter["pid"]=pid
		parameter["user"]=user;
		parameter["pw"]=pw;
		parameter["software_id"]=software_id;
		parameters.append(parameter);
		root["type"]=type;
		root["parameters"]=parameters;
    }

    string parse_msg(string input_string,info_base & info)
    {
    	Json::Value root;
    	
    	    if(!jreader.parse(instruction,jroot,false))
                            {
                                perror("json reader");
                                exit(-1);
                            }
                            int j_request_type=jroot["type"].asInt();

                                Json::Value parameters=jroot["parameters"];
                                int i, j;
                                T_TORRENT *torrent;
                                for(i = 0; i < torrents.size(); i++)
                                {
                                    torrent = torrents.at(i);
                                    for(j = 0; j < parameters.size(); j++)
                                    {
                                        if(torrent->torrent_id == parameters[j]["torrent_id"].asInt()) break;
                                    }
                                    if(j != parameters.size()) break;
                                }
                                if(i == torrents.size()) printf("invalid torrent id\n");
                                else
                                {
                                    strcpy(res_buf,(c_r_client.generate_response(CLINET_RESPONSE_TYPE.C_C_REQUEST_SHAKE_HAND_REPLY,torrent)).c_str());
                                    std::cout<<"server response:"<<res_buf<<std::endl;
                                    int  len = send(sockfd, res_buf, strlen(res_buf) , 0);
                                    if (len > 0)
                                        printf("msg:%s send successful锛宼otalbytes: %d锛乗n", res_buf, len);
                                    else {
                                        printf("msg:'%s  failed!\n", res_buf);
                                    }
                                }
                            }

    }

};