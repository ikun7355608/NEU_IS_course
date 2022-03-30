#pragma once


// CDlgBtnCreate 对话框

class CDlgBtnCreate : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBtnCreate)
private:
	CButton m_btnMine;
public:
	CDlgBtnCreate(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgBtnCreate();
	void CDlgBtnCreate::OnOK();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTL_CREATE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
