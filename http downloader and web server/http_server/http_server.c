/* 
This is one of the project for ECE463 at Purdue University
It's for academic and educational purpose only
Please read only, do not share, post, or copy this code
Especially if you are a student who is taking this course  

Student name: Tingzhang Li
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MYPORT 8080 /* server should bind to port 8080 */
                    /* bind to IP address INADDR_ANY */

#define MAXPENDING 50 /* Maximum outstanding connection requests; listen() param */

#define DBADDR "127.0.0.1"
#define DBPORT 53004
#define BACKLOG 10
#define MAXSIZE 4096
#define BUFSIZE 1024

void getLink(int clientSocket, char *buf1, char *buf2, char *buf3, char *buf4);
int end_with(const char *str1, char *str2);
int is_directory(const char *path);
void ExactSearch(char *SearchStr, char *returnBuf);

int main()
{
	char goodReq[19] = "HTTP/1.0 200 OK\r\n"
							"\r\n";
	char badReq[BUFSIZE] = "HTTP/1.0 400 Bad Request\r\n"
							"\r\n"
							"<html><body><h1>400 Bad Request</h1></body></html>";

	char noImple[BUFSIZE] = "HTTP/1.0 501 Not Implemented\r\n"
							"\r\n"
							"<html><body><h1>501 Not Implemented</h1></body></html>";

	char notFound[BUFSIZE] = "HTTP/1.0 404 Not Found\r\n"
							"\r\n"
							"<html><body><h1>404 Not Found</h1></body></html>";
	char timeEnd[BUFSIZE] = "HTTP/1.0 408 Request Timeout\r\n"
							"\r\n"
							"<html><body><h1>408 Request Timeout</h1></body></html>";

	// Stocket setup
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(MYPORT);
	server_addr.sin_addr.s_addr = INADDR_ANY; /* bind to all local interfaces */
	bzero(&(server_addr.sin_zero), 8);

	if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) < 0) {
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) < 0) {
		perror("listen");
		exit(1);
	}

	int clientSocket;
	int sin_size;
	struct sockaddr_in their_addr; /* client's address info */
	char dst[INET_ADDRSTRLEN];
	char buf[MAXSIZE] = {0};
	char method[BUFSIZE] = {0};
	char filePath[BUFSIZE] = {0};
	char httpV[BUFSIZE] = {0};
	char printPath[BUFSIZE] = {0};
	char searchBuf[BUFSIZE] = {0};
	int len;
	int numbytes;
	// Open UDP socket for database
	struct hostent* he;
	int sockfdUDP;
	struct sockaddr_in data_addr;
	if ((he=gethostbyname(DBADDR)) == NULL) {
		herror("gethostbyname");
		exit(1);
	}

	if ((sockfdUDP = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socketUDP");
		exit(1);
	}

	//create a non-blocking socket
	//int status = fcntl(sockfdUDP, F_SETFL, fcntl(sockfdUDP, F_GETFL, 0) | O_NONBLOCK);
	data_addr.sin_family = AF_INET;
	data_addr.sin_port = htons(DBPORT);
	data_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	bzero(&(data_addr.sin_zero), 8);

	if (connect(sockfdUDP, (struct sockaddr *) &data_addr,sizeof(struct sockaddr)) < 0)
	{
		perror("connectUDP");
		//exit(1);
	}

	struct timeval tv={5,0};
	setsockopt(sockfdUDP, SOL_SOCKET,SO_RCVTIMEO,(char *)&tv,sizeof(struct timeval));

	while(1) {
		sin_size = sizeof(struct sockaddr_in);
		if ((clientSocket = accept(sockfd,
        (struct sockaddr *) &their_addr, &sin_size)) < 0) {
			perror("accept");
			continue;
		}

		inet_ntop(AF_INET, &(their_addr.sin_addr), dst, INET_ADDRSTRLEN);
		//printf("server: got connection from %s\n", dst);

		memset(method, 0, BUFSIZE);
		memset(filePath, 0, BUFSIZE);
		memset(httpV, 0, BUFSIZE);
		memset(printPath, 0, BUFSIZE);
		char exactPath[BUFSIZE] = {"./Webpage"};
		getLink(clientSocket, method,filePath,httpV,printPath);
		strcat (exactPath,filePath);
		
		fprintf(stdout, "%s", dst);
		fprintf(stdout, "%s", printPath);

		if(strcmp(method, "GET") != 0)
		{
        	if(send(clientSocket, noImple, sizeof(noImple), 0) < 0) {
            	perror("send");
            	//exit(1);
        	}
			fprintf(stdout, "501 Not Implemented\n");
			close(clientSocket);
			continue;
		}

		if(strncmp(filePath, "/", 1) != 0)
		{
        	if(send(clientSocket, badReq, sizeof(badReq), 0) < 0) {
            	perror("send");
            	//exit(1);
        	}
			fprintf(stdout, "400 Bad Request\n");
			close(clientSocket);
			continue;
		}

		if(strstr(filePath, "/../"))
		{
        	if(send(clientSocket, badReq, sizeof(badReq), 0) < 0) {
            	perror("send");
            	//exit(1);
        	}
			fprintf(stdout, "400 Bad Request\n");
			close(clientSocket);
			continue;
		}
		if(end_with(filePath, "/..")==1)
		{
        	if(send(clientSocket, badReq, sizeof(badReq), 0) < 0) {
            	perror("send");
            	//exit(1);
        	}
			fprintf(stdout, "400 Bad Request\n");
			close(clientSocket);
			continue;
		}


		if(end_with(filePath, "/")==1)
		{
			strcat (exactPath,"index.html");
		}
		else if(is_directory(exactPath))
		{
			strcat (exactPath,"/index.html");
		}

		//Search for pic 
		if(strstr(filePath, "?key="))
		{
			memset(searchBuf, 0, BUFSIZE);
			ExactSearch(filePath, searchBuf);
        	if(send(sockfdUDP, searchBuf, sizeof(searchBuf), 0) < 0) {
        		perror("sendUDP");
        		//exit(1);
    		}
			memset(buf, 0, MAXSIZE);
	        if ((numbytes = recv(sockfdUDP, buf, MAXSIZE, MSG_WAITALL)) < 0)
			{	
    			if(send(clientSocket, timeEnd, sizeof(timeEnd), 0) < 0) 
				{
        			perror("send");
        			//exit(1);
    			}
				fprintf(stdout, "408 Request Timeout\n");
				close(clientSocket);
				continue;
    		}
			if (strstr(buf, "File Not Found")!=NULL) 
			{
    			if(send(clientSocket, notFound, sizeof(notFound), 0) < 0) 
				{
        			perror("send");
        			//exit(1);
    			}
				fprintf(stdout, "404 Not Found\n");
				close(clientSocket);
				continue;
			}
			else
			{
        		if(send(clientSocket, goodReq, sizeof(goodReq), 0) < 0) 
				{
        			perror("send");
        			//exit(1);
    			}

				if (strstr(buf, "DONE")!=NULL) 
				{
    				if(send(clientSocket, buf, (numbytes-4), 0) < 0) 
					{
        				perror("send");
        				//exit(1);
    				}
					fprintf(stdout, "200 OK\n");
					close(clientSocket);
					continue;
				}
   				if(send(clientSocket, buf, numbytes, 0) < 0) 
				{
        			perror("send");
        			//exit(1);
    			}
				while(strstr(buf, "DONE")==NULL)
				{
					memset(buf, 0, MAXSIZE);
					if ((numbytes = recv(sockfdUDP, buf, MAXSIZE, MSG_WAITALL)) < 0) {
    					if(send(clientSocket, timeEnd, sizeof(timeEnd), 0) < 0) 
						{
        					perror("send");
        					//exit(1);
    					}
						fprintf(stdout, "408 Request Timeout\n");
						close(clientSocket);
						continue;
    				}
    				if(send(clientSocket, buf, numbytes, 0) < 0) 
					{
        				perror("send");
        				//exit(1);
    				}
				}
    			if(send(clientSocket, buf, (numbytes-4), 0) < 0) 
				{
        			perror("send");
        			//exit(1);
    			}
				fprintf(stdout, "200 OK\n");
				close(clientSocket);
				continue;
			}
		}

		//locate file
		int fp = open(exactPath, O_RDONLY);
		if(fp == -1)
		{
        	if(send(clientSocket, notFound, sizeof(notFound), 0) < 0) {
            	perror("send");
            	//exit(1);
        	}
			fprintf(stdout, "404 Not Found\n");
			close(clientSocket);
			continue;
		}

		// Send successthe file
        if(send(clientSocket, goodReq, sizeof(goodReq), 0) < 0) {
        	perror("send");
        	//exit(1);
    	}

		memset(buf, 0, MAXSIZE);

		do{
			len=read(fp,buf,MAXSIZE);
            if(send(clientSocket, buf, len, 0) < 0) {
                perror("send");
                //exit(1);
            }
			usleep(10000);
		} while(len);
		close(fp);
		fprintf(stdout, "200 OK\n");
		close(clientSocket);

	}

	close(sockfdUDP);
	close(sockfd);

    return 0;
}


