#include<iostream>
using namespace std;
int main()
{
	char ch = '\0';
	int a,b;
	cin >> a;
	cin >> b;
	//加密
/*
	while ((ch = getchar()) != ';')
	{
		if (ch >= 'a' && ch <= 'z')
		{
			cout << (char)(((ch - 'a') * a + b) % 26 + 'a');
		}
		else if (ch >= 'A' && ch <= 'Z')
		{
			cout << (char)(((ch - 'A') * a + b) % 26 + 'A');
		}
	}
*/
	//解密
	for (int i = 0; i < 26; i++)
	{
		if ((a * i) % 26 == 1)
		{
			a = i;
		}
	}//乘法逆元
	while ((ch = getchar()) != ';')
	{
		if (ch >= 'a' && ch <= 'z')
		{
			cout << (char)((((ch - 'a') - b + 26) * a) % 26 + 'a');
		}
		else if (ch >= 'A' && ch <= 'Z')
		{
			cout << (char)((((ch - 'A') - b + 26) * a) % 26 + 'A');
		}
	}
	return 0;
}