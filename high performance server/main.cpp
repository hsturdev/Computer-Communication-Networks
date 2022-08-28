#include "config.h"

int main(int argc, char *argv[])
{
    //configure the database infor
    string user = "tliaws";
    string passwd = "WYYXHTF1314520++";
    string databasename = "webserver";

    // handle the user input when start program
    Config config;
    config.parse_arg(argc, argv);

    WebServer server;

    //initialization
    server.init(config.PORT, user, passwd, databasename, config.LOGWrite, 
                config.OPT_LINGER, config.TRIGMode,  config.sql_num,  config.thread_num, 
                config.close_log, config.actor_model);
    

    //log
    server.log_write();

    //database
    server.sql_pool();

    //thread
    server.thread_pool();

    //triger mode
    server.trig_mode();

    //listen event
    server.eventListen();

    //run the server
    server.eventLoop();

    return 0;
}