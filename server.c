#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERV_PORT 9999
#define MAXDATA 1024   //每次传输最大字节

int main(int argc,char **argv){
    int socketfd;
    char buf[MAXDATA]="hello world!welcome to UNP";
    //struct hostent;
    struct sockaddr_in clientaddr;
    struct sockaddr_in servaddr;

    if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr,"socket create failed!\n");
        exit(1);
    }
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    bzero(&(servaddr.sin_zero),8);
    if(bind(socketfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr))<0){
        fprintf(stderr,"bind failed!\n");
        exit(1);
    }

    if(listen(socketfd,10)<0){
        fprintf(stderr,"listen failed!\n");
        exit(1);
    }
    
    int clientfd;
    while(1){
        socklen_t sin_size=sizeof(struct sockaddr);
        if((clientfd=accept(socketfd,(struct sockaddr*)&clientaddr,&sin_size))<0){
            fprintf(stderr,"accept error!\n");
            continue;
        }
        printf("receive a connet from %s \n",inet_ntoa(clientaddr.sin_addr));
        int pid=fork();
        if(pid==0){
            if(send(clientfd,buf,MAXDATA,0)<0){
                fprintf(stderr,"send failed!\n");
            }
            close(clientfd);
            exit(0);
        }
        close(clientfd);
    }
}
