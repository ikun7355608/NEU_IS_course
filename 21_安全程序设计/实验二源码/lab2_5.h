#pragma once
#include<iostream>
using namespace std;
class Vehicle2 //车基类
{
public:
	virtual void Run()
	{
		cout << "车启动了	";
	}
	virtual void Stop()
	{
		cout << "车停了" << endl;
	}
};
class Bicycle2 :public Vehicle2 //自行车派生类
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
class Motorcar2 :public Vehicle2 //汽车派生类
{
public:
	void Run()
	{
		cout << "汽车启动了	";
	}
	void Stop()
	{
		cout << "汽车停了" << endl;
	}
};

class Motorcycle2 :public Bicycle2, public Motorcar2 //摩托车派生类
{
public:
	void Run()
	{
		cout << "摩托车启动了	";
	}
	void Stop()
	{
		cout << "摩托车停了" << endl;
	}
};
void lab2_5()
{
	cout << "未使用指针：" << endl;
	//Vehicle类对象并调用其Run()、Stop()函数
	Vehicle2 v;
	v.Run();
	v.Stop();
	//Bicycle类对象并调用其Run()、Stop()函数
	Bicycle2 b;
	b.Run();
	b.Stop();
	//Motorcar类对象并调用其Run()、Stop()函数
	Motorcar2 m;
	m.Run();
	m.Stop();
	//Motorcycle类对象并调用其Run()、Stop()函数
	Motorcycle2 mc;
	mc.Run();
	mc.Stop();
	cout << "使用指针：" << endl;
	//通过指针实现
	Vehicle2* p;
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
