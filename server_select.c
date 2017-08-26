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
#define BUFFERSIZE 1024   //每次传输最大字节
#define CLIENTNUM 20    //应用层能处理的最大连接

int clients[CLIENTNUM]={0};

int main(int argc,char **argv){
    int socketfd;
    char inputMsg[BUFFERSIZE];
    char recvMsg[BUFFERSIZE];
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

    fd_set servSet;
    int maxfd=-1;
    struct timeval tv;//超时设置
    while(1){
        tv.tv_sec=20;
        tv.tv_usec=0;
        FD_ZERO(&servSet);
        FD_SET(STDIN_FILENO,&servSet);
        if(maxfd<STDIN_FILENO){
            maxfd=STDIN_FILENO;
        }
        FD_SET(socketfd,&servSet);
        if(maxfd<socketfd){
            maxfd=socketfd;
        }
        for(int i=0;i<CLIENTNUM;i++){
            if(clients[i]!=0){
                FD_SET(clients[i],&servSet);
                if(maxfd<clients[i]){
                    maxfd=clients[i];
                }
            }
        }
        int ret=select(maxfd+1,&servSet,NULL,NULL,&tv);
        if(ret<0){
            fprintf(stderr,"select failed!\n");
            continue;
        }else if(ret==0){
            fprintf(stderr,"select timeout!\n");
            continue;
        }else{
            if(FD_ISSET(STDIN_FILENO,&servSet)){
                //标准输入有数据要发送
                printf("send message:\n");
                bzero(inputMsg,BUFFERSIZE);
                fgets(inputMsg,BUFFERSIZE,stdin);
                if(strcmp(inputMsg,"quit")==0){
                    return 0;
                }
                for(int i=0;i<CLIENTNUM;i++){
                    if(clients[i]!=0){
                        send(clients[i],inputMsg,BUFFERSIZE,0);

                    }
                }
            }
            if(FD_ISSET(socketfd,&servSet)){
                //有新的连接请求
                struct sockaddr_in clientAddr;
                socklen_t sin_size=sizeof(struct sockaddr);
                int clientfd;
                if((clientfd=accept(socketfd,(struct sockaddr*)&clientAddr,&sin_size))<0){
                    fprintf(stderr,"accept failed!\n");
                    continue;
                }
                int index=-1;
                for(int i=0;i<CLIENTNUM;i++){
                    if(clients[i]==0){
                        clients[i]=clientfd;
                        index=i;
                        break;
                    }
                }
                if(index>=0){
                    printf("新连接%d加入成功%s\n",index,inet_ntoa(clientAddr.sin_addr));
                }else{
                    bzero(inputMsg,BUFFERSIZE);
                    strcpy(inputMsg,"服务器加入的客户端数达到最大值，无法加入|\n");
                    send(clientfd,inputMsg,BUFFERSIZE,0);
                    printf("客户端数量以达到最大，加入失败！\n");
                }
            }
            for(int i=0;i<CLIENTNUM;i++){
                if(clients[i]!=0){
                    if(FD_ISSET(clients[i],&servSet)){
                    //处理来自客户的消息
                        bzero(recvMsg,BUFFERSIZE);
                        int recvBytes=recv(clients[i],recvMsg,BUFFERSIZE,0);
                        if(recvBytes>0){
                            if(recvBytes>BUFFERSIZE){
                                recvBytes=BUFFERSIZE;
                            }
                            recvMsg[recvBytes]='\0';
                            printf("接收到客户端%d消息：%s\n",i,recvMsg);
                        }else if(recvBytes<0){
                            printf("recv failed !\n");
                        }else{
                            FD_CLR(clients[i],&servSet);
                            clients[i]=0;
                            printf("客户端%d 退出!\n",i);
                        }
                    }
                }
            }
        }
    }
}
