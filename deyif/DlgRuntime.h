#pragma once
#include "afxdialogex.h"


// DlgRuntime 对话框

class DlgRuntime : public CDialogEx
{
	DECLARE_DYNAMIC(DlgRuntime)

public:
	DlgRuntime(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgRuntime();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RUNTIME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

public:
	double runTime;
};

