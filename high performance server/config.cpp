#include "config.h"

Config::Config(){
    //port number, defaul is 9006
    PORT = 9006;

    //log write mode, defaul is synchronous
    LOGWrite = 0;

    //triger mode listenfd LT + connfd LT
    TRIGMode = 0;

    //listenfd triger mode defaul is LT
    LISTENTrigmode = 0;

    //connfd triger mode, defaul is LT
    CONNTrigmode = 0;

    //close connection with linger, defaul is n
    OPT_LINGER = 0;

    //database connection, defaul is 8
    sql_num = 8;

    //thread number, defaul is 8
    thread_num = 8;

    // defaul log is open, set to 1 to clse
    close_log = 0;

    //mode, defaul is proactor
    actor_model = 0;
}

void Config::parse_arg(int argc, char*argv[]){
    int opt;
    const char *str = "p:l:m:o:s:t:c:a:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 'p':
        {
            PORT = atoi(optarg);
            break;
        }
        case 'l':
        {
            LOGWrite = atoi(optarg);
            break;
        }
        case 'm':
        {
            TRIGMode = atoi(optarg);
            break;
        }
        case 'o':
        {
            OPT_LINGER = atoi(optarg);
            break;
        }
        case 's':
        {
            sql_num = atoi(optarg);
            break;
        }
        case 't':
        {
            thread_num = atoi(optarg);
            break;
        }
        case 'c':
        {
            close_log = atoi(optarg);
            break;
        }
        case 'a':
        {
            actor_model = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
}