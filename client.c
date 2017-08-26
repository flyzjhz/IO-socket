#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVPORT 9999
#define MAXDATA 1024

int main(int argc, char **argv){
    int sockfd;
    char buf[MAXDATA];
    struct sockaddr_in servaddr;

    while(argc<2){
        fprintf(stderr,"please input the server hostname!\n");
        continue;
    }
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr,"client socket create failed!\n");
        exit(1);
    }

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERVPORT);
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);
    bzero(&(servaddr.sin_zero),8);

    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr))<0){
        fprintf(stderr,"connect failed!\n");
        exit(1);
    }
    int recvbytes;
    if((recvbytes=recv(sockfd,buf,MAXDATA,0))<0){
        fprintf(stderr,"recv failed!\n");
        exit(1);
    }
    buf[recvbytes]='\0';
    printf("receuv from server %s \n",buf);
    close(sockfd);
}
