// DlgRuntime.cpp: 实现文件
//

#include "pch.h"
#include "DEYIF_Experiment1.h"
#include "afxdialogex.h"
#include "DlgRuntime.h"


// DlgRuntime 对话框

IMPLEMENT_DYNAMIC(DlgRuntime, CDialogEx)

DlgRuntime::DlgRuntime(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RUNTIME, pParent)
{

}

DlgRuntime::~DlgRuntime()
{
}

void DlgRuntime::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgRuntime, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// DlgRuntime 消息处理程序


void DlgRuntime::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()

    // 设置文本颜色和字体
    dc.SetTextColor(RGB(0, 0, 0)); // 字体颜色黑色
    dc.SetBkMode(TRANSPARENT); // 文本背景透明
    CFont font;
    font.CreatePointFont(120, _T("Arial")); // 字体大小 12
    dc.SelectObject(&font);

    // 绘制文本
    CString strText;
    strText.Format(_T("程序运行时间：%f 毫秒"), runTime);
    dc.TextOut(50, 50, strText); // 在坐标 (10, 10) 处绘制文本
}
