#include<iostream>
using namespace std;
int main()
{
	char ch = '\0';
	int key;
	cin >> key;
	//加密
/*	while ((ch = getchar()) != ';')
	{
		if (ch >= 'a' && ch <= 'z')
		{
			cout << (char)((ch - 'a' + key) % 26 + 'a');
		}
		else if (ch >= 'A' && ch <= 'Z')
		{
			cout << (char)((ch - 'A' + key) % 26 + 'A');
		}
	}*/
	//解密
	while ((ch = getchar()) != ';')
	{
		if (ch >= 'a' && ch <= 'z')
		{
			cout << (char)((ch - 'a' - key + 26) % 26 + 'a');
		}
		else if (ch >= 'A' && ch <= 'Z')
		{
			cout << (char)((ch - 'A' - key + 26) % 26 + 'A');
		}
	}
	return 0;
}