// CViewAssign.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CViewAssign.h"

#include "CDocDevice.h"

#include "CDeviceList.h"

#include "MainFrm.h"

// CViewAssign

IMPLEMENT_DYNCREATE(CViewAssign, CViewDevice)

CViewAssign::CViewAssign()
	: CViewDevice(IDD_DEVICE_ASSIGN)
{

}

CViewAssign::~CViewAssign()
{
}

void CViewAssign::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewAssign, CFormView)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SELECT1, &CViewAssign::OnBnClickedBtnSelect1)
	ON_BN_CLICKED(IDC_BTN_SELECT2, &CViewAssign::OnBnClickedBtnSelect2)
	ON_BN_CLICKED(IDC_BTN_SELECT3, &CViewAssign::OnBnClickedBtnSelect3)
	ON_BN_CLICKED(IDC_BTN_SELECT4, &CViewAssign::OnBnClickedBtnSelect4)
	ON_BN_CLICKED(IDC_BTN_SELECT5, &CViewAssign::OnBnClickedBtnSelect5)
	ON_BN_CLICKED(IDC_BTN_SELECT6, &CViewAssign::OnBnClickedBtnSelect6)
	ON_BN_CLICKED(IDC_BTN_SELECT7, &CViewAssign::OnBnClickedBtnSelect7)
	ON_BN_CLICKED(IDC_BTN_SELECT8, &CViewAssign::OnBnClickedBtnSelect8)
	ON_BN_CLICKED(IDC_BTN_START, &CViewAssign::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_RESTORATION, &CViewAssign::OnBnClickedBtnRestoration)
END_MESSAGE_MAP()


// CViewAssign diagnostics

#ifdef _DEBUG
void CViewAssign::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewAssign::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewAssign message handlers
void CViewAssign::OnInitialUpdate()
{
	CViewDevice::OnInitialUpdate();

	if (1)
	{
		const INT nResID[][2] = {
			IDC_ST_SIGNAL0,	0,
			IDC_ST_SIGNAL1,	1,
			IDC_ST_SIGNAL2,	1,
			IDC_ST_SIGNAL3,	1,
			IDC_ST_SIGNAL4,	1,
			IDC_ST_SIGNAL5,	1,
			IDC_ST_SIGNAL6,	1,
			IDC_ST_SIGNAL7,	1,
			IDC_ST_SIGNAL8,	1,
			NULL, NULL,
		};

		for (INT nIndex = 0; nResID[nIndex][0]; nIndex++)
		{
			if (NULL == m_wndSignal[nIndex].GetSafeHwnd())
			{
				m_wndSignal[nIndex].SubclassDlgItem(nResID[nIndex][0], this);
				m_wndSignal[nIndex].SetSize(4, 4);
				m_wndSignal[nIndex].SetFormID(nResID[nIndex][1]);	// 0 : Status I/O
			}
		}
	}

	if (1)
	{
		const COLORREF m_colorDefaultBk = RGB(0x00, 0x00, 0x00);
		const COLORREF m_colorDefaultText = RGB(0xc8, 0xc8, 0xc8);

		const INT nResID[] = {
			IDC_ST_ERROR1,
			IDC_ST_ERROR2,
			IDC_ST_ERROR3,
			IDC_ST_ERROR4,
			IDC_ST_ERROR5,
			IDC_ST_ERROR6,
			IDC_ST_ERROR7,
			IDC_ST_ERROR8,
			NULL,
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_wndStatus[nIndex].SubclassDlgItem(nResID[nIndex], this);

			m_wndStatus[nIndex].SetBkColor(m_colorDefaultBk);
			m_wndStatus[nIndex].SetTextColor(m_colorDefaultText);
		}
	}

	if (1)
	{
		const INT nResID[] = {
			IDC_BTN_SELECT1,
			IDC_BTN_SELECT2,
			IDC_BTN_SELECT3,
			IDC_BTN_SELECT4,
			IDC_BTN_SELECT5,
			IDC_BTN_SELECT6,
			IDC_BTN_SELECT7,
			IDC_BTN_SELECT8,
			NULL,
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_btnSelection[nIndex].SubclassDlgItem(nResID[nIndex], this);
		}		
	}

	m_wndName.SubclassDlgItem(IDC_ST_NAME, this);

	if (1)
	{
		const INT nResID[] = {
			IDC_ST_MOTOR0,
			IDC_ST_MOTION_GATE,
			IDC_ST_MOTION_ID,
			NULL
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_wndCondition[nIndex].SubclassDlgItem(nResID[nIndex], this);
		}
	}

	if (1)
	{
		m_wndAutoRestoration.SubclassDlgItem(IDC_ST_RESTORATION, this);
		m_bAutoRestoration = FALSE;
		OnBnClickedBtnRestoration();
	}

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
}

void CViewAssign::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_INITIALIZE:
	{
		KillTimer(nIDEvent);

		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice)
		{
			break;
		}

		DestroyDeviceWindow(this);

		pDevice->EnableAutoRestoration(FALSE);

		ChangeSelection(static_cast<CDocDevice*>(GetDocument())->GetSelectMotorIndex());

		SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);

		if (pDevice->IsMotiongateConnected())
		{
			CheckNameMatched(pDevice);
		}
	}	break;

	case TIMER_POLLING:
	{
		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice || FALSE == pDevice->IsMotiongateConnected())
		{
			GetParent()->DestroyWindow();
			return;
		}

		CDocDevice* pDoc = static_cast<CDocDevice*>(GetDocument());

		pDoc->CheckDeviceTitle();

		DisplayMotionGateStatus(pDoc);

		DisplayMotorStatus(pDoc);
		
		DisplayConditions(pDoc);

		SetTimer(nIDEvent, ELAPSE_TIMER_POLLING, NULL);
	}	break;
	}	//	end of 'switch (nIDEvent)'

	CViewDevice::OnTimer(nIDEvent);
}

void CViewAssign::DisplayMotionGateStatus(CDocument* pDoc)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	const DWORD dwMotiongateStatus = pDevice->GetMotiongateStatus();
	for (INT nIndex = 0; 16 > nIndex; nIndex++)
	{
		m_wndSignal[0].EnableSignal(nIndex, (dwMotiongateStatus >> nIndex) & 0x01);
	}

	BOOL bEnable = FALSE;

	if ((dwMotiongateStatus & 0x00000010) &&
		pDevice->IsBareMotorConnected() &&
		-1 != m_nSelectedMotorIndex)
	{
		//	활성화 조건을 보완해 나가야 한다.
		bEnable = TRUE;
	}

	CWnd* pBtnStart = GetDlgItem(IDC_BTN_START);
	if (pBtnStart->IsWindowEnabled() != bEnable)
	{
		pBtnStart->EnableWindow(bEnable);
	}

	this->DisplayName(pDevice);
}

void CViewAssign::DisplayName(CDeviceMotionGate* pDevice)
{
	/*
	CString strDeviceName(pDevice->GetRealName()), strRegisterName(pDevice->GetSystemName());

	COLORREF m_colorBkName = RGB(0x0a, 0x0a, 0x50);
	COLORREF m_colorTxtName = RGB(0xc8, 0xc8, 0xc8);
	if (strDeviceName != strRegisterName)
	{
		if (CTime::GetCurrentTime().GetSecond() % 2)
		{
			m_colorTxtName = RGB(0xc8, 0x00, 0x00);
			m_colorBkName = RGB(0x00, 0x00, 0x00);
		}
		else
		{
			m_colorTxtName = RGB(0x00, 0x00, 0x00);
			m_colorBkName = RGB(0xc8, 0x00, 0x00);
		}
	}

	CString strTemp;

	m_wndName.GetWindowText(strTemp);

	if (strTemp != strRegisterName)
	{
		m_wndName.SetWindowText(strRegisterName);
	}

	if (m_wndName.GetBkColor() != m_colorBkName)
	{
		m_wndName.SetBkColor(m_colorBkName);
	}

	if (m_wndName.GetTextColor() != m_colorTxtName)
	{
		m_wndName.SetTextColor(m_colorTxtName);
	}
	*/
}

void CViewAssign::DisplayMotorStatus(CDocument* pDoc)
{
	const DWORD dwAddrss = static_cast<CDocDevice*>(pDoc)->GetAddress();
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == pDeviceList->GetMotiongateByAddress(dwAddrss, pDevice))
	{
		return;
	}

	CString strTemp, strValue;
	for (INT iAxisNo = 0; iAxisNo < 8; iAxisNo++)
	{
		const INT nMotorIndex = iAxisNo + 1;

		const BOOL bDisconnected = pDevice->IsMotorConnected(nMotorIndex) ? FALSE : TRUE;
		if (bDisconnected != m_btnSelection[iAxisNo].IsWindowEnabled())
		{
			m_btnSelection[iAxisNo].EnableWindow(bDisconnected);
		}

		if (FALSE == bDisconnected && iAxisNo == m_nSelectedMotorIndex)
		{
			ChangeSelection((8 != m_nSelectedMotorIndex) ? m_nSelectedMotorIndex + 1 : -1);
		}

		const DWORD dwMotorStatus = pDevice->GetMotorStatus(nMotorIndex);
		for (INT nIndex = 0; 16 > nIndex; nIndex++)
		{
			m_wndSignal[nMotorIndex].EnableSignal(nIndex, (dwMotorStatus >> nIndex) & 0x01);
		}

		if (1)
		{
			INT nResID[] = {
				IDC_ST_ERROR1,
				IDC_ST_ERROR2,
				IDC_ST_ERROR3,
				IDC_ST_ERROR4,
				IDC_ST_ERROR5,
				IDC_ST_ERROR6,
				IDC_ST_ERROR7,
				IDC_ST_ERROR8,
			};

			const DWORD dwErrorCode = pDevice->GetMotorErrorCode(nMotorIndex);

			COLORREF colorBk = RGB(0x00, 0xc8, 0x00);
			COLORREF colorTxt = RGB(0x00, 0x00, 0x00);

			if (FALSE == pDevice->IsMotorConnected(nMotorIndex))
			{
				if (iAxisNo)
				{
					//	Disconnected와 Not used를 구분해야 하겠네
					strValue = _T("Disconnected");
					colorBk = RGB(0xc8, 0x00, 0x00);
					colorTxt = RGB(0xc8, 0xc8, 0xc8);
				}
				else
				{
					//	Factory Reset을 위하여 0번 모터가 비어있음
					//	Disconnected와 Not used를 구분해야 하겠네
					strValue = _T("Available");
				}
			}
			else if (0 == dwErrorCode)
			{
				if (TRUE == pDevice->IsServoEnabled(nMotorIndex))
				{
					strValue = _T("Servo ON");
					colorBk = RGB(0xc8, 0xc8, 0x00);
				}
				else
				{
					strValue = _T("Ready");
					colorBk = RGB(0x00, 0xc8, 0x00);
				}
			}
			else
			{
				strValue.Format(_T("Alarm #%d"), dwErrorCode);

				colorBk = RGB(0xc8, 0x00, 0x00);
			}

			GetDlgItemText(nResID[iAxisNo], strTemp);
			if (strTemp != strValue)
			{
				CColorStaticST* pWnd = static_cast<CColorStaticST*>(GetDlgItem(nResID[iAxisNo]));
				pWnd->SetWindowText(strValue);
				pWnd->SetBkColor(colorBk);
				pWnd->SetTextColor(colorTxt);
			}
		}
	}
}

void CViewAssign::ChangeSelection(const INT nSelectedIndex)
{
	CString strSelect, strTemp;
	for (INT nIndex = 0; nIndex < 8; nIndex++)
	{
		const BOOL bCheck = nIndex == nSelectedIndex;
		if (bCheck != m_btnSelection[nIndex].GetCheck())
		{
			m_btnSelection[nIndex].SetCheck(bCheck);
		}

		const UINT nResID = bCheck ? IDB_CHECK : IDB_UNCHECK;
		UINT nCurResID = 0;
		m_btnSelection[nIndex].GetImageEx(&nCurResID);
		if (nResID != nCurResID)
		{
			m_btnSelection[nIndex].SetImageEx(nResID);
		}

		m_btnSelection[nIndex].GetWindowText(strTemp);
		strSelect = bCheck ? _T("This") : _T("-");
		if (strSelect != strTemp)
		{
			m_btnSelection[nIndex].SetWindowText(strSelect);
		}
	}

	m_nSelectedMotorIndex = nSelectedIndex;
}


void CViewAssign::OnBnClickedBtnSelect1()
{
	this->ChangeSelection(0);
}


void CViewAssign::OnBnClickedBtnSelect2()
{
	this->ChangeSelection(1);
}


void CViewAssign::OnBnClickedBtnSelect3()
{
	this->ChangeSelection(2);
}


void CViewAssign::OnBnClickedBtnSelect4()
{
	this->ChangeSelection(3);
}


void CViewAssign::OnBnClickedBtnSelect5()
{
	this->ChangeSelection(4);
}


void CViewAssign::OnBnClickedBtnSelect6()
{
	this->ChangeSelection(5);
}


void CViewAssign::OnBnClickedBtnSelect7()
{
	this->ChangeSelection(6);
}


void CViewAssign::OnBnClickedBtnSelect8()
{
	this->ChangeSelection(7);
}


void CViewAssign::OnBnClickedBtnStart()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->SelectAssignID(m_nSelectedMotorIndex + 1);

	pDevice->EnableAutoRestoration( m_bAutoRestoration );
	if (m_bAutoRestoration)
	{
		::PostMessage(AfxGetMainWnd()->m_hWnd, CMainFrame::UM_SHOW_AUTO_RESTORATION_VIEW, pDevice->GetAddress(), m_nSelectedMotorIndex);
	}
	else
	{
		if (pDevice->StartAssignID())
		{
			DestroyDeviceWindow();
		}
	}
}

void CViewAssign::OnBnClickedBtnRestoration()
{
	m_bAutoRestoration = !m_bAutoRestoration;

	CString strText(_T("Yes")), strTemp;
	COLORREF colorBk = RGB(0x00, 0xc8, 0x00);
	COLORREF colorTxt = RGB(0x00, 0x00, 0x00);

	if (FALSE == m_bAutoRestoration)
	{
		colorBk = RGB(0xc8, 0x00, 0x00);
		colorTxt = RGB(0xc8, 0xc8, 0xc8);
		strText = _T("No");
	}

	m_wndAutoRestoration.GetWindowText(strTemp);

	if (strTemp != strText)
	{
		m_wndAutoRestoration.SetWindowText(strText);
	}

	if (m_wndAutoRestoration.GetBkColor() != colorBk)
	{
		m_wndAutoRestoration.SetBkColor(colorBk);
	}

	if (m_wndAutoRestoration.GetTextColor() != colorTxt)
	{
		m_wndAutoRestoration.SetTextColor(colorTxt);
	}
}

void CViewAssign::DisplayConditions(CDocument* pDoc)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	const INT nSecond = CTime::GetCurrentTime().GetSecond();

	if (1)	// Condition of bare motor
	{
		COLORREF colorBk = RGB(0x00, 0xc8, 0x00);
		COLORREF colorText = RGB(0x00, 0x00, 0x00);

		if (FALSE == pDevice->IsBareMotorConnected())
		{
			colorText = RGB(0xc8, 0xc8, 0xc8);
			colorBk = (nSecond & 0x1) ? RGB(0xc8, 0x00, 0x00) : RGB(0x00, 0x00, 0x00);
		}

		const INT nIndex = 0;
		if (m_wndCondition[nIndex].GetBkColor() != colorBk)
		{
			m_wndCondition[nIndex].SetBkColor(colorBk);
		}

		if (m_wndCondition[nIndex].GetTextColor() != colorText)
		{
			m_wndCondition[nIndex].SetTextColor(colorText);
		}
	}

	if (1)	// Condition of motiongate
	{
		COLORREF colorBk = RGB(0x00, 0xc8, 0x00);
		COLORREF colorText = RGB(0x00, 0x00, 0x00);

		if (FALSE == pDevice->IsMotiongateControllable() )
		{
			colorText = RGB(0xc8, 0xc8, 0xc8);
			colorBk = (nSecond & 0x1) ? RGB(0xc8, 0x00, 0x00) : RGB(0x00, 0x00, 0x00);
		}

		const INT nIndex = 1;
		if (m_wndCondition[nIndex].GetBkColor() != colorBk)
		{
			m_wndCondition[nIndex].SetBkColor(colorBk);
		}

		if (m_wndCondition[nIndex].GetTextColor() != colorText)
		{
			m_wndCondition[nIndex].SetTextColor(colorText);
		}
	}

	if (1)	// Condition of ID selection
	{
		COLORREF colorBk = RGB(0x00, 0xc8, 0x00);
		COLORREF colorText = RGB(0x00, 0x00, 0x00);

		if ( m_nSelectedMotorIndex >= pDevice->GetConnectedMotorCount() )
		{
			if (nSecond & 0x1)
			{
				colorText = RGB(0x00, 0x00, 0x00);
				colorBk = RGB(0xc8, 0xc8, 0x00);
			}
			else
			{
				colorText = RGB(0xc8, 0xc8, 0xc8);
				colorBk =  RGB(0x00, 0x00, 0x00);
			}
		}

		const INT nIndex = 2;
		if (m_wndCondition[nIndex].GetBkColor() != colorBk)
		{
			m_wndCondition[nIndex].SetBkColor(colorBk);
		}

		if (m_wndCondition[nIndex].GetTextColor() != colorText)
		{
			m_wndCondition[nIndex].SetTextColor(colorText);
		}
	}
}
