#include<iostream>
#include"lab2_1.h"
#include"lab2_2.h"
#include"lab2_3.h"
#include"lab2_4.h"
#include"lab2_5.h"
using namespace std;

void fun()
{
	cout << "请输入要运行的程序编号,按0退出(1,2,3…)：" << endl;
	cout << "1.strcpy函数（两个字符串之间的拷贝）" << endl;
	cout << "2.strcat函数(字符串链接)" << endl;
	cout << "3.strfnd函数(字符串查找)" << endl;
	cout << "4.不使用虚函数的自定义类" << endl;
	cout << "5.使用虚函数的自定义类" << endl;
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
			lab2_1();
			break;
		case 2:
			lab2_2();
			break;
		case 3:
			lab2_3();
			break;
		case 4:
			lab2_4();
			break;
		case 5:
			lab2_5();
			break;
		default:
			cout << "输入错误，请重新输入:";
		}
		system("pause");
		system("cls");
		fun();
		cin >> a;
	}
	cout << "^_^本次调试结束，欢迎下次使用。^_^" << endl;
	return 0;
}
