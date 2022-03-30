#include<iostream>
#include<string>
using namespace std;
int main()
{
	char ch = '\0';
	char key[20] = {};
	char ptext[100] = {};
	char ctext[100] = {};
	int i = 0;
	cin >> key;
/*	//¼ÓÃÜ
	cin >> ptext;
	for (i = 0; i < strlen(ptext); i++)
	{
		if (i < strlen(key))
			ctext[i] = char((key[i] - 'a' + ptext[i] - 'a') % 26 + 'a');
		else
			ctext[i] = char((ptext[i - strlen(key)] - 'a' + ptext[i] - 'a') % 26 + 'a');
	}
	for (i = 0; i < strlen(ptext); i++)
	{
		cout << ctext[i];
	}*/
	//½âÃÜ
	cin >> ctext;
	for (i = 0; i < strlen(ctext); i++)
	{
		if (i < strlen(key))
			ptext[i] = (ctext[i] + 26 - key[i]) % 26 + 'a';
		else
			ptext[i] = (ctext[i] + 26 - ptext[i - strlen(key)]) % 26 + 'a';
	}
	return 0;
}