#pragma once


// CPropPage1 对话框

class CPropPage1 : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPropPage1)

public:
	CPropPage1(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropPage1();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROP1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	INT m_nOccupation;
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
};
