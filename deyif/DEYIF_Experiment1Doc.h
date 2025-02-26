
// DEYIF_Experiment1Doc.h: CDEYIFExperiment1Doc 类的接口
//

#include"Dib.h"
#include"CdlgHistogram.h"
#include"DlgRuntime.h"
#include"COpenCVProcess.h"
#pragma once


class CDEYIFExperiment1Doc : public CDocument
{
protected: // 仅从序列化创建
	CDEYIFExperiment1Doc() noexcept;
	DECLARE_DYNCREATE(CDEYIFExperiment1Doc)

// 特性
public:
	CDib* m_pDib=NULL;
	CDib* m_pBuffer=NULL;
// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CDEYIFExperiment1Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnInvert();
	afx_msg void OnReturn();
	afx_msg void OnPielintrans();
	
	afx_msg void OnPaletteinvert();
	afx_msg void Onshowbitplane0();
	afx_msg void Onshowbitplane7();
	afx_msg void OnHistoview();
	afx_msg void OnHistoequ();
	afx_msg void OnSmooth();
	afx_msg void OnConvsobel();
	afx_msg void OnLaplacian();
	afx_msg void OnGaussian2d();
	afx_msg void OnGaussian1d();
	afx_msg void OnDispbone();
	afx_msg void OnDisplung();
	afx_msg void OnWhiteblock();
	afx_msg void OnFftamplitude();
	afx_msg void OnInversefft();
	afx_msg void Onideallp();
	afx_msg void Onidealhp();
	afx_msg void OnGaussianlp();
	afx_msg void OnGaussianhp();
	afx_msg void OnButterlp();
	afx_msg void OnButterhp();
	afx_msg void Onmedianfilter();
	afx_msg void OnAdaptivemedianfilter();
	afx_msg void OnHnsdegradation();
	afx_msg void OnInversefiltering();
	afx_msg void OnWienerfilter();
	afx_msg void Ondenoisegaussian();
	afx_msg void OnDispfront();
	afx_msg void OnDispnext();
	void RightKey();
	void LeftKey();
	afx_msg void OnDisptrans();
	afx_msg void OnDispcoronal();
	afx_msg void OnDispmedian();
	afx_msg void OnCreatergbcircle();
	afx_msg void OnRgbhue();
	afx_msg void OnRgbsaturation();
	afx_msg void OnRgbintensity();
	afx_msg void OnRgb2hsi();
	afx_msg void OnHsi2rgb();
	afx_msg void OnRgbtrans();
	afx_msg void OnOpencvGaussian();
	afx_msg void Onbinary();
	afx_msg void OnErosion();
	afx_msg void OnDilate();
	afx_msg void OnFillhole();
	afx_msg void OnEdgeextraction();
	afx_msg void OnAddedge();
	afx_msg void OnOpencvBinary();
	afx_msg void OnOpencvInvert();
	afx_msg void OnOpencvErosion();
	afx_msg void OnOpencvDilation();
	afx_msg void OnOpencvFillhole();
	afx_msg void OnOpencvEdgeextraction();
	afx_msg void OnOpencvAddedge();
	afx_msg void OnCannysegmentation();
	afx_msg void OnCannygaussian();
	afx_msg void OnCannygrad();
	afx_msg void OnCannynms();
	afx_msg void OnCannydt();
	afx_msg void OnCannyedgeconnect();
};
