
// MFCApplication2Dlg.h: 头文件
//

#pragma once


// CMFCApplication2Dlg 对话框
class CMFCApplication2Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication2Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNum1();
	CString m_temp,m_result;
	double data1,data2, answer;
	int ope1=0,ope2=0;								//运算符替代成数字
	int flag = 0;									//优先级占用状态
	afx_msg void OnBnClickedNum2();
	afx_msg void OnBnClickedNum3();
	afx_msg void OnBnClickedNum4();
	afx_msg void OnBnClickedNum5();
	afx_msg void OnBnClickedNum6();
	afx_msg void OnBnClickedNum7();
	afx_msg void OnBnClickedNum8();
	afx_msg void OnBnClickedNum9();
	afx_msg void OnBnClickedNum0();
	afx_msg void OnBnClickedSpot();
	afx_msg void OnBnClickedPlus();
	afx_msg void OnBnClickedMinus();
	afx_msg void OnBnClickedTimes();
	afx_msg void OnBnClickedDivide();
	afx_msg void OnBnClickedEqual();
	afx_msg void OnBnClickedClear();
	afx_msg void OnEnChangeEdit();
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
