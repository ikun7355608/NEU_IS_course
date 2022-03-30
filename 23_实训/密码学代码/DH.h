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
	for (ll i = num-1; i > 2; i--)
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
