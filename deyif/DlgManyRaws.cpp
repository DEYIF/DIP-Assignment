// DlgManyRaws.cpp: 实现文件
//

#include "pch.h"
#include "DEYIF_Experiment1.h"
#include "afxdialogex.h"
#include "DlgManyRaws.h"


// DlgManyRaws 对话框

IMPLEMENT_DYNAMIC(DlgManyRaws, CDialogEx)

DlgManyRaws::DlgManyRaws(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_ManyRaws, pParent)
	, mEditHeight(512)
	, mEditWidth(512)
	, mEditNum(313)
{

}

DlgManyRaws::~DlgManyRaws()
{
}

void DlgManyRaws::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EditHeight, mEditHeight);
	DDX_Text(pDX, IDC_EditWidth, mEditWidth);
	DDX_Text(pDX, IDC_editNum, mEditNum);
}


BEGIN_MESSAGE_MAP(DlgManyRaws, CDialogEx)
END_MESSAGE_MAP()


// DlgManyRaws 消息处理程序
