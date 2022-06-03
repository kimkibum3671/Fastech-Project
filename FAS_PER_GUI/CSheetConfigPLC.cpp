// CSheetConfigPLC.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetConfigPLC.h"
#include "afxdialogex.h"


// CSheetConfigPLC 대화 상자

IMPLEMENT_DYNAMIC(CSheetConfigPLC, CDialogEx)

CSheetConfigPLC::CSheetConfigPLC(CWnd* pParent /*=nullptr*/)
	
{

}

CSheetConfigPLC::~CSheetConfigPLC()
{
}

void CSheetConfigPLC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_INDEX, m_edit);
}


BEGIN_MESSAGE_MAP(CSheetConfigPLC, CDialogEx)
END_MESSAGE_MAP()


// CSheetConfigPLC 메시지 처리기


BOOL CSheetConfigPLC::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::OnCommand(wParam, lParam);
}
