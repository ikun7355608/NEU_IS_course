
// MFCApplication1View.cpp: CMFCApplication1View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCApplication1.h"
#endif

#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MainFrm.h"


// CMFCApplication1View

IMPLEMENT_DYNCREATE(CMFCApplication1View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication1View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
//	ON_WM_MOUSEMOVE()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CMFCApplication1View 构造/析构

CMFCApplication1View::CMFCApplication1View() noexcept
{
	// TODO: 在此处添加构造代码

}

CMFCApplication1View::~CMFCApplication1View()
{
}

BOOL CMFCApplication1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCApplication1View 绘图

void CMFCApplication1View::OnDraw(CDC* pDC)
{
	CMFCApplication1Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
/*	if (!pDoc)
		return;
	for (int i = 0; i < 300; i++)
	{
		CPoint p(i, i);
		pDC->MoveTo(p);
		pDC->SetPixel(p, RGB(255, 0, 0));
	}
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(0, 0);
	pDC->LineTo(100, 100);
	pDC->MoveTo(0, 100);
	pDC->LineTo(100, 0);

	pDC->SelectObject(pOldPen);
	CPoint p[5] = { CPoint(50,50),CPoint(100,50),CPoint(100,100),
								   CPoint(50,100),CPoint(50,50) };
	pDC->Polyline(p, 5);

	pDC->SelectObject(pOldPen);
	pDC->MoveTo(0, 0);
	CPoint q[5] = { CPoint(50,50),CPoint(300,50),CPoint(300,300),
							   CPoint(50,300),CPoint(50,50) };
	pDC->PolylineTo(q, 5);
	pDC->Arc(CRect(50, 50, 300, 300), CPoint(175, 50), CPoint(175, 300));
	
	pDC->SelectObject(pOldPen);
	CBrush  bs;
	bs.CreateSolidBrush(RGB(255, 0, 0));
	CBrush* pOldBrush = pDC->SelectObject(&bs);

	CRect rect(50, 50, 300, 300);
	pDC->Rectangle(&rect);

	pDC->SelectObject(pOldBrush);
	bs.CreateHatchBrush(HS_CROSS, RGB(255, 0, 0));
	CBrush* pOldBrush = pDC->SelectObject(&bs);

	CRect rect0(0, 0, 100, 100);
	pDC->Rectangle(&rect0);
	CRect rect1(0, 0, 100, 100);
	pDC->Ellipse(&rect1);
	//CRect rect2(0,0,50,100);
	//pDC->Ellipse(&rect2); 

	pDC->SelectObject(pOldBrush);


	pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

	CPoint poly[4];
	poly[0] = CPoint(50, 50);
	poly[1] = CPoint(100, 100);
	poly[2] = CPoint(100, 150);
	poly[3] = CPoint(50, 100);
	pDC->Polygon(poly, 4);

	pDC->SelectObject(pOldPen);
	*/

/*	CPen newPen(PS_DASHDOTDOT, 2, (COLORREF)0);  // black pen,
										  //  2 pixels wide
	CPen* pOldPen = pDC->SelectObject(&newPen);

	pDC->MoveTo(10, 10);
	pDC->LineTo(110, 10);
	pDC->SelectObject(pOldPen);                   // newPen is deselected
	CGdiObject* pOld;
	pOld = pDC->SelectStockObject(DKGRAY_BRUSH);
	pDC->Rectangle(0, 0, 300, 300);*/
	CRect rc(10, 10, 200, 140);
	pDC->Rectangle(rc);
	pDC->DrawText(L"单行文本居中", rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rc.OffsetRect(200, 0);
	pDC->Rectangle(rc);
	int nTab = 40;
	pDC->TabbedTextOut(rc.left, rc.top, L"绘制\tTab\t文本\t示例",
		1, &nTab, rc.left);
	nTab = 80;
	pDC->TabbedTextOut(rc.left, rc.top + 20, L"绘制\tTab\t文本\t示例",
		1, &nTab, rc.left);
	pDC->TabbedTextOut(rc.left, rc.top + 40, L"绘制\tTab\t文本\t示例",
		0, NULL, 0);
/*	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 0;
	lf.lfCharSet = GB2312_CHARSET;
	_tcsncpy_s(lf.lfFaceName, 100, _T("Arial"), 7);
	CFont cf;
	cf.CreateFontIndirect(&lf);
	CFont* oldfont = pDC->SelectObject(&cf);
	pDC->TextOut(100, 100, L"Hello");
	pDC->SelectObject(oldfont);
	cf.DeleteObject();*/
	CString str = L"字体效果";
	CFont NewFont;
	NewFont.CreateFont
	(
		65,                         		//字体高度
		0,			//由系统根据高宽比选取字体最佳宽度
		0,			//文本的倾斜度为0,表示水平
		0,			//字体的倾斜度为0
		FW_DONTCARE,		//字体的粗度
		true,			//斜体
		true,			//下滑线
		true,			//删除线
		DEFAULT_CHARSET,	//所用的字符集
		OUT_DEFAULT_PRECIS,	//输出精度
		CLIP_DEFAULT_PRECIS,	//剪裁精度
		DEFAULT_QUALITY,		//默认输出质量
		DEFAULT_PITCH | FF_DONTCARE,	//字间距和字体系列使用默认值
		L"黑体"				//字体名称
	);	
	CFont* pOldFont;
	pOldFont = pDC->SelectObject(&NewFont);
	pDC->SetTextColor(RGB(255, 0, 0));
	pDC->TextOut(10, 10, str);



	// TODO: 在此处为本机数据添加绘制代码
}


// CMFCApplication1View 打印

BOOL CMFCApplication1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCApplication1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCApplication1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMFCApplication1View 诊断

#ifdef _DEBUG
void CMFCApplication1View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication1Doc* CMFCApplication1View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication1Doc)));
	return (CMFCApplication1Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplication1View 消息处理程序




void CMFCApplication1View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString str;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CStatusBar* pStatus = &pFrame->m_wndStatusBar;
	if (pStatus)
	{
		pStatus->SetPaneStyle(1, SBPS_POPOUT);
		str.Format(L"X=%d, Y=%d", point.x, point.y);
		pStatus->SetPaneText(1, str);
	}
	CView::OnMouseMove(nFlags, point);
}
