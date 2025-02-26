// CdlgHistogram.cpp: 实现文件
//

#include "pch.h"
#include "DEYIF_Experiment1.h"
#include "afxdialogex.h"
#include "CdlgHistogram.h"
#include "Dib.h"


// CdlgHistogram 对话框

IMPLEMENT_DYNAMIC(CdlgHistogram, CDialogEx)

CdlgHistogram::CdlgHistogram(CDib *pDib, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_HISTO, pParent)
{
	if (pDib != NULL) {
		m_pGrayValueArray=pDib->GrayValueCount();
		m_nLength = pDib->GetMaxColorTableEntries();
	}
	else {
		return;
	}
}

CdlgHistogram::~CdlgHistogram()
{
	if (m_pGrayValueArray != NULL) {
		delete[] m_pGrayValueArray;
	}
}

void CdlgHistogram::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CdlgHistogram, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CdlgHistogram 消息处理程序


void CdlgHistogram::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	if (m_pGrayValueArray == NULL)
	{
		return;
	}
	CRect rect;

	CWnd* pWnd = GetDlgItem(IDC_STATIC);
	// 	this->GetWindowRect(&rect);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	CDC* pDC = &dc;


	pWnd->UpdateWindow();
	// 	this->UpdateWindow();
	pDC->Rectangle(rect);


	CPen* pRedPen = new CPen;
	pRedPen->CreatePen(PS_SOLID, 2, RGB(255, 0, 0));//画坐标轴的笔，红色
	CPen* pOldPen = pDC->SelectObject(pRedPen);//选择新画笔，返回并保存原值

	CPen* pBluePen = new CPen;
	pBluePen->CreatePen(PS_SOLID, 2, RGB(0, 0, 255));

	rect.DeflateRect(20, 20);

	/*	pDC->Rectangle(&rect);*/

	pDC->MoveTo(rect.left, rect.bottom);//从当前点移动到参数点
	pDC->LineTo(rect.left, rect.top);//画纵坐标
	pDC->LineTo(rect.left - 5, rect.top + 5);//画箭头
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left + 5, rect.top + 5);

	pDC->MoveTo(rect.left, rect.bottom);//画横坐标
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.right - 5, rect.bottom - 5);
	pDC->MoveTo(rect.right, rect.bottom);
	pDC->LineTo(rect.right - 5, rect.bottom + 5);


	float DeltaCount = float(rect.Width() - 10) / m_nLength;

	pDC->SelectObject(pBluePen);//选择蓝色画笔，开始画直方图
	int GreyNumberMax = 0;

	for (int i = 0; i < m_nLength; i++)
	{
		if (GreyNumberMax < m_pGrayValueArray[i])
		{
			GreyNumberMax = m_pGrayValueArray[i];//直方图中灰度次数最多的值
		}
	}
	for (int i = 0; i < m_nLength; i++)
	{
		if (GreyNumberMax != 0)
		{
			double Ratio = ((double)m_pGrayValueArray[i]) / GreyNumberMax;
			pDC->MoveTo(rect.left + (int)(i * DeltaCount) + 2, rect.bottom - 2);
			pDC->LineTo(rect.left + (int)(i * DeltaCount) + 2, rect.bottom - 2 - (int)(Ratio * (rect.Height() - 10)));
		}
	}
	for (int i = 0; i < m_nLength; i++)
	{
		if (i % 10 == 0)
		{
			pDC->MoveTo(rect.left + (int)(i * DeltaCount) + 2, rect.bottom - 2);
			pDC->LineTo(rect.left + (int)(i * DeltaCount) + 2, rect.bottom - 2 + 4);
		}
		if (i % 50 == 0)
		{
			pDC->MoveTo(rect.left + (int)(i * DeltaCount) + 2, rect.bottom - 2);
			pDC->LineTo(rect.left + (int)(i * DeltaCount) + 2, rect.bottom - 2 + 10);
		}
	}

	pDC->SelectObject(pOldPen);

	delete pRedPen;
	delete pBluePen;

}
