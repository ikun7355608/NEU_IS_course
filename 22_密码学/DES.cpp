#include<iostream>
#include<string>
using namespace std;
/*
	由于函数过多，在此标明每个函数开始的行数
	明/密文检测		12；进制转换	33；IT	54； E-BOX 74	密钥检查	96；密钥扩展 120；
	PC-1	140；LF		158；PC-2	185；密钥参与异或	244；S-BOX	252；P-BOX 325
	FT	367; 加密	408；解密	451；main 496；
*/

//明/密文检测(保证输入为16位2进制数）
int check(char* text)
{
	if (strlen(text) != 16)
	{
		cout << "明/密文输入位数错误！" << endl;
		return 0;
	}
	for (int i = 0; i < 16; i++)
	{
		if ((text[i] >= '0' && text[i] <= '9') || (text[i] >= 'A' && text[i] <= 'F'))
		{
			continue;
		}
		else
		{
			cout << "明/密文输入内容错误！" << endl;
			return 0;
		}
	}
	return 1;	
}
//进制转换(16进制转2进制)
void base_conversion(char* intext, char* outtext)
{
	for (int i = 0; i < strlen(intext); i++)
	{
		int temp = 0;
		if (intext[i] >= '0' && intext[i] <= '9') 
		{
			temp = intext[i] - '0';
		}
		else
		{
			temp = intext[i] - 'A' + 10;
		}
		outtext[4 * i] = temp / 8 + '0';
		outtext[4 * i + 1] = (temp / 4) % 2 + '0';
		outtext[4 * i + 2] = (temp / 2) % 2 + '0';
		outtext[4 * i + 3] = temp % 2 + '0';
	}
}
//Initial Transposition
void IT(char* text)
{
	char temp[70] = {1};
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				temp[i + 8 * j + 32 * k] = text[57 - 8 * i + 2 * j - k];
			}
		}
	}	
	for (i = 0; i < 64; i++)
	{
		text[i] = temp[i];
	}
}
//E-BOX
void e_box(char* rp,char* temp)
{
	//首尾特殊
	temp[0] = rp[31];
	temp[47] = rp[0];
	for (int i = 0; i < 8; i++)
	{
		if (i != 0)
		{
			temp[6 * i] = rp[4 * i - 1];
		}
		for (int j = 0; j < 4; j++)
		{
			temp[6 * i + 1 + j] = rp[4 * i + j];
		}
		if (i != 7)
		{
			temp[6 * i + 5] = rp[4 * i + 4];
		}
	}
}
//密钥检查（14位16进制数）
int check_key(char* key)
{
	{
		if (strlen(key) != 14)
		{
			cout << "密钥输入位数错误！" << endl;
			return 0;
		}
		for (int i = 0; i < 14; i++)
		{
			if ((key[i] >= '0' && key[i] <= '9' )|| (key[i] >= 'A' && key[i] <= 'F'))
			{
				continue;
			}
			else
			{
				cout << "密钥输入内容错误！" << endl;
				return 0;
			}
		}
		return 1;
	}
}
//密钥扩展（16进制转2进制并加入奇偶校验位）
void extend_key(char* intext,char* outtext)
{
	base_conversion(intext, outtext);
	char temp[70] = { 0 };
	for (int i = 0; i < 8; i++)
	{
		int s = 1;
		for (int j = 0; j < 7; j++)
		{
			temp[8 * i + j] = outtext[7 * i + j];
			s += outtext[7 * i + j] - '0';
		}
		temp[8 * i + 7] = s % 2 + '0';
	}
	for (int i = 0; i < 64; i++)
	{
		outtext[i] = temp[i];
	}
}
//PC-1
void pc_1(char* key,char* temp)
{
	for (int i = 0; i < 8; i++)
	{
		temp[i] = key[56 - 8 * i];
		temp[i + 8] = key[57 - 8 * i];
		temp[i + 16] = key[58 - 8 * i];
		temp[i + 28] = key[62 - 8 * i];
		temp[i + 36] = key[61 - 8 * i];
		temp[i + 44] = key[60 - 8 * i];
	}
	for (int i = 0; i < 4; i++)
	{
		temp[i + 24] = key[59 - 8 * i];
		temp[i + 52] = key[27 - 8 * i];
	}
}
//循环左移 left shift
void LF(char* key, int i)
{
	char temp[30] = {};
	//第1、2、9、16轮左移一位
	if (i == 0 || i == 1 || i == 8 || i == 15)
	{
		for (int j = 0; j < 27; j++)
		{
			temp[j] = key[j + 1];
		}
		temp[27] = key[0];
	}
	else
	{
		for (int j = 0; j < 26; j++)
		{
			temp[j] = key[j + 2];
		}
		temp[26] = key[0];
		temp[27] = key[1];
	}
	for (i = 0; i < 28; i++)
	{
		key[i] = temp[i];
	}
}
// PC-2 未找到规律，手动输入  l_key,left key,密钥左半部分；r_key，right key，密钥右半部分。
void pc_2(char* l_key, char* r_key,char* key)
{
	char temp[70] = {};
	for (int i = 0; i < 28; i++)
	{
		temp[i] = l_key[i];
		temp[28 + i] = r_key[i];
	}
	//手动赋值
	key[0] = temp[13];
	key[1] = temp[16];
	key[2] = temp[10];
	key[3] = temp[23];
	key[4] = temp[0];
	key[5] = temp[4];
	key[6] = temp[2];
	key[7] = temp[27];
	key[8] = temp[14];
	key[9] = temp[5];
	key[10] = temp[20];
	key[11] = temp[9];
	key[12] = temp[22];
	key[13] = temp[18];
	key[14] = temp[11];
	key[15] = temp[3];
	key[16] = temp[25];
	key[17] = temp[7];
	key[18] = temp[15];
	key[19] = temp[6];
	key[20] = temp[26];
	key[21] = temp[19];
	key[22] = temp[12];
	key[23] = temp[1];
	key[24] = temp[40];
	key[25] = temp[51];
	key[26] = temp[30];
	key[27] = temp[36];
	key[28] = temp[46];
	key[29] = temp[54];
	key[30] = temp[29];
	key[31] = temp[39];
	key[32] = temp[50];
	key[33] = temp[44];
	key[34] = temp[32];
	key[35] = temp[47];
	key[36] = temp[43];
	key[37] = temp[48];
	key[38] = temp[38];
	key[39] = temp[55];
	key[40] = temp[33];
	key[41] = temp[52];
	key[42] = temp[45];
	key[43] = temp[41];
	key[44] = temp[49];
	key[45] = temp[35];
	key[46] = temp[28];
	key[47] = temp[31];
}
//密钥与扩展后的RP异或
void k_xor_p(char* key,char* text)
{;
	for (int i = 0; i < 48; i++)
	{
		text[i] = (key[i] - '0' + text[i] - '0') % 2 + '0';   //异或
	}
}
// S-BOX 
void s_box(char* text,char* rtext)
{
	//8个4行16列的盒子
	int box[8][4][16] = {
		{ 
			//S-BOX1
			{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
			{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
			{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
			{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
		},
		{  
			//S-BOX2
			{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
			{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
			{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
			{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
		},
		{
			//S-BOX3
			{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
			{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
			{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
			{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
		},
		{
			//S-BOX4
			{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
			{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
			{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
			{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
		},
		{
			//S-BOX5
			{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
			{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
			{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
			{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
		},
		{
			//S-BOX6
			{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
			{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
			{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
			{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
		},
		{
			//S-BOX7
			{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
			{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
			{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
			{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
		},
		{
			//S-BOX8
			{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
			{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
			{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
			{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
		}
	};
	for (int k = 0; k < 8; k++)  //S-BOX数
	{
		int i = (text[6 * k] - '0') * 2 + text[6 * k + 5] - '0';  //行数
		int j = (text[6 * k + 1] - '0') * 8 + (text[6 * k + 2] - '0') * 4 + (text[6 * k + 3] - '0') * 2 + text[6 * k + 4] - '0';  //列数
		int temp = box[k][i][j];  
		rtext[4 * k] = temp / 8 + '0';
		rtext[4 * k + 1] = (temp / 4) % 2 + '0';
		rtext[4 * k + 2] = (temp / 2) % 2 + '0';
		rtext[4 * k + 3] = temp % 2 + '0';
	}
}
//  P-BOX text为rp,temp为操作后的结果
void p_box(char* text)
{
	char temp[50] = {};
	temp[0] = text[15];
	temp[1] = text[6];
	temp[2] = text[19];
	temp[3] = text[20];
	temp[4] = text[28];
	temp[5] = text[11];
	temp[6] = text[27];
	temp[7] = text[16];
	temp[8] = text[0];
	temp[9] = text[14];
	temp[10] = text[22];
	temp[11] = text[25];
	temp[12] = text[4];
	temp[13] = text[17];
	temp[14] = text[30];
	temp[15] = text[9];
	temp[16] = text[1];
	temp[17] = text[7];
	temp[18] = text[23];
	temp[19] = text[13];
	temp[20] = text[31];
	temp[21] = text[26];
	temp[22] = text[2];
	temp[23] = text[8];
	temp[24] = text[18];
	temp[25] = text[12];
	temp[26] = text[29];
	temp[27] = text[5];
	temp[28] = text[21];
	temp[29] = text[10];
	temp[30] = text[3];
	temp[31] = text[24];
	for (int i = 0; i < 32; i++)
	{
		text[i] = temp[i];
	}

}
//Finial Transposition
void FT(char* lp, char* rp)
{
	char temp[70] = {}, text[70] = {};
	for (int i = 0; i < 32; i++)
	{
		temp[i] = rp[i];
		temp[32 + i] = lp[i];
	}
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				text[1 + 8 * i + 2 * j - k] = temp[7 - i + 8 * j + 32 * k];
			}
		}
	}
	cout << "二进制表示为：" << endl;
	for (int i = 0; i < 64; i++)
	{
		cout << text[i];
	}
	cout << endl;
	cout << "十六进制表示为:" << endl;
	for (int i = 0; i < 16; i++)
	{
		int sum = 0;
		for (int j = 0; j < 4; j++)
		{
			sum = sum * 2 + text[4 * i + j] - '0';
		}
		if (sum < 10)
		{
			cout << char(sum + '0');
		}
		else
			cout << char(sum + 'A' - 10);
	}
}
//加密(建立在明文、密钥均处理完成的基础上)
void encrypt(char* ptext,char* ktext)
{
	IT(ptext);
	char lp[40] = {}, rp[40] = {};
	for (int i = 0; i < 32; i++)
	{
		lp[i] = ptext[i];		//明文左半端 left plaintext
		rp[i] = ptext[i + 32];
	}
	char k_temp[70] = {};
	pc_1(ktext, k_temp);
	char lk[40] = {}, rk[40] = {};
	for (int i = 0; i < 28; i++)
	{
		lk[i] = k_temp[i];		//密钥左半端 left key
		rk[i] = k_temp[28 + i];
	}
	//16次循环
	char key[50] = {}; //处理后的密钥(48bit)
	char e_ptext[50] = {};	//经e_box扩展后的明文(48bit)
	char s_ptext[50] = {};	//经s_box压缩后的明文(32bit)
	for (int i = 0; i < 16; i++)
	{
		//密钥生成
		LF(lk, i);
		LF(rk, i);
		pc_2(lk, rk, key); 
		//明文处理
		e_box(rp, e_ptext);
		k_xor_p(key, e_ptext);
		s_box(e_ptext, s_ptext);
		p_box(s_ptext);
		//LP,RP交换
		for (int j = 0; j < 32; j++)
		{
			s_ptext[j] = (lp[j] - '0' + s_ptext[j] - '0') % 2 + '0';
			lp[j] = rp[j];
			rp[j] = s_ptext[j];
		}
	}
	FT(lp, rp);
}
//解密
void decrypt(char* ctext, char* ktext)
{
	IT(ctext);
	char lp[40] = {}, rp[40] = {};
	for (int i = 0; i < 32; i++)
	{
		lp[i] = ctext[i];		//明文左半端 left plaintext
		rp[i] = ctext[i + 32];
	}
	char k_temp[70] = {};
	pc_1(ktext, k_temp);
	char lk[40] = {}, rk[40] = {};
	for (int i = 0; i < 28; i++)
	{
		lk[i] = k_temp[i];		//密钥左半端 left key
		rk[i] = k_temp[28 + i];
	}
	//16次循环
	char key[16][50] = {}; //处理后的密钥(48bit)
	char e_ptext[50] = {};	//经e_box扩展后的明文(48bit)
	char s_ptext[50] = {};	//经s_box压缩后的明文(32bit)
	for (int i = 0; i < 16; i++)
	{
		//密钥生成
		LF(lk, i);
		LF(rk, i);
		pc_2(lk, rk, key[i]);
	}
	for (int i = 0; i < 16; i++)
	{
		//明文处理
		e_box(rp, e_ptext);
		k_xor_p(key[15-i], e_ptext);	//最后一轮的密钥第一轮使用
		s_box(e_ptext, s_ptext);
		p_box(s_ptext);
		//LP,RP交换
		for (int j = 0; j < 32; j++)
		{
			s_ptext[j] = (lp[j] - '0' + s_ptext[j] - '0') % 2 + '0';
			lp[j] = rp[j];
			rp[j] = s_ptext[j];
		}
	}
	FT(lp, rp);
}

int main()
{
	char text16[20] = {}, key16[20] = {}; //输入16进制（便于输入及校验）
	char text2[70] = {}, key2[70] = {};
	int k=0;	
	cout << "欢迎使用DES加/解密算法" << endl;
	cout << "请选择您要执行的操作" << endl;
	cout << "1.加密" << endl;
	cout << "2.解密" << endl;
	cin >> k;
	cout << "请输入16位16进制明/密文" << endl;
	cout << "注：A--F大写" << endl;
	cin >> text16;
	cout << "请输入14位16进制密钥" << endl;
	cout << "注：A--F大写" << endl;
	cin >> key16;
	while (k)
	{
		if (check(text16) && check_key(key16))
		{
			base_conversion(text16, text2);
			extend_key(key16, key2);
			switch (k)
			{
			case 1:
				encrypt(text2, key2);
				break;
			case 2:
				decrypt(text2, key2);
				break;
			default:
				cout << "输入有误!" << endl;
				break;
			}
		}
		system("pause");
		system("cls");
		cout << "欢迎使用DES加/解密算法" << endl;
		cout << "请选择您要执行的操作" << endl;
		cout << "1.加密" << endl;
		cout << "2.解密" << endl;
		cout << "按0退出" << endl;
		cin >> k;
		if (k)
		{
			cout << "请输入16位16进制明/密文" << endl;
			cout << "注：A--F大写" << endl;
			cin >> text16;
			cout << "请输入14位16进制密钥" << endl;
			cout << "注：A--F大写" << endl;
			cin >> key16;
		}
	}
	cout << "欢迎下次使用" << endl;
	return 0;
}