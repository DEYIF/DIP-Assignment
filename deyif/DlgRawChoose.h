#pragma once
#include "afxdialogex.h"


// DlgRawChoose 对话框

class DlgRawChoose : public CDialogEx
{
	DECLARE_DYNAMIC(DlgRawChoose)

public:
	DlgRawChoose(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgRawChoose();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RAW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeComboformat();
	afx_msg void OnEnChangeEditheight();
	afx_msg void OnEnChangeEditwidth();
	CComboBox combo;
	CEdit editHeight;
	CEdit editWidth;
	CString format;
	int rawWidth;
	int rawHeight;
	int bitCount;
};
