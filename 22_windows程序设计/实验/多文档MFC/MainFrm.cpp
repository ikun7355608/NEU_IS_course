
// MainFrm.cpp: CMainFrame 类的实现
//
#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MainFrm.h"
#include "CFirstDlg.h"
#include "CSecondDlg.h"
#include "CDlgBtnCreate.h"
#include "CDlgNetQuestionary.h"
#include "CBkColorDlg.h"
#include "CListDlg.h"
#include "CPropSheetMine.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_MODAL_DLG, &CMainFrame::OnModalDlg)
	ON_COMMAND(ID_MODELESS_DLG, &CMainFrame::OnModelessDlg)
	ON_COMMAND(IDM_FILE_DLG, &CMainFrame::OnFileDlg)
	ON_COMMAND(IDM_COLOR_DLG, &CMainFrame::OnColorDlg)
	ON_COMMAND(ID_COLOR_DLG, &CMainFrame::OnColorDlg)
	ON_COMMAND(IDM_CTL_CREATE_BTN, &CMainFrame::OnCtlCreateBtn)
	ON_COMMAND(IDM_CTL_NET, &CMainFrame::OnCtlNet)
	ON_COMMAND(IDM_CTL_COLOR, &CMainFrame::OnCtlColor)
	ON_COMMAND(IDM_CTL_STUDENT_LIST, &CMainFrame::OnCtlStudentList)
	ON_COMMAND(IDM_CTL_PROP_SHEET, &CMainFrame::OnCtlPropSheet)
	ON_COMMAND(IDM_COMMAND_TEST, &CMainFrame::OnCommandTest)
	ON_COMMAND(IDM_MENU_CHECK, &CMainFrame::OnMenuCheck)
	ON_UPDATE_COMMAND_UI(IDM_MENU_CHECK, &CMainFrame::OnUpdateMenuCheck)
	ON_UPDATE_COMMAND_UI(IDM_MENU_DEFAULT, &CMainFrame::OnUpdateMenuDefault)
	ON_UPDATE_COMMAND_UI(IDM_MENU_PIC, &CMainFrame::OnUpdateMenuPic)
	ON_COMMAND(IDM_MENU_UPDATE, &CMainFrame::OnMenuUpdate)
	ON_COMMAND(IDM_MENU_RETURN, &CMainFrame::OnMenuReturn)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(IDM_MENU_UPDATE, &CMainFrame::OnUpdateMenuUpdate)
	ON_UPDATE_COMMAND_UI(IDM_MENU_RETURN, &CMainFrame::OnUpdateMenuReturn)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TIME, OnUpdateTime)
	ON_UPDATE_COMMAND_UI(ID_FONT, &CMainFrame::OnUpdateFont)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_SEPARATOR,
	ID_INDICATOR_TIME,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,

};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	if (!m_wndToolBarMine.CreateEx(this, TBSTYLE_FLAT, WS_CHILD
		| WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS
		| CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarMine.LoadToolBar(IDR_TOOLBAR_NEW))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBarMine.EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBarMine);
	ShowControlBar(&m_wndToolBarMine, FALSE, FALSE);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	CMenu* pSysMenu = GetMenu(); // 获得程序菜单指针
	CMenu* pSubMenu = pSysMenu->GetSubMenu(1);
	// 获得第二个子菜单的指针
	CString StrMenuItem(L"新的菜单项");
	// 增加一水平分隔线
	pSubMenu->AppendMenu(MF_SEPARATOR);
	pSubMenu->AppendMenu
	(MF_STRING, ID_NEW_MENUITEM, StrMenuItem);
	// 在子菜单中增加一菜单项
	// 允许使用ON_UPDATE_COMMAND_UI或ON_COMMAND的菜单项
	m_bAutoMenuEnable = FALSE;     // 关闭系统自动更新菜单状态  
	pSysMenu->EnableMenuItem
	(ID_NEW_MENUITEM, MF_BYCOMMAND | MF_ENABLED);
	// 激活菜单项  
	DrawMenuBar();		     // 更新菜单
	GetMenu()->GetSubMenu(5)->SetDefaultItem(IDM_MENU_DEFAULT);
	m_bitmap.LoadBitmap(IDB_BITMAP1);
	GetMenu()->GetSubMenu(5)
		->SetMenuItemBitmaps(3, MF_BYPOSITION, &m_bitmap, &m_bitmap);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序



