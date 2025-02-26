
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
		DlgRawChoose dlg;
		if (dlg.DoModal() == IDOK) {
			if (m_pDib != NULL) {
				delete m_pDib;
			}
			if (m_pBuffer != NULL) {
				delete m_pBuffer;
			}
			m_pDib = new CDib;
			m_pDib->LoadRawFile(lpszPathName, dlg.rawWidth, dlg.rawHeight, dlg.format,dlg.bitCount);
			m_pBuffer = new CDib(*m_pDib);
			UpdateAllViews(NULL);
		}
		return TRUE;
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
