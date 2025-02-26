// FolderSegView.cpp: 实现文件
//

#include "pch.h"
#include "11121220-dyf-MT.h"
#include "afxdialogex.h"
#include "FolderSegView.h"


// FolderSegView 对话框

IMPLEMENT_DYNAMIC(FolderSegView, CDialogEx)

FolderSegView::FolderSegView(CDib* t1NDib, CDib* t1WDib, CDib* t2WDib, CDib* t2FDib,CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FolderSegmentView, pParent),
	t1NTransImg(nullptr),
	t1WTransImg(nullptr),
	t2WTransImg(nullptr),
	t2FTransImg(nullptr),
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
	if (t1NDib != NULL) {
		transSegFlag = false;
		m_brush.CreateSolidBrush(RGB(255, 255, 255));
		nWidth = t1NDib->getWidth();
		nHeight = t1NDib->getHeight();
		nImgNum = t1NDib->GetImgNum();
		nRawWidth = t1NDib->getHeight();
		nRawHeight = t1NDib->getWidth();
		nWidthBytes = abs(t1NDib->GetPitch());
		if (t1NTransImg != NULL) {
			delete[] t1NTransImg;
			t1NTransImg = NULL;
		}
		if (t1WTransImg != NULL) {
			delete[] t1WTransImg;
			t1WTransImg = NULL;
		}
		if (t2WTransImg != NULL) {
			delete[] t2WTransImg;
			t2WTransImg = NULL;
		}
		if (t2FTransImg != NULL) {
			delete[] t2FTransImg;
			t2FTransImg = NULL;
		}

		//初始化图像
		const float row_col_ratio = 1.0f;
		int sagWidth = nRawHeight;
		int sagHeight = static_cast<int>(nImgNum * row_col_ratio);
		int corWidth = nRawWidth;
		int corHeight = static_cast<int>(nImgNum * row_col_ratio);

		t1NTransImg = new uint16_t[nWidth * nHeight * nImgNum];
		memset(t1NTransImg, 0, nWidth * nHeight * nImgNum * sizeof(uint16_t));
		memcpy(t1NTransImg, t1NDib->GetRawBuffer(), nWidth * nHeight * nImgNum * sizeof(uint16_t));
		dibT1NTrans.CreateDisplayDib(t1NTransImg, nWidth, nHeight, 8);

		t1WTransImg = new uint16_t[nWidth * nHeight * nImgNum];
		memset(t1WTransImg, 0, nWidth * nHeight * nImgNum * sizeof(uint16_t));
		memcpy(t1WTransImg, t1WDib->GetRawBuffer(), nWidth * nHeight * nImgNum * sizeof(uint16_t));
		dibT1WTrans.CreateDisplayDib(t1WTransImg, nWidth, nHeight, 8);

		t2WTransImg = new uint16_t[nWidth * nHeight * nImgNum];
		memset(t2WTransImg, 0, nWidth * nHeight * nImgNum * sizeof(uint16_t));
		memcpy(t2WTransImg, t2WDib->GetRawBuffer(), nWidth * nHeight * nImgNum * sizeof(uint16_t));
		dibT2WTrans.CreateDisplayDib(t2WTransImg, nWidth, nHeight, 8);

		t2FTransImg = new uint16_t[nWidth * nHeight * nImgNum];
		memset(t2FTransImg, 0, nWidth * nHeight * nImgNum * sizeof(uint16_t));
		memcpy(t2FTransImg, t2FDib->GetRawBuffer(), nWidth * nHeight * nImgNum * sizeof(uint16_t));
		dibT2FTrans.CreateDisplayDib(t2FTransImg, nWidth, nHeight, 8);

		//矢状面
		t1NSagImg = Trans2Sagittal(t1NTransImg);
		dibT1NSagittal.CreateDisplayDib(t1NSagImg, sagWidth, sagHeight, 8);

		t1WSagImg = Trans2Sagittal(t1WTransImg);
		dibT1WSagittal.CreateDisplayDib(t1WSagImg, sagWidth, sagHeight, 8);

		t2WSagImg = Trans2Sagittal(t2WTransImg);
		dibT2WSagittal.CreateDisplayDib(t2WSagImg, sagWidth, sagHeight, 8);

		t2FSagImg = Trans2Sagittal(t2FTransImg);
		dibT2FSagittal.CreateDisplayDib(t2FSagImg, sagWidth, sagHeight, 8);

		//冠状面
		t1NCorImg = Trans2Coronal(t1NTransImg);
		dibT1NCoronal.CreateDisplayDib(t1NCorImg, corWidth, corHeight, 8);

		t1WCorImg = Trans2Coronal(t1WTransImg);
		dibT1WCoronal.CreateDisplayDib(t1WCorImg, corWidth, corHeight, 8);

		t2WCorImg = Trans2Coronal(t2WTransImg);
		dibT2WCoronal.CreateDisplayDib(t2WCorImg, corWidth, corHeight, 8);

		t2FCorImg = Trans2Coronal(t2FTransImg);
		dibT2FCoronal.CreateDisplayDib(t2FCorImg, corWidth, corHeight, 8);

	}
	else {
		return;
	}
}

FolderSegView::~FolderSegView()
{
	if (t1NTransImg != NULL) {
		delete[] t1NTransImg;
		delete[] t1WTransImg;
		delete[] t2WTransImg;
		delete[] t2FTransImg;
		t1NTransImg = NULL;
		t1WTransImg = NULL;
		t2WTransImg = NULL;
		t2FTransImg = NULL;
	}
}

void FolderSegView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_sldTrans, sldTrans);
	DDX_Control(pDX, IDC_SegTrans, btnTransSeg);
	DDX_Control(pDX, IDC_segSag, btnSag);
	DDX_Control(pDX, IDC_sldSag, sldSag);
	DDX_Control(pDX, IDC_segCor, btnCor);
	DDX_Control(pDX, IDC_sldCor, sldCor);
}

BOOL FolderSegView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化滑块
	sldTrans.SetRange(0, nImgNum - 1);
	sldTrans.SetTicFreq(1);
	sldTrans.SetPageSize(1);
	sldTrans.SetPos(0);

	sldSag.SetRange(0, nRawHeight - 1);
	sldSag.SetTicFreq(1);
	sldSag.SetPageSize(1);
	sldSag.SetPos(0);

	sldCor.SetRange(0, nRawWidth - 1);
	sldCor.SetTicFreq(1);
	sldCor.SetPageSize(1);
	sldCor.SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(FolderSegView, CDialogEx)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SegTrans, &FolderSegView::OnBnClickedSegtrans)
	ON_BN_CLICKED(IDC_segSag, &FolderSegView::OnBnClickedsegsag)
	ON_BN_CLICKED(IDC_segCor, &FolderSegView::OnBnClickedsegcor)
END_MESSAGE_MAP()


// FolderSegView 消息处理程序


void FolderSegView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	if (t1NTransImg == NULL)
	{
		return;
	}

	/////////// 绘制横截面
	// 获取控件DC，绘制t1N
	CWnd* pWnd = GetDlgItem(IDC_TransT1N);
	CDC* pDC = pWnd->GetDC();
	// 获取控件的客户区矩形
	CRect rect;
	pWnd->GetClientRect(&rect);
	// 绘制图像到控件DC
	dibT1NTrans.Draw(pDC->GetSafeHdc(), rect);
	// 释放DC
	pWnd->ReleaseDC(pDC);

	//绘制t1W
	pWnd = GetDlgItem(IDC_TransT1W);
	pDC = pWnd->GetDC();
	pWnd->GetClientRect(&rect);
	dibT1WTrans.Draw(pDC->GetSafeHdc(), rect);
	pWnd->ReleaseDC(pDC);

	//绘制t2W
	pWnd = GetDlgItem(IDC_TransT2W);
	pDC = pWnd->GetDC();
	pWnd->GetClientRect(&rect);
	dibT2WTrans.Draw(pDC->GetSafeHdc(), rect);
	pWnd->ReleaseDC(pDC);

	//绘制t2F
	pWnd = GetDlgItem(IDC_TransT2F);
	pDC = pWnd->GetDC();
	pWnd->GetClientRect(&rect);
	dibT2FTrans.Draw(pDC->GetSafeHdc(), rect);
	pWnd->ReleaseDC(pDC);

	//// 绘制矢状面

	pWnd = GetDlgItem(IDC_SagT1N);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT1NSagittal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
	pWnd->ReleaseDC(pDC);	// 释放DC

	pWnd = GetDlgItem(IDC_SagT1W);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT1WSagittal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
	pWnd->ReleaseDC(pDC);	// 释放DC

	pWnd = GetDlgItem(IDC_SagT2W);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT2WSagittal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
	pWnd->ReleaseDC(pDC);	// 释放DC

	pWnd = GetDlgItem(IDC_SagT2F);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT2FSagittal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
	pWnd->ReleaseDC(pDC);	// 释放DC

	// 绘制冠状面
	pWnd = GetDlgItem(IDC_corT1N);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT1NCoronal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
	pWnd->ReleaseDC(pDC);	// 释放DC

	pWnd = GetDlgItem(IDC_corT1W);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT1WCoronal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC

	pWnd = GetDlgItem(IDC_corT2W);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT2WCoronal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC

	pWnd = GetDlgItem(IDC_corT2F);
	pDC = pWnd->GetDC();	// 获取控件的DC
	pWnd->GetClientRect(&rect);	// 获取控件的客户区矩形
	dibT2FCoronal.Draw(pDC->GetSafeHdc(), rect);	// 绘制图像到控件DC
}


void FolderSegView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == sldTrans.GetSafeHwnd()) {
		OnSldScroll();
	}
	else if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == sldSag.GetSafeHwnd()) {
		OnSldScroll();
	}
	else if (pScrollBar != nullptr && pScrollBar->GetSafeHwnd() == sldCor.GetSafeHwnd()) {
		OnSldScroll();
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}





void FolderSegView::OnSldScroll()
{
	// TODO: 在此处添加实现代码.
	

	int TransPos = sldTrans.GetPos();
	int SagitalPos = sldSag.GetPos();
	int CoronalPos = sldCor.GetPos();
	currentTrans = TransPos;
	currentSagital = SagitalPos;
	currentCoronal = CoronalPos;
	
	dibT1NTrans.CreateDisplayDib(t1NTransImg + nWidth * nHeight * TransPos, nWidth, nHeight, 8);
	dibT1WTrans.CreateDisplayDib(t1WTransImg + nWidth * nHeight * TransPos, nWidth, nHeight, 8);
	dibT2WTrans.CreateDisplayDib(t2WTransImg + nWidth * nHeight * TransPos, nWidth, nHeight, 8);
	dibT2FTrans.CreateDisplayDib(t2FTransImg + nWidth * nHeight * TransPos, nWidth, nHeight, 8);
	
	if (transSegFlag == TRUE) {
		COpenCVProcess cvpT2F(&dibT2FTrans);
		cvpT2F.SegWatershed();
		cvpT2F.Mat2Dib(dibT2FTrans);

		COpenCVProcess cvpT1N(&dibT1NTrans);
		COpenCVProcess cvpT1W(&dibT1WTrans);
		COpenCVProcess cvpT2W(&dibT2WTrans);
		cvpT1N.SegAll(cvpT2F.cvimg);
		cvpT1W.SegAll(cvpT2F.cvimg);
		cvpT2W.SegAll(cvpT2F.cvimg);

		cvpT1N.Mat2Dib(dibT1NTrans);
		cvpT1W.Mat2Dib(dibT1WTrans);
		cvpT2W.Mat2Dib(dibT2WTrans);
	}

	const float row_col_ratio = 1.0f;
	int sagWidth = nRawHeight;
	int sagHeight = static_cast<int>(nImgNum * row_col_ratio);
	int corWidth = nRawWidth;
	int corHeight = static_cast<int>(nImgNum * row_col_ratio);
	//矢状面
	if (t1NSagImg != NULL) {
		delete[] t1NSagImg;
		delete[] t1WSagImg;
		delete[] t2WSagImg;
		delete[] t2FSagImg;
		t1NSagImg = NULL;
		t1WSagImg = NULL;
		t2WSagImg = NULL;
		t2FSagImg = NULL;
	}
	t1NSagImg = Trans2Sagittal(t1NTransImg);
	dibT1NSagittal.CreateDisplayDib(t1NSagImg, sagWidth, sagHeight, 8);

	t1WSagImg = Trans2Sagittal(t1WTransImg);
	dibT1WSagittal.CreateDisplayDib(t1WSagImg, sagWidth, sagHeight, 8);

	
	t2WSagImg = Trans2Sagittal(t2WTransImg);
	dibT2WSagittal.CreateDisplayDib(t2WSagImg, sagWidth, sagHeight, 8);

	t2FSagImg = Trans2Sagittal(t2FTransImg);
	dibT2FSagittal.CreateDisplayDib(t2FSagImg, sagWidth, sagHeight, 8);

	if(sagSegFlag == TRUE){
		COpenCVProcess cvpT2F(&dibT2FSagittal);
		cvpT2F.SegWatershed();
		cvpT2F.Mat2Dib(dibT2FSagittal);

		COpenCVProcess cvpT1N(&dibT1NSagittal);
		COpenCVProcess cvpT1W(&dibT1WSagittal);
		COpenCVProcess cvpT2W(&dibT2WSagittal);
		cvpT1N.SegAll(cvpT2F.cvimg);
		cvpT1W.SegAll(cvpT2F.cvimg);
		cvpT2W.SegAll(cvpT2F.cvimg);

		cvpT1N.Mat2Dib(dibT1NSagittal);
		cvpT1W.Mat2Dib(dibT1WSagittal);
		cvpT2W.Mat2Dib(dibT2WSagittal);
	}
	//冠状面
	if (t1NCorImg != NULL) {
		delete[] t1NCorImg;
		delete[] t1WCorImg;
		delete[] t2WCorImg;
		delete[] t2FCorImg;
		t1NCorImg = NULL;
		t1WCorImg = NULL;
		t2WCorImg = NULL;
		t2FCorImg = NULL;
	}
	t1NCorImg = Trans2Coronal(t1NTransImg);
	dibT1NCoronal.CreateDisplayDib(t1NCorImg, corWidth, corHeight, 8);

	t1WCorImg = Trans2Coronal(t1WTransImg);
	dibT1WCoronal.CreateDisplayDib(t1WCorImg, corWidth, corHeight, 8);

	t2WCorImg = Trans2Coronal(t2WTransImg);
	dibT2WCoronal.CreateDisplayDib(t2WCorImg, corWidth, corHeight, 8);

	t2FCorImg = Trans2Coronal(t2FTransImg);
	dibT2FCoronal.CreateDisplayDib(t2FCorImg, corWidth, corHeight, 8);

	if (corSegFlag == TRUE) {
		COpenCVProcess cvpT2F(&dibT2FCoronal);
		cvpT2F.SegWatershed();
		cvpT2F.Mat2Dib(dibT2FCoronal);

		COpenCVProcess cvpT1N(&dibT1NCoronal);
		COpenCVProcess cvpT1W(&dibT1WCoronal);
		COpenCVProcess cvpT2W(&dibT2WCoronal);
		cvpT1N.SegAll(cvpT2F.cvimg);
		cvpT1W.SegAll(cvpT2F.cvimg);
		cvpT2W.SegAll(cvpT2F.cvimg);

		cvpT1N.Mat2Dib(dibT1NCoronal);
		cvpT1W.Mat2Dib(dibT1WCoronal);
		cvpT2W.Mat2Dib(dibT2WCoronal);
	}

	if (transSegFlag == TRUE) {
		//OnBnClickedsegTrans();
	}

	CRect rect(0,0,1,1);
	InvalidateRect(rect);
	UpdateWindow();
}


uint16_t* FolderSegView::Trans2Sagittal(uint16_t* TransImg)
{
	// TODO: 在此处添加实现代码.
	//定义新图像的行列比例，如果行列比例不为1，则需要进行插值
	//const float row_col_ratio = 1.0f / 0.68359375f; 
	const float row_col_ratio = 1.0f;
	int sagWidth = nRawHeight;
	int sagHeight = static_cast<int>(nImgNum * row_col_ratio);
	uint16_t* SagImg = new uint16_t[sagHeight * sagWidth];
	memset(SagImg, 255, sagWidth * sagHeight * sizeof(uint16_t));

	uint16_t* pTemp = new uint16_t[nImgNum * nRawHeight];
	memset(pTemp, 255, nRawHeight * nImgNum * sizeof(uint16_t));
	for (int z = 0; z < nImgNum; ++z) {
		for (int y = 0; y < nRawHeight; ++y) {
			int index = z * nWidth * nRawHeight + y * nWidth + currentSagital;
			pTemp[y + z * nRawHeight] = TransImg[index];
		}
	}
		
	memcpy(SagImg, pTemp, nRawWidth * nImgNum * sizeof(uint16_t));
	delete[] pTemp;
	return SagImg;
}


uint16_t* FolderSegView::Trans2Coronal(uint16_t* TransImg)
{
	// TODO: 在此处添加实现代码.
	//定义新图像的行列比例，如果行列比例不为1，则需要进行插值
	//const float row_col_ratio = 1.0f / 0.68359375f; 
	const float row_col_ratio = 1.0f;
	int corWidth = nRawWidth;
	int corHeight = static_cast<int>(nImgNum * row_col_ratio);
	uint16_t* CorImg = new uint16_t[corHeight * corWidth];
	memset(CorImg, 255, corWidth * corHeight * sizeof(uint16_t));

	uint16_t* pTemp = new uint16_t[nRawWidth * nImgNum];
	memset(pTemp, 255, nRawWidth * nImgNum * sizeof(uint16_t));
	for (int z = 0; z < nImgNum; ++z) {
		for (int x = 0; x < nRawWidth; ++x) {
			int index = z * nRawWidth * nRawHeight + x + currentCoronal * nRawWidth;
			pTemp[x + z * nRawWidth] = TransImg[index];
		}
	}

	memcpy(CorImg, pTemp, corWidth * corHeight * sizeof(uint16_t));
	return CorImg;
}


void FolderSegView::OnBnClickedSegtrans()
{
	// TODO: 在此添加控件通知处理程序代码
	transSegFlag = !transSegFlag;
	OnSldScroll();
}


void FolderSegView::OnBnClickedsegsag()
{
	// TODO: 在此添加控件通知处理程序代码
	sagSegFlag = !sagSegFlag;
	OnSldScroll();
}


void FolderSegView::OnBnClickedsegcor()
{
	// TODO: 在此添加控件通知处理程序代码
	corSegFlag = !corSegFlag;
	OnSldScroll();
}
