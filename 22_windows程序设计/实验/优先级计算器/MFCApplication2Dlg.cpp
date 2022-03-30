
// MFCApplication2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 对话框



CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_EDIT, m_result);
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NUM_1, &CMFCApplication2Dlg::OnBnClickedNum1)
	ON_BN_CLICKED(IDC_NUM_2, &CMFCApplication2Dlg::OnBnClickedNum2)
	ON_BN_CLICKED(IDC_NUM_3, &CMFCApplication2Dlg::OnBnClickedNum3)
	ON_BN_CLICKED(IDC_NUM_4, &CMFCApplication2Dlg::OnBnClickedNum4)
	ON_BN_CLICKED(IDC_NUM_5, &CMFCApplication2Dlg::OnBnClickedNum5)
	ON_BN_CLICKED(IDC_NUM_6, &CMFCApplication2Dlg::OnBnClickedNum6)
	ON_BN_CLICKED(IDC_NUM_7, &CMFCApplication2Dlg::OnBnClickedNum7)
	ON_BN_CLICKED(IDC_NUM_8, &CMFCApplication2Dlg::OnBnClickedNum8)
	ON_BN_CLICKED(IDC_NUM_9, &CMFCApplication2Dlg::OnBnClickedNum9)
	ON_BN_CLICKED(IDC_NUM_0, &CMFCApplication2Dlg::OnBnClickedNum0)
	ON_BN_CLICKED(IDC_SPOT, &CMFCApplication2Dlg::OnBnClickedSpot)
	ON_BN_CLICKED(IDC_PLUS, &CMFCApplication2Dlg::OnBnClickedPlus)
	ON_BN_CLICKED(IDC_MINUS, &CMFCApplication2Dlg::OnBnClickedMinus)
	ON_BN_CLICKED(IDC_TIMES, &CMFCApplication2Dlg::OnBnClickedTimes)
	ON_BN_CLICKED(IDC_DIVIDE, &CMFCApplication2Dlg::OnBnClickedDivide)
	ON_BN_CLICKED(IDC_EQUAL, &CMFCApplication2Dlg::OnBnClickedEqual)
	ON_BN_CLICKED(IDC_CLEAR, &CMFCApplication2Dlg::OnBnClickedClear)
	ON_EN_CHANGE(IDC_EDIT, &CMFCApplication2Dlg::OnEnChangeEdit)
//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CMFCApplication2Dlg 消息处理程序

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication2Dlg::OnBnClickedNum1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"1";
	m_temp += L"1";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"2";
	m_temp += L"2";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"3";
	m_temp += L"3";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"4";
	m_temp += L"4";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum5()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"5";
	m_temp += L"5";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum6()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"6";
	m_temp += L"6";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum7()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"7";
	m_temp += L"7";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum8()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"8";
	m_temp += L"8";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum9()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"9";
	m_temp += L"9";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedNum0()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"0";
	m_temp += L"0";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedSpot()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L".";
	m_temp += L".";
	UpdateData(FALSE);
}


void CMFCApplication2Dlg::OnBnClickedPlus()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"+";
	data1= _wtof(m_temp);
	m_temp = L"";
	if (flag)		//前面的两个运算符，前一个为+-，后一个为乘除
	{
		switch (flag)
		{
		case 3:data2 *= data1; break;
		case 4:data2 /= data1; break;
		default:
			break;
		}
		flag = 0;		//由于为+, 取消占用
		switch (ope1)	//由于为+，前一个运算符可以正常运算了。
		{
		case 1:answer += data2; break;
		case 2:answer -= data2; break;
		case 3:answer *= data2; break;
		case 4:answer /= data2; break;
		default:
			answer = data1;
			break;
		}
		ope1 = 1;
	}
	else if (!ope1)
	{
		ope1 = 1;
		answer = data1;
	}
	else
	{
		ope2 = 1;
		switch (ope1)
		{
		case 1:answer += data1; break;
		case 2:answer -= data1; break;
		case 3:answer *= data1; break;
		case 4:answer /= data1; break;
		default:
			answer = data1;
			break;
		}
		ope1 = ope2;
	}
}


