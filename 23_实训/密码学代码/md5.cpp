#include<iostream>
using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;
#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476
uint A_temp = 0, B_temp = 0, C_temp = 0, D_temp = 0;
//ti矩阵  4294967296*abs(sin(i))
uint slen = 0;
uint t[] = {
	0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
	0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
	0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
	0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
	0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
	0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
	0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
	0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391 };
//循环左移位数
uint s[] = {
	7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
	5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
	4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
	6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21 };
//左移
uint left_move(uint x, uint y)
{
	return (x << y) | (x >> (32 - y));
}
//四个扰乱函数
uint F(uint x, uint y, uint z)
{
	return ((x & y) | ((~x) & z));
}
uint G(uint x, uint y, uint z)
{
	return ((x & z) | (y & (~z)));
}
uint H(uint x, uint y, uint z)
{
	return (x ^ y ^ z);
}
uint I(uint x, uint y, uint z)
{

	return (y ^ (x | (~z)));
}
void FF(uint &a, uint b, uint c, uint d, uint m, uint s, uint t)
{
	a = b + left_move((a + F(b, c, d) + m + t), s);
}
void GG(uint &a, uint b, uint c, uint d, uint m, uint s, uint t)
{
	a = b + left_move((a + G(b, c, d) + m + t), s);
}
void HH(uint &a, uint b, uint c, uint d, uint m, uint s, uint t)
{
	a = b + left_move((a + H(b, c, d) + m + t), s);
}
void II(uint &a, uint b, uint c, uint d, uint m, uint s, uint t)
{
	a = b + left_move((a + I(b, c, d) + m + t), s);
}
//压缩函数
void process(uint M[])
{
	uint a = A_temp, b = B_temp, c = C_temp, d = D_temp;
	int i = 0;
	FF(a, b, c, d, M[0], s[i], t[i]); i++;
	FF(d, a, b, c, M[1], s[i], t[i]); i++;
	FF(c, d, a, b, M[2], s[i], t[i]); i++;
	FF(b, c, d, a, M[3], s[i], t[i]); i++;
	FF(a, b, c, d, M[4], s[i], t[i]); i++;
	FF(d, a, b, c, M[5], s[i], t[i]); i++;
	FF(c, d, a, b, M[6], s[i], t[i]); i++;
	FF(b, c, d, a, M[7], s[i], t[i]); i++;
	FF(a, b, c, d, M[8], s[i], t[i]); i++;
	FF(d, a, b, c, M[9], s[i], t[i]); i++;
	FF(c, d, a, b, M[10], s[i], t[i]); i++;
	FF(b, c, d, a, M[11], s[i], t[i]); i++;
	FF(a, b, c, d, M[12], s[i], t[i]); i++;
	FF(d, a, b, c, M[13], s[i], t[i]); i++;
	FF(c, d, a, b, M[14], s[i], t[i]); i++;
	FF(b, c, d, a, M[15], s[i], t[i]); i++;

	GG(a, b, c, d, M[1], s[i], t[i]); i++;
	GG(d, a, b, c, M[6], s[i], t[i]); i++;
	GG(c, d, a, b, M[11], s[i], t[i]); i++;
	GG(b, c, d, a, M[0], s[i], t[i]); i++;
	GG(a, b, c, d, M[5], s[i], t[i]); i++;
	GG(d, a, b, c, M[10], s[i], t[i]); i++;
	GG(c, d, a, b, M[15], s[i], t[i]); i++;
	GG(b, c, d, a, M[4], s[i], t[i]); i++;
	GG(a, b, c, d, M[9], s[i], t[i]); i++;
	GG(d, a, b, c, M[14], s[i], t[i]); i++;
	GG(c, d, a, b, M[3], s[i], t[i]); i++;
	GG(b, c, d, a, M[8], s[i], t[i]); i++;
	GG(a, b, c, d, M[13], s[i], t[i]); i++;
	GG(d, a, b, c, M[2], s[i], t[i]); i++;
	GG(c, d, a, b, M[7], s[i], t[i]); i++;
	GG(b, c, d, a, M[12], s[i], t[i]); i++;

	HH(a, b, c, d, M[5], s[i], t[i]); i++;
	HH(d, a, b, c, M[8], s[i], t[i]); i++;
	HH(c, d, a, b, M[11], s[i], t[i]); i++;
	HH(b, c, d, a, M[14], s[i], t[i]); i++;
	HH(a, b, c, d, M[1], s[i], t[i]); i++;
	HH(d, a, b, c, M[4], s[i], t[i]); i++;
	HH(c, d, a, b, M[7], s[i], t[i]); i++;
	HH(b, c, d, a, M[10], s[i], t[i]); i++;
	HH(a, b, c, d, M[13], s[i], t[i]); i++;
	HH(d, a, b, c, M[0], s[i], t[i]); i++;
	HH(c, d, a, b, M[3], s[i], t[i]); i++;
	HH(b, c, d, a, M[6], s[i], t[i]); i++;
	HH(a, b, c, d, M[9], s[i], t[i]); i++;
	HH(d, a, b, c, M[12], s[i], t[i]); i++;
	HH(c, d, a, b, M[15], s[i], t[i]); i++;
	HH(b, c, d, a, M[2], s[i], t[i]); i++;

	II(a, b, c, d, M[0], s[i], t[i]); i++;
	II(d, a, b, c, M[7], s[i], t[i]); i++;
	II(c, d, a, b, M[14], s[i], t[i]); i++;
	II(b, c, d, a, M[5], s[i], t[i]); i++;
	II(a, b, c, d, M[12], s[i], t[i]); i++;
	II(d, a, b, c, M[3], s[i], t[i]); i++;
	II(c, d, a, b, M[10], s[i], t[i]); i++;
	II(b, c, d, a, M[1], s[i], t[i]); i++;
	II(a, b, c, d, M[8], s[i], t[i]); i++;
	II(d, a, b, c, M[15], s[i], t[i]); i++;
	II(c, d, a, b, M[6], s[i], t[i]); i++;
	II(b, c, d, a, M[13], s[i], t[i]); i++;
	II(a, b, c, d, M[4], s[i], t[i]); i++;
	II(d, a, b, c, M[11], s[i], t[i]); i++;
	II(c, d, a, b, M[2], s[i], t[i]); i++;
	II(b, c, d, a, M[9], s[i], t[i]); i++;

	A_temp += a;
	B_temp += b;
	C_temp += c;
	D_temp += d;
}

