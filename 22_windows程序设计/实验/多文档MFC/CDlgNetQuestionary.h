#pragma once


// CDlgNetQuestionary 对话框

class CDlgNetQuestionary : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNetQuestionary)

public:
	CDlgNetQuestionary(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgNetQuestionary();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NET_QUESTIONARY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAgeL18();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
