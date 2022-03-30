#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <winsock2.h>
#include "DH.h"
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll

int main() {
    //初始化 DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = INADDR_ANY;  //具体的IP地址
    sockAddr.sin_port = htons(1234);  //端口
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //进入监听状态
    listen(servSock, 20);
	//生成随机素数
    ll a;
    ll p;
    FILE *fp;
    fp=fopen("prime_number.txt","r");
    fscanf(fp,"%lld",&p);
    fscanf(fp,"%lld",&a);
    fclose(fp);
	srand((unsigned)time(NULL));
    ll Xa = rand() % p;
	ll Ya = q_pow(a, Xa, p);
    printf("服务器端所选取的数为%lld\n",Xa);
    //接收客户端请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    char ch[100]={};
    itoa(Ya,ch,10);
    char* str=ch;
    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
    //向客户端发送数据
    send(clntSock, str, strlen(str)+sizeof(char), 0);
    char szBuffer[MAXBYTE] = {};
    recv(clntSock, szBuffer, MAXBYTE, 0);
    //关闭套接字
    printf("从客户端端接收到的数据为: %s\n", szBuffer);
    int Yb=0;
    for(int i=0;szBuffer[i]!='\0';i++)
    {
        Yb*=10;
        Yb+=szBuffer[i]-'0';
    }
    ll Ka=q_pow(Yb,Xa,p);
    printf("B计算出的密钥为：%lld\n",Ka);
    closesocket(clntSock);
    closesocket(servSock);

    //终止 DLL 的使用
    WSACleanup();

    return 0;
}