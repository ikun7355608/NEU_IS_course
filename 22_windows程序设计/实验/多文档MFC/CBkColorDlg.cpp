// CBkColorDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CBkColorDlg.h"
#include "afxdialogex.h"


// CBkColorDlg 对话框

IMPLEMENT_DYNAMIC(CBkColorDlg, CDialogEx)

CBkColorDlg::CBkColorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COLOR, pParent)
	, m_nGreen(0)
	, m_nBlue(0)
{

}

CBkColorDlg::~CBkColorDlg()
{
}

void CBkColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR_RED, m_scrollRed);
	DDX_Control(pDX, IDC_SLIDER_GREEN, m_sliderGreen);
	DDX_Slider(pDX, IDC_SLIDER_GREEN, m_nGreen);
	DDX_Control(pDX, IDC_SLIDER_BLUE, m_sliderBlue);
	DDX_Slider(pDX, IDC_SLIDER_BLUE, m_nBlue);
}


BEGIN_MESSAGE_MAP(CBkColorDlg, CDialogEx)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BLUE, &CBkColorDlg::OnNMCustomdrawSliderBlue)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CBkColorDlg 消息处理程序


void CBkColorDlg::OnNMCustomdrawSliderBlue(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


BOOL CBkColorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_scrollRed.SetScrollRange(0, 255);
	m_sliderBlue.SetRange(0, 255);
	m_sliderGreen.SetRange(0, 255);
	m_nBlue = m_nGreen = m_nRedValue = 192;
	UpdateData(FALSE);
	m_scrollRed.SetScrollPos(m_nRedValue);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CBkColorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nID = pScrollBar->GetDlgCtrlID();
	if (nID == IDC_SCROLLBAR_RED)
	{
		switch (nSBCode)
		{
		case SB_LINELEFT: m_nRedValue--;
			break;
		case SB_LINERIGHT: m_nRedValue++;
			break;
		case SB_PAGELEFT: m_nRedValue -= 10;
			break;
		case SB_PAGERIGHT: m_nRedValue += 10;
			break;
		case SB_THUMBTRACK: m_nRedValue = nPos;
			break;
		}//end switch
		if (m_nRedValue < 0) m_nRedValue = 0;
		if (m_nRedValue > 255) m_nRedValue = 255;
		m_scrollRed.SetScrollPos(m_nRedValue);
	}//end if
	Invalidate();		// repaint the dialog
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


HBRUSH CBkColorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	UpdateData(TRUE);
	COLORREF color = RGB(m_nRedValue, m_nGreen, m_nBlue);
	m_Brush.Detach();
	m_Brush.CreateSolidBrush(color);
	pDC->SetBkColor(color);
	return (HBRUSH)m_Brush;

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
}
