#include<iostream>
using namespace std;
int main()
{
	char ch = '\0';
	char key[6] = {};
	cin >> key;
	while ((ch = getchar()) != ';')
	{
		if (ch >= 'a' && ch <= 'z')
		{
			if (ch <= 'i')
			{
				cout << key[(ch - 'a') / 5];
				cout << key[(ch - 'a') % 5];
			}
			else
			{
				cout << key[(ch - 'a' - 1) / 5];
				cout << key[(ch - 'a' - 1) % 5];
			}
		}
		else if (ch >= 'A' && ch <= 'Z')
		{
			if (ch <= 'I')
			{
				cout << key[(ch - 'A') / 5];
				cout << key[(ch - 'A') % 5];
			}
			else
			{
				cout << key[(ch - 'A' - 1) / 5];
				cout << key[(ch - 'A' - 1) % 5];
			}
		}
	}
	return 0;
}