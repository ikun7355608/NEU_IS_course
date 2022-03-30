// CSecondDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CSecondDlg.h"
#include "afxdialogex.h"


// CSecondDlg 对话框

IMPLEMENT_DYNAMIC(CSecondDlg, CDialogEx)

CSecondDlg::CSecondDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SECOND, pParent)
{

}

CSecondDlg::~CSecondDlg()
{
}

void CSecondDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSecondDlg, CDialogEx)
END_MESSAGE_MAP()


// CSecondDlg 消息处理程序
