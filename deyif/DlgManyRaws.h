#pragma once
#include "afxdialogex.h"


// DlgManyRaws 对话框

class DlgManyRaws : public CDialogEx
{
	DECLARE_DYNAMIC(DlgManyRaws)

public:
	DlgManyRaws(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgManyRaws();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ManyRaws };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int mEditHeight;
	int mEditWidth;
	int mEditNum;
};
