// CDlgFileCfg.cpp: 实现文件
//

#include "pch.h"
#include "11121220-dyf-MT.h"
#include "afxdialogex.h"
#include "CDlgFileCfg.h"


// CDlgFileCfg 对话框

IMPLEMENT_DYNAMIC(CDlgFileCfg, CDialog)

CDlgFileCfg::CDlgFileCfg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_FILECFG, pParent)
	, m_nImgNum(155)
	, m_nImgHeight(240)
	, m_nImgWidth(240)
{

}

CDlgFileCfg::~CDlgFileCfg()
{
}

void CDlgFileCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nImgNum);
	DDX_Text(pDX, IDC_EDIT2, m_nImgHeight);
	DDX_Text(pDX, IDC_EDIT3, m_nImgWidth);
}


BEGIN_MESSAGE_MAP(CDlgFileCfg, CDialog)
END_MESSAGE_MAP()


// CDlgFileCfg 消息处理程序
