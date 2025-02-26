#pragma once
#include "afxdialogex.h"


// CDlgFileCfg 对话框

class CDlgFileCfg : public CDialog
{
	DECLARE_DYNAMIC(CDlgFileCfg)

public:
	CDlgFileCfg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFileCfg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_FILECFG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nImgNum;
	int m_nImgHeight;
	int m_nImgWidth;
};
