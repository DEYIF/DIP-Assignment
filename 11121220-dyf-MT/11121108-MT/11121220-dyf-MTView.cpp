
// 11121220-dyf-MTView.cpp: CMy11121108MTView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "11121220-dyf-MT.h"
#endif

#include "11121220-dyf-MTDoc.h"
#include "11121220-dyf-MTView.h"
#include "Dib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy11121108MTView

IMPLEMENT_DYNCREATE(CMy11121108MTView, CView)

BEGIN_MESSAGE_MAP(CMy11121108MTView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CMy11121108MTView 构造/析构

CMy11121108MTView::CMy11121108MTView() noexcept
{
	// TODO: 在此处添加构造代码

}

CMy11121108MTView::~CMy11121108MTView()
{
}

BOOL CMy11121108MTView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy11121108MTView 绘图

void CMy11121108MTView::OnDraw(CDC* /*pDC*/)
{
	CMy11121220MTDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	if (pDoc->m_pDib != NULL)
	{
		pDoc->m_pDib->Draw(GetDC()->m_hDC, 0, 0);
	}
}


// CMy11121108MTView 打印

BOOL CMy11121108MTView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy11121108MTView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMy11121108MTView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy11121108MTView 诊断

#ifdef _DEBUG
void CMy11121108MTView::AssertValid() const
{
	CView::AssertValid();
}

void CMy11121108MTView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy11121220MTDoc* CMy11121108MTView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy11121220MTDoc)));
	return (CMy11121220MTDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy11121108MTView 消息处理程序


void CMy11121108MTView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//判断方向键右键是否被按下
	if (nChar == VK_RIGHT || nChar == 68) {
		GetDocument()->OnNextPage();
	}
	if (nChar == VK_LEFT || nChar == 65) {
		GetDocument()->OnFormerPage();
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