//填充至 bit=448 mod512
uint* padding(string str)
{
	//每512bit分为1组，进行一次循环。确定循环次数，即N个M[16]
	// (length*8+64)/512 向上取整
	uint num = (str.length() + 8) / 64 + 1;
	slen = num * 16;
	//每轮16块
	uint* n_str = new uint[slen];
	//均初始化为0
	for (uint i = 0; i < slen; i++)
	{
		n_str[i] = 0;
	}
	//一块 32bit  一个ascii 8bit  
	for (uint i = 0; i < str.length(); i++) 
	{
		n_str[i / 4] |= (str[i]) << ((i % 4) * 8);
	}
	//448部分的明文尾部追加10000....  0x80 128 10000000
	n_str[str.length() / 4] |= 0x80 << ((str.length() % 4) * 8);
	//最后64bit的长度
	n_str[num * 16 - 2] = str.length() * 8;
	//返回
	return n_str;
}
string toHex(uint num)
{
	int x;
	string tp;
	char HexArray[] = "0123456789abcdef";
	string str = "";
	for (int i = 0; i < 4; i++)
	{
		tp = "";
		x = ((num >> i * 8) % (1 << 8)) & 0xff;
		for (int j = 0; j < 2; j++)
		{
			tp.insert(0, 1, HexArray[x % 16]);
			x /= 16;
		}
		str += tp;
	}
	return str;
}
int main()
{
	A_temp = A;
	B_temp = B;
	C_temp = C;
	D_temp = D;
	string ptext;
	cin >> ptext;
	uint* temp = padding(ptext);
	for (int i = 0; i < slen/16; i++)
	{
		uint num[16] = {};
		for (int j = 0; j < 16; j++)
		{
			num[j] = temp[16 * i + j];
		}
		process(num);
	}
	string str = "";
	str += toHex(A_temp);
	str += toHex(B_temp);
	str += toHex(C_temp);
	str += toHex(D_temp);
	cout << str;
/*	char ch[4][8] = {};
	_itoa(A_temp, ch[0], 16);
	_itoa(B_temp, ch[1], 16);
	_itoa(C_temp, ch[2], 16);
	_itoa(D_temp, ch[3], 16);
	char f_ch[40] = {};
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			f_ch[j + 8 * i] = ch[i][j];
		}
	}
	cout << f_ch;*/
	return 0;
}