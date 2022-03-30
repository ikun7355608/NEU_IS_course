#pragma once


// CBkColorDlg 对话框

class CBkColorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBkColorDlg)

public:
	CBkColorDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBkColorDlg();
private:
	int         m_nRedValue;
	CBrush m_Brush;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COLOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSliderBlue(NMHDR* pNMHDR, LRESULT* pResult);
	CScrollBar m_scrollRed;
	CSliderCtrl m_sliderGreen;
	int m_nGreen;
	CSliderCtrl m_sliderBlue;
	int m_nBlue;
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
