#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"

using namespace std;

class connection_pool
{
public:
	MYSQL *GetConnection();				 //get database connection
	bool ReleaseConnection(MYSQL *conn); //release connection
	int GetFreeConn();					 //get free connection
	void DestroyPool();					 //colse all connection

	static connection_pool *GetInstance();

	void init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log); 

private:
	connection_pool();
	~connection_pool();

	int m_MaxConn;  //max numer of connection
	int m_CurConn;  //connection that is already in use
	int m_FreeConn; //number of free connection at now
	locker lock;
	list<MYSQL *> connList; //connection pool
	sem reserve;

public:
	string m_url;		  //usl of the server address
	string m_Port;		 //port number
	string m_User;		 //databse user name
	string m_PassWord;	 //databse passwrd
	string m_DatabaseName; //database name
	int m_close_log;	//if open the log
};

class connectionRAII{

public:
	connectionRAII(MYSQL **con, connection_pool *connPool);
	~connectionRAII();
	
private:
	MYSQL *conRAII;
	connection_pool *poolRAII;
};

#endif
