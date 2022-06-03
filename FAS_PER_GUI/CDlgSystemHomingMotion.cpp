// CDlgSystemHomingMotion.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSystemHomingMotion.h"
#include "afxdialogex.h"


// CDlgSystemHomingMotion 대화 상자

IMPLEMENT_DYNAMIC(CDlgSystemHomingMotion, CDialogEx)

CDlgSystemHomingMotion::CDlgSystemHomingMotion(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYTEM_HOMING_MOTION, pParent)
{

}

CDlgSystemHomingMotion::~CDlgSystemHomingMotion()
{
}

void CDlgSystemHomingMotion::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSystemHomingMotion, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBUTTON1, &CDlgSystemHomingMotion::OnBnClickedMfcbutton1)
END_MESSAGE_MAP()


// CDlgSystemHomingMotion 메시지 처리기


void CDlgSystemHomingMotion::OnBnClickedMfcbutton1()
{
	
}
