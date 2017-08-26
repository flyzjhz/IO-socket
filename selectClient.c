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
#define BUFFERSIZE 1024

int main(int argc, char **argv){
    int sockfd;
    char inputMsg[BUFFERSIZE];
    char recvMsg[BUFFERSIZE];
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

    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr))==0){
        fd_set clientSet;
        struct timeval tv;
        while(1){
            FD_ZERO(&clientSet);
            tv.tv_sec=20;
            tv.tv_usec=0;
            int maxfd=-1;
            FD_SET(STDIN_FILENO,&clientSet);
            if(maxfd<STDIN_FILENO){
                maxfd=STDIN_FILENO;
            }
            FD_SET(sockfd,&clientSet);
            if(maxfd<sockfd){
                maxfd=sockfd;
            }
            select(maxfd+1,&clientSet,NULL,NULL,&tv);
            if(FD_ISSET(STDIN_FILENO,&clientSet)){
                //要发送数据给服务器
                bzero(inputMsg,BUFFERSIZE);
                fgets(inputMsg,BUFFERSIZE,stdin);
                if(send(sockfd,inputMsg,BUFFERSIZE,0)<0){
                    fprintf(stderr,"send error");
                }
            }
            if(FD_ISSET(sockfd,&clientSet)){
                //接收来自服务器的消息
                bzero(recvMsg,BUFFERSIZE);
                int recvBytes=recv(sockfd,recvMsg,BUFFERSIZE,0);
                if(recvBytes<0){
                    fprintf(stderr,"recv failed!\n");

                }else if(recvBytes==0){
                    printf("服务器已关闭\n");
                    exit(0);
                }else{
                    if(recvBytes>BUFFERSIZE){
                        recvBytes=BUFFERSIZE;
                    }
                    recvMsg[recvBytes]='\0';
                    printf("接收到来自服务器的消息：%s",recvMsg);

                }
            }
        }
    }
    return 0;
}
