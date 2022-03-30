#pragma once


// CPropPage2 对话框

class CPropPage2 : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPropPage2)

public:
	CPropPage2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropPage2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROP2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bFootball;
	BOOL m_bBasketball;
	BOOL m_bVolleyball;
	BOOL m_bSwim;
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
};
