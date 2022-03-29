#pragma once
//基于lab1_4的溢出、回绕问题的解决方案
#include<iostream>
#include<math.h>
using namespace std;
void lab1_6()
{
	int a, b, c;
	cout << "请输入底数a" << endl;
	cin >> a;
	cout << "请输入指数b" << endl;
	cin >> b;
	if (pow(a, b) <= INT_MAX)
	{
		c = pow(a, b);
		cout << a << "的" << b << "次方为" << c << endl;
	}
	else
		cout << "数值过大，已溢出。" << endl;
}