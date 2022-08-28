#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "block_queue.h"

using namespace std;

class Log
{
public:
    //handle lock due to new feature for C++11
    static Log *get_instance()
    {
        static Log instance;
        return &instance;
    }

    static void *flush_log_thread(void *args)
    {
        Log::get_instance()->async_write_log();
    }
    
    //file_name: name of the file
    //close_log: if close the log
    //log_buf_size: buffer size for the log load
    //split_lines: max number of lines allow in the log
    //max_queue_size: max queu size in the log
    bool init(const char *file_name, int close_log, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);

    void write_log(int level, const char *format, ...);

    void flush(void);

private:
    Log();
    virtual ~Log();
    void *async_write_log()
    {
        string single_log;
        //take a log string from the blocking queue, write it to file
        while (m_log_queue->pop(single_log))
        {
            m_mutex.lock();
            fputs(single_log.c_str(), m_fp);
            m_mutex.unlock();
        }
    }

private:
    char dir_name[128]; //dir name
    char log_name[128]; //log file name
    int m_split_lines;  //max number of lines allow in the log
    int m_log_buf_size; //log buffer load size
    long long m_count;  //log line number
    int m_today;        //date of today, since log use date for recording purpose
    FILE *m_fp;         //open log file pointer
    char *m_buf;
    block_queue<string> *m_log_queue; //blocking queue
    bool m_is_async;                  //if async
    locker m_mutex;
    int m_close_log; //if close log
};

#define LOG_DEBUG(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(0, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_INFO(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(1, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_WARN(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(2, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#define LOG_ERROR(format, ...) if(0 == m_close_log) {Log::get_instance()->write_log(3, format, ##__VA_ARGS__); Log::get_instance()->flush();}

#endif
