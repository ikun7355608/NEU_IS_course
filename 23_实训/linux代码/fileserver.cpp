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
#define MAXSIZE 819200
int main()
{
    char str[MAXSIZE] = {};
    int soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(6666);
    bind(soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(soc, 16);
    FILE *fp;
    int temp;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(soc, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    char name[256] = {};
    //接收文件名并创建文件
    struct sockaddr_in clnt_addr2;
    socklen_t clnt_addr_size2 = sizeof(clnt_addr2);
    int clnt_sock2 = accept(soc, (struct sockaddr *)&clnt_addr2, &clnt_addr_size2);
    read(clnt_sock2, name, sizeof(name));
    string const name_temp = string("temp_") + string(name);
    const char *tmp = name_temp.c_str();
    fp = fopen(tmp, "ab+");
    close(clnt_sock2);
    while (1)
    {
        temp = read(clnt_sock, str, MAXSIZE);
        if(temp == 0)
        {
            break;
        }
        fwrite(str, 1, temp, fp);
    }
    str[temp] = '\0';
    close(clnt_sock);
    fclose(fp);
    close(soc);
    return 0;
}
