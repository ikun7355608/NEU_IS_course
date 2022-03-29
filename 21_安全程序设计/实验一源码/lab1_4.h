#pragma once
//使用系统函数计算x的y次方
#include<iostream>
#include<math.h>
using namespace std;
void lab1_4()
{
	int a, b;
	double c;
	cout << "请输入底数a" << endl;
	cin >> a;
	cout << "请输入指数b" << endl;
	cin >> b;
	c = pow(a, b);
	cout <<a<< "的"<<b<<"次方为" << c << endl;
}