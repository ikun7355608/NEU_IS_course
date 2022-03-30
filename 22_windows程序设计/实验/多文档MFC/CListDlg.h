#pragma once


// CListDlg 对话框

class CListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListDlg)

public:
	CListDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	void CListDlg::SetCtrlStyle(HWND hWnd, DWORD dwNewStyle);

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioLarge();
	afx_msg void OnBnClickedRadioSmall();
	afx_msg void OnBnClickedRadioList();
	afx_msg void OnBnClickedRadioReport();
	afx_msg void OnBnClickedButtonAdd();
};
