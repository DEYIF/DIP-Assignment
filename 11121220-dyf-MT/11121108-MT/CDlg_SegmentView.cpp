// CDlg_SegmentView.cpp: 实现文件
//

#include "pch.h"
#include "11121220-dyf-MT.h"
#include "afxdialogex.h"
#include "CDlg_SegmentView.h"


// CDlg_SegmentView 对话框

IMPLEMENT_DYNAMIC(CDlg_SegmentView, CDialogEx)

CDlg_SegmentView::CDlg_SegmentView(CDib* pDib, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SegmentView, pParent), 
	segTransImg(nullptr),
	nWidth(0),
	nHeight(0),
	nRawHeight(0),
	nRawWidth(0),
	nImgNum(0),
	nWidthBytes(0),
	currentTrans(0),
	currentSagital(0),
	currentCoronal(0)
{
	if (pDib != NULL) {
		transSegFlag = false;
		m_brush.CreateSolidBrush(RGB(255, 255, 255));
		nWidth = pDib->getWidth();
		nHeight = pDib->getHeight();
		nImgNum = pDib->GetImgNum();
		nRawWidth = pDib->getHeight();
		nRawHeight = pDib->getWidth();
		nWidthBytes = abs(pDib->GetPitch());
		if (segTransImg != NULL) {
			delete[] segTransImg;
			segTransImg = NULL;
		}
		//初始化图像
		segTransImg = new uint16_t[nWidth * nHeight * nImgNum];
		memset(segTransImg, 0, nWidth * nHeight * nImgNum * sizeof(uint16_t));
		memcpy(segTransImg, pDib->GetRawBuffer(), nWidth * nHeight * nImgNum * sizeof(uint16_t));
		dibTrans.CreateDisplayDib(segTransImg, nWidth, nHeight, 8);

		//矢状面
		showSagittal();

		//冠状面
		showCoronal();
		
	}
	else {
		return;
	}
}

CDlg_SegmentView::~CDlg_SegmentView()
{
	if (segTransImg != NULL) {
		delete[] segTransImg;
		segTransImg = NULL;
	}
}

void CDlg_SegmentView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TransView, m_picTrans);
	DDX_Control(pDX, IDC_sldTrans, m_sldTrans);
	DDX_Control(pDX, IDC_SagittalView, m_picSag);
	DDX_Control(pDX, IDC_CoronalView, m_picCorn);
	DDX_Control(pDX, IDC_sldSagittal, m_sldSag);
	DDX_Control(pDX, IDC_slgCoronal, m_sldCorn);

}


BEGIN_MESSAGE_MAP(CDlg_SegmentView, CDialogEx)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_segTransBtn, &CDlg_SegmentView::OnBnClickedsegTrans)
	ON_BN_CLICKED(IDC_segSagBtn, &CDlg_SegmentView::OnBnClickedsegsagbtn)
	ON_BN_CLICKED(IDC_segCorBtn, &CDlg_SegmentView::OnBnClickedsegcorbtn)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlg_SegmentView 消息处理程序
BOOL CDlg_SegmentView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//初始化滑块
	m_sldTrans.SetRange(0, nImgNum - 1);
	m_sldTrans.SetPos(0);
	m_sldTrans.SetTicFreq(1);
	m_sldTrans.SetPageSize(1);

	m_sldSag.SetRange(0, nImgNum - 1);
	m_sldSag.SetPos(0);
	m_sldSag.SetTicFreq(1);
	m_sldSag.SetPageSize(1);

	m_sldCorn.SetRange(0, nImgNum - 1);
	m_sldCorn.SetPos(0);
	m_sldCorn.SetTicFreq(1);
	m_sldCorn.SetPageSize(1);
	return TRUE;
	// return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlg_SegmentView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	if (segTransImg == NULL)
	{
		return;
	}

	// 绘制横截面
	// 获取控件的DC
	CWnd* pWnd = GetDlgItem(IDC_TransView);
	CDC* pDC = pWnd->GetDC();

	// 获取控件的客户区矩形
	CRect rect;
	pWnd->GetClientRect(&rect);

	// 绘制图像到控件DC
	dibTrans.Draw(pDC->GetSafeHdc(), rect);

	// 释放DC
	pWnd->ReleaseDC(pDC);

	// 绘制矢状面
	
	pWnd = GetDlgItem(IDC_SagittalView);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibSagittal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
	pWnd->ReleaseDC(pDC);	// 释放DC


	// 绘制冠状面
	pWnd = GetDlgItem(IDC_CoronalView);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibCoronal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
	pWnd->ReleaseDC(pDC);	// 释放DC

	

}

void CDlg_SegmentView::showSagittal()
{
	// TODO: 在此处添加实现代码.
	//定义新图像的行列比例，如果行列比例不为1，则需要进行插值
	//const float row_col_ratio = 1.0f / 0.68359375f; 
	const float row_col_ratio = 1.0f;
	if (segSagImg != NULL) {
		delete[] segSagImg;
		segSagImg = NULL;
	}
	int sagWidth = nRawHeight;
	int sagHeight = static_cast<int>(nImgNum * row_col_ratio);

	uint16_t* pTemp = new uint16_t[nImgNum * nRawHeight];
	memset(pTemp, 255, nRawHeight * nImgNum * sizeof(uint16_t));
	for (int z = 0; z < nImgNum; ++z) {
		for (int y = 0; y < nRawHeight; ++y) {
			int index = z * nWidth * nRawHeight + y * nWidth + currentSagital;
			pTemp[y + z * nRawHeight] = segTransImg[index];
		}
	}
	segSagImg = new uint16_t[sagHeight * sagWidth];
	memset(segSagImg, 255, sagWidth * sagHeight * sizeof(uint16_t));
	if (row_col_ratio != 1.0f)
	{
		//如果行列比例不为1，则需要进行插值
		dibSagittal.BilinearInterpolate(segSagImg, pTemp, nRawWidth, nImgNum, row_col_ratio);
	}
	else {
		//如果行列比例为1，则直接复制
		memcpy(segSagImg, pTemp, nRawWidth * nImgNum * sizeof(uint16_t));
	}
	dibSagittal.CreateDisplayDib(segSagImg, sagWidth, sagHeight, 8);
	delete[] pTemp;

}

void CDlg_SegmentView::showCoronal()
{
	// TODO: 在此处添加实现代码.
	//const float row_col_ratio = 1.0f / 0.68359375f; 
	const float row_col_ratio = 1.0f;
	if (segCorImg != NULL) {
		delete[] segCorImg;
		segCorImg = NULL;
	}
	int newWidth = nRawWidth;
	int newHeight = static_cast<int>(nImgNum * row_col_ratio);
	uint16_t* pTemp = new uint16_t[nRawWidth * nImgNum];
	memset(pTemp, 255, nRawWidth * nImgNum * sizeof(uint16_t));
	for (int z = 0; z < nImgNum; ++z) {
		for (int x = 0; x < nRawWidth; ++x) {
			int index = z * nRawWidth * nRawHeight + x + currentCoronal * nRawWidth;
			pTemp[x + z * nRawWidth] = segTransImg[index];
		}
	}
	segCorImg = new uint16_t[nRawWidth * newHeight];
	memset(segCorImg, 255, newWidth * newHeight * sizeof(uint16_t));
	if (row_col_ratio != 1.0f)
	{
		//如果行列比例不为1，则需要进行插值
		dibCoronal.BilinearInterpolate(segCorImg, pTemp, nRawWidth, nImgNum, row_col_ratio);
	}
	else {
		//如果行列比例为1，则直接复制
		memcpy(segCorImg, pTemp, nRawWidth * nImgNum * sizeof(uint16_t));
	}
	dibCoronal.CreateDisplayDib(segCorImg, newWidth, newHeight, 8);
	delete[] pTemp;
}

void CDlg_SegmentView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == m_sldTrans.GetSafeHwnd()) {
		OnSldScroll();
	}
	if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == m_sldSag.GetSafeHwnd()) {
		OnSldScroll();
	}
	if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == m_sldCorn.GetSafeHwnd()) {
		OnSldScroll();
	}
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDlg_SegmentView::OnSldScroll()
{
	// TODO: 在此处添加实现代码.

	int TransPos = m_sldTrans.GetPos();
	int SagitalPos = m_sldSag.GetPos();
	int CoronalPos = m_sldCorn.GetPos();
	currentTrans = TransPos;
	currentSagital = SagitalPos;
	currentCoronal = CoronalPos;
	dibTrans.CreateDisplayDib(segTransImg + nWidth * nHeight * TransPos, nWidth, nHeight, 8);
	showCoronal();
	showSagittal();
	if (transSegFlag == TRUE) {
		OnBnClickedsegTrans();
	}
	
	// 获取控件的DC
	CWnd* pWnd = GetDlgItem(IDC_TransView);
	// 获取控件的客户区矩形
	CRect rect;
	pWnd->GetClientRect(&rect);


	InvalidateRect(rect);
	UpdateWindow();
}


BOOL CDlg_SegmentView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}


void CDlg_SegmentView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nChar == VK_RIGHT || nChar == 68) {
		currentTrans++;
		currentSagital++;
		currentCoronal++;
		if (currentTrans >= nImgNum) {
			currentTrans = 0;
		}
		if (currentSagital >= nImgNum) {
			currentSagital = 0;
		}
		if (currentCoronal >= nImgNum) {
			currentCoronal = 0;
		}
	}
	if (nChar == VK_LEFT || nChar == 65) {
		currentTrans--;
		currentSagital--;
		currentCoronal--;
		if (currentTrans < 0) {
			currentTrans = nImgNum - 1;
		}
		if (currentSagital < 0) {
			currentSagital = nImgNum - 1;
		}
		if (currentCoronal < 0) {
			currentCoronal = nImgNum - 1;
		}
	}
	m_sldTrans.SetPos(currentTrans);
	m_sldSag.SetPos(currentSagital);
	m_sldCorn.SetPos(currentCoronal);
	OnSldScroll();

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CDlg_SegmentView::OnBnClickedsegTrans()
{
	// TODO: 在此添加控件通知处理程序代码
	transSegFlag = true;
	COpenCVProcess cvp(&dibTrans);
	cvp.SegWatershed();
	//cvp.CannyEdge();
	cvp.Mat2Dib(dibTrans);
	Invalidate();
	UpdateWindow();
}


void CDlg_SegmentView::OnBnClickedsegsagbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	COpenCVProcess cvp(&dibSagittal);
	cvp.SegWatershed();
	cvp.Mat2Dib(dibSagittal);
	Invalidate();
	UpdateWindow();
}


void CDlg_SegmentView::OnBnClickedsegcorbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	COpenCVProcess cvp(&dibCoronal);
	cvp.SegWatershed();
	cvp.Mat2Dib(dibCoronal);
	Invalidate();
	UpdateWindow();
}


HBRUSH CDlg_SegmentView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_DLG)

	{
		return m_brush;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}




