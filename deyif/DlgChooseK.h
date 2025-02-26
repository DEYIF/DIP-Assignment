#pragma once
#include "afxdialogex.h"


// DlgChooseK 对话框

class DlgChooseK : public CDialogEx
{
	DECLARE_DYNAMIC(DlgChooseK)

public:
	DlgChooseK(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgChooseK();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIG_kDegration };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double k;
	CEdit m_edit;
	afx_msg void OnEnChangeK();
};
