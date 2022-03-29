#pragma once
//摄氏度转华氏度
#include<iostream>
using namespace std;
float Convert(float TempFer)
{
	return 5 * (TempFer - 32) / 9;
}
//调试
void lab1_1()
{
	float t1, t2;
	cout << "请输入要转化的华氏温度（单位：℉）" << endl;
	cin >> t1;
	t2 = Convert(t1);
	cout << "对应的摄氏温度为：" << t2 << "℃" << endl;
}
