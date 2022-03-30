#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <sys/time.h>
#include "DH.h"
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
int main(){
    //初始化DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        ll a;
    ll p;
    FILE *fp;
    fp=fopen("prime_number.txt","r");
    fscanf(fp,"%lld",&p);
    fscanf(fp,"%lld",&a);
    fclose(fp);
	srand((unsigned)time(NULL));
    ll Xb = rand() % p;
	ll Yb = q_pow(a, Xb, p);
    printf("客户端所选取的数为%lld\n",Xb);
    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //接收服务器传回的数据
    char szBuffer[MAXBYTE] = {};
    recv(sock, szBuffer, MAXBYTE, 0);
    char ch[100]={};
    itoa(Yb,ch,10);
    char* str=ch;
    send(sock, str, strlen(str)+sizeof(char), 0);
    //输出接收到的数据
    printf("从服务器端接收到的数据为: %s\n", szBuffer);
    int Ya=0;
    for(int i=0;szBuffer[i]!='\0';i++)
    {
        Ya*=10;
        Ya+=szBuffer[i]-'0';
    }
    ll Kb=q_pow(Ya,Xb,p);
    printf("B计算出的密钥为：%lld\n",Kb);
    //关闭套接字
    closesocket(sock);

    //终止使用 DLL
    WSACleanup();

    system("pause");
    return 0;
}