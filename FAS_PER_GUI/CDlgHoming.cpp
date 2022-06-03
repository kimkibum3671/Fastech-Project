// CViewHom.cpp : implementation file
// CViewHom.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgHoming.h"

#include "CDocDevice.h"

#include "CDeviceList.h"

// CDlgHoming

IMPLEMENT_DYNCREATE(CDlgHoming, CDlgDevice)

CDlgHoming::CDlgHoming()
	: CDlgDevice(IDD_HOMING)
{
	m_nClickItemID = \
		m_nLastClickItemID = 0;

	m_dwTimerOn = 500;
	m_dwTickCount = 0;
	m_bHoming = FALSE;
}

CDlgHoming::~CDlgHoming()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->EnableMonitoringMode(FALSE);
	}
}

void CDlgHoming::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgHoming, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ADMIN, &CDlgHoming::OnBnClickedBtnAdmin)
	ON_BN_CLICKED(IDC_BTN_RESET, &CDlgHoming::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_ESTOP, &CDlgHoming::OnBnClickedBtnEstop)
	ON_BN_CLICKED(IDC_BTN_STOP, &CDlgHoming::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_SEL_ALL, &CDlgHoming::OnBnClickedBtnSelAll)
	ON_BN_CLICKED(IDC_BTN_SEL_NOT, &CDlgHoming::OnBnClickedBtnSelNot)
	ON_BN_CLICKED(IDOK, &CDlgHoming::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgHoming diagnostics

#ifdef _DEBUG
void CDlgHoming::AssertValid() const
{
	CDialog::AssertValid();
}

#ifndef _WIN32_WCE
void CDlgHoming::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);
}
#endif
#endif //_DEBUG


// CDlgHoming message handlers


//void CDlgHoming::OnInitialUpdate()
//{
//	CDlgDevice::OnInitialUpdate();
//
//	if (1)
//	{
//		const INT nResID[][2] = {
//			IDC_ST_SIGNAL0,	0,
//			IDC_ST_SIGNAL1,	1,
//			IDC_ST_SIGNAL2,	1,
//			IDC_ST_SIGNAL3,	1,
//			IDC_ST_SIGNAL4,	1,
//			IDC_ST_SIGNAL5,	1,
//			IDC_ST_SIGNAL6,	1,
//			IDC_ST_SIGNAL7,	1,
//			IDC_ST_SIGNAL8,	1,
//			NULL, NULL
//		};
//
//		for (INT nIndex = 0; nResID[nIndex][0]; nIndex++)
//		{
//			if (NULL == m_wndSignal[nIndex].GetSafeHwnd())
//			{
//				m_wndSignal[nIndex].SubclassDlgItem(nResID[nIndex][0], this);
//				m_wndSignal[nIndex].SetSize(4, 4);
//				m_wndSignal[nIndex].SetFormID(nResID[nIndex][1]);	// 0 : Status I/O
//			}
//		}
//	}
//
//	if (1)
//	{
//		const COLORREF m_colorDefaultBk = RGB(0x00, 0x00, 0x00);
//		const COLORREF m_colorDefaultText = RGB(0xc8, 0xc8, 0xc8);
//		const COLORREF m_colorDefaultBk2 = RGB(0x0a, 0x0a, 0x50);
//		const COLORREF m_colorDefaultText2 = RGB(0xc8, 0xc8, 0xc8);
//
//		const INT nResID[][3] = {
//			IDC_ST_ERROR1,	IDC_ST_POSITION_TABLE1, IDC_ST_POSITION1,
//			IDC_ST_ERROR2,	IDC_ST_POSITION_TABLE2, IDC_ST_POSITION2,
//			IDC_ST_ERROR3,	IDC_ST_POSITION_TABLE3, IDC_ST_POSITION3,
//			IDC_ST_ERROR4,	IDC_ST_POSITION_TABLE4, IDC_ST_POSITION4,
//			IDC_ST_ERROR5,	IDC_ST_POSITION_TABLE5, IDC_ST_POSITION5,
//			IDC_ST_ERROR6,	IDC_ST_POSITION_TABLE6, IDC_ST_POSITION6,
//			IDC_ST_ERROR7,	IDC_ST_POSITION_TABLE7, IDC_ST_POSITION7,
//			IDC_ST_ERROR8,	IDC_ST_POSITION_TABLE8, IDC_ST_POSITION8,
//			NULL, NULL, NULL
//		};
//
//		for (INT nIndex = 0; nResID[nIndex][0]; nIndex++)
//		{
//			for (INT nIndex2 = 0; nIndex2 < 3; nIndex2++)
//			{
//				m_wndColor[nIndex][nIndex2].SubclassDlgItem(nResID[nIndex][nIndex2], this);
//
//				if (nIndex2 == 0)
//				{
//					m_wndColor[nIndex][nIndex2].SetBkColor(m_colorDefaultBk);
//					m_wndColor[nIndex][nIndex2].SetTextColor(m_colorDefaultText);
//				}
//				else
//				{
//					m_wndColor[nIndex][nIndex2].SetBkColor(m_colorDefaultBk2);
//					m_wndColor[nIndex][nIndex2].SetTextColor(m_colorDefaultText);
//				}
//
//			}
//		}
//	}
//
//	if (1)
//	{
//		const COLORREF m_colorDefaultBk = RGB(0xc8, 0xc8, 0x00);
//		const COLORREF m_colorDefaultText = RGB(0x00, 0x00, 0x00);
//
//		const INT nResID[] = {
//			IDC_ST_HOMING1,
//			IDC_ST_HOMING2,
//			IDC_ST_HOMING3,
//			IDC_ST_HOMING4,
//			IDC_ST_HOMING5,
//			IDC_ST_HOMING6,
//			IDC_ST_HOMING7,
//			IDC_ST_HOMING8,
//			NULL
//		};
//
//		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
//		{
//			m_wndHomingTimer[nIndex].SubclassDlgItem(nResID[nIndex], this);
//			m_wndHomingTimer[nIndex].SetBkColor(m_colorDefaultBk);
//			m_wndHomingTimer[nIndex].SetTextColor(m_colorDefaultText);
//		}
//	}
//
//	::SetWindowTheme(GetDlgItem(IDC_PROGRESS_ORIGIN)->m_hWnd, _T(""), _T(""));
//
//	if (1)
//	{
//		if (m_tooltip.Create(this))
//		{
//			CString strTooltip;
//			strTooltip.LoadString(IDS_HOLD_DOWN);
//
//			m_tooltip.AddTool(GetDlgItem(IDC_BTN_ORIGIN), strTooltip);
//		}
//	}
//
//	m_wndName.SubclassDlgItem(IDC_ST_NAME, this);
//
//	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
//}


void CDlgHoming::OnTimer(UINT_PTR nIDEvent)
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

		//DestroyDeviceWindow(this);
		CString strCaption;
		strCaption.Format(_T("%s - Homming"), pDevice->GetSystemName());
		this->SetWindowTextW(strCaption);

		OnBnClickedBtnAdmin();

		m_bHoming = pDevice->IsAutoHomingEnabled();

		if (m_bHoming)
		{
			OnBnClickedBtnAdmin();
			OnBnClickedBtnSelAll();
			pDevice->ExecuteOriginSerachIndex(GetSelectedMotorIndex());
			pDevice->EnableAutoHoming(FALSE);
		}

		if (pDevice->IsMotiongateConnected())
		{
			CheckNameMatched(pDevice);
		}

		//	Show or Hide Connected motor
		for (INT nIndex = 1; nIndex <= 8; nIndex++)
		{
			const BOOL bVisible = pDevice->IsMotorScanned(nIndex);
//			if (bVisible != m_wndSignal[nIndex].IsWindowVisible())
			{
				ShowMotorInformation(nIndex, bVisible ? SW_SHOWNORMAL : SW_HIDE);
			}
		}

		SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);
		SetTimer(TIMER_LBUTTONDOWN, ELAPSE_TIMER_LBUTTONDOWN, NULL);
	}	break;

	case TIMER_POLLING:
	{
		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice || FALSE == pDevice->IsMotiongateConnected())
		{
			GetParent()->DestroyWindow();
			return;
		}

		/*CDocDevice* pDoc = static_cast<CDocDevice*>(GetDocument());

		pDoc->CheckDeviceTitle();*/

		DisplayMotionGateStatus(pDevice);

		DisplayMotorStatus(pDevice);

		EnableControls(pDevice);

		CheckHomingResult();

		SetTimer(nIDEvent, ELAPSE_TIMER_POLLING, NULL);
	}	break;

	case TIMER_LBUTTONDOWN:
	{
		if (m_nClickItemID)
		{
			CDeviceMotionGate* pDevice = GetMotiongateCtrl();
			if (NULL == pDevice)
			{
				m_nClickItemID = m_nLastClickItemID = 0;
				break;
			}

			CWnd* pWnd = GetCapture();
			if (NULL == pWnd)
			{
				switch (m_nClickItemID)
				{
				case IDC_BTN_ORIGIN:
				{
					CProgressCtrl* pCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS_ORIGIN));
					INT nMin, nMax;
					pCtrl->GetRange(nMin, nMax);

					if (pCtrl->GetPos() == nMax)
					{
						pDevice->ExecuteOriginSerachIndex(GetSelectedMotorIndex());
					}

					pCtrl->SetPos(nMin);
				}	break;

				default:
				{
					m_nClickItemID = m_nLastClickItemID = 0;
				}	break;
				}

				m_nClickItemID = 0;
			}
			else
			{
				if (m_nLastClickItemID == m_nClickItemID)
				{
					switch (m_nClickItemID)
					{
					case IDC_BTN_ORIGIN:
					{
						CProgressCtrl* pCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS_ORIGIN));
						INT nMin, nMax;
						pCtrl->GetRange(nMin, nMax);
						const INT nWidth = nMax - nMin;
						DOUBLE dbRatio = static_cast<DOUBLE>(::GetTickCount() - m_dwTickCount) / m_dwTimerOn;

						if (1.0f <= dbRatio)
						{
							dbRatio = 1.0f;
						}

						const INT nNewPos = nMin + static_cast<INT>(dbRatio * nWidth);
						if (pCtrl->GetPos() != nNewPos)
						{
							pCtrl->SetPos(nNewPos);

							pCtrl->Invalidate(FALSE);
						}
					}	break;

					default:
					{

					}	break;
					}
				}
			}

			m_nLastClickItemID = m_nClickItemID;
		}
	}	break;
	}

	CDlgDevice::OnTimer(nIDEvent);
}

const DWORD CDlgHoming::GetSelectedMotorIndex()
{
	DWORD dwResult = 0;
	const INT nResID[] = {
		IDC_BTN_CHECK1,
		IDC_BTN_CHECK2,
		IDC_BTN_CHECK3,
		IDC_BTN_CHECK4,
		IDC_BTN_CHECK5,
		IDC_BTN_CHECK6,
		IDC_BTN_CHECK7,
		IDC_BTN_CHECK8,
	};

	for (INT nIndex = 0; nIndex < 8; nIndex++)
	{
		if (static_cast<CButton*>(GetDlgItem(nResID[nIndex]))->GetCheck())
		{
			dwResult |= (0x1 << (nIndex + 1));
		}
	}

	return dwResult;
}

void CDlgHoming::DisplayMotionGateStatus(CDeviceMotionGate* pDevice)
{
	//CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	const DWORD dwMotiongateStatus = pDevice->GetMotiongateStatus();
	for (INT nIndex = 0; 16 > nIndex; nIndex++)
	{
		m_wndSignal[0].EnableSignal(nIndex, (dwMotiongateStatus >> nIndex) & 0x01);
	}

	this->DisplayName(pDevice);
}

void CDlgHoming::DisplayMotorStatus(CDeviceMotionGate* pDevice)
{
	/*const DWORD dwAddrss = static_cast<CDocDevice*>(pDoc)->GetAddress();
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == pDeviceList->GetMotiongateByAddress(dwAddrss, pDevice))
	{
		return;
	}*/

	CString strTemp, strValue;
	for (INT iAxisNo = 1; iAxisNo <= 8; iAxisNo++)
	{
		DWORD dwMotorStatus = pDevice->GetMotorStatus(iAxisNo);
		for (INT nIndex = 0; 16 > nIndex; nIndex++)
		{
			m_wndSignal[iAxisNo].EnableSignal(nIndex, dwMotorStatus & 0x01);
			dwMotorStatus >>= 1;
		}

		if (1)
		{
			INT nResID[] = {
				NULL,
				IDC_ST_POSITION1,
				IDC_ST_POSITION2,
				IDC_ST_POSITION3,
				IDC_ST_POSITION4,
				IDC_ST_POSITION5,
				IDC_ST_POSITION6,
				IDC_ST_POSITION7,
				IDC_ST_POSITION8,
			};

			strValue.Format(_T("%.2f [mm]"), pDevice->GetMotorCurPosition(iAxisNo));
			GetDlgItemText(nResID[iAxisNo], strTemp);
			if (strTemp != strValue)
			{
				SetDlgItemText(nResID[iAxisNo], strValue);
			}
		}

		if (1)
		{
			INT nResID[] = {
				NULL,
				IDC_ST_POSITION_TABLE1,
				IDC_ST_POSITION_TABLE2,
				IDC_ST_POSITION_TABLE3,
				IDC_ST_POSITION_TABLE4,
				IDC_ST_POSITION_TABLE5,
				IDC_ST_POSITION_TABLE6,
				IDC_ST_POSITION_TABLE7,
				IDC_ST_POSITION_TABLE8,
			};

			strValue.Format(_T("PT #%d"), pDevice->GetMotorCurPositionTable(iAxisNo));
			GetDlgItemText(nResID[iAxisNo], strTemp);
			if (strTemp != strValue)
			{
				SetDlgItemText(nResID[iAxisNo], strValue);
			}
		}

		if (1)
		{
			INT nResID[] = {
				NULL,
				IDC_ST_ERROR1,
				IDC_ST_ERROR2,
				IDC_ST_ERROR3,
				IDC_ST_ERROR4,
				IDC_ST_ERROR5,
				IDC_ST_ERROR6,
				IDC_ST_ERROR7,
				IDC_ST_ERROR8,
			};

			const DWORD dwErrorCode = pDevice->GetMotorErrorCode(iAxisNo);

			COLORREF colorBk = RGB(0x00, 0xc8, 0x00);
			COLORREF colorTxt = RGB(0x00, 0x00, 0x00);

			if (FALSE == pDevice->IsMotorConnected(iAxisNo))
			{
				//	Disconnected와 Not used를 구분해야 하겠네
				strValue = _T("Disconnected");
				colorBk = RGB(0xc8, 0x00, 0x00);
				colorTxt = RGB(0xc8, 0xc8, 0xc8);
			}
			else if (0 == dwErrorCode)
			{
				if (TRUE == pDevice->IsServoEnabled(iAxisNo))
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

		if (1)	//	Homing Status
		{
			const INT nIndex = iAxisNo - 1;
			COLORREF colorBk = m_wndHomingTimer[nIndex].GetBkColor();
			COLORREF colorTxt = m_wndHomingTimer[nIndex].GetTextColor();

			CString strNewText;
			m_wndHomingTimer[nIndex].GetWindowText(strNewText);
			strTemp = strNewText;

			if (pDevice->IsHoming(iAxisNo))
			{
				strNewText.Format(_T("#%d Homing"), iAxisNo);
				colorBk = RGB(0xc8, 0xc8, 0x00);
				colorTxt = RGB(0x00, 0x00, 0x00);
			}
			else if (pDevice->IsHomingOK(iAxisNo))
			{
				strNewText.Format(_T("#%d OK"), iAxisNo);
				colorBk = RGB(0x00, 0xc8, 0x00);
				colorTxt = RGB(0x00, 0x00, 0x00);
			}
			else if (pDevice->IsHomingNG(iAxisNo))
			{
				strNewText.Format(_T("#%d NG"), iAxisNo);
				colorBk = RGB(0xc8, 0x00, 0x00);
				colorTxt = RGB(0xc8, 0xc8, 0xc8);
			}

			if (colorBk != m_wndHomingTimer[nIndex].GetBkColor())
			{
				m_wndHomingTimer[nIndex].SetBkColor(colorBk);
			}

			if (colorTxt != m_wndHomingTimer[nIndex].GetTextColor())
			{
				m_wndHomingTimer[nIndex].SetTextColor(colorTxt);
			}

			if (strTemp != strNewText)
			{
				m_wndHomingTimer[nIndex].SetWindowText(strNewText);
			}
		}
	}
}

void CDlgHoming::DisplayName(CDeviceMotionGate* pDevice)
{
	/*CString strDeviceName(pDevice->GetRealName()), strRegisterName(pDevice->GetSystemName());

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
	}*/
}

BOOL CDlgHoming::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg);

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	{
		const INT nCtrlID = ::GetDlgCtrlID(pMsg->hwnd);
		switch (nCtrlID)
		{
		case IDC_BTN_ORIGIN:
		{
			::SetCapture(pMsg->hwnd);
			m_nClickItemID = nCtrlID;
			m_dwTickCount = ::GetTickCount();
		}	break;
		}
	}	break;

	case WM_LBUTTONUP:
	{
		switch (::GetDlgCtrlID(pMsg->hwnd))
		{
		case IDC_BTN_ORIGIN:
		{
			::ReleaseCapture();
		}	break;
		}
	}	break;
	}

	return CDlgDevice::PreTranslateMessage(pMsg);
}

void CDlgHoming::EnableControls(CDeviceMotionGate* pDevice)
{
	EnableMotorSelection(pDevice);
	EnableReset(pDevice);
	EnableNormalMotionControl(pDevice);
	EnableStopMotionControl(pDevice);
}

void CDlgHoming::EnableMotorSelection(CDeviceMotionGate* pDevice)
{
	DWORD dwMotorSelection = 0x0;
	DWORD dwMotorEnable = 0x0;

	const INT nResID[] = {
		IDC_BTN_CHECK1,
		IDC_BTN_CHECK2,
		IDC_BTN_CHECK3,
		IDC_BTN_CHECK4,
		IDC_BTN_CHECK5,
		IDC_BTN_CHECK6,
		IDC_BTN_CHECK7,
		IDC_BTN_CHECK8,
	};

	//CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	BOOL bControlable = FALSE;
	switch (pDevice->GetOpCode())
	{
	case CDeviceMotionGate::opCode_read_motiongate_status:
	case CDeviceMotionGate::opCode_standby_motiongate:
	{
		bControlable = TRUE;
	}	break;
	}

	for (INT nIndex = 0; nIndex < 8; nIndex++)
	{
		CWnd* pWnd = GetDlgItem(nResID[nIndex]);

		BOOL bEnable = FALSE;
		if (bControlable)
		{
			bEnable = pDevice->IsMotorConnected(nIndex + 1);
			if (bEnable)
			{
				dwMotorEnable |= (0x1 << nIndex);
				if (static_cast<CButton*>(pWnd)->GetCheck())
				{
					dwMotorSelection |= (0x1 << nIndex);
				}
			}
		}

		if (pWnd->IsWindowEnabled() != bEnable)
		{
			pWnd->EnableWindow(bEnable);
		}
	}

	if (1)
	{
		CWnd* pWnd = GetDlgItem(IDC_BTN_SEL_ALL);
		const BOOL bEnable = bControlable && dwMotorSelection != dwMotorEnable;
		if (pWnd->IsWindowEnabled() != bEnable)
		{
			pWnd->EnableWindow(bEnable);
		}
	}

	if (1)
	{
		CWnd* pWnd = GetDlgItem(IDC_BTN_SEL_NOT);
		const BOOL bEnable = bControlable && dwMotorSelection == dwMotorEnable;
		if (pWnd->IsWindowEnabled() != bEnable)
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}


void CDlgHoming::EnableReset(CDeviceMotionGate* pDevice)
{
	//CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	BOOL bControlable = FALSE;
	switch (pDevice->GetOpCode())
	{
	case CDeviceMotionGate::opCode_read_motiongate_status:
	case CDeviceMotionGate::opCode_standby_motiongate:
	{
		bControlable = TRUE;
	}	break;
	}

	GetDlgItem(IDC_BTN_RESET)->EnableWindow(bControlable);
}


void CDlgHoming::EnableNormalMotionControl(CDeviceMotionGate* pDevice)
{
	//CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	BOOL bControlable = FALSE;
	switch (pDevice->GetOpCode())
	{
	case CDeviceMotionGate::opCode_read_motiongate_status:
	case CDeviceMotionGate::opCode_standby_motiongate:
	{
		bControlable = TRUE;
	}	break;
	}

	const INT nResID[] = {
		IDC_BTN_ORIGIN,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		GetDlgItem(nResID[nIndex])->EnableWindow(bControlable);
	}
}


void CDlgHoming::EnableStopMotionControl(CDeviceMotionGate* pDevice)
{
	//CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	BOOL bControlable = FALSE;
	switch (pDevice->GetOpCode())
	{
	case CDeviceMotionGate::opCode_read_motiongate_status:
	case CDeviceMotionGate::opCode_standby_motiongate:
	{
		bControlable = TRUE;
	}	break;
	}

	const INT nResID[] = {
		IDC_BTN_ESTOP,
		IDC_BTN_STOP,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		GetDlgItem(nResID[nIndex])->EnableWindow(bControlable);
	}
}


void CDlgHoming::ShowControls(const BOOL bEnable)
{
	const INT nResID[] = {
		IDC_ST_GROUP1,
		IDC_BTN_SEL_ALL,
		IDC_BTN_SEL_NOT,
		IDC_BTN_CHECK1,
		IDC_BTN_CHECK2,
		IDC_BTN_CHECK3,
		IDC_BTN_CHECK4,
		IDC_BTN_CHECK5,
		IDC_BTN_CHECK6,
		IDC_BTN_CHECK7,
		IDC_BTN_CHECK8,
		IDC_ST_GROUP2,
		IDC_BTN_RESET,
		IDC_PROGRESS_ORIGIN,
		IDC_BTN_ORIGIN,
		IDC_BTN_ESTOP,
		IDC_BTN_STOP,
		IDC_ST_HOMING1,
		IDC_ST_HOMING2,
		IDC_ST_HOMING3,
		IDC_ST_HOMING4,
		IDC_ST_HOMING5,
		IDC_ST_HOMING6,
		IDC_ST_HOMING7,
		IDC_ST_HOMING8,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		GetDlgItem(nResID[nIndex])->EnableWindow(bEnable);
		GetDlgItem(nResID[nIndex])->ShowWindow(bEnable ? SW_SHOW : SW_HIDE);
	}
}

void CDlgHoming::OnBnClickedBtnAdmin()
{
	CRect rtButton, rtGroup1, rtGroup2;
	CWnd* pWnd = GetDlgItem(IDC_BTN_ADMIN);
	pWnd->GetWindowRect(rtButton);
	GetDlgItem(IDC_ST_GROUP1)->GetWindowRect(rtGroup1);
	GetDlgItem(IDC_ST_GROUP2)->GetWindowRect(rtGroup2);

	CDeviceMotionGate* pMotionGate = GetMotiongateCtrl();
	if (rtButton.right == rtGroup2.right)
	{
		pWnd->SetWindowText(_T(">"));

		rtButton.right = rtGroup1.left;

		ShowControls(TRUE);

		if (pMotionGate)
		{
			SetTimer(TIMER_ENTER_MONITOR_MODE, 0, NULL);
			pMotionGate->EnableMonitoringMode(TRUE);
		}
	}
	else
	{
		pWnd->SetWindowText(_T("Enable Administration Mode"));

		rtButton.right = rtGroup2.right;

		ShowControls(FALSE);

		if (pMotionGate)
		{
			KillTimer(TIMER_ENTER_MONITOR_MODE);
			pMotionGate->EnableMonitoringMode(FALSE);
		}
	}

	pWnd->SetWindowPos(&wndTop, rtButton.left, rtButton.top, rtButton.Width(), rtButton.Height(), SWP_NOMOVE);
}


void CDlgHoming::OnBnClickedBtnReset()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->AlarmReset();
	}
}


void CDlgHoming::OnBnClickedBtnEstop()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->EmergencyStopIndex(pDevice->GetConnectedMotorStatus());	//	0번 모터가 빠져있으므로 1비트를 시프트한다.
	}
}


void CDlgHoming::OnBnClickedBtnStop()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->MoveStopIndex(GetSelectedMotorIndex());
	}
}


void CDlgHoming::OnBnClickedBtnSelAll()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	const INT nResID[] = {
		IDC_BTN_CHECK1,
		IDC_BTN_CHECK2,
		IDC_BTN_CHECK3,
		IDC_BTN_CHECK4,
		IDC_BTN_CHECK5,
		IDC_BTN_CHECK6,
		IDC_BTN_CHECK7,
		IDC_BTN_CHECK8,
	};

	for (INT nIndex = 0; nIndex < 8; nIndex++)
	{
		CButton* pBtn = static_cast<CButton*>(GetDlgItem(nResID[nIndex]));

		pBtn->SetCheck(pDevice->IsMotorConnected(nIndex + 1));
	}
}


void CDlgHoming::OnBnClickedBtnSelNot()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	const INT nResID[] = {
		IDC_BTN_CHECK1,
		IDC_BTN_CHECK2,
		IDC_BTN_CHECK3,
		IDC_BTN_CHECK4,
		IDC_BTN_CHECK5,
		IDC_BTN_CHECK6,
		IDC_BTN_CHECK7,
		IDC_BTN_CHECK8,
	};

	for (INT nIndex = 0; nIndex < 8; nIndex++)
	{
		CButton* pBtn = static_cast<CButton*>(GetDlgItem(nResID[nIndex]));

		if (pDevice->IsMotorConnected(nIndex + 1))
		{
			pBtn->SetCheck(FALSE);
		}
	}
}

