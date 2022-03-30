#include<iostream>
#include<stdio.h>
using namespace std;
int main()
{
	//n种硬币
	FILE* Ip, * Op;
	int n;
	Ip = fopen("input.txt", "r");
	fscanf(Ip, "%d", &n);
	//coins数组存储已有硬币种类
	int* coins = new int[n];
	for (int i = 0; i < n; i++)
	{
		fscanf(Ip, "%d", &coins[i]);
	}
	//m为要实现的钱数
	int m = 0;
	fscanf(Ip, "%d",&m);
	int* dp = new int[m+1];
	fclose(Ip);
	/// 999默认表示找不到合适方案，即无穷
	for (int i = 1; i <= m; i++)
	{
		dp[i] = 999;
	}
	//0元需要0个
	dp[0] = 0;
	//
	for (int i = 0; i <= m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i >= coins[j])
			{
				dp[i] = min(dp[i - coins[j]] + 1, dp[i]);
			}
		}
	}
	Op = fopen("output.txt", "w");
	if (dp[m] == 999)
	{
		fprintf(Op, "%s", "∞");
	}
	else
	{
		fprintf(Op, "%d", dp[m]);
	}
	fclose(Op);
	delete[]dp;
	delete[]coins;
	return 0;
}