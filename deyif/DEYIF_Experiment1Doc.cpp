
// DEYIF_Experiment1Doc.cpp: CDEYIFExperiment1Doc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DEYIF_Experiment1.h"
#endif

#include "DEYIF_Experiment1Doc.h"

#include <propkey.h>
#include"DlgRawChoose.h"
#include"CutoffChoose.h"
#include"DlgChooseK.h"
#include<sstream>
#include"resource.h"
#include <afx.h>
#include"DlgManyRaws.h"
#include"CViewChooseManyRaws.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDEYIFExperiment1Doc

IMPLEMENT_DYNCREATE(CDEYIFExperiment1Doc, CDocument)

BEGIN_MESSAGE_MAP(CDEYIFExperiment1Doc, CDocument)
	ON_COMMAND(ID_Invert, &CDEYIFExperiment1Doc::OnInvert)
	ON_COMMAND(ID_Return, &CDEYIFExperiment1Doc::OnReturn)
	ON_COMMAND(ID_PieLinTrans, &CDEYIFExperiment1Doc::OnPielintrans)
	ON_COMMAND(ID_PaletteInvert, &CDEYIFExperiment1Doc::OnPaletteinvert)
	ON_COMMAND(ID_showBitPlane0, &CDEYIFExperiment1Doc::Onshowbitplane0)
	ON_COMMAND(ID_showBitPlane7, &CDEYIFExperiment1Doc::Onshowbitplane7)
	ON_COMMAND(ID_HistoView, &CDEYIFExperiment1Doc::OnHistoview)
	ON_COMMAND(ID_HistoEQU, &CDEYIFExperiment1Doc::OnHistoequ)
	ON_COMMAND(ID_Smooth, &CDEYIFExperiment1Doc::OnSmooth)
	ON_COMMAND(ID_ConvSobel, &CDEYIFExperiment1Doc::OnConvsobel)
	ON_COMMAND(ID_Laplacian, &CDEYIFExperiment1Doc::OnLaplacian)
	ON_COMMAND(ID_GAUSSIAN_2D, &CDEYIFExperiment1Doc::OnGaussian2d)
	ON_COMMAND(ID_GAUSSIAN_1D, &CDEYIFExperiment1Doc::OnGaussian1d)
	ON_COMMAND(ID_DispBone, &CDEYIFExperiment1Doc::OnDispbone)
	ON_COMMAND(ID_DispLung, &CDEYIFExperiment1Doc::OnDisplung)
	ON_COMMAND(ID_WhiteBlock, &CDEYIFExperiment1Doc::OnWhiteblock)
	ON_COMMAND(ID_FFTAmplitude, &CDEYIFExperiment1Doc::OnFftamplitude)
	ON_COMMAND(ID_InverseFFT, &CDEYIFExperiment1Doc::OnInversefft)
	ON_COMMAND(ID_idealLP, &CDEYIFExperiment1Doc::Onideallp)
	ON_COMMAND(ID_idealHP, &CDEYIFExperiment1Doc::Onidealhp)
	ON_COMMAND(ID_GaussianLP, &CDEYIFExperiment1Doc::OnGaussianlp)
	ON_COMMAND(ID_GaussianHP, &CDEYIFExperiment1Doc::OnGaussianhp)
	ON_COMMAND(ID_ButterLP, &CDEYIFExperiment1Doc::OnButterlp)
	ON_COMMAND(ID_ButterHP, &CDEYIFExperiment1Doc::OnButterhp)
	ON_COMMAND(ID_medianFilter, &CDEYIFExperiment1Doc::Onmedianfilter)
	ON_COMMAND(ID_AdaptiveMedianFilter, &CDEYIFExperiment1Doc::OnAdaptivemedianfilter)
	ON_COMMAND(ID_HnSDegradation, &CDEYIFExperiment1Doc::OnHnsdegradation)
	ON_COMMAND(ID_InverseFiltering, &CDEYIFExperiment1Doc::OnInversefiltering)
	ON_COMMAND(ID_WienerFilter, &CDEYIFExperiment1Doc::OnWienerfilter)
	ON_COMMAND(ID_denoiseGaussian, &CDEYIFExperiment1Doc::Ondenoisegaussian)
	ON_COMMAND(ID_DispFront, &CDEYIFExperiment1Doc::OnDispfront)
	ON_COMMAND(ID_DispNext, &CDEYIFExperiment1Doc::OnDispnext)
	ON_COMMAND(ID_DispTrans, &CDEYIFExperiment1Doc::OnDisptrans)
	ON_COMMAND(ID_DispCoronal, &CDEYIFExperiment1Doc::OnDispcoronal)
	ON_COMMAND(ID_DispMedian, &CDEYIFExperiment1Doc::OnDispmedian)
	ON_COMMAND(ID_CreateRGBCircle, &CDEYIFExperiment1Doc::OnCreatergbcircle)
	ON_COMMAND(ID_RGBHue, &CDEYIFExperiment1Doc::OnRgbhue)
	ON_COMMAND(ID_RGBSaturation, &CDEYIFExperiment1Doc::OnRgbsaturation)
	ON_COMMAND(ID_RGBIntensity, &CDEYIFExperiment1Doc::OnRgbintensity)
	ON_COMMAND(ID_RGB2HSI, &CDEYIFExperiment1Doc::OnRgb2hsi)
	ON_COMMAND(ID_HSI2RGB, &CDEYIFExperiment1Doc::OnHsi2rgb)
	ON_COMMAND(ID_RGBTrans, &CDEYIFExperiment1Doc::OnRgbtrans)
	ON_COMMAND(ID_OPENCV_Gaussian, &CDEYIFExperiment1Doc::OnOpencvGaussian)
	ON_COMMAND(ID_binary, &CDEYIFExperiment1Doc::Onbinary)
	ON_COMMAND(ID_Erosion, &CDEYIFExperiment1Doc::OnErosion)
	ON_COMMAND(ID_Dilate, &CDEYIFExperiment1Doc::OnDilate)
	ON_COMMAND(ID_FillHole, &CDEYIFExperiment1Doc::OnFillhole)
	ON_COMMAND(ID_EdgeExtraction, &CDEYIFExperiment1Doc::OnEdgeextraction)
	ON_COMMAND(ID_AddEdge, &CDEYIFExperiment1Doc::OnAddedge)
	ON_COMMAND(ID_OPENCV_BINARY, &CDEYIFExperiment1Doc::OnOpencvBinary)
	ON_COMMAND(ID_OPENCV_INVERT, &CDEYIFExperiment1Doc::OnOpencvInvert)
	ON_COMMAND(ID_OPENCV_EROSION, &CDEYIFExperiment1Doc::OnOpencvErosion)
	ON_COMMAND(ID_OPENCV_DILATION, &CDEYIFExperiment1Doc::OnOpencvDilation)
	ON_COMMAND(ID_OPENCV_FILLHOLE, &CDEYIFExperiment1Doc::OnOpencvFillhole)
	ON_COMMAND(ID_OPENCV_EDGEEXTRACTION, &CDEYIFExperiment1Doc::OnOpencvEdgeextraction)
	ON_COMMAND(ID_OPENCV_ADDEDGE, &CDEYIFExperiment1Doc::OnOpencvAddedge)
	ON_COMMAND(ID_CANNYSegmentation, &CDEYIFExperiment1Doc::OnCannysegmentation)
	ON_COMMAND(ID_CannyGaussian, &CDEYIFExperiment1Doc::OnCannygaussian)
	ON_COMMAND(ID_CannyGrad, &CDEYIFExperiment1Doc::OnCannygrad)
	ON_COMMAND(ID_CannyNMS, &CDEYIFExperiment1Doc::OnCannynms)
	ON_COMMAND(ID_CannyDT, &CDEYIFExperiment1Doc::OnCannydt)
	ON_COMMAND(ID_CannyEdgeConnect, &CDEYIFExperiment1Doc::OnCannyedgeconnect)
END_MESSAGE_MAP()


// CDEYIFExperiment1Doc 构造/析构

CDEYIFExperiment1Doc::CDEYIFExperiment1Doc() noexcept
{
	// TODO: 在此添加一次性构造代码
	
}

CDEYIFExperiment1Doc::~CDEYIFExperiment1Doc()
{
	if (m_pDib != NULL) {
		delete m_pDib;
	}
	if (m_pBuffer != NULL) {
		delete m_pBuffer;
	}
}


BOOL CDEYIFExperiment1Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	SetTitle(L"医学图像处理");


	return TRUE;
}




// CDEYIFExperiment1Doc 序列化

void CDEYIFExperiment1Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CDEYIFExperiment1Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CDEYIFExperiment1Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CDEYIFExperiment1Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CDEYIFExperiment1Doc 诊断

#ifdef _DEBUG
void CDEYIFExperiment1Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDEYIFExperiment1Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDEYIFExperiment1Doc 命令


BOOL CDEYIFExperiment1Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	wstring ws = lpszPathName;
	string strName(ws.begin(), ws.end());
	string ext = strName.substr(strName.find_last_of('.') + 1);
	if (ext == "raw") {
		CFile file;
		ULONGLONG fileSize = 0;
		if (file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyNone))
		{
			fileSize = file.GetLength();
			file.Close();
		}
		if (fileSize == 0) {
			AfxMessageBox(L"文件为空");
			return FALSE;
		}
		if (fileSize > 512 * 512 * 100) {
			DlgManyRaws dlg;
			if (dlg.DoModal() == IDOK) {
				if (m_pDib != NULL) {
					delete m_pDib;
				}
				if (m_pBuffer != NULL) {
					delete m_pBuffer;
				}
				m_pDib = new CDib;
				m_pDib->LoadManyRawFiles(lpszPathName,dlg.mEditWidth,dlg.mEditHeight,dlg.mEditNum,16);
				m_pBuffer = new CDib(*m_pDib);
				UpdateAllViews(NULL);
			}

		}
		else {
			DlgRawChoose dlg;
			if (dlg.DoModal() == IDOK) {
				if (m_pDib != NULL) {
					delete m_pDib;
				}
				if (m_pBuffer != NULL) {
					delete m_pBuffer;
				}
				m_pDib = new CDib;
				m_pDib->LoadRawFile(lpszPathName, dlg.rawWidth, dlg.rawHeight, dlg.format, dlg.bitCount);
				m_pBuffer = new CDib(*m_pDib);
				UpdateAllViews(NULL);
			}
			return TRUE;
		}
	}
	else {
		// TODO:  在此添加您专用的创建代码
		if (m_pDib != NULL)
		{
			delete m_pDib;
		}
		m_pDib = new CDib;
		m_pDib->LoadFile(lpszPathName);

		if (m_pBuffer != NULL)
		{
			delete m_pBuffer;
		}
		m_pBuffer = new CDib(*m_pDib);

		UpdateAllViews(NULL);

		return TRUE;
	}
}


void CDEYIFExperiment1Doc::OnInvert()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		
		m_pDib->Invert();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnReturn()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		delete m_pDib;
		m_pDib = new CDib(*m_pBuffer);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnPielintrans()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->PieLinTrans();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnPaletteinvert()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pDib!=NULL){
		m_pDib->InvertPalette();
		UpdateAllViews(NULL);
	}
}

void CDEYIFExperiment1Doc::Onshowbitplane0()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->showBitPlane0();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::Onshowbitplane7()
{
	// TODO: 在此添加命令处理程序代码
	if(m_pDib != NULL) {
		m_pDib->showBitPlane7();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnHistoview()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		CdlgHistogram dlg(m_pDib);
		dlg.DoModal();
	}
}


void CDEYIFExperiment1Doc::OnHistoequ()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->HistoEQU();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnSmooth()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->ConvSmoth();
		UpdateAllViews(NULL);
	}
}



void CDEYIFExperiment1Doc::OnConvsobel()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->ConvSobel();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnLaplacian()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->ConvLaplacian();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnGaussian2d()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		double time = m_pDib->ConvGaussian(2);
		UpdateAllViews(NULL);
		DlgRuntime dlg;
		dlg.runTime = time;
		dlg.DoModal();
	}
}


void CDEYIFExperiment1Doc::OnGaussian1d()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		double time = m_pDib->ConvGaussian(1);
		UpdateAllViews(NULL);
		DlgRuntime dlg;
		dlg.runTime = time;
		dlg.DoModal();
	}
}


void CDEYIFExperiment1Doc::OnDispbone()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		CString win;
		win.Format(L"Bone");
		m_pDib->DispWindow(win);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnDisplung()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		CString win;
		win.Format(L"Lung");
		m_pDib->DispWindow(win);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnWhiteblock()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		delete m_pDib;
	}
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
	}
	m_pDib = new CDib;
	m_pDib->CreateWhiteBlock(512, 512, 8, 20, 100);

	m_pBuffer = new CDib(*m_pDib);

	UpdateAllViews(NULL);

}


void CDEYIFExperiment1Doc::OnFftamplitude()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->myFFT(m_pDib->GetWidth(), m_pDib->GetHeight());
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnInversefft()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		BOOL flag = m_pDib->myInverseFFT(m_pDib->GetWidth(), m_pDib->GetHeight());
		if (flag) {
			UpdateAllViews(NULL);
		}
		else {
			AfxMessageBox(L"FFT失败");
		}
	}
}


void CDEYIFExperiment1Doc::Onideallp()
{
	double cutOff = 0;
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		CutoffChoose dlg;
		if (dlg.DoModal() == IDOK) {
			cutOff = dlg.cutoff;
		}
		else
			return;
		BOOL flag= m_pDib->Filter(0,cutOff);
		if (flag) {
			UpdateAllViews(NULL);
		}
		else {
			AfxMessageBox(L"操作失败");
		}
	}
}


void CDEYIFExperiment1Doc::Onidealhp()
{
	// TODO: 在此添加命令处理程序代码
	double cutOff = 0;
	if (m_pDib != NULL) {
		CutoffChoose dlg;
		if (dlg.DoModal() == IDOK) {
			cutOff = dlg.cutoff;
		}
		else
			return;
		BOOL flag = m_pDib->Filter(1, cutOff);
		if (flag) {
			UpdateAllViews(NULL);
		}
		else {
			AfxMessageBox(L"操作失败");
		}
	}
}


void CDEYIFExperiment1Doc::OnGaussianlp()
{
	// TODO: 在此添加命令处理程序代码
	double cutOff = 0;
	if (m_pDib != NULL) {
		CutoffChoose dlg;
		if (dlg.DoModal() == IDOK) {
			cutOff = dlg.cutoff;
		}
		else
			return;
		BOOL flag = m_pDib->Filter(2, cutOff);
		if (flag) {
			UpdateAllViews(NULL);
		}
		else {
			AfxMessageBox(L"操作失败");
		}
	}
}


void CDEYIFExperiment1Doc::OnGaussianhp()
{
	// TODO: 在此添加命令处理程序代码
	double cutOff = 0;
	if (m_pDib != NULL) {
		CutoffChoose dlg;
		if (dlg.DoModal() == IDOK) {
			cutOff = dlg.cutoff;
			BOOL flag = m_pDib->Filter(3, cutOff);
			if (flag) {
				UpdateAllViews(NULL);
			}
			else {
				AfxMessageBox(L"操作失败");
			}
		}
	}
}


void CDEYIFExperiment1Doc::OnButterlp()
{
	// TODO: 在此添加命令处理程序代码
	double cutOff = 0;
	if (m_pDib != NULL) {
		CutoffChoose dlg;
		if (dlg.DoModal() == IDOK) {
			cutOff = dlg.cutoff;
			BOOL flag = m_pDib->Filter(4, cutOff);
			if (flag) {
				UpdateAllViews(NULL);
			}
			else {
				AfxMessageBox(L"操作失败");
			}
		}
	}
}


void CDEYIFExperiment1Doc::OnButterhp()
{
	// TODO: 在此添加命令处理程序代码
	double cutOff = 0;
	if (m_pDib != NULL) {
		CutoffChoose dlg;
		if (dlg.DoModal() == IDOK) {
			cutOff = dlg.cutoff;
		}
		else
			return;
		BOOL flag = m_pDib->Filter(5, cutOff);
		if (flag) {
			UpdateAllViews(NULL);
		}
		else {
			AfxMessageBox(L"操作失败");
		}
	}
}


void CDEYIFExperiment1Doc::Onmedianfilter()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->medianFilter();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnAdaptivemedianfilter()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->AdaptiveMedianFilter();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnHnsdegradation()
{
	// TODO: 在此添加命令处理程序代码
	double k = 0.0;
	if (m_pDib != NULL) {
		DlgChooseK dlg;
		if (dlg.DoModal() == IDOK) {
			k = dlg.k;
		}
		else
			return;
		m_pDib->Degradation(k);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnInversefiltering()
{
	// TODO: 在此添加命令处理程序代码
	double k = 0.0;
	double cutOff = 0.0;
	if (m_pDib != NULL) {
		DlgChooseK dlg;
		if (dlg.DoModal() == IDOK) {
			k = dlg.k;
		}
		else
			return;
		CutoffChoose dlg1;
		if (dlg1.DoModal() == IDOK) {
			cutOff = dlg1.cutoff;
		}
		else
			return;
		m_pDib->InverseFiltering(k,cutOff);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnWienerfilter()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->WienerFilter();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::Ondenoisegaussian()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		//打开lena原图
		CDib* lena = new CDib;
		//lena->LoadFile(L"E:\\桌面\\大三下\\数字图像处理\\lena.bmp");	//用双斜杠才不会报错
		lena->LoadFile(L".\\res\\lena.bmp");	//用双斜杠才不会报错
		//计算原本的PSNR
		double originalPSNR = 0.0;
		originalPSNR= m_pDib->CalculatePSNR(lena->m_pDibBits);

		//图像处理
		double bestGauFre = 60.0;
		double sigma = 0.1;
		m_pDib->Filter(2, bestGauFre);	//高斯低通
		UpdateAllViews(NULL);
		m_pDib->AdaptiveLocalDenoise();	//自适应局部降噪滤波
		UpdateAllViews(NULL);
		m_pDib->AdaptiveMedianFilter();	//自适应中值滤波
		UpdateAllViews(NULL);

		//CDib* temp = new CDib(*m_pDib);
		//temp->ConvSobel();	//边缘检测
		//temp->Filter(5, 10);	//高通滤波
		//temp->tempOpet();
		//temp->ConvLaplacian();
		//m_pDib->ImgAdd(m_pDib->m_pDibBits,temp->m_pDibBits);	//图像相加

		//计算PSNR
		double PSNR = 0.0;
		PSNR = m_pDib->CalculatePSNR(lena->m_pDibBits);

		UpdateAllViews(NULL);
		delete lena;
		if (PSNR >= 10000) {
			AfxMessageBox(L"这是原图");
			return;
		}
		stringstream ss;
		ss << "处理前PSNR=" << originalPSNR << '\n' << "处理后PSNR=" << PSNR << '\n';
		string result = ss.str();
		CString Cresult(result.c_str());
		AfxMessageBox(Cresult);
	}
	
}


void CDEYIFExperiment1Doc::OnDispfront()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib->pointerArray.size() != 0) {
		m_pDib->Destroy();
		if (m_pDib->nowFrame == 0) {
			m_pDib->nowFrame = m_pDib->pointerArray.size() - 1;
		}
		else {
			(m_pDib->nowFrame)--;
		}
		if (m_pDib->viewManyRawStatus == 0) {
			m_pDib->DispManyRawFiles(m_pDib->pointerArray[m_pDib->nowFrame], m_pDib->m_nWidth, m_pDib->m_nHeight, 8);
		}
		else if (m_pDib->viewManyRawStatus == 1) {
			m_pDib->DispDiffView(1, m_pDib->nowFrame);
		}
		else if (m_pDib->viewManyRawStatus == 2) {
			m_pDib->DispDiffView(2, m_pDib->nowFrame);
		}
		delete m_pBuffer;
		m_pBuffer = new CDib(*m_pDib);
		UpdateAllViews(NULL);
		AfxMessageBox(L"可以使用键盘的左、右方向键或者A、D键进行图像切换");
	}
}


void CDEYIFExperiment1Doc::OnDispnext()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib->pointerArray.size() != 0) {
		m_pDib->Destroy();
		if (m_pDib->nowFrame == m_pDib->pointerArray.size()-1) {
			m_pDib->nowFrame = 0;
		}
		else {
			(m_pDib->nowFrame)++;
		}
		if (m_pDib->viewManyRawStatus == 0) {
			m_pDib->DispManyRawFiles(m_pDib->pointerArray[m_pDib->nowFrame], m_pDib->m_nWidth, m_pDib->m_nHeight, 8);
		}
		else if (m_pDib->viewManyRawStatus == 1) {
			m_pDib->DispDiffView(1, m_pDib->nowFrame);
		}
		else if (m_pDib->viewManyRawStatus == 2) {
			m_pDib->DispDiffView(2, m_pDib->nowFrame);
		}
		delete m_pBuffer;
		m_pBuffer = new CDib(*m_pDib);
		UpdateAllViews(NULL);
		AfxMessageBox(L"可以使用键盘的左、右方向键或者A、D键进行图像切换");
	}
}


void CDEYIFExperiment1Doc::RightKey()
{
	// TODO: 在此处添加实现代码.
	if (m_pDib->pointerArray.size() != 0) {
		m_pDib->Destroy();
		if (m_pDib->viewManyRawStatus == 0) {
			if (m_pDib->nowFrame == m_pDib->pointerArray.size() - 1) {
				m_pDib->nowFrame = 0;
			}
			else {
				(m_pDib->nowFrame)++;
			}
			m_pDib->DispManyRawFiles(m_pDib->pointerArray[m_pDib->nowFrame], m_pDib->m_nWidth, m_pDib->m_nHeight, 8);
		}
		else if (m_pDib->viewManyRawStatus == 1) {
			if (m_pDib->nowFrame == m_pDib->CronalArrary.size() - 1) {
				m_pDib->nowFrame = 0;
			}
			else {
				(m_pDib->nowFrame)++;
			}
			m_pDib->DispDiffView(1, m_pDib->nowFrame);
		}
		else if (m_pDib->viewManyRawStatus == 2) {
			if (m_pDib->nowFrame == m_pDib->MedianArrary.size() - 1) {
				m_pDib->nowFrame = 0;
			}
			else {
				(m_pDib->nowFrame)++;
			}
			m_pDib->DispDiffView(2, m_pDib->nowFrame);
		}
		delete m_pBuffer;
		m_pBuffer = new CDib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::LeftKey()
{
	// TODO: 在此处添加实现代码.
	if (m_pDib->pointerArray.size() != 0) {
		m_pDib->Destroy();
		if (m_pDib->nowFrame == 0) {
			m_pDib->nowFrame = m_pDib->pointerArray.size() - 1;
		}
		else {
			(m_pDib->nowFrame)--;
		}
		if (m_pDib->viewManyRawStatus == 0) {
			m_pDib->DispManyRawFiles(m_pDib->pointerArray[m_pDib->nowFrame], m_pDib->m_nWidth, m_pDib->m_nHeight, 8);
		}
		else if (m_pDib->viewManyRawStatus == 1) {
			m_pDib->DispDiffView(1, m_pDib->nowFrame);
		}
		else if (m_pDib->viewManyRawStatus == 2) {
			m_pDib->DispDiffView(2, m_pDib->nowFrame);
		}
		delete m_pBuffer;
		m_pBuffer = new CDib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnDisptrans()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib->pointerArray.size() != 0) {
		if (m_pDib->viewManyRawStatus == 0)
			return;
		else {
			int frame= m_pDib->nowFrame;
			if (frame > m_pDib->pointerArray.size() - 1)
			{
				frame = m_pDib->pointerArray.size() - 1;
			}
			m_pDib->Destroy();
			m_pDib->viewManyRawStatus = 0;
			m_pDib->DispDiffView(0, frame);
			delete m_pBuffer;
			m_pBuffer = new CDib(*m_pDib);
			UpdateAllViews(NULL);
		}
	}
}


void CDEYIFExperiment1Doc::OnDispcoronal()
{
	// TODO: 在此添加命令处理程序代码
	//插值实现冠状面（正视图）的显示
	//raw data的像素大小为0.68359375mm，层距为1mm。
	if (m_pDib->pointerArray.size() != 0) {
		if (m_pDib->viewManyRawStatus == 1)
			return;
		else {
			m_pDib->Destroy();
			m_pDib->viewManyRawStatus = 1;
			m_pDib->DispDiffView(1, m_pDib->nowFrame);
			delete m_pBuffer;
			m_pBuffer = new CDib(*m_pDib);
			UpdateAllViews(NULL);
		}
	}
}


void CDEYIFExperiment1Doc::OnDispmedian()
{
	// TODO: 在此添加命令处理程序代码
	//插值实现矢状面（侧视图）的显示
	//raw data的像素大小为0.68359375mm，层距为1mm。
	if (m_pDib->pointerArray.size() != 0) {
		if (m_pDib->viewManyRawStatus == 2)
			return;
		else {
			m_pDib->Destroy();
			m_pDib->viewManyRawStatus = 2;
			m_pDib->DispDiffView(2, m_pDib->nowFrame);
			delete m_pBuffer;
			m_pBuffer = new CDib(*m_pDib);
			UpdateAllViews(NULL);
		}
	}
}


void CDEYIFExperiment1Doc::OnCreatergbcircle()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		delete m_pDib;
	}
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
	}
	m_pDib = new CDib;
	m_pDib->CreateRGBCircle(512, 512, 24, 100);
	m_pBuffer = new CDib(*m_pDib);
	UpdateAllViews(NULL);
}


void CDEYIFExperiment1Doc::OnRgbhue()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->RGB2HSI(1);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnRgbsaturation()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->RGB2HSI(2);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnRgbintensity()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->RGB2HSI(3);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnRgb2hsi()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->RGB2HSI(0);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnHsi2rgb()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->RGB2HSI(4);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnRgbtrans()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->RGBTrans();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvGaussian()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.OpenCVGaussianBlur();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::Onbinary()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Binary();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnErosion()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Erosion();
		UpdateAllViews(NULL);
	}

}


void CDEYIFExperiment1Doc::OnDilate()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Dilate();
		UpdateAllViews(NULL);
	}

}


void CDEYIFExperiment1Doc::OnFillhole()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->FillHole();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnEdgeextraction()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->EdgeExtraction();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnAddedge()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->AddEdge();
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvBinary()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.BinaryCV();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvInvert()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.InvertCV();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvErosion()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.ErosionCV();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvDilation()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.DilationCV();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvFillhole()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.FillHoleCV();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvEdgeextraction()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.EdgeExtractionCV();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnOpencvAddedge()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.AddEdgeCV(*m_pDib);
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnCannysegmentation()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		COpenCVProcess cvp(m_pDib);
		cvp.CVCanny();
		cvp.Mat2Dib(*m_pDib);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnCannygaussian()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		//double cutOff = 45;
		if (m_pDib != NULL) {
			/*CutoffChoose dlg;
			if (dlg.DoModal() == IDOK) {
				cutOff = dlg.cutoff;
			}
			else
				return;*/

			BOOL flag = m_pDib->ConvGaussian(1);
			if (flag) {
				UpdateAllViews(NULL);
			}
			else {
				AfxMessageBox(L"操作失败");
			}
		}
	}
}


void CDEYIFExperiment1Doc::OnCannygrad()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->CannyEdge(1);
		//COpenCVProcess cvp(m_pDib);

		//m_pDib->ConvSobel();

		//Mat grad_x, grad_y;
		//Mat src = cvp.cvimg.clone();
		//Sobel(src, grad_x, CV_64F, 1, 0, 3); // X方向梯度
		//Sobel(src, grad_y, CV_64F, 0, 1, 3); // Y方向梯度

		//// 计算梯度幅值和方向
		//Mat magnitude, angle;
		//cartToPolar(grad_x, grad_y, magnitude, angle, true);

		//// 显示结果
		//imshow("Gradient Angle", angle / 360); // 归一化显示
		UpdateAllViews(NULL);
	}
}




void CDEYIFExperiment1Doc::OnCannynms()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->CannyEdge(2);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnCannydt()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->CannyEdge(3);
		UpdateAllViews(NULL);
	}
}


void CDEYIFExperiment1Doc::OnCannyedgeconnect()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->CannyEdge(4);
		UpdateAllViews(NULL);
	}
}
