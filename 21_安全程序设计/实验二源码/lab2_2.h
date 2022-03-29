#pragma once
#include<iostream>
using namespace std;
char* Strcat(char* p1, char* p2)
{
	char* p = p1;
	//p指向p1结尾
	while (*p1!='\0')
	{
		p1++;
		p++;
	}
	//将p2写在p所指位置
	while (*p2!='\0')
	{
		*p = *p2;
		p++;
		p2++;
	}
	*p = '\0';
	return p1;
}

void lab2_2()
{
	char a[100], b[50];
	cout << "请输入字符串a,b" << endl;
	cin >> a >>  b;
	cout << "续接前：" << endl << "a = " << a << "\t" << "b = " << b << endl;
	Strcat(a, b);
	cout << "续接后：" << endl << "a = " << a << "\t" << "b = " << b << endl;
}