
#include<iostream>
#include<stdio.h>
using namespace std;
int middle_select(int *x, int *y, int x_low, int x_high, int y_low, int y_high)
{
	//仅有一个元素，直接返回
	if ((x_low == x_high) && (y_high == y_low))
	{
		return (x[x_low] + y[y_low]) / 2;
	}
	else if (x_low == x_high)
	{
		return x[x_low];
	}
	else if (y_low == y_high)
	{
		return y[y_low];
	}
	//各有两个元素，中位数介于二者较小值的最大值和 较大值的最小值之间
	else if ((x_high - x_low == 1) && (y_high - y_low == 1))
	{
		return (max(x[x_low], y[y_low]) + min(x[x_high], y[y_high])) / 2;
	}
	int x_mid, y_mid;
	x_mid = (x_low + x_high) / 2;
	y_mid = (y_low + y_high) / 2;

	if (x[x_mid] > y[y_mid])
	{
		x_high = x_mid;
		y_low = y_mid;
	}
	else if (x[x_mid] < y[y_mid])
	{
		x_low = x_mid;
		y_high = y_mid;
	}
	else
	{
		return x[x_mid];
	}
	middle_select(x, y, x_low, x_high, y_low, y_high);
}

int main()
{
	FILE* Ip, * Op;
	int n;
	Ip = fopen("input.txt", "r");
	fscanf(Ip, "%d", &n);
	int* x = new int[n];
	int* y = new int[n];
	for (int i = 0; i < n; i++)
	{
		fscanf(Ip, "%d", &x[i]);
	}
	for (int i = 0; i < n; i++)
	{
		fscanf(Ip, "%d", &y[i]);
	}
	fclose(Ip);

	int x_low, x_high, y_low, y_high;
	x_low = 0;
	x_high = n - 1;
	y_low = 0;
	y_high = n - 1;

	int mid_Num;
	mid_Num = middle_select(x, y, x_low, x_high, y_low, y_high);
	Op = fopen("output.txt", "w");
	fprintf(Op, "%d", mid_Num);
	fclose(Op);
	delete[]x;
	delete[]y;
	return 0;
}