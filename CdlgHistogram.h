#pragma once
#include "afxdialogex.h"


// CdlgHistogram 对话框

class CDib;

class CdlgHistogram : public CDialogEx
{
	DECLARE_DYNAMIC(CdlgHistogram)

public:
	CdlgHistogram(CDib *pDib, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CdlgHistogram();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_HISTO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	long *m_pGrayValueArray;
	int m_nLength;
	afx_msg void OnPaint();
};
