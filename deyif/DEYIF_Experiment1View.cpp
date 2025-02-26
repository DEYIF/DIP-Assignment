
// DEYIF_Experiment1View.cpp: CDEYIFExperiment1View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DEYIF_Experiment1.h"
#endif

#include "DEYIF_Experiment1Doc.h"
#include "DEYIF_Experiment1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDEYIFExperiment1View

IMPLEMENT_DYNCREATE(CDEYIFExperiment1View, CView)

BEGIN_MESSAGE_MAP(CDEYIFExperiment1View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CDEYIFExperiment1View 构造/析构

CDEYIFExperiment1View::CDEYIFExperiment1View() noexcept
{
	// TODO: 在此处添加构造代码


}

CDEYIFExperiment1View::~CDEYIFExperiment1View()
{
}

BOOL CDEYIFExperiment1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CDEYIFExperiment1View 绘图

void CDEYIFExperiment1View::OnDraw(CDC* pDC)
{
	CDEYIFExperiment1Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码

	if (pDoc -> m_pDib != NULL) 
	{
		pDoc -> m_pDib ->Draw(GetDC()->m_hDC, 0, 0);
	}
}


// CDEYIFExperiment1View 打印

BOOL CDEYIFExperiment1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CDEYIFExperiment1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CDEYIFExperiment1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CDEYIFExperiment1View 诊断

#ifdef _DEBUG
void CDEYIFExperiment1View::AssertValid() const
{
	CView::AssertValid();
}

void CDEYIFExperiment1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDEYIFExperiment1Doc* CDEYIFExperiment1View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDEYIFExperiment1Doc)));
	return (CDEYIFExperiment1Doc*)m_pDocument;
}
#endif //_DEBUG


// CDEYIFExperiment1View 消息处理程序

bool CDEYIFExperiment1View::DrawRawPic(CDib* dib, BITMAPINFO bitmapInfo)
{
	// TODO: 在此处添加实现代码.
	//CDC* pDC = GetDC();
	//HBITMAP hBitmap;
	//hBitmap = CreateDIBSection(pDC->m_hDC, &bitmapInfo, DIB_RGB_COLORS, (void**)&(dib->m_pDibBits), NULL, 0);
	//// 将 CDib 对象转换为 CBitmap 对象
	//CBitmap bitmap;
	//bitmap.Attach(dib->GetBitmap());

	//// 创建一个内存 DC
	//CDC memDC;
	//memDC.CreateCompatibleDC(pDC);

	//// 将位图选入内存 DC
	//HBITMAP hOldBitmap = (HBITMAP)memDC.SelectObject(bitmap.GetSafeHandle());

	//pDC->BitBlt(0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, &memDC, 0, 0, SRCCOPY);



	return false;
}



void CDEYIFExperiment1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//判断方向键右键是否被按下
	if (nChar == VK_RIGHT||nChar==68) {
		GetDocument()->RightKey();
	}
	if (nChar == VK_LEFT||nChar==65) {
		GetDocument()->LeftKey();
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
