// DlgChooseK.cpp: 实现文件
//

#include "pch.h"
#include "DEYIF_Experiment1.h"
#include "afxdialogex.h"
#include "DlgChooseK.h"


// DlgChooseK 对话框

IMPLEMENT_DYNAMIC(DlgChooseK, CDialogEx)

DlgChooseK::DlgChooseK(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIG_kDegration, pParent)
{
	k = 0.0025;
}

DlgChooseK::~DlgChooseK()
{
}

void DlgChooseK::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	m_edit.SetWindowTextW(_T("0.0025"));
}


BEGIN_MESSAGE_MAP(DlgChooseK, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &DlgChooseK::OnEnChangeK)
END_MESSAGE_MAP()


// DlgChooseK 消息处理程序


void DlgChooseK::OnEnChangeK()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_edit.GetWindowTextW(str);
	k = _ttoi(str);
}
