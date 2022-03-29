#pragma once
#include<iostream>
using namespace std;
char* Strfnd(char* a, char* b)
{
	int i=1, j=0;
	char* temp = a;
	//字符串b首字符在字符串a中匹配
	{
		while (*temp != '\0')
		{
			if (*temp == *b)
			{
				//判断后续字符是否相等
				char* p1 = temp, * p2 = b;
				while (*p2 != '\0')
				{
					if (*p1 != *p2)
						goto loop;
					p1++;
					p2++;
				}
				//表示字符串b在a中位置
				char* x = a;
				while (x != temp)
				{
					i++;
					x++;
				}
				x = b+1;
				while (*x++ != '\0')
				{
					j++;
				}
				cout << "字符串"<<b<<"位于字符串"<<a<<"的" << i << "~" << i + j << "位" << endl;
				return 0;
			}
loop:		temp++;
		}
	}
	cout << "未在字符串"<<a<<"中查找到字符串" <<b<< endl;
	return 0;
}
void lab2_3()
{
	char a[100], b[50];
	cout << "请输入字符串1及要在1中查找的字符串2" << endl;
	cin >> a >> b;
	Strfnd(a, b);
}