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
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFSIZE 1024
char *GetFileName(char *FilePath);
int getHttpHead(int fd, char *buf, char *buf1, int bufLen);


int main(int argc, char *argv[]) //argv[1]=[host](domain name); argv[2]=[port number]; argv[3]=[filepath];
{
    if (argc != 4) {
        fprintf(stderr,"usage: ./http_client [host] [port number] [filepath]\n");
        exit(1);
    }

	char buf[BUFSIZE] = {0};
	char fileName[BUFSIZE] = {0};
	int sockfd, numbytes;
	struct sockaddr_in cadd; 
	struct hostent *pURL = NULL;
    
    // Get the file name from the URL
    strncpy(fileName, GetFileName(argv[3]), BUFSIZE);
    
    char host[BUFSIZE], GET[BUFSIZE];
	char request[BUFSIZE];
	char text[BUFSIZE] = {0};
    char text1[BUFSIZE] = {0};
    int portNum;

	memset(host, 0, BUFSIZE);
	memset(GET, 0, BUFSIZE);
	memset(request, 0, BUFSIZE);
	memset(text, 0, BUFSIZE);
    memset(text1, 0, BUFSIZE);
    
    sscanf(argv[1], "%s", host);
    sscanf(argv[2], "%d", &portNum);
    sscanf(argv[3], "%s", GET);

	//printf("host = %s\n", host);
    //printf("port = %d\n", portNum);
    //printf("GET = %s\n", GET);
    
    // Set socket parameter


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	if ((pURL=gethostbyname(host)) == NULL) {
		herror("gethostbyname");
		exit(1);
	}

    // Define IP address structure
    bzero(&cadd, sizeof(struct sockaddr_in));
	cadd.sin_family			= AF_INET;
	cadd.sin_addr.s_addr	= *((unsigned long *)pURL->h_addr_list[0]);
	cadd.sin_port			= htons(portNum);

    // Sent URL information to HTTP server
	snprintf(request, BUFSIZE, 		\
				"GET %s HTTP/1.1\r\n"
				"HOST: %s:%d\r\n"
				"Cache-Control: no-cache\r\n"
				"Connection: close\r\n\r\n",
				GET,host,portNum);

    // Connect the server
	if (connect(sockfd, (struct sockaddr *)&cadd, (socklen_t)sizeof(cadd)) < 0)
	{
		perror("connect");
		exit(1);
	}

    // Send request to the server that url request
	if (send(sockfd, request, strlen( request ), 0 ) < 0)
	{
		perror("request");
		exit(1);
	}

    // Client receiver the information return by the server
    getHttpHead(sockfd, text, text1, sizeof(text));
    if(strncmp(text1, "HTTP/1.1 200", 12) != 0)
    {
        if(strncmp(text1, "HTTP/1.0 200", 12) != 0)
        {
            fprintf(stdout, "%s\n", text1);
            exit(1);
        }
    }
    
	//printf("fileName = %s\n", fileName);
	remove(fileName);
    int	fd = -1;
    fd = open(fileName, O_WRONLY | O_CREAT, 777 );
	if(fd == -1)
	{
		perror( "Open error" );
		exit(1);
	}

	while (1)
	{
		memset( buf, 0, BUFSIZE );
		int cr;
        int temp;
		cr = recv(sockfd, buf, BUFSIZE, 0);
		if ( cr <= 0 )
		{
			//printf("break\n");
			break;
		}

	    if (write(fd, buf, cr) == -1)
	    {
		    perror("write");
		    exit(1);
	    }
	}
	
	close(fd);
	close(sockfd);

    return 0;
}


char *GetFileName(char *FilePath)
{
    char ch = '/';
    char *q = strrchr(FilePath, ch) + 1;
    return q;
}

int getHttpHead(int fd, char *buf, char *buf1, int bufLen)
{
	char tmp[1] = {0};
	int i = 0;
    int j = 0;
	int offset = 0;
	int nbytes = 0;

	while((nbytes=recv(fd,tmp,1, 0))==1)
	{
		if(offset > bufLen-1)
		{
			return bufLen;
		}
		
		if(i < 4)
		{
			if(tmp[0] == '\r' || tmp[0] == '\n') i++;
			else i = 0;
			
            if(i==0)
            {
                strncpy(buf1+offset, tmp, 1);
            }

			strncpy(buf+offset, tmp, 1);
            
			offset++;
		}
		
		if(4 == i)
		{
			return offset;
		}
	}
	
	return -1;
}