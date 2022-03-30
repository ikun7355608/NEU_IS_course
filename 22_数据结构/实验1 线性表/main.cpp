#include<iostream>
#include<string>
#include<math.h>
using namespace std;
//定义
typedef struct Node {
	int num;
	struct Node* prior, * next;
}Dlink;
//初始化,返回头节点
Dlink *init()
{
	Dlink* head=NULL, * last=NULL, * p=NULL;
	char ch = {};
	int num = 0,flag=1;	//flag用于判断正负
	head = (Dlink*)malloc(sizeof(Dlink));
	head->next = NULL;
	head->prior = NULL;
	head->num = 0;
	while ((ch = getchar()) != ';')
	{
		//消除第二个数输入前的回车影响。
		if (ch == '\n')
		{
			continue;
		}
		//正负判断
		else if (ch == '-')
		{
			flag = -1;
		}
		else if (ch != ',')
		{
			num *= 10;
			num += ch - '0';
		}
		else
		{
			//赋值
			if (head->next == NULL)
			{
				head->num = num * flag;
			}
			else
			{
				p->num = num * flag;
			}
			p = (Dlink*)malloc(sizeof(Dlink));
			if (head->next == NULL)
			{
				head->next = p;
				p->prior = head;
				last = p;
			}
			else
			{
				p->prior = last;
				last->next = p;
				last = p;
			}
			num = 0;
			flag = 1;
		}
	}
	//循环,头尾相连。
	if (head->next == NULL)
	{
		head->num = num * flag;
		head->next = head;
		head->prior = head;
	}
	else
	{
		p->num = num * flag;
		p->next = head;
		head->prior = p;
	}
	return head;	
}
//加法运算，返回尾结点
Dlink* add(Dlink* a, Dlink* b)
{
	Dlink* a_temp = a->prior, * b_temp = b->prior;
	Dlink* tail,*p,*last=NULL;
	tail = (Dlink*)malloc(sizeof(Dlink)); //从后往前保存结果
	int temp = 0;	//临时保存计算结果
	int flag = 1; //判断结果正负
	int num = 0; //用于判断循环轮次
	last = tail;	//方便循环调用
	//同号相加
	if ((a->num >= 0 && b->num >= 0) || (a->num <= 0 && b->num <= 0))
	{
		//判断正负
		if (a->num < 0)
		{
			flag = -1;
		}
/*		//从后往前读
		temp = abs(a_temp->num) + abs(b_temp->num);
		tail->num = temp % 10000;
		temp /= 10000;
		//继续往前读
		a_temp = a_temp->prior;
		b_temp = b_temp->prior;
		if (a_temp != a->prior || b_temp != b->prior)	//还有未读完项
		{
			p = (Dlink*)malloc(sizeof(Dlink));
			tail->prior = p;
			p->next = tail;
			last = p;
		}
		else if (temp == 1)				//读完但要进位
		{
			p = (Dlink*)malloc(sizeof(Dlink));
			tail->prior = p;
			p->next = tail;
			p->prior = tail;
			tail->next = p;
			p->num = flag;
			return p;		//万位进位的1和后面的一个节点。
		}
		else		//结束，等待输出.
		{
			tail->prior = tail;
			tail->next = tail;
			tail->num = tail->num * flag;
			return tail;	//仅有一个节点
		}*/
		//未读完继续往前读
		while ((a_temp != a->prior && b_temp != b->prior) || num == 0)
		{
			last->num = (abs(a_temp->num) + abs(b_temp->num) + temp) % 10000;
			temp = (abs(a_temp->num) + abs(b_temp->num) + temp) / 10000;
			a_temp = a_temp->prior;
			b_temp = b_temp->prior;
			if (a_temp == a->prior)		
			{
				num = 1;
			}
			//如果后续还有,则再申请内存
			if (a_temp != a->prior || b_temp != b->prior)
			{
			p = (Dlink*)malloc(sizeof(Dlink));
			last->prior = p;
			p->next = last;
			last = p;
			}
			else if (temp==1) //后续无但要进位
			{
				p = (Dlink*)malloc(sizeof(Dlink));
				last->prior = p;
				p->next = last;
				p->num = flag;
				p->prior = tail;
				tail->next = p;
				return tail;
			}
			else   //无需进位
			{
				last->prior = tail;
				tail->next = last;
				last->num = last->num * flag;
				return tail;
			}
		}
		//较短长度的读完
		//a较长
		while (a_temp != a->prior)
		{
			last->num = (abs(a_temp->num) + temp) % 10000;
			temp = (abs(a_temp->num) + temp) / 10000;
			a_temp = a_temp->prior;
			if (a_temp != a->prior)
			{
				p = (Dlink*)malloc(sizeof(Dlink));
				last->prior = p;
				p->next = last;
				last = p;
			}
			else if (temp==1)	//进位
			{
				p = (Dlink*)malloc(sizeof(Dlink));
				tail->prior = p;
				p->next = tail;
				p->prior = tail;
				tail->next = p;
				p->num = flag;
				return tail;
			}
			else
			{
				last->prior = tail;
				tail->next = last;
				last->num = last->num * flag;
				return tail;
			}

		}
		//b较长
		while(b_temp != b->prior)
		{
			last->num = (abs(b_temp->num) + temp) % 10000;
			temp = (abs(b_temp->num) + temp) / 10000;
			b_temp = b_temp->prior;
			if (b_temp != b->prior)
			{
				p = (Dlink*)malloc(sizeof(Dlink));
				last->prior = p;
				p->next = last;
				last = p;
			}
			else if (temp == 1)	//进位
			{
				p = (Dlink*)malloc(sizeof(Dlink));
				tail->prior = p;
				p->next = tail;
				p->prior = tail;
				tail->next = p;
				p->num = flag;
				return tail;
			}
			else
			{
				last->prior = tail;
				tail->next = last;
				last->num = last->num * flag;
				return tail;
			}
		}
	}
	//异号相加(相减)
	else
	{
	// 默认较大的数减较小的数。
		int alen = 0, blen = 0, t = 0;	//t用于判断等长时谁较大
		while (a_temp != a->prior || alen == 0)
		{
			alen++;
			a_temp = a_temp->prior;
		}
		while (b_temp != b->prior || blen == 0)
		{
			blen++;
			b_temp = b_temp->prior;
		}
		if (alen == blen)
		{
			//等长，从前往后判断大小
			a_temp = a;
			b_temp = b;
			//相等继续往后读
			while (abs(a_temp->num) == abs(b_temp->num)&&num==0)
			{
				a_temp = a_temp->prior;
				b_temp = b_temp->prior;
				if (a_temp == a->prior)
				{
					num = 1;
				}
			}
			//读到不等长时退出循环。
			if (abs(a_temp->num) == abs(b_temp->num))	//a+b=0
			{
				//a与b互为相反数，直接返回
				p = (Dlink*)malloc(sizeof(Dlink));
				p->prior = p;
				p->next = p;
				p->num = 0;
				return p;
			}
			else if (abs(a_temp->num) > abs(b_temp->num))
			{
				t = 1;
			}
			else
			{
				t = -1;
			}
		}
		//初始化
		a_temp = a->prior;
		b_temp = b->prior;
		num = 0;
		if ((alen > blen) || t == 1)		//a绝对值更大
		{
			if (a->num < 0)
			{
				flag = -1;
			}
			while (a_temp != a->prior && b_temp != b->prior || num == 0)
			{
				if ((abs(a_temp->num) - abs(b_temp->num) + temp) >= 0)			//无需借位
				{
					last->num = abs(a_temp->num) - abs(b_temp->num) + temp;
					temp = 0;
				}
				else		//借位
				{
					last->num = abs(a_temp->num) - abs(b_temp->num) + temp + 10000;
					temp = -1;
				}
				a_temp = a_temp->prior;
				b_temp = b_temp->prior;
					num = 1;
				p = (Dlink*)malloc(sizeof(Dlink));
				p->num = 0;   //先置于0，后续根据实际长度进行删减。
				last->prior = p;
				p->next = last;
				last = p;
			}
			while (a_temp != a->prior)		//不等长
			{
				if ((abs(a_temp->num) + temp) >= 0)
				{
					last->num = abs(a_temp->num) + temp;
					temp = 0;
				}
				else
				{
					last->num = 10000 + temp;
					temp = -1;
				}
				a_temp = a_temp->prior;
				p = (Dlink*)malloc(sizeof(Dlink));
				p->num = 0;   //先置于0，后续根据实际长度进行删减。
				last->prior = p;
				p->next = last;
				last = p;
			}
		}
		if ((alen < blen) || t == -1)	//b的绝对值大
		{
			if (b->num < 0)
			{
				flag = -1;
			}
			while (a_temp != a->prior && b_temp != b->prior || num == 0)
			{
				if ((abs(b_temp->num) - abs(a_temp->num) + temp) >= 0)		//无需借位
				{
					last->num = abs(b_temp->num) - abs(a_temp->num) + temp;
					temp = 0;
				}
				else		//借位
				{
					last->num = abs(b_temp->num) - abs(a_temp->num) + temp + 10000;
					temp = -1;
				}
				a_temp = a_temp->prior;
				b_temp = b_temp->prior;
				num = 1;
				p = (Dlink*)malloc(sizeof(Dlink));
				p->num = 0;   //先置于0，后续根据实际长度进行删减。
				last->prior = p;
				p->next = last;
				last = p;
			}
			while (b_temp != b->prior)		//不等长
			{
				if ((abs(b_temp->num) +temp) >= 0)
				{
					last->num = abs(b_temp->num) + temp;
					temp = 0;
				}
				else
				{
					last->num = 10000 + temp;
					temp = -1;
				}
				b_temp = b_temp->prior;
				p = (Dlink*)malloc(sizeof(Dlink));
				p->num = 0;   //先置于0，后续根据实际长度进行删减。
				last->prior = p;
				p->next = last;
				last = p;
			}
		}
		while (last->num == 0)
		{
			last = last->next;
		}
		if (last != tail)
		{
			last->prior = tail;
			tail->next = last;
			last->num = last->num * flag;
		}
		else
		{
			tail->next = tail;
			tail->prior = tail;
			tail->num = tail->num * flag;
		}
		return tail;
	}
	return tail;
}
void putout(Dlink* p)
{
	Dlink* temp = p->next;
	cout << temp->num;
	if (temp != p)
	{
		cout << ",";
	}
	temp = temp->next;
	while ((temp != p->next))
	{
		if (temp->num >= 1000)//四位数直接输出
		{
			cout << temp->num;
		}
		else if (temp->num >= 100)	//三位数，补一个0
		{
			cout << "0" << temp->num;
		}
		else if (temp->num >= 10)
		{
			cout << "00" << temp->num;	//两位数，补两个0
		}
		else if(temp->num > 0)
		{
			cout << "000" << temp->num;		//一位数，补3个0
		}
		else
		{
			cout << "0000";		//0，输出四个0
		}
		if (temp != p)
		{
			cout << ",";
		}
		temp = temp->next;

	}
}
int main()
{
	Dlink* a, * b, *tail;
	cout << "请输入a的值（逗号分万进制表示，以英文分号结尾）" << endl;
	a = init();
	cout << "请输入b的值（逗号分万进制表示，以英文分号结尾）" << endl;
	b = init();
	tail = add(a, b);
	cout << "二者之和为：" << endl;
	putout(tail);
	return 0;
}