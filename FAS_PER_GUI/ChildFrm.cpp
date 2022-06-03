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

// ChildFrm.cpp: CChildFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "FAS_PER_GUI.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, &CChildFrame::OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, &CChildFrame::OnViewStyle)
	ON_COMMAND(ID_FILE_PRINT, &CChildFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CChildFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnUpdateFilePrintPreview)
END_MESSAGE_MAP()

// CChildFrame 생성/소멸

CChildFrame::CChildFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	/*
	// 분할기 창을 만듭니다.
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CFASPERGUIView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	*/
	return TRUE;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 진단

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 메시지 처리기
CFASPERGUIView* CChildFrame::GetRightPane()
{
//	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
//	CFASPERGUIView* pView = DYNAMIC_DOWNCAST(CFASPERGUIView, pWnd);
//	return pView;

	return NULL;
}

void CChildFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	//if (!pCmdUI)
	//	return;

	//// TODO: 이 코드를 사용자 지정하거나 확장하여 [보기] 메뉴에서의 선택을 처리합니다.
	//CFASPERGUIView* pView = GetRightPane(); 

	//// 오른쪽 창이 만들어지지 않았거나 뷰가 아닌 경우 해당 범위에 있는 명령을 비활성화합니다.
	//if (pView == nullptr)
	//	pCmdUI->Enable(FALSE);
	//else
	//{
	//	DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;
	//	// 명령이 ID_VIEW_LINEUP이면
	//	// LVS_ICON 또는 LVS_SMALLICON 모드인 경우에만 이 명령을 활성화합니다.
	//	if (pCmdUI->m_nID == ID_VIEW_LINEUP)
	//	{
	//		if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
	//			pCmdUI->Enable();
	//		else
	//			pCmdUI->Enable(FALSE);
	//	}
	//	else
	//	{
	//		// 그렇지 않으면 점을 사용하여 뷰의 스타일을 반영합니다.
	//		pCmdUI->Enable();
	//		BOOL bChecked = FALSE;

	//		switch (pCmdUI->m_nID)
	//		{
	//		case ID_VIEW_DETAILS:
	//			bChecked = (dwStyle == LVS_REPORT);
	//			break;

	//		case ID_VIEW_SMALLICON:
	//			bChecked = (dwStyle == LVS_SMALLICON);
	//			break;

	//		case ID_VIEW_LARGEICON:
	//			bChecked = (dwStyle == LVS_ICON);
	//			break;

	//		case ID_VIEW_LIST:
	//			bChecked = (dwStyle == LVS_LIST);
	//			break;

	//		default:
	//			bChecked = FALSE;
	//			break;
	//		}

	//		pCmdUI->SetRadio(bChecked ? 1 : 0);
	//	}
	//}
}

void CChildFrame::OnViewStyle(UINT nCommandID)
{
	//// TODO: 이 코드를 사용자 지정하거나 확장하여 [보기] 메뉴에서의 선택을 처리합니다.
	//CFASPERGUIView* pView = GetRightPane();

	//// 오른쪽 창이 만들어졌고 CFASPERGUIView이면 메뉴 명령을 처리합니다.
	//if (pView != nullptr)
	//{
	//	int nStyle = -1;

	//	switch (nCommandID)
	//	{
	//	case ID_VIEW_LINEUP:
	//		{
	//			// list 컨트롤을 모눈에 맞춥니다.
	//			CListCtrl& refListCtrl = pView->GetListCtrl();
	//			refListCtrl.Arrange(LVA_SNAPTOGRID);
	//		}
	//		break;

	//	// 다른 명령에서 list 컨트롤의 스타일을 변경합니다.
	//	case ID_VIEW_DETAILS:
	//		nStyle = LVS_REPORT;
	//		break;

	//	case ID_VIEW_SMALLICON:
	//		nStyle = LVS_SMALLICON;
	//		break;

	//	case ID_VIEW_LARGEICON:
	//		nStyle = LVS_ICON;
	//		break;

	//	case ID_VIEW_LIST:
	//		nStyle = LVS_LIST;
	//		break;
	//	}

	//	// 스타일을 변경합니다. 창이 자동으로 다시 그려집니다.
	//	if (nStyle != -1)
	//		pView->ModifyStyle(LVS_TYPEMASK, nStyle);
	//}
}

void CChildFrame::OnFilePrint()
{
	if (m_dockManager.IsPrintPreviewValid())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CChildFrame::OnFilePrintPreview()
{
	if (m_dockManager.IsPrintPreviewValid())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 인쇄 미리 보기 모드를 닫습니다.
	}
}

void CChildFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_dockManager.IsPrintPreviewValid());
}
