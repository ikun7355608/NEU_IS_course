#include<iostream>
#include<math.h>
#include<string>
using namespace std;
//检查密钥
int checkkey(char* key)
{
	for (int i = 0; i < strlen(key); i++)
	{
		if (key[i] >= 'a' && key[i] <= 'z')
		{
			break;
		}
		else
			cout << "密钥输入有误" << endl;
		return 0;
	}
	//去重
	char temp[20] = {};
		for (int i = 0; i < strlen(key); i++)
		{
			int k = 0;
			for (int j = 0; j < strlen(temp); j++)
			{
				if (key[i] != temp[j])
				{
					k++;
				}
			}
			if (k == strlen(temp))
			{
				temp[strlen(temp)] = key[i];
			}
		}
		for (int i = 0; i < strlen(temp); i++)
		{
			key[i] = temp[i];
		}
		key[strlen(temp)] = '\0';
	return 1;
}
//密钥排序(列读取顺序)
void sort_key(char* key, int* temp)
{
	int i = 0, j = 0, k = 0;

	for (i = 0; i < 26; i++)
	{
		for (j = 0; j < strlen(key); j++)
		{
			if (key[j] == 'a' + i)
			{
				temp[k] = j + 1;
				k++;
			}
		}
	}
}
//明文填充
void fill(char* text, char* key)
{
	int klen = strlen(key), tlen = strlen(text),temp=tlen%klen;
	if (temp != 0)
	{
		//不整除则末尾均填z，直到能构成方阵
		for (int i = 0; i < klen - temp; i++)
		{
			text[tlen + i] = 'z';
		}
	}
}
//加密

void encrypt(char* plaintext, char* key,char* ciphertext)
{
	int k = 0, sort[20] = {};
	sort_key(key,sort);
	int col_num = strlen(plaintext) / strlen(key);
	for (int i = 0; i < strlen(key); i++)
	{
		for (int j = 0; j < col_num; j++)
		{
			//以二维数组的存储方式来模拟矩阵
			//j * strlen(key)即表示第j行
			ciphertext[k] = plaintext[j * strlen(key) + sort[i] - 1];
			k++;
		}
	}
}
//解密
void decrypt(char* plaintext, char* key, char* ciphertext)
{
	//先确定出明文的长度
	for (int i = 0; i < strlen(ciphertext); i++)
	{
		plaintext[i] = 'a';
	}
	int k = 0, sort[20] = {};
	sort_key(key, sort);
	int col_num = strlen(plaintext) / strlen(key);
	for (int i = 0; i < strlen(key); i++)
	{
		for (int j = 0; j < col_num; j++)
		{
			//按照加密时的顺序返回
			plaintext[j * strlen(key) + sort[i] - 1] = ciphertext[k];
				k++;
		}
	}
}
int main()
{
	char text[1000] = {}, rtext[1000] = {}, key[20] = {};
	int k = 1;
	cout << "欢迎使用column permutation 密码" << endl;
	cout << "请选择您要进行的操作" << endl;
	cout << "1.加密" << endl;
	cout << "2.解密" << endl;
	cin >> k;
	while (k)
	{
		cout << "请输入您要进行加/解密的字符" << endl;
		cout << "特别声明：您输入的应均为字母" << endl;
		cin >> text;
		cout << "请输入密钥" << endl;
		cin >> key;
		while (checkkey(key)&&k)
		{
			switch (k)
			{
			case 1:
				fill(text, key);
				encrypt(text, key, rtext);
				cout << "密文为" << endl;
				cout << rtext;
				break;
			case 2:
				decrypt(rtext, key, text);
				cout << "明文为" << endl;
					cout << rtext;
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
		if(k)
		{
			cout << "请输入您要进行加/解密的字符" << endl;
			cin >> text;
			cout << "请输入密钥" << endl;
			cin >> key;
			fill(text, key);
		}
		}
	}
	cout << "欢迎下次使用" << endl;
	return 0;
}
