// CFirstDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CFirstDlg.h"
#include "afxdialogex.h"

// CFirstDlg 对话框

IMPLEMENT_DYNAMIC(CFirstDlg, CDialogEx)

CFirstDlg::CFirstDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FIRST, pParent)
{

}

CFirstDlg::~CFirstDlg()
{
}

void CFirstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_RelBtn);
	DDX_Text(pDX, IDC_EDIT1, m_strEdit);
	DDV_MaxChars(pDX, m_strEdit, 20);

}
void CFirstDlg::OnButton1()
{
	UpdateData(TRUE);
	m_RelBtn.SetWindowText(m_strEdit);
}


BEGIN_MESSAGE_MAP(CFirstDlg, CDialogEx)
END_MESSAGE_MAP()


// CFirstDlg 消息处理程序


BOOL CFirstDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_STATIC);
	pWnd->SetWindowText(_T("这是我的第一个对话框！"));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
