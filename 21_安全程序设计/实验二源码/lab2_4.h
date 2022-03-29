#pragma once
#include<iostream>
using namespace std;
class Vehicle1 //车基类
{
public:
	void Run()
	{
		cout << "车启动了	" ;
	}
	void Stop()
	{
		cout << "车停了" << endl;
	}
};
class Bicycle1:public Vehicle1 //自行车派生类
{
public:
	void Run()
	{
		cout << "自行车启动了	";
	}
	void Stop()
	{
		cout << "自行车停了" << endl;
	}
};
class Motorcar1 :public Vehicle1 //汽车派生类
{
public:
	void Run()
	{
		cout << "汽车启动了	" ;
	}
	void Stop()
	{
		cout << "汽车停了" << endl;
	}
};

class Motorcycle1 :public Bicycle1, public Motorcar1 //摩托车派生类
{
public:
	void Run()
	{
		cout << "摩托车启动了	" ;
	}
	void Stop()
	{
		cout << "摩托车停了" << endl;
	}
};
void lab2_4()
{
	cout << "未使用指针：" << endl;
	//Vehicle类对象并调用其Run()、Stop()函数
	Vehicle1 v;
	v.Run();
	v.Stop();
	//Bicycle类对象并调用其Run()、Stop()函数
	Bicycle1 b;
	b.Run();
	b.Stop();
	//Motorcar类对象并调用其Run()、Stop()函数
	Motorcar1 m;
	m.Run();
	m.Stop();
	//Motorcycle类对象并调用其Run()、Stop()函数
	Motorcycle1 mc;
	mc.Run();
	mc.Stop();
	cout << "使用指针：" << endl;
	//通过指针实现
	Vehicle1* p;
	//Vehicle类型的指针来调用Vehicle类对象的成员函数
	p = &v;
	p->Run();
	p->Stop();
	//Vehicle类型的指针来调用Bicycle类对象的成员函数
	p = &b;
	p->Run();
	p->Stop();
	//Vehicle类型的指针来调用Motorcar类对象的成员函数
	p = &m;
	p->Run();
	p->Stop();
}