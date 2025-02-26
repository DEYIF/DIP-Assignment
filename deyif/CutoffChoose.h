#pragma once
#include "afxdialogex.h"


// CutoffChoose 对话框

class CutoffChoose : public CDialogEx
{
	DECLARE_DYNAMIC(CutoffChoose)

public:
	CutoffChoose(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CutoffChoose();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_Cutoff };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double cutoff;
	CEdit m_edit;
	afx_msg void OnEnChangeeditcutoff();
};
