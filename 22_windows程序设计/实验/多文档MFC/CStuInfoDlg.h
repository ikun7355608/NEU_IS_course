#pragma once


// CStuInfoDlg 对话框

class CStuInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStuInfoDlg)

public:
	CStuInfoDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStuInfoDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	CString m_strNo;
	CTime m_tBirth;
	CComboBox m_comboSpecial;
	CString m_strSpecial;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
