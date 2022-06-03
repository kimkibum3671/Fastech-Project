// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// FAS_PER_GUI.h: FAS_PER_GUI 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.


#include "..\FAS_PER_DLL\FAS_EziMotionPlusER.h"

#ifdef DEBUG
#pragma comment (lib, "../Bin/Debug/EziMOTIONPlusER.lib")
#else
#pragma comment (lib, "../Bin/Release/EziMOTIONPlusER.lib")
#endif

#include <afxtaskdialog.h>

//#define DEBUG_MEMORY_LEAK

#ifdef DEBUG_MEMORY_LEAK
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

//#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
#endif

#define WM_TRAY_NOTIFYCATION WM_APP

// CFASPERGUIApp:
// 이 클래스의 구현에 대해서는 FAS_PER_GUI.cpp을(를) 참조하세요.
//

class CFASPERGUIApp : public CWinAppEx
{
public:
	CFASPERGUIApp() noexcept;


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	typedef enum _TRAY_CONTROL_COMMAND
	{
		TRAY_ADD = 0,
		TRAY_REMOVE,
		TRAY_MOTIFY,
	}	TRAY_CONTROL_COMMAND;

public:
	const BOOL TrayIconControl(const INT nCommand, const WPARAM wParam = 0, const LPARAM lParam = 0);
};

extern CFASPERGUIApp theApp;
