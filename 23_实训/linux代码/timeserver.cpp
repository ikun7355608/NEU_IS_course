#include<stdio.h>
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<sys/time.h>
using namespace std;
int main()
{
    //创建
    int soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //初始化
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(1234);
    // 绑定
    bind(soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    // 监听
    listen(soc, 16);
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(soc, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    // 获取与该socket通信的用户信息
    getpeername(soc, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    char *socket_ip = inet_ntoa(clnt_addr.sin_addr);
    std::cout << "与此服务器建立连接的ip地址为："<<socket_ip;
    char str[100] = {};
    // 获取时间
    time_t t;
    struct tm *tp;
    time(&t);
    tp = localtime(&t);
    // 时间格式转换
    strftime(str, 100, "%r,%a,%d,%y", tp);
    // 发送时间
    write(clnt_sock, str, sizeof(str));
    close(clnt_sock);
    close(soc);
    // 打开文件
    FILE *fp;
    fp = fopen("data.txt", "a");
    // 写入ip
    fprintf(fp, " \r\n%s\t", socket_ip);
    // 将时间转化为标准再写入
    fprintf(fp, "%d-%d-%d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
    fprintf(fp, " %2d:%2d:%2d", tp->tm_hour, tp->tm_min, tp->tm_sec);
    fclose(fp);
    return 0;
}