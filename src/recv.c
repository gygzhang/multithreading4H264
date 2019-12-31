#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
 
typedef struct fram{
    int len;
    char *data;
    
}fram;

typedef struct frame_list{
    fram fram;
    
}frame_list;

#define MAX_SIZE 99999

int main()
{
    int CreateSocket = 0,n = 0;
    char dataReceived[1024*25];
    struct sockaddr_in ipOfServer;
 
    FILE *fh264 = fopen("./BB.sck.h264.mp4","wb");
    memset(dataReceived, '0' ,sizeof(dataReceived));
 
    if((CreateSocket = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("Socket not created \n");
        return 1;
    }
 
    ipOfServer.sin_family = AF_INET; //ipv4
    ipOfServer.sin_port = htons(2020);
    ipOfServer.sin_addr.s_addr = inet_addr("127.0.0.1");
 
    if(connect(CreateSocket, (struct sockaddr *)&ipOfServer, sizeof(ipOfServer))<0)
    {
        printf("Connection failed due to port and ip problems\n");
        return 1;
    }
    int len;
    char *data = (char*)malloc(sizeof(char)*MAX_SIZE);
    while((n = read(CreateSocket, dataReceived, sizeof(dataReceived)-1)) > 0)
    {
        if(n) printf("read bytes: %d\n",n);
        dataReceived[n] = 0;
        len = (int)dataReceived;
        data = dataReceived;
        fwrite((void*)data,n,1,fh264);
        // if(fputs(dataReceived, stdout) == EOF)
        // {
        //     printf("\nStandard output error");
        // }      
    }
    free(data);

 
    if( n < 0)
    {
        printf("Standard input error \n");
    }
 
    return 0;
}