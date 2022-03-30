#include<iostream>
#include<math.h>
using namespace std;
typedef long long ll;
//素数判断
int prime(int n)
{
	for (int i = 2; i <= pow(n, 0.5); i++)
	{
		if (n % i == 0)
		{
			return	0;
		}
	}
	return 1;
}
//素数表生成
ll* initprime(int m)
{
	ll* num = new ll[m];
	int n = 0;
	for (int i = 10001; m != n; i += 2)
	{
		if (prime(i))
		{
			num[n++] = i;
		}
	}
	return num;
}
//快速幂 x^m mod n
ll q_pow(ll x, ll m, ll n)
{
	if (m == 0)
	{
		return 1;
	}
	ll temp = 1;
	while (m > 0)
	{
		if (m & 1)
		{
			temp = (temp * x) % n;
		}
		x = (x * x) % n;
		m = m / 2;
	}
	return temp;
}
//求原根 求num mod p的原根
ll pri_root(ll num)
{
	int flag = 1;
	for (ll i = 2; i < num; i++)
	{
		flag = 1;
		for (ll j = 1; j < num - 1; j++)
		{
			if (q_pow(i,j,num)==1)
			{
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			return i;
		}
	}
}
//Diffie-Hellman算法
int main()
{
	//生成随机素数
	int m = 1000;
	ll* ptr = initprime(m);
	srand((unsigned)time(NULL));
	ll p = ptr[rand() % m];
	//素数p的最小原根a
	ll a = pri_root(p);
	cout << "公钥p和a分别为p:" << p << "\t" << " a:" << a << endl;
	ll Xa = rand() % p;
	ll Ya = q_pow(a, Xa, p);
	ll Xb = rand() % p;
	ll Yb = q_pow(a, Xb, p);
	cout << "A选取的随机数Xa:" << Xa;
	cout << "\t B发送给A的信息Yb:" << Yb << endl;
	cout << "B选取的随机数Xb:" << Xb;
	cout << "\t A发送给B的信息Ya:" << Ya << endl;
	ll Ka = q_pow(Yb, Xa, p);
	ll Kb = q_pow(Ya, Xb, p);
	cout << "A所得到的密钥K1:" << Ka << endl;
	cout << "B所得到的密钥K2:" << Kb << endl;
	return 0;
}