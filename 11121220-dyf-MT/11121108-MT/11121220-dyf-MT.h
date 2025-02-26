
// 11121220-dyf-MT.h: 11121220-dyf-MT 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"       // 主符号


// CMy11121108MTApp:
// 有关此类的实现，请参阅 11121220-dyf-MT.cpp
//

class CMy11121108MTApp : public CWinApp
{
public:
	CMy11121108MTApp() noexcept;


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMy11121108MTApp theApp;
