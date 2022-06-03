// CPageRestorationRead.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageRestorationRead.h"
#include "afxdialogex.h"

#include "CSheetRestoration.h"

// CPageRestorationRead dialog

IMPLEMENT_DYNAMIC(CPageRestorationRead, CMFCPropertyPageEx)

CPageRestorationRead::CPageRestorationRead(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_RESTORATION_READ)
{
	m_bShow = FALSE;
	m_ProcessProject = { 0, };
}

CPageRestorationRead::~CPageRestorationRead()
{
}

void CPageRestorationRead::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageRestorationRead, CMFCPropertyPage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageRestorationRead::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageRestorationRead::OnBnClickedBtnNext)
END_MESSAGE_MAP()


// CPageRestorationRead message handlers
BOOL CPageRestorationRead::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();
	TRACE(_T("CPageRestorationRead::OnInitDialog \r\n"));
	m_colorBkComplete = RGB(0x0a, 0x0a, 0x50);
	m_colorTxtComplete = RGB(0xc8, 0xc8, 0xc8);

	m_btnPrev.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_NEXT);

	m_wndNotice.SubclassDlgItem(IDC_ST_NOTICE, this);
	
	m_wndProgress.SubclassDlgItem(IDC_ST_PROGRESS, this);
	m_wndProgress.SetBkColor(m_colorBkComplete);
	m_wndProgress.SetTextColor(m_colorTxtComplete);

	m_bRun = FALSE;
	m_bShow = TRUE;

	CProgressCtrl* pProgressCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS));
	pProgressCtrl->SetPos(0);

	ActiveComparing();

	InitControlTextSize();
	return TRUE;
}


void CPageRestorationRead::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_POLLING:
	{
		CProgressCtrl* pProgressCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS));
		CSheetRestoration* pParent = static_cast<CSheetRestoration*>(GetParent());
		BOOL bReading = FALSE;


		if (pParent->IsParameterReaded())
		{
			KillTimer(nIDEvent);
			
			bReading = FALSE;
			
			pParent->CheckParameterReaded();

			DisplayComplete();
		}
		else
		{
			INT nCurMin, nCurMax;
			pProgressCtrl->GetRange(nCurMin, nCurMax);
			m_bRun = bReading = TRUE;
			const INT nCurPos = pProgressCtrl->GetPos();

			CDeviceMotionGate* pDevice = GetParent()->GetMotiongateCtrl();
			if (pDevice)
			{
				INT nNewPos, nNewMin, nNewMax;
				pDevice->GetDetailStatus(&nNewPos, &nNewMin, &nNewMax);

				BOOL bCalcPercent = FALSE;

				if (nCurMin != nNewMin ||
					nCurMax != nNewMax)
				{
					bCalcPercent = TRUE;
					pProgressCtrl->SetRange(nNewMin, nNewMax);
				}

				if (nNewPos != nCurPos)
				{
					bCalcPercent = TRUE;
					pProgressCtrl->SetPos(nNewPos);
				}

				if (bCalcPercent)
				{
					CString strPercent;
					strPercent.Format(_T("%d %%"), nNewPos * 100 / (nNewMax - nNewMin) );
					SetDlgItemText(IDC_ST_PROGRESS, strPercent);
				}

				
			}
			//m_btnPrev.EnableWindow(FALSE);
		}

		pParent->SetReading(bReading);

	}	break;
	case TIMER_WATING_STOP:
	{
		if (GetParent()->GetMotiongateCtrl()->StartReadParameter()) StartPolling();

		KillTimer(nIDEvent);
	}	break;
	}

	CMFCPropertyPageEx::OnTimer(nIDEvent);
}


void CPageRestorationRead::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageRestorationRead::OnBnClickedBtnNext()
{
	ActivateNextPage();
}

void CPageRestorationRead::DisplayComplete()
{
	CProgressCtrl* pProgressCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS));
	SetDlgItemText(IDC_ST_PROGRESS, _T("100 %"));
	GetDlgItem(IDC_BTN_NEXT)->EnableWindow();
	pProgressCtrl->SetRange(0, 100);
	pProgressCtrl->SetPos(100);

	m_wndNotice.StartBkBlink(FALSE);
	m_wndNotice.StartTextBlink(FALSE);
	m_wndNotice.SetBkColor(m_colorBkComplete);
	m_wndNotice.SetTextColor(m_colorTxtComplete);

	m_wndNotice.SetWindowText(_T("Reading parameter from motor(s) has been done."));

	m_bRun = FALSE;
	//m_btnPrev.EnableWindow(TRUE);
}


const BOOL CPageRestorationRead::IsComparing()
{// 사용 X
	return m_bRun;
}


void CPageRestorationRead::StartPolling()
{
	if (m_bRun) KillTimer(TIMER_POLLING);

	SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);
}


const BOOL CPageRestorationRead::IsShow()
{
	// 사용X
	return m_bShow;
}


BOOL CPageRestorationRead::OnSetActive()
{
	
	/*
	 
	m_ProcessProject = m_SelectedProject;
	TRACE(_T("CPageRestorationRead::OnSetActive \r\n"));
	if (true)
	{
		if (IsShow() && // 화면은 이미 생성되었음.
			((m_ProcessProject.nBackupID != m_SelectedProject.nBackupID) && // 선택된 파일과 진행되었던 파일 비교
				(m_ProcessProject.strTimeStamp != m_SelectedProject.strTimeStamp)))
		{
			m_ProcessProject = m_SelectedProject;
			static_cast<CSheetRestoration*>(GetParent())->SetTimeParameter(COleDateTime::GetCurrentTime());

			BOOL bStop = StopCompare();
			SetTimer(TIMER_WATING_STOP, 500, NULL);

		}
		TRACE(_T("CPageRestorationRead::OnSetActive \r\n"));
	}
	*/
	
	
	return CMFCPropertyPageEx::OnSetActive();
}


void CPageRestorationRead::SetProjectInfo(const INT nBackupID, LPCTSTR lpszTimeStamp, LPCTSTR lpszComment)
{ // 사용 X
	m_SelectedProject.nBackupID = nBackupID;
	m_SelectedProject.strTimeStamp = lpszTimeStamp;
	m_SelectedProject.strComment = lpszComment;
}


const BOOL CPageRestorationRead::StopCompare()
{
	CDeviceMotionGate* pDevice = GetParent()->GetMotiongateCtrl();
	
	if (NULL == pDevice)
	{
		return FALSE;
	}

	if (pDevice->IsBackupState())
	{
		BOOL bStop = pDevice->StopExecution();
		INT a = 0;
	}

	return TRUE;
}


void CPageRestorationRead::ActiveComparing()
{
	if (static_cast<CSheetRestoration*>(GetParent())->IsParameterReaded())
	{
		DisplayComplete();
	}
	else
	{
		m_wndNotice.SetBlinkBkColors(RGB(0x00, 0x00, 0x00), RGB(0xc8, 0xc8, 0x00));
		m_wndNotice.SetBlinkTextColors(RGB(0xc8, 0xc8, 0xc8), RGB(0x00, 0x00, 0x00));

		const INT nElapse = 1000;
		m_wndNotice.StartBkBlink(TRUE, nElapse);
		m_wndNotice.StartTextBlink(TRUE, nElapse);
		
		
		// Select Method 화면의 List Control에서 다른 데이터 설정하여 들어올 경우 강제로 Timer를 다시 실행해야한다.
		GetParent()->GetMotiongateCtrl()->StartReadParameter();
		SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);

		/*if (GetParent()->GetMotiongateCtrl()->StartReadParameter())
		{
			SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);
		}
		else
		{

		}*/
	}

}


void CPageRestorationRead::GetProcessInfo(INT& nBackupID, CString& strTime, CString& strComment)
{
	nBackupID = m_ProcessProject.nBackupID;
	strTime = m_ProcessProject.strTimeStamp;
	strComment = m_ProcessProject.strComment;
}


void CPageRestorationRead::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_INFO)->SetFont(&m_BigFont);
		GetDlgItem(IDC_PROGRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_PROGRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);
		m_BigFont.Detach();
		
		lf.lfHeight = -20;
		m_BigFont.CreateFontIndirect(&lf);
		GetDlgItem(IDC_ST_NOTICE)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
	m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_STATIC_INFO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_NOTICE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_PROGRESS)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_PROGRESS)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/
	
}
