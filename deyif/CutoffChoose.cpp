// CutoffChoose.cpp: 实现文件
//

#include "pch.h"
#include "DEYIF_Experiment1.h"
#include "afxdialogex.h"
#include "CutoffChoose.h"


// CutoffChoose 对话框

IMPLEMENT_DYNAMIC(CutoffChoose, CDialogEx)

CutoffChoose::CutoffChoose(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_Cutoff, pParent)
{
	cutoff = 15;
}

CutoffChoose::~CutoffChoose()
{
}

void CutoffChoose::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_editCutoff, m_edit);
	m_edit.SetWindowTextW(_T("15"));
}


BEGIN_MESSAGE_MAP(CutoffChoose, CDialogEx)
	ON_EN_CHANGE(IDC_editCutoff, &CutoffChoose::OnEnChangeeditcutoff)
END_MESSAGE_MAP()


// CutoffChoose 消息处理程序


void CutoffChoose::OnEnChangeeditcutoff()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_edit.GetWindowTextW(str);
	cutoff = _ttoi(str);
}
