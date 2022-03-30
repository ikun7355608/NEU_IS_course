#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<iostream>
#define MAXSIZE 819200
using namespace std;
int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(6666);  
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    char buff[MAXSIZE] = {};
    FILE *fp;
    string str;
    cout << "请输入要备份的文件名" << endl;
    cin >> str;
    const char *ch = str.c_str();
    fp = fopen(ch, "rb");
    int sock2 = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock2, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    write(sock2, ch, strlen(ch));
    while (!feof(fp))
    {
        int temp = fread(buff, 1, sizeof(buff), fp);
        if(temp)
        {
            write(sock, buff, temp);
            break;
        }
    }
    close(sock2);
    close(sock);
    fclose(fp);
    return 0;
}
