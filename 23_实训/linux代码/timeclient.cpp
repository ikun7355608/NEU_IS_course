#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<iostream>
int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(1234);  
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    char buff[100] = {};
    read(sock, buff, sizeof(buff));
    std::cout << "从服务器获取到的时间为："<<buff;
    close(sock);
    return 0;
}