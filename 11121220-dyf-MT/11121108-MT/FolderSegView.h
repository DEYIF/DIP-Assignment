#pragma once
#include "afxdialogex.h"
#include"Dib.h"
#include"COpenCVProcess.h"

// FolderSegView 对话框
class CDib;

class FolderSegView : public CDialogEx
{
	DECLARE_DYNAMIC(FolderSegView)

public:
	FolderSegView(CDib* t1NDib, CDib* t1WDib, CDib* t2WDib, CDib* t2FDib, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FolderSegView();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FolderSegmentView };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL transSegFlag;
	BOOL sagSegFlag;
	BOOL corSegFlag;
	CBrush m_brush;
	int nWidth;
	int nHeight;
	int nImgNum;
	int nWidthBytes;
	int nRawWidth;
	int nRawHeight;
	int currentTrans;
	int currentSagital;
	int currentCoronal;

	uint16_t* t1NTransImg;
	uint16_t* t1NSagImg;
	uint16_t* t1NCorImg;
	uint16_t* t1WTransImg;
	uint16_t* t1WSagImg;
	uint16_t* t1WCorImg;
	uint16_t* t2WTransImg;
	uint16_t* t2WSagImg;
	uint16_t* t2WCorImg;
	uint16_t* t2FTransImg;
	uint16_t* t2FSagImg;
	uint16_t* t2FCorImg;

	CDib dibT1NTrans;
	CDib dibT1NSagittal;
	CDib dibT1NCoronal;
	CDib dibT1WTrans;
	CDib dibT1WSagittal;
	CDib dibT1WCoronal;
	CDib dibT2WTrans;
	CDib dibT2WSagittal;
	CDib dibT2WCoronal;
	CDib dibT2FTrans;
	CDib dibT2FSagittal;
	CDib dibT2FCoronal;

	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	CSliderCtrl sldTrans;
	CButton btnTransSeg;
	void OnSldScroll();
	CButton btnSag;
	CSliderCtrl sldSag;
	CButton btnCor;
	CSliderCtrl sldCor;
	uint16_t* Trans2Sagittal(uint16_t* TransImg);
	uint16_t* Trans2Coronal(uint16_t* TransImg);
	afx_msg void OnBnClickedSegtrans();
	afx_msg void OnBnClickedsegsag();
	afx_msg void OnBnClickedsegcor();
};