void CMainFrame::OnModalDlg()
{
	// TODO: 在此添加命令处理程序代码
	CFirstDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnModelessDlg()
{
	// TODO: 在此添加命令处理程序代码
	CSecondDlg* pDlg = new CSecondDlg;
	pDlg->Create(IDD_DIALOG_SECOND);
	pDlg->ShowWindow(SW_SHOWNORMAL);
}

void CMainFrame::OnFileDlg()
{
	// TODO: 在此添加命令处理程序代码
	CString filter;
	filter = "文本文件(*.txt)|*.txt|C++文件(*.h,*.cpp)|*.h;*.cpp||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString str;
		POSITION pos;
		pos = dlg.GetStartPosition();
		while (pos != NULL)
		{
			str = dlg.GetNextPathName(pos);
			AfxMessageBox(str);
		}
	}

}


void CMainFrame::OnColorDlg()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog dlg(0,0,NULL);
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		TRACE(_T("RGB value of the selected color - red = %u, ")
			_T("green = %u, blue = %u\n"),
		GetRValue(color), GetGValue(color), GetBValue(color));
		CString str1,str2;
		str1.Format(_T("%d"), GetRValue(color));
		str2 += _T("Red:") + str1 + _T("\n");
		str1.Format(_T("%d"), GetGValue(color));
		str2 += _T("Greed:") + str1 + _T("\n");
		str1.Format(_T("%d"), GetBValue(color));
		str2 += _T("Blue:") + str1 + _T("\n");
		MessageBox(str2,L"颜色信息",MB_OK);
	}
}


void CMainFrame::OnCtlCreateBtn()
{
	// TODO: 在此添加命令处理程序代码
	CDlgBtnCreate dlg;
	dlg.DoModal();

}


void CMainFrame::OnCtlNet()
{
	// TODO: 在此添加命令处理程序代码
	CDlgNetQuestionary dlg;
	dlg.DoModal();
}


void CMainFrame::OnCtlColor()
{
	// TODO: 在此添加命令处理程序代码
	CBkColorDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnCtlStudentList()
{
	// TODO: 在此添加命令处理程序代码
	CListDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnCtlPropSheet()
{
	// TODO: 在此添加命令处理程序代码
	CPropSheetMine propSheetMine(L"工作意向表单");
	propSheetMine.SetWizardMode();
	if (ID_WIZFINISH != propSheetMine.DoModal())
		return;
	CString strTemp;
	strTemp = L"你的职业：";
	switch (propSheetMine.m_propPage1.m_nOccupation)
	{
	case 0:
		strTemp += L"程序员";
		break;
	case 1:
		strTemp += L"系统工程师";
		break;
	case 2:
		strTemp += L"项目经理";
		break;
	default:
		break;
	}
	strTemp += L"\n你的兴趣爱好： ";
	if (propSheetMine.m_propPage2.m_bFootball)
	{
		strTemp += L"足球 ";
	}
	if (propSheetMine.m_propPage2.m_bBasketball)
	{
		strTemp += L"篮球 ";
	}
	if (propSheetMine.m_propPage2.m_bVolleyball)
	{
		strTemp += L"排球 ";
	}
	if (propSheetMine.m_propPage2.m_bSwim)
	{
		strTemp += L"游泳 ";
	}
	MessageBox(strTemp);

}


void CMainFrame::OnCommandTest()
{
	// TODO: 在此添加命令处理程序代码
	MessageBox(L"按下了命令响应菜单！");
}


BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (LOWORD(wParam) == ID_NEW_MENUITEM)
		MessageBox(L"你选中了新的菜单项");
	return CFrameWnd::OnCommand(wParam, lParam);
}


void CMainFrame::OnMenuCheck()
{
	// TODO: 在此添加命令处理程序代码
	m_bTag = !m_bTag;
}


void CMainFrame::OnUpdateMenuCheck(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
	if (m_bTag)
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);

}


