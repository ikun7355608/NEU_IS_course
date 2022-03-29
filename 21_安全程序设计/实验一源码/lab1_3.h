#pragma once
//通过函数模板实现函数重载
#include<iostream>
using namespace std;
template<typename T>
T max1(T x, T y)
{
	if (x > y)
		return x;
	else
		return y;
}
template<typename T>
T max1(T x, T y, T z)
{
	T temp = x;
	if (y > temp)
		temp = y;
	if (z > temp)
		temp = z;
	return temp;
}
//调试
void lab1_3()
{
	float a1, b1, c1;
	int a2, b2, c2;
	cout << "请输入两个整数" << endl;
	cin >> a2 >> b2;
	cout << "最大值为：" << max1(a2, b2) << endl;
	cout << "请输入三个整数" << endl;
	cin >> a2 >> b2 >> c2;
	cout << "最大值为：" << max1(a2, b2, c2) << endl;
	cout << "请输入两个浮点数" << endl;
	cin >> a1 >> b1;
	cout << max1(a1, b1) << endl;
	cout << "请输入三个浮点数" << endl;
	cin >> a1 >> b1 >> c1;
	cout << "最大值为：" << max1(a1, b1, c1) << endl;
}