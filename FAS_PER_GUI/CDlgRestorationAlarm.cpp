// CDlgRestorationAlarm.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgRestorationAlarm.h"
#include "afxdialogex.h"


// CDlgRestorationAlarm 대화 상자

IMPLEMENT_DYNAMIC(CDlgRestorationAlarm, CDialogEx)

CDlgRestorationAlarm::CDlgRestorationAlarm(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESTORATION_ALARM, pParent)
{

}

CDlgRestorationAlarm::~CDlgRestorationAlarm()
{
}

void CDlgRestorationAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRestorationAlarm, CDialogEx)
END_MESSAGE_MAP()


// CDlgRestorationAlarm 메시지 처리기


void CDlgRestorationAlarm::SetRestorationEnableStatus(CString strIndexError, CString strCountError, CString strEndPointError, CString strDbError)
{
	INT nCnt = 0;

	if (strIndexError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strIndexError = strIndexError;
		nCnt++;
	}

	if (strCountError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strCountError = strCountError;
		nCnt++;
	}

	if (strEndPointError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strCountError = strEndPointError;
		nCnt++;
	}

	if (strDbError != _T(""))
	{
		m_alarm.strError[nCnt] = m_alarm.strDBError = strDbError;
		nCnt++;
	}
	
	m_alarm.nErrorCount = nCnt;
}


BOOL CDlgRestorationAlarm::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_WARNING));
	this->SetIcon(hIcon, FALSE);

	for (INT i = 0; i < 4; i++)
	{
		BOOL bEnable = FALSE;
		if (i < m_alarm.nErrorCount)
		{
			bEnable = TRUE;

			//GetDlgItem(IDC_STATIC_NOTICE0 + i)->SetWindowText(m_alarm.strError[i]);
			SetDlgItemText(IDC_STATIC_NOTICE0 + i, m_alarm.strError[i]);
		}

		GetDlgItem(IDC_STATIC_NOTICE0 + i)->ShowWindow(bEnable);
		GetDlgItem(IDC_STATIC_ALARM0 + i)->ShowWindow(bEnable);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