void CMFCApplication2Dlg::OnBnClickedMinus()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"-";
	data1 = _wtof(m_temp);
	m_temp = L"";
	if (flag)		//前面的运算符，前一个为+-，后一个为乘除
	{
		switch (flag)
		{
		case 3:data2 *= data1; break;
		case 4:data2 /= data1; break;
		default:
			break;
		}
		flag = 0;		//由于仍为*, 依旧占用
		switch (ope1)	//由于为+，前一个运算符可以正常运算了。
		{
		case 1:answer += data2; break;
		case 2:answer -= data2; break;
		case 3:answer *= data2; break;
		case 4:answer /= data2; break;
		default:
			answer = data1;
			break;
		}
		ope1 = 2;
	}
	else if (!ope1)
	{
		ope1 = 2;
		answer = data1;
	}
	else
	{
		ope2 = 2;
		switch (ope1)
		{
		case 1:answer += data1; break;
		case 2:answer -= data1; break;
		case 3:answer *= data1; break;
		case 4:answer /= data1; break;
		default:
			answer = data1;
			break;
		}
		ope1 = ope2;
	}
}


void CMFCApplication2Dlg::OnBnClickedTimes()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"*";
	data1 = _wtof(m_temp);
	m_temp = L"";
	if (flag)		//前面的运算符，前一个为+-，后一个为乘除
	{
		switch (flag)
		{
		case 3:data2 *= data1; break;	//保留前一次占用的结果
		case 4:data2 /= data1; break;
		default:
			break;
		}
		flag = 3;		//由于仍为*, 依旧占用
	}
	else if (!ope1)				//优先级占用表明该运算符至少为第三个，与是否为首个冲突
	{
		ope1 = 3;
		answer = data1;
	}
	else
	{
		ope2 = 3;
		if (ope1 / 3)	//同优先级，运算前一个。
		{
			switch (ope1)
			{
			case 3:answer *= data1; break;
			case 4:answer /= data1; break;
			default:
				answer = data1;
				break;
			}
			ope1 = ope2;
		}
		else                //前一个为+-，后一个为*/
		{
			flag = ope2;
			data2 = data1;

		}
	}
}


void CMFCApplication2Dlg::OnBnClickedDivide()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result += L"/";
	data1 = _wtof(m_temp);
	m_temp = L"";
	if (flag)		//前面的运算符，前一个为+-，后一个为乘除
	{
		switch (flag)
		{
		case 3:data2 *= data1; break;			//保留前一次占用的结果
		case 4:data2 /= data1; break;
		default:
			break;
		}
		flag = 4;		//由于仍为*, 依旧占用
	}
	else if (!ope1)				//优先级占用表明该运算符至少为第三个，与是否为首个冲突
	{
		ope1 = 4;
		answer = data1;
	}
	else
	{
		ope2 = 4;
		if (ope1 / 3)	//同优先级，运算前一个。
		{
			switch (ope1)
			{
			case 3:answer *= data1; break;
			case 4:answer /= data1; break;
			default:
				answer = data1;
				break;
			}
			ope1 = ope2;
		}
		else                //前一个为+-，后一个为*/
		{
			flag = ope2;
			data2 = data1;

		}
	}
}


void CMFCApplication2Dlg::OnBnClickedEqual()
{
	// TODO: 在此添加控件通知处理程序代码
	data1 = _wtof(m_temp);
	m_temp = L"";
	if (flag)
	{
		switch (flag)
		{
		case 3:data2 *= data1; break;
		case 4:data2 /= data1; break;
		default:
			break;
		}
		flag = 0;
		switch (ope1)
		{
		case 1:answer += data2; break;
		case 2:answer -= data2; break;
		case 3:answer *= data2; break;
		case 4:answer /= data2; break;
		default:
			answer = data1;
			break;
		}
	}
	else
	{
		switch (ope1)
		{
		case 1:answer += data1; break;
		case 2:answer -= data1; break;
		case 3:answer *= data1; break;
		case 4:answer /= data1; break;
		default:
			break;
		}
	}
	m_result += "=";
	m_temp = L"";
	CString C;
	C.Format(_T("%f"), answer);
	m_result += C;
	UpdateData(FALSE);
	m_result = L"";
}


void CMFCApplication2Dlg::OnBnClickedClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_result = L"";
	data1 = 0;
	data2 = 0;
	answer = 0;
	ope1 = 0;
	ope2 = 0;
	m_temp = L"";
	UpdateData(FALSE);
}



void CMFCApplication2Dlg::OnEnChangeEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}