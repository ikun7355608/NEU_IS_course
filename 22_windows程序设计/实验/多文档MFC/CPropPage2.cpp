// CPropPage2.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CPropPage2.h"
#include "afxdialogex.h"


// CPropPage2 对话框

IMPLEMENT_DYNAMIC(CPropPage2, CMFCPropertyPage)

CPropPage2::CPropPage2(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROP2, NULL)
	, m_bFootball(FALSE)
	, m_bBasketball(FALSE)
	, m_bVolleyball(FALSE)
	, m_bSwim(FALSE)
{

}

CPropPage2::~CPropPage2()
{
}

void CPropPage2::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_FOOTBALL, m_bFootball);
	DDX_Check(pDX, IDC_CHECK_BASKETBALL, m_bBasketball);
	DDX_Check(pDX, IDC_CHECK_VOLLEYBALL, m_bVolleyball);
	DDX_Check(pDX, IDC_CHECK_SWIM, m_bSwim);
}


BEGIN_MESSAGE_MAP(CPropPage2, CMFCPropertyPage)
END_MESSAGE_MAP()


// CPropPage2 消息处理程序


BOOL CPropPage2::OnWizardFinish()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData();
	if (m_bFootball || m_bBasketball || m_bVolleyball || m_bSwim)
	{
		return CPropertyPage::OnWizardFinish();
	}
	else
	{
		MessageBox(L"请选择你的兴趣爱好！");
		return -1;
	}
	return CMFCPropertyPage::OnWizardFinish();
}


BOOL CPropPage2::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CPropertySheet*)GetParent())->
		SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return CMFCPropertyPage::OnSetActive();
}
