#include<iostream>
#include<math.h>
using namespace std;
//最大公因数
typedef long long ll;
ll ExEuclid(ll a, ll b)
{
	if (b > a)
	{
		ll temp = b;
		b = a;
		a = temp;
	}
	if (a % b != 0)
	{
		ExEuclid(b, a % b);
	}
	else
	{
		return b;
	}
}
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
//素数生成

void rsa_encrypt()
{
	int m = 1000;
	ll* p = initprime(m);
	srand((unsigned)time(NULL));
	//生成随机数
	ll prime_p = p[rand() % m];
	ll prime_q = p[rand() % m];
	ll n = prime_p * prime_q;
	ll fn = (prime_p - 1) * (prime_q - 1);
	ll e = rand() % fn;
	//生成e与fn互素.
	while (ExEuclid(e, fn) != 1)
	{
		e = rand() % fn;
	}
	ll d = 0;
	for (d = 1; d < fn; d++)
	{
		if ((d * e) % fn == 1)
		{
			break;
		}
	}
	//加密
	ll ptext = 0;
	while (!(ptext > 1 && ptext < n))
	{
		cout << "please input plaintext";
		cin >> ptext;
	}
	ll ctext = q_pow(ptext, e, n);
	cout << ctext;
	//解密
	/*
	ll ctext = 0;
	while (!(ctext > 1 && ctext < n))
	{
		cout << "输入密文" << endl;
		cin >> ctext;
	}
	ll ptext = q_pow(ctext, d, n);*/

}
int main()
{
	rsa_encrypt();
	return 0;
}