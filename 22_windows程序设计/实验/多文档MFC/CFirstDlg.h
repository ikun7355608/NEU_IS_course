#pragma once
#include "afxwin.h"

// CFirstDlg 对话框

class CFirstDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFirstDlg)

public:
	CFirstDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFirstDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FIRST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void CFirstDlg::OnButton1();
	CButton m_RelBtn;
	CString m_strEdit;
};
