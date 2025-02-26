// DlgRawChoose.cpp: 实现文件
//

#include "pch.h"
#include "DEYIF_Experiment1.h"
#include "afxdialogex.h"
#include "DlgRawChoose.h"
using namespace std;


// DlgRawChoose 对话框

IMPLEMENT_DYNAMIC(DlgRawChoose, CDialogEx)

DlgRawChoose::DlgRawChoose(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RAW, pParent)
{
	rawHeight = 512;
	rawWidth = 512;
	format = "GRAY";
	bitCount = 16;
}

DlgRawChoose::~DlgRawChoose()
{
}

void DlgRawChoose::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ComboFormat, combo);
	DDX_Control(pDX, IDC_EditHeight, editHeight);
	DDX_Control(pDX, IDC_EditWidth, editWidth);
	combo.InsertString(0, _T("8 bit"));
	combo.InsertString(1, _T("16 bit unsigned"));
	combo.InsertString(2, _T("16 bit signed"));
	combo.InsertString(3, _T("32 bit unsigned"));
	combo.InsertString(4, _T("32 bit signed"));
	combo.SetCurSel(1);
	editHeight.SetWindowTextW(_T("512"));
	editWidth.SetWindowTextW(_T("512"));
}


BEGIN_MESSAGE_MAP(DlgRawChoose, CDialogEx)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_ComboFormat, &DlgRawChoose::OnCbnSelchangeComboformat)
	ON_EN_CHANGE(IDC_EditHeight, &DlgRawChoose::OnEnChangeEditheight)
	ON_EN_CHANGE(IDC_EditWidth, &DlgRawChoose::OnEnChangeEditwidth)
END_MESSAGE_MAP()


// DlgRawChoose 消息处理程序


void DlgRawChoose::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	dc.TextOutW(10, 10, _T("请选择RAW文件的长宽以及编码格式"));
}



void DlgRawChoose::OnCbnSelchangeComboformat()//选择Raw的编码格式
{
	// TODO: 在此添加控件通知处理程序代码
	int index= combo.GetCurSel();
	switch (index)
	{
	case 0:
		format = "GRAY";
		bitCount = 8;
		break;
	case 1:
		format = "GRAY";
		bitCount = 16;
		break;
	case 2:
		format = "GRAY";
		bitCount = 16;
		break;
	case 3:
		format = "RGB";
		bitCount = 32;
		break;
	case 4:
		format = "RGB";
		bitCount = 32;
		break;
	}
}


void DlgRawChoose::OnEnChangeEditheight()//设置Raw的高度
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	editHeight.GetWindowTextW(str);
	rawHeight = _ttoi(str);

}


void DlgRawChoose::OnEnChangeEditwidth()//设置Raw的宽度
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	editWidth.GetWindowTextW(str);
	rawWidth = _ttoi(str);

}
