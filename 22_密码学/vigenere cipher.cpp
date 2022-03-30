#include<iostream>
#include<string>
using namespace std;

//加解密算法中密文ciphertext记为c；明文plaintext记为p
//加密算法
int encrypt(char* c, char* p, char* key)
{
	//klen为密钥长度keylength缩写 len表示明/密文长度
	int klen = strlen(key), len = strlen(p), j = 0;
	for (int i = 0; i < len; i++)
	{
		//由于密钥是循环的，i%keylength结果j即为第i次加密时所使用的密钥为key中的第j位
		j = i % klen;
		//考虑明文中的空格、大小写及越界
		if (p[i] >= 'a' && p[i] <= 'z')
		{
			c[i] = (p[i] - 'a' + key[j] - 'a') % 26 + 'a';
		}
		else if (p[i] >= 'A' && p[i] <= 'Z')
		{
			c[i] = (p[i] - 'A' + key[j] - 'a') % 26 + 'A';
		}
		/*
		else if (p[i] == ' ')
		{
			c[i] = p[i];
		}
		*/
		else
		{
			cout << "输入错误" << endl;
			return 0;
		}
	}
	//输出结果
	for (int i = 0; i < len; i++)
	{
		/*
		if (p[i] != ' ')
		{
			cout << c[i];
		}
		*/
		cout << c[i];
	}
	return 0;
}
//解密算法
int decrypt(char* c, char* p, char* key)
{
	int klen = strlen(key), len = strlen(c), j = 0;
	//考虑密文中大小写、空格及越界
	for (int i = 0; i < len; i++)
	{
		j = i % klen;
		if (c[i] >= 'a' && c[i] <= 'z')
		{
			p[i] = (c[i] - key[j] + 26) % 26 + 'a';
		}
		/*
		else if (c[i] == ' ')
		{
			p[i] = c[i];
		}
		*/
		else if (c[i] >= 'A' && c[i] <= 'Z')
		{
			p[i] = (c[i] - 'A' - key[j] + 'a' + 130) % 26 + 'A';
		}
		else
		{
			cout << "输入错误" << endl;
			return 0;
		}
	}
	for (int i = 0; i < len; i++)
	{
		/*
		if (p[i] != ' ')
		{
			cout << p[i];
		}
		*/
		cout << p[i];
	}
	return 0;
}
int main()
{

	//text为输入的字符串,rtext为函数所返回的明/密文
	char text[1000] = {}, rtext[1000] = {}, keyword[30] = {};
	int k = 1;
	cout << "欢迎使用vigenere密码" << endl;
	cout << "请选择您要进行的操作" << endl;
	cout << "1.加密" << endl;
	cout << "2.解密" << endl;
	cout << "按0退出" << endl;
	cin >> k;
	while (k != 0)
	{
		cout << "请输入您要进行加/解密的字符" << endl;
		cin >> text;
		cout << "请输入密钥" << endl;
		cin >> keyword;
		switch (k)
		{
		case 1:
			encrypt(rtext, text, keyword);
			break;
		case 2:
			decrypt(text, rtext, keyword);
			break;
		default:
			cout << "输入错误，请重新输入" << endl;
			break;
		}
		system("pause");
		system("cls");
		cout << "请选择您要进行的操作" << endl;
		cout << "1.加密" << endl;
		cout << "2.解密" << endl;
		cout << "按0退出" << endl;
		cin >> k;
	}
	cout << "欢迎下次使用" << endl;
	return 0;
}