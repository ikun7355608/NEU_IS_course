#pragma once
//递归求斐波那契数列
#include<iostream>
using namespace std;
int fib( int n)
{
	if (n > 2)
		return fib(n - 1) + fib(n - 2);
	else
		return 1;
}
void lab1_5()
{
	int n;
	cout << "请输入要输出斐波那契数列的第几项：" << endl;
	cin >> n;
	cout << "该项为" << fib(n) << endl;
}