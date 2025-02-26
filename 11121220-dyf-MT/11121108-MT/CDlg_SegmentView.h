#pragma once
#include "afxdialogex.h"
#include"Dib.h"
#include"COpenCVProcess.h"

// CDlg_SegmentView 对话框
class CDib;

class CDlg_SegmentView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg_SegmentView)

public:
	CDlg_SegmentView(CDib *pDib, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlg_SegmentView();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SegmentView };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

public:
	BOOL transSegFlag;
	LPCTSTR filePath;
	uint16_t* segTransImg;
	uint16_t* segSagImg;
	uint16_t* segCorImg;
	int nWidth;
	int nHeight;
	int nImgNum;
	int nWidthBytes;
	int nRawWidth;
	int nRawHeight;
	int currentTrans;
	int currentSagital;
	int currentCoronal;
	CDib dibTrans;
	CDib dibSagittal;
	CDib dibCoronal;
	// 横截面视图
	CStatic m_picTrans;
	CStatic m_picSag;
	CStatic m_picCorn;
	CSliderCtrl m_sldTrans;
	CSliderCtrl m_sldSag;
	CSliderCtrl m_sldCorn;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnSldScroll();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	void showCoronal();
	void showSagittal();
	afx_msg void OnBnClickedsegTrans();
	afx_msg void OnBnClickedsegsagbtn();
	afx_msg void OnBnClickedsegcorbtn();
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CBrush m_brush;

};
