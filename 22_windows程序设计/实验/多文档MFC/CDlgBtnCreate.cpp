// CDlgBtnCreate.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CDlgBtnCreate.h"
#include "afxdialogex.h"


// CDlgBtnCreate 对话框

IMPLEMENT_DYNAMIC(CDlgBtnCreate, CDialogEx)

CDlgBtnCreate::CDlgBtnCreate(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTL_CREATE, pParent)
{

}

CDlgBtnCreate::~CDlgBtnCreate()
{
}

void CDlgBtnCreate::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgBtnCreate, CDialogEx)
END_MESSAGE_MAP()

void CDlgBtnCreate::OnOK()
{
	if (m_btnMine.m_hWnd == NULL)
	{
		m_btnMine.Create(L"动态创建的按钮", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			CRect(20, 20, 300, 100), this, 2000);
		GetDlgItem(IDOK)->SetWindowText(L"删除动态创建的按钮");
	}
	else
	{
		m_btnMine.DestroyWindow();
		GetDlgItem(IDOK)->SetWindowText(L"运行时创建按钮");
	}
	//CDialog::OnOK();
}

// CDlgBtnCreate 消息处理程序


BOOL CDlgBtnCreate::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::OnCommand(wParam, lParam);
	WORD  nCode = HIWORD(wParam);   // 控件的通知消息
	WORD  nID = LOWORD(wParam);	  // 控件的ID号
	if ((nID == 2000) && (nCode == BN_CLICKED))
		MessageBox(L"你按下了动态创建的按钮！");

	return CDialogEx::OnCommand(wParam, lParam);

}
