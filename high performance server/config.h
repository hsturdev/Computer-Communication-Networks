#ifndef CONFIG_H
#define CONFIG_H

#include "webserver.h"

using namespace std;

class Config
{
public:
    Config();
    ~Config(){};

    void parse_arg(int argc, char*argv[]);

    //port number
    int PORT;

    //log write mode
    int LOGWrite;

    //Triger mode
    int TRIGMode;

    //listenfd triger mode 
    int LISTENTrigmode;

    //connfd triger mode
    int CONNTrigmode;

    //close the connection with linger, 1/0
    int OPT_LINGER;

    //number of connection to the database
    int sql_num;

    //number of thread
    int thread_num;

    //weather close log: 1/0
    int close_log;

    //mode select
    int actor_model;
};

#endif