#include<iostream>
using namespace std;
//  运算数栈
typedef struct Stack_Operand
{
	int num;	//数值
	struct Stack_Operand* next; //入栈顺序
	struct Stack_Operand* prior = NULL; //出栈顺序
}opd;
// 运算符栈
typedef struct Stack_Operator
{
	char op = '\0';
	struct Stack_Operator* next;
	struct Stack_Operator* prior = NULL;
}opr;
int num_temp = 0;
char ch_temp = '\0';
// 运算数入栈
opd* opd_push(opd* last, int data)
{
	opd* temp;
	temp = (opd*)malloc(sizeof(opd));
	temp->num = 0;
	temp->next = NULL;
	temp->prior = last;
	last->num = data;
	last->next = temp;
	return temp;
}
//运算符入栈
opr* opr_push(opr* last, char ch)
{
	opr* temp;
	temp = (opr*)malloc(sizeof(opr));
	temp->next = NULL;
	temp->prior = last;
	temp->op = '\0';
	last->op = ch;
	last->next = temp;
	return temp;
}
//运算数出栈
opd* opd_pop(opd* top)
{
	//栈顶是空的

	//当前位置作为新的栈顶
	if (top->prior != NULL)
	{
		opd* temp = top->prior;
		num_temp = temp->num;	//保留弹出元素
		top = top->prior;
		free(temp->next);
		return top;
	}
}
//运算符出栈
opr* opr_pop(opr* top)
{
	//当前位置作为新的栈顶
	if (top->prior != NULL)
	{
		opr* temp = top->prior;
		ch_temp = temp->op;
		top->op = '\0';
		top = top->prior;
		free(temp->next);
		return top;
	}
}
//改变栈顶元素
opd* cal(opd* top)
{
	switch (ch_temp)
	{
	case '+':
		top->prior->num += num_temp;
		break;
	case '-':
		top->prior->num -= num_temp;
		break;
	case '*':
		top->prior->num *= num_temp;
		break;
	case '/':
		top->prior->num /= num_temp;
		break;
	default:
		break;
	}
	return top;
}
int pri_opr(char ch1, char ch2) //运算符优先级比较
{	
	//ch1为栈顶运算符，ch2为当前扫描到运算符
	//当前为')'，一定出栈
	if (ch2 == ')')
	{
		return 0;
	}
	//栈顶为*/，只要当前不是'('均出栈
	else if ((ch1 == '*' || ch1 == '/') && (ch2 != '('))
	{
		return 0;
	}
	//栈顶为+-，当前为+-出栈
	else if ((ch1 == '+' || ch1 == '-') && (ch2 == '+' || ch2 == '-'))
	{
		return 0;
	}
	//其余均入栈
	else
	{
		return 1;
	}
}
int init(opd* stack1, opr* stack2)
{
	int ans = 0; //保存结果
	int num = 0; //计算运算数
	char ch='\0',ch1='\0'; //ch1,ch用于比较运算符的优先级
	while ((ch = getchar()) != ';')
	{
		//运算数
		if (ch >= '0' && ch <= '9')
		{
			num *= 10;
			num += ch - '0';
		}
		//运算符
		else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')')
		{
			//读到运算符时运算数入栈
			if (ch != '(' && ch_temp != '(')
			{
				stack1 = opd_push(stack1, num);
			}
			num = 0;
			if (stack2->prior != NULL)
			{
				ch1 = stack2->prior->op;	//栈顶元素

			//判断运算符入栈还是出栈
				if (pri_opr(ch1, ch))
				{
					stack2 = opr_push(stack2, ch);
				}
				else
				{
					//如果当前运算符优先级一直小于栈顶运算符优先级则一直弹
					//弹到左括号或当前运算符优先级高于栈顶为止
					if (ch == ')')
					{
						while (ch1 != '(')
						{
							stack2 = opr_pop(stack2);
							stack1 = opd_pop(stack1);
							stack1 = cal(stack1);
							ch1 = stack2->prior->op;
						}
						stack2 = opr_pop(stack2);
					}
					else   //如果不是因为弹到右括号而出栈，一直弹
					{
						while (!pri_opr(ch1, ch))
						{
							//弹出的操作数和运算符
							stack2 = opr_pop(stack2);
							stack1 = opd_pop(stack1);
							stack1 = cal(stack1);
							if (stack2->prior != NULL)
							{
								ch1 = stack2->prior->op;
							}
							else
								break;
						}
						stack2 = opr_push(stack2, ch);
					}
				}
			}
			else
			{
				stack2 = opr_push(stack2, ch);
			}
		}
	}
	//扫描结束，最后一个数入栈
	if (ch_temp != '(')
	{
		stack1 = opd_push(stack1, num);
	}
	//全部弹出。

	while (stack2->prior != NULL)
	{
		stack2 = opr_pop(stack2);
		stack1 = opd_pop(stack1);
		stack1 = cal(stack1);

	}
	ans = stack1->prior->num;
	return ans;
}
int main()
{
	opd* head1 = (opd*)malloc(sizeof(opd));
	head1->prior = NULL;
	opr* head2 = (opr*)malloc(sizeof(opr));
	head2->prior = NULL;
	cout << "请输入要进行运算的中缀表达式，以;结尾" << endl;
	int a = init(head1, head2);
	cout << "计算结果为";
	cout << a << endl;
	cout << "是否继续计算？ 按Y继续";
	char ch = '\0';
	cin >> ch;
	while (ch == 'Y' || ch == 'y')
	{
		cout << "请输入要进行运算的中缀表达式，以;结尾" << endl;
		int a = init(head1, head2);
		cout << "计算结果为";
		cout << a << endl;
		cout << "是否继续计算？ 按Y继续";
		cin >> ch;
	}
	system("pause");
	return 0;
}