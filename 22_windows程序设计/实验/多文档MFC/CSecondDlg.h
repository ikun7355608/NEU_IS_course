#pragma once


// CSecondDlg 对话框

class CSecondDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSecondDlg)

public:
	CSecondDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSecondDlg();
	void CSecondDlg::OnBnClickedOk()
{
	//CDialogEx::OnOK();
	DestroyWindow();
	delete this;
}void CSecondDlg::OnCancel()
{
	//CDialogEx::OnCancel();
	DestroyWindow();
	delete this;
}

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SECOND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