void CMainFrame::OnUpdateMenuDefault(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();

}


void CMainFrame::OnUpdateMenuPic(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();

}


void CMainFrame::OnMenuUpdate()
{
	// TODO: 在此添加命令处理程序代码
	if (m_menuMine.m_hMenu)
		m_menuMine.DestroyMenu();
	m_menuMine.LoadMenu(IDR_MENU_NEW);
	SetMenu(&m_menuMine);
	ShowControlBar(&m_wndToolBar, FALSE, FALSE);
	ShowControlBar(&m_wndToolBarMine, TRUE, FALSE);

	m_bTagNewBar = 1;

}


void CMainFrame::OnMenuReturn()
{
	// TODO: 在此添加命令处理程序代码
	if (m_menuMine.m_hMenu)
		m_menuMine.DestroyMenu();
	m_menuMine.LoadMenu(IDR_MAINFRAME);
	ShowControlBar(&m_wndToolBar, TRUE, FALSE);
	ShowControlBar(&m_wndToolBarMine, FALSE, FALSE);
	m_bTagNewBar = 0;

	SetMenu(&m_menuMine);
	GetMenu()->GetSubMenu(4)->SetDefaultItem(IDM_MENU_DEFAULT);
	GetMenu()->GetSubMenu(4)->SetMenuItemBitmaps
	(3, MF_BYPOSITION, &m_bitmap, &m_bitmap);

}


void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CMenu* pSysMenu = GetMenu();
	int nCount = pSysMenu->GetMenuItemCount();
	if (nCount > 1)
	{
		pSysMenu->GetSubMenu(nCount - 1)
			->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, this);
	}

}


void CMainFrame::OnUpdateMenuUpdate(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (m_bTagNewBar)
		pCmdUI->Enable(0);
	else
		pCmdUI->Enable(1);

}


void CMainFrame::OnUpdateMenuReturn(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (m_bTagNewBar)
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}


void CMainFrame::OnUpdateTime(CCmdUI* pCmdUI)
{
	CTime t = CTime::GetCurrentTime();
	CString s = t.Format("%H:%M:%S");
/*	char szTime[6];
	int nHour = t.GetHour();
	int nMinute = t.GetMinute();
	int nSecond = t.GetSecond();
	if (nHour > 12)
		nHour = nHour - 12;
	wsprintf(szTime, L"%i:%02i:%02i", nHour, nMinute, nSecond);*/
	m_wndStatusBar.SetPaneText(
		m_wndStatusBar.CommandToIndex(ID_INDICATOR_TIME),s);
/*	CMainFrame::OnUpdateTime(pCmdUI);*/	
	pCmdUI->Enable();
}



void CMainFrame::OnUpdateFont(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
/*	LOGFONT	lf;
	CFont	cf;
	CDC* pDC = GetDC();
	memset(&lf, 0, sizeof(LOGFONT));
	CFontDialog dlg(&lf);
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&lf);
		pDC->SetTextColor(dlg.GetColor());
		cf.CreateFontIndirect(&lf);
		CFont* oldfont = pDC->SelectObject(&cf);
		pDC->TextOut(100, 100, L"Hello");
		pDC->SelectObject(oldfont);
		cf.DeleteObject();
	}
	ReleaseDC(pDC);*/
	LOGFONT	lf;
	CFont	cf;
	CDC* pDC = GetDC();
	memset(&lf, 0, sizeof(LOGFONT));
	CFontDialog dlg(&lf);
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&lf);
		pDC->SetTextColor(dlg.GetColor());
		cf.CreateFontIndirect(&lf);
		CFont* oldfont = pDC->SelectObject(&cf);
		pDC->TextOut(100, 100, L"东北大学");
		pDC->SelectObject(oldfont);
		cf.DeleteObject();
	}
	ReleaseDC(pDC);

}
