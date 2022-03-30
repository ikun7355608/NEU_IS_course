
// MainFrm.h: CMainFrame 类的接口
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // 控件条嵌入成员
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bTag=1;
	CBitmap m_bitmap;
	CMenu m_menuMine;
protected:
	CToolBar    m_wndToolBarMine;
private:
	BOOL    	    m_bTagNewBar=0;

public:
	afx_msg void OnModalDlg();
	afx_msg void OnModelessDlg();
	afx_msg void OnFileDlg();
	afx_msg void OnColorDlg();
	afx_msg void OnCtlCreateBtn();
	afx_msg void OnCtlNet();
	afx_msg void OnCtlColor();
	afx_msg void OnCtlStudentList();
	afx_msg void OnCtlPropSheet();
	afx_msg void OnCommandTest();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMenuCheck();
	afx_msg void OnUpdateMenuCheck(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuDefault(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuPic(CCmdUI* pCmdUI);
	afx_msg void OnMenuUpdate();
	afx_msg void OnMenuReturn();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnUpdateMenuUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuReturn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTime(CCmdUI* pCmdUI);

	afx_msg void OnUpdate32822(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFont(CCmdUI* pCmdUI);
};


