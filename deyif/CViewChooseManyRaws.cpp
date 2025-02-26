// CViewChooseManyRaws.cpp: 实现文件
//

#include "pch.h"
#include "DEYIF_Experiment1.h"
#include "CViewChooseManyRaws.h"
#include "DEYIF_Experiment1Doc.h"


// CViewChooseManyRaws

IMPLEMENT_DYNCREATE(CViewChooseManyRaws, CFormView)

CViewChooseManyRaws::CViewChooseManyRaws()
	: CFormView(IDD_DLG_SliderManyRaws)
	, m_progressBar(0)
{

}

CViewChooseManyRaws::~CViewChooseManyRaws()
{
}

void CViewChooseManyRaws::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDER1, m_progressBar);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
	m_slider.SetRange(1, 313);
	m_slider.SetPos(1);
	m_slider.SetLineSize(1);
}

BEGIN_MESSAGE_MAP(CViewChooseManyRaws, CFormView)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CViewChooseManyRaws 诊断

#ifdef _DEBUG
void CViewChooseManyRaws::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewChooseManyRaws::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewChooseManyRaws 消息处理程序


void CViewChooseManyRaws::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_progressBar = m_slider.GetPos();
	//CDEYIFExperiment1Doc* pDoc = GetDocument();

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CViewChooseManyRaws::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	// 启动定时器，间隔为 50 毫秒
	//SetTimer(TIMER_ID, 50, NULL);

}
