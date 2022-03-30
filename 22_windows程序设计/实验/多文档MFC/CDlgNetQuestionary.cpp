// CDlgNetQuestionary.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CDlgNetQuestionary.h"
#include "afxdialogex.h"


// CDlgNetQuestionary 对话框

IMPLEMENT_DYNAMIC(CDlgNetQuestionary, CDialogEx)

CDlgNetQuestionary::CDlgNetQuestionary(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NET_QUESTIONARY, pParent)
{

}

CDlgNetQuestionary::~CDlgNetQuestionary()
{
}

void CDlgNetQuestionary::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNetQuestionary, CDialogEx)
	ON_BN_CLICKED(IDC_AGE_L18, &CDlgNetQuestionary::OnBnClickedAgeL18)
END_MESSAGE_MAP()


// CDlgNetQuestionary 消息处理程序


void CDlgNetQuestionary::OnBnClickedAgeL18()
{
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CDlgNetQuestionary::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CDialogEx::OnInitDialog();

	CheckRadioButton(IDC_AGE_L18, IDC_AGE_M38, IDC_AGE_18T27);
	CheckRadioButton(IDC_CM_FTTL, IDC_CM_OTHER, IDC_CM_FTTL);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_DO_POP);
	pBtn->SetCheck(1);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgNetQuestionary::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CString str, strCtrl;
	str = L"你的年龄：";
	UINT nID = GetCheckedRadioButton(IDC_AGE_L18,
		IDC_AGE_M38);
	GetDlgItemText(nID, strCtrl);
	str = str + strCtrl;
	str = str + L"\n你使用的接入方式：";
	nID = GetCheckedRadioButton(IDC_CM_FTTL,
		IDC_CM_OTHER);
	GetDlgItemText(nID, strCtrl);
	str = str + strCtrl;
	str = str + L"\n你上网主要是：\n";
	UINT nCheckIDs[4] = { IDC_DO_POP, IDC_DO_NEWS,
										 IDC_DO_GAME, IDC_DO_OTHER };
	CButton* pBtn;
	for (int i = 0; i < 4; i++)
	{
		pBtn = (CButton*)GetDlgItem(nCheckIDs[i]);
		if (pBtn->GetCheck())
		{
			pBtn->GetWindowText(strCtrl);
			str = str + strCtrl;
			str = str + L"  ";
		}
	}
	MessageBox(str);

	CDialogEx::OnOK();
}
