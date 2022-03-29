#pragma once
#include<iostream>
using namespace std;
char* Strcpy(char* a,char* b)
{
	while (*b!= '\0')
	{
		*a = *b;
		a++;
		b++;
	}
	*a = '\0';
	return a;
}
void lab2_1()
{
	char a[100], b[100];
	cout << "ÇëÊäÈë×Ö·û´®a,b" << endl;
	cin >> a >> b;
	cout << "¿½±´Ç°:" << endl << "a = " << a << endl << "b = " << b << endl;
	Strcpy(a, b);
	cout << "¿½±´ºó:" << endl << "a = " << a << endl << "b = " << b << endl;
}