void CDlgHoming::CheckHomingResult()
{
	if (FALSE == m_bHoming)
	{
		return;

	}
	
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (NULL == pDevice || FALSE == pDevice->IsMotiongateConnected())
	{
		return;
	}

	BOOL bResult = TRUE;
	for (INT nIndex = 1; nIndex < 16; nIndex++)
	{
		if (pDevice->IsMotorConnected(nIndex))
		{
			if (pDevice->IsHoming(nIndex))
			{
				return;
			}
			else if (TRUE == pDevice->IsHomingNG(nIndex))
			{
				bResult = FALSE;
				break;
			}
			else if (FALSE == pDevice->IsHomingOK(nIndex))
			{
				return;
			}
		}
	}

	m_bHoming = FALSE;

	if (bResult)
	{
		MessageBox(_T("Homing Success"), _T("Result"), MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(_T("Homging Failed"), _T("Result"), MB_ICONERROR);
	}
}


void CDlgHoming::ShowMotorInformation(const INT nIndex, const INT nCmdShow)
{
	const INT nResID[][5] = {
		NULL,					NULL,			NULL,			NULL,				NULL,
		IDC_ST_GROUP_MOTOR1,	IDC_ST_SIGNAL1,	IDC_ST_ERROR1,	IDC_ST_POSITION1,	IDC_ST_POSITION_TABLE1,
		IDC_ST_GROUP_MOTOR2,	IDC_ST_SIGNAL2,	IDC_ST_ERROR2,	IDC_ST_POSITION2,	IDC_ST_POSITION_TABLE2,
		IDC_ST_GROUP_MOTOR3,	IDC_ST_SIGNAL3,	IDC_ST_ERROR3,	IDC_ST_POSITION3,	IDC_ST_POSITION_TABLE3,
		IDC_ST_GROUP_MOTOR4,	IDC_ST_SIGNAL4,	IDC_ST_ERROR4,	IDC_ST_POSITION4,	IDC_ST_POSITION_TABLE4,
		IDC_ST_GROUP_MOTOR5,	IDC_ST_SIGNAL5,	IDC_ST_ERROR5,	IDC_ST_POSITION5,	IDC_ST_POSITION_TABLE5,
		IDC_ST_GROUP_MOTOR6,	IDC_ST_SIGNAL6,	IDC_ST_ERROR6,	IDC_ST_POSITION6,	IDC_ST_POSITION_TABLE6,
		IDC_ST_GROUP_MOTOR7,	IDC_ST_SIGNAL7,	IDC_ST_ERROR7,	IDC_ST_POSITION7,	IDC_ST_POSITION_TABLE7,
		IDC_ST_GROUP_MOTOR8,	IDC_ST_SIGNAL8,	IDC_ST_ERROR8,	IDC_ST_POSITION8,	IDC_ST_POSITION_TABLE8,
	};

	if (1 > nIndex || 8 < nIndex)
	{
		return;
	}

	for (INT nColumn = 0; nColumn < 5; nColumn++)
	{
		CWnd* pWnd = GetDlgItem(nResID[nIndex][nColumn]);
//		if ((nCmdShow) ? TRUE : FALSE != pWnd->IsWindowVisible())
		{
			pWnd->ShowWindow(nCmdShow);
		}
	}
}


BOOL CDlgHoming::OnInitDialog()
{
	CDlgDevice::OnInitDialog();

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
			NULL, NULL
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
		const COLORREF m_colorDefaultBk2 = RGB(0x0a, 0x0a, 0x50);
		const COLORREF m_colorDefaultText2 = RGB(0xc8, 0xc8, 0xc8);

		const INT nResID[][3] = {
			IDC_ST_ERROR1,	IDC_ST_POSITION_TABLE1, IDC_ST_POSITION1,
			IDC_ST_ERROR2,	IDC_ST_POSITION_TABLE2, IDC_ST_POSITION2,
			IDC_ST_ERROR3,	IDC_ST_POSITION_TABLE3, IDC_ST_POSITION3,
			IDC_ST_ERROR4,	IDC_ST_POSITION_TABLE4, IDC_ST_POSITION4,
			IDC_ST_ERROR5,	IDC_ST_POSITION_TABLE5, IDC_ST_POSITION5,
			IDC_ST_ERROR6,	IDC_ST_POSITION_TABLE6, IDC_ST_POSITION6,
			IDC_ST_ERROR7,	IDC_ST_POSITION_TABLE7, IDC_ST_POSITION7,
			IDC_ST_ERROR8,	IDC_ST_POSITION_TABLE8, IDC_ST_POSITION8,
			NULL, NULL, NULL
		};

		for (INT nIndex = 0; nResID[nIndex][0]; nIndex++)
		{
			for (INT nIndex2 = 0; nIndex2 < 3; nIndex2++)
			{
				m_wndColor[nIndex][nIndex2].SubclassDlgItem(nResID[nIndex][nIndex2], this);

				if (nIndex2 == 0)
				{
					m_wndColor[nIndex][nIndex2].SetBkColor(m_colorDefaultBk);
					m_wndColor[nIndex][nIndex2].SetTextColor(m_colorDefaultText);
				}
				else
				{
					m_wndColor[nIndex][nIndex2].SetBkColor(m_colorDefaultBk2);
					m_wndColor[nIndex][nIndex2].SetTextColor(m_colorDefaultText);
				}

			}
		}
	}

	if (1)
	{
		const COLORREF m_colorDefaultBk = RGB(0xc8, 0xc8, 0x00);
		const COLORREF m_colorDefaultText = RGB(0x00, 0x00, 0x00);

		const INT nResID[] = {
			IDC_ST_HOMING1,
			IDC_ST_HOMING2,
			IDC_ST_HOMING3,
			IDC_ST_HOMING4,
			IDC_ST_HOMING5,
			IDC_ST_HOMING6,
			IDC_ST_HOMING7,
			IDC_ST_HOMING8,
			NULL
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_wndHomingTimer[nIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndHomingTimer[nIndex].SetBkColor(m_colorDefaultBk);
			m_wndHomingTimer[nIndex].SetTextColor(m_colorDefaultText);
		}
	}

	::SetWindowTheme(GetDlgItem(IDC_PROGRESS_ORIGIN)->m_hWnd, _T(""), _T(""));

	if (1)
	{
		if (m_tooltip.Create(this))
		{
			CString strTooltip;
			strTooltip.LoadString(IDS_HOLD_DOWN);

			m_tooltip.AddTool(GetDlgItem(IDC_BTN_ORIGIN), strTooltip);
		}
	}

	m_wndName.SubclassDlgItem(IDC_ST_NAME, this);

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgHoming::OnBnClickedOk()
{
	
}