void getLink(int clientSocket, char *buf1, char *buf2, char *buf3, char *buf4)
{
	char tmp[1] = {0};
	int nbytes = 0;
	int offset = 0;
	int offsetnew = 2;
	int i = 0;

	strncpy(buf4, " \"", 2);
	
	while((nbytes=recv(clientSocket,tmp,1, 0))==1)
	{
		if(tmp[0] == '\r' || tmp[0] == '\n')
		{
			strncpy(buf4+offsetnew, "\" ", 2);
			return;
		}
		if(tmp[0] == ' ')
		{
			i++;
			offset = 0;
			strncpy(buf4+offsetnew, tmp, 1);
			offsetnew++;
			continue;
		}

		if(i == 0)
		{
			strncpy(buf1+offset, tmp, 1);
		}
		if(i == 1)
		{
			strncpy(buf2+offset, tmp, 1);
		}
		if(i == 2)
		{
			strncpy(buf3+offset, tmp, 1);
		}
		
		strncpy(buf4+offsetnew, tmp, 1);
		offset++;
		offsetnew++;
	}
	return;
}



//string 1 ends with string2?
// 1 - yes
// 0	- no 
// -1 - error
int end_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}


int is_directory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void ExactSearch(char *SearchStr, char *returnBuf)
{
	int i= 0;
	int j= 0;
	int len = strlen(SearchStr);
	int flag = 0;

    for (i = 0; i < len; i++)
    {
		if(flag == 1)
		{
			if(SearchStr[i] != '+')
			{
				strncpy(returnBuf+j, SearchStr+i, 1);
			}
			else
			{
				strncpy(returnBuf+j, " ", 1);
			}
	   		j++;
		}
		if(SearchStr[i] == '=')
		{
			flag = 1;
		}
    }

    return;
}
