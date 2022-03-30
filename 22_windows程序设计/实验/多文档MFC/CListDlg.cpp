// CListDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CListDlg.h"
#include "afxdialogex.h"
#include "CStuInfoDlg.h"

// CListDlg 对话框

IMPLEMENT_DYNAMIC(CListDlg, CDialogEx)

CListDlg::CListDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LIST, pParent)
{

}

CListDlg::~CListDlg()
{
}

void CListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

void CListDlg::SetCtrlStyle(HWND hWnd, DWORD dwNewStyle)
{
	DWORD	dwOldStyle;
	dwOldStyle = GetWindowLong(hWnd, GWL_STYLE);
	if ((dwOldStyle & LVS_TYPEMASK) != dwNewStyle)
	{
		dwOldStyle &= ~LVS_TYPEMASK;
		dwNewStyle |= dwOldStyle;
		SetWindowLong(hWnd, GWL_STYLE, dwNewStyle);
	}
}
BEGIN_MESSAGE_MAP(CListDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_LARGE, &CListDlg::OnBnClickedRadioLarge)
	ON_BN_CLICKED(IDC_RADIO_SMALL, &CListDlg::OnBnClickedRadioSmall)
	ON_BN_CLICKED(IDC_RADIO_LIST, &CListDlg::OnBnClickedRadioList)
	ON_BN_CLICKED(IDC_RADIO_REPORT, &CListDlg::OnBnClickedRadioReport)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CListDlg::OnBnClickedButtonAdd)
END_MESSAGE_MAP()


// CListDlg 消息处理程序


BOOL CListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CheckRadioButton(IDC_RADIO_LARGE, IDC_RADIO_REPORT, IDC_RADIO_REPORT);
	SetCtrlStyle(m_ListCtrl.m_hWnd, LVS_REPORT);
	CImageList* pImageList;
	pImageList = new CImageList();
	pImageList->Create(32, 32, ILC_COLOR, 0, 1);
	pImageList->Add(AfxGetApp()->
		LoadIcon(IDI_ICON1));//show icon
	m_ListCtrl.SetImageList(pImageList,
		LVSIL_NORMAL);
	CString strHeader[5] = { L"学号", L"姓名",
												  L"出生年月", L"专业" };
	int nWidth[4] = { 80, 100, 100, 200 };
	for (int nCol = 0; nCol < 4; nCol++)
		m_ListCtrl.InsertColumn(nCol, strHeader[nCol], LVCFMT_LEFT, nWidth[nCol]);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CListDlg::OnBnClickedRadioLarge()
{
	SetCtrlStyle(m_ListCtrl.m_hWnd, LVS_ICON);
	// TODO: 在此添加控件通知处理程序代码
}


void CListDlg::OnBnClickedRadioSmall()
{
	// TODO: 在此添加控件通知处理程序代码
	SetCtrlStyle(m_ListCtrl.m_hWnd, LVS_SMALLICON);
}


void CListDlg::OnBnClickedRadioList()
{
	// TODO: 在此添加控件通知处理程序代码
	SetCtrlStyle(m_ListCtrl.m_hWnd, LVS_LIST);

}


void CListDlg::OnBnClickedRadioReport()
{
	// TODO: 在此添加控件通知处理程序代码
	SetCtrlStyle(m_ListCtrl.m_hWnd, LVS_REPORT);
}


void CListDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CStuInfoDlg dlg;
	if (IDOK != dlg.DoModal())
		return;
	LVFINDINFO info;
	info.flags = LVFI_PARTIAL | LVFI_STRING;
	info.psz = dlg.m_strNo;
	if (m_ListCtrl.FindItem(&info) != -1)
	{
		CString str;
		str.Format(L"学号为%s的学生基本信息已添加过！", dlg.m_strNo);
		MessageBox(str);
		return;
	}
	// add student information
	int nIndex =
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), dlg.m_strNo);
	m_ListCtrl.SetItemText(nIndex, 1, dlg.m_strName);
	m_ListCtrl.SetItemText(nIndex, 2,
		dlg.m_tBirth.Format(L"%Y-%m-%d"));
	m_ListCtrl.SetItemText(nIndex, 3, dlg.m_strSpecial);
}
