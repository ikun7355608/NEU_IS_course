#include<iostream>
#include"lab1_1.h"
#include"lab1_2.h"
#include"lab1_3.h"
#include"lab1_4.h"
#include"lab1_5.h"
#include"lab1_6.h"
using namespace std;
//摄氏度转华氏度
void fun()
{
	cout << "请输入要运行的程序编号,按0退出(1,2,3…)：" << endl;
	cout << "1.摄氏度转华氏度" << endl;
	cout << "2.手动编写函数重载" << endl;
	cout << "3.通过函数模板实现函数重载" << endl;
	cout << "4.使用系统函数求出a的b次方" << endl;
	cout << "5.求斐波那契数列第n项" << endl;
	cout << "6.使用系统函数求a的b次方（溢出判断）" << endl;
}
int main()
{
	int a;
	fun();
	cin >> a;
	while (a)
	{
		switch (a)
		{
		case 1:
			lab1_1();
			break;
		case 2:
			lab1_2();
			break;
		case 3:
			lab1_3();
			break;
		case 4:
			lab1_4();
			break;
		case 5:
			lab1_5();
			break;
		case 6:
			lab1_6();
			break;
		default:
			cout<<"输入错误，请重新输入:";
		}
		system("pause");
		system("cls");
		fun();
		cin >> a;
	}
	cout << "^_^本次调试结束，欢迎下次使用。^_^" << endl;
	return 0;
}
