#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define SERVPORT 3333
#define BACKLOG 10
#define MAX_CONNECTED_NO 10
#define MAXDATASIZE 100

int main()
{
	struct sockaddr_in server_sockaddr,client_sockaddr;
	int sin_size,recvbytes,flags;
	int sockfd,client_fd;
	char buf[MAXDATASIZE];
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket");
		exit(1);
	}
	printf("socket success!,sockfd=%d\n",sockfd);
	server_sockaddr.sin_family=AF_INET;
	server_sockaddr.sin_port=htons(SERVPORT);
	server_sockaddr.sin_addr.s_addr=INADDR_ANY;
	bzero(&(server_sockaddr.sin_zero),8);
	if(bind(sockfd,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr))==-1){
		perror("bind");
		exit(1);
	}
	printf("bind success!\n");
	if(listen(sockfd,BACKLOG)==-1){
		perror("listen");
		exit(1);
	}
	printf("listening....\n");
	if((flags=fcntl( sockfd, F_SETFL, 0))<0)
			perror("fcntl F_SETFL");
		flags |= O_NONBLOCK;
		if(fcntl( sockfd, F_SETFL,flags)<0)
			perror("fcntl");
	while(1){
		sin_size=sizeof(struct sockaddr_in);
		if((client_fd=accept(sockfd,(struct sockaddr*)&client_sockaddr,&sin_size))==-1){
			perror("accept");
			exit(1);
		}
		if((recvbytes=recv(client_fd,buf,MAXDATASIZE,0))==-1){
			perror("recv");
			exit(1);
		}
		if(read(client_fd,buf,MAXDATASIZE)<0){
			perror("read");
			exit(1);
		}
		printf("received a connection :%s",buf);
	close(client_fd);
	exit(1);
	}/*while*/
}

