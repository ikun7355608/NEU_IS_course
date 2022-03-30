#include<iostream>
#include<stdio.h>
using namespace std;
int main()
{
	char ch = {};
	char ptext[1000];
	memset(ptext, 0, sizeof(ptext));
	int n = 0;
	while ((ch = getchar()) != ';')
	{
		ptext[n++] = ch;
	}
	char ctext[1000] = {};
	int s[256] = {};
	int i = 0, j = 0;
	for (i = 0; i < 256; i++)
	{
		s[i] = i;
	}
	int k[256] = {};
	char ch = '\n';
	int temp = 0;
	while (ch = getchar() != ';')
	{
		if (ch >= '0' && ch <= '9')
		{
			temp *= 10;
			temp += ch - '0';
		}
		else if (ch == ' ')
		{
			k[n++] = temp;
			temp = 0;
		}
	}
	//退出后保存最后一个数字
	k[n++] = temp;
	for (i = 0; i < 256; i++)
	{
		k[i] = k[i % n];
	}
	//KSA
	for (i = 0; i < 256; i++)
	{
		j = (s[i] + k[i] + j) % 256;
		//交换
		swap(s[i], s[j]);
	}
	//PRGA
	int m = 0;
	//m为明文长度
	int x = 0;
	i = 0;
	j = 0;
	int* p = new int[n];
	for (x = 0; x < n; x++)
	{
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		swap(s[i], s[j]);
		int t = (s[i] + s[j]) % 256;
		p[x] = s[t];
	}
	//p为生成的密钥流
	for (i = 0; i < m; i++)
	{
		ctext[i] = ptext[i] ^ p[i];
	}
}