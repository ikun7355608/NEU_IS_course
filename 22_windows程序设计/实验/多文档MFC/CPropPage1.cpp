// CPropPage1.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CPropPage1.h"
#include "afxdialogex.h"


// CPropPage1 对话框

IMPLEMENT_DYNAMIC(CPropPage1, CMFCPropertyPage)

CPropPage1::CPropPage1(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROP1, NULL)
	, m_nOccupation(0)
{

}

CPropPage1::~CPropPage1()
{
}

void CPropPage1::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_nOccupation);
}


BEGIN_MESSAGE_MAP(CPropPage1, CMFCPropertyPage)
END_MESSAGE_MAP()


// CPropPage1 消息处理程序


LRESULT CPropPage1::OnWizardNext()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData();
	if (m_nOccupation == -1)
	{
		MessageBox(L"请选择你的职业！");
		return -1;
	}
	return CMFCPropertyPage::OnWizardNext();
}


BOOL CPropPage1::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CPropertySheet*)GetParent())->
		SetWizardButtons(PSWIZB_NEXT);
	return CMFCPropertyPage::OnSetActive();
}
