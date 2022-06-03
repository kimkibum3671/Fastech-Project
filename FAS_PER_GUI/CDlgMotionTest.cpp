// CDlgMotionTest.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgMotionTest.h"

#include "CDeviceList.h"

#include "MainFrm.h"

#include "CDocDevice.h"
#include "CDlgRename.h"

// CDlgMotionTest
IMPLEMENT_DYNCREATE(CDlgMotionTest, CDlgDevice)

CDlgMotionTest::CDlgMotionTest()
	: CDlgDevice(IDD_MOTION_TEST)
{
	m_nClickItemID = \
		m_nLastClickItemID = 0;

	m_dwTimerOn = 500;
	m_dwTickCount = 0;
}

CDlgMotionTest::~CDlgMotionTest()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		BOOL bWait = TRUE;

		if (pDevice->IsMonitoringModeEnabled())
		{
			Sleep(1);

			while (bWait)
			{
				for (INT i = 0; i < 8; i++)
				{
					pDevice->EnableServo(i, FALSE);
				}

				bWait = FALSE;
				for (INT i = 0; i < 8; i++)
				{
					if (pDevice->IsServoEnabled(i))
					{
						bWait = TRUE;
						break;
					}
				}
			}

			for (INT i = 0; i < 8; i++)
			{
				pDevice->EmergencyStop(i);
			}

			pDevice->EnableMonitoringMode(FALSE);
		}
	}
}

void CDlgMotionTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgMotionTest, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SERVO_ON, &CDlgMotionTest::OnBnClickedBtnServoOn)
	ON_BN_CLICKED(IDC_BTN_SERVO_OFF, &CDlgMotionTest::OnBnClickedBtnServoOff)
	ON_BN_CLICKED(IDC_BTN_SEL_ALL, &CDlgMotionTest::OnBnClickedBtnSelAll)
	ON_BN_CLICKED(IDC_BTN_SEL_NOT, &CDlgMotionTest::OnBnClickedBtnSelNot)
	ON_BN_CLICKED(IDC_BTN_ADMIN, &CDlgMotionTest::OnBnClickedBtnAdmin)
	ON_BN_CLICKED(IDC_BTN_ESTOP, &CDlgMotionTest::OnBnClickedBtnEstop)
	ON_BN_CLICKED(IDC_BTN_STOP, &CDlgMotionTest::OnBnClickedBtnStop)
	//	ON_BN_CLICKED(IDC_BTN_NAME, &CDlgMotionTest::OnBnClickedBtnName)
	ON_BN_CLICKED(IDC_BTN_RESET, &CDlgMotionTest::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDOK, &CDlgMotionTest::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMotionTest::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgMotionTest diagnostics

#ifdef _DEBUG
void CDlgMotionTest::AssertValid() const
{
	CDialog::AssertValid();
}

#ifndef _WIN32_WCE
void CDlgMotionTest::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);
}
#endif
#endif //_DEBUG


BOOL CDlgMotionTest::OnInitDialog()
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
		CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_PT));
		CString strTemp;
		for (INT nIndex = 1; nIndex <= 20; nIndex++)
		{
			strTemp.Format(_T("%02d"), nIndex);
			pCmbCtrl->AddString(strTemp);
		}
		pCmbCtrl->SetCurSel(0);
	}

	::SetWindowTheme(GetDlgItem(IDC_PROGRESS_PT)->m_hWnd, _T(""), _T(""));
	::SetWindowTheme(GetDlgItem(IDC_PROGRESS_ORIGIN)->m_hWnd, _T(""), _T(""));

	if (1)
	{
		if (m_tooltip.Create(this))
		{
			CString strTooltip;
			strTooltip.LoadString(IDS_HOLD_DOWN);

			m_tooltip.AddTool(GetDlgItem(IDC_BTN_MOVE_PT), strTooltip);
			m_tooltip.AddTool(GetDlgItem(IDC_BTN_ORIGIN), strTooltip);
		}
	}

	if (1)
	{
		CComboBox* pCmbSpeed = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_SPEED));
		pCmbSpeed->AddString(_T("Jog Speed - Low"));
		pCmbSpeed->AddString(_T("Jog Speed - High"));
		pCmbSpeed->SetCurSel(0);
	}

	m_wndName.SubclassDlgItem(IDC_ST_NAME, this);

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);

	return TRUE;
}

// CDlgMotionTest message handlers
//void CDlgMotionTest::OnInitialUpdate()
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
//		CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_PT));
//		CString strTemp;
//		for (INT nIndex = 1; nIndex <= 20; nIndex++)
//		{
//			strTemp.Format(_T("%02d"), nIndex);
//			pCmbCtrl->AddString(strTemp);
//		}
//		pCmbCtrl->SetCurSel(0);
//	}
//
//	::SetWindowTheme(GetDlgItem(IDC_PROGRESS_PT)->m_hWnd, _T(""), _T(""));
//	::SetWindowTheme(GetDlgItem(IDC_PROGRESS_ORIGIN)->m_hWnd, _T(""), _T(""));
//
//	if (1)
//	{
//		if (m_tooltip.Create(this))
//		{
//			CString strTooltip;
//			strTooltip.LoadString(IDS_HOLD_DOWN);
//
//			m_tooltip.AddTool(GetDlgItem(IDC_BTN_MOVE_PT), strTooltip);
//			m_tooltip.AddTool(GetDlgItem(IDC_BTN_ORIGIN), strTooltip);
//		}
//	}
//	
//	if (1)
//	{
//		CComboBox* pCmbSpeed = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_SPEED));
//		pCmbSpeed->AddString(_T("Jog Speed - Low"));
//		pCmbSpeed->AddString(_T("Jog Speed - High"));
//		pCmbSpeed->SetCurSel(0);
//	}
//
//	m_wndName.SubclassDlgItem(IDC_ST_NAME, this);
//
//	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
//}

void CDlgMotionTest::OnTimer(UINT_PTR nIDEvent)
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
		//KKB ??
		CString strCaption;
		strCaption.Format(_T("%s - Motion Test"), pDevice->GetSystemName());
		this->SetWindowText(strCaption);

		OnBnClickedBtnAdmin();

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

		DisplayMotionGateStatus(pDevice);

		DisplayMotorStatus(pDevice);

		EnableControls(pDevice);

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
				case IDC_BTN_MOVE_PT:
				case IDC_BTN_ORIGIN:
				{
					CProgressCtrl* pCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_BTN_MOVE_PT == m_nClickItemID ? IDC_PROGRESS_PT : IDC_PROGRESS_ORIGIN));
					INT nMin, nMax;
					pCtrl->GetRange(nMin, nMax);

					if (pCtrl->GetPos() == nMax)
					{
						if (IDC_BTN_MOVE_PT == m_nClickItemID)
						{
							const INT nItemNo = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_PT))->GetCurSel() + 1;
							pDevice->ExecutePositionTableIndex(GetSelectedMotorIndex(), nItemNo);
						}
						else
						{
							pDevice->ExecuteOriginSerachIndex(GetSelectedMotorIndex());
						}
					}

					pCtrl->SetPos(nMin);
				}	break;

				case IDC_BTN_JOG_CCW:
				case IDC_BTN_JOG_CW:
				{
					pDevice->MoveStopIndex(GetSelectedMotorIndex());
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
					case IDC_BTN_MOVE_PT:
					case IDC_BTN_ORIGIN:
					{
						CProgressCtrl* pCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_BTN_MOVE_PT == m_nClickItemID ? IDC_PROGRESS_PT : IDC_PROGRESS_ORIGIN));
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
				else // if (m_nLastClickItemID != m_nClickItemID)
				{
					switch (m_nClickItemID)
					{
					case IDC_BTN_JOG_CCW:
					{
						pDevice->JogMinus(GetSelectedMotorIndex(), static_cast<CComboBox*>(GetDlgItem(IDC_CMB_SPEED))->GetCurSel());
					}	break;

					case IDC_BTN_JOG_CW:
					{
						pDevice->JogPlus(GetSelectedMotorIndex(), static_cast<CComboBox*>(GetDlgItem(IDC_CMB_SPEED))->GetCurSel());
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


void CDlgMotionTest::DisplayMotionGateStatus(CDeviceMotionGate* pDevice)
{
	const DWORD dwMotiongateStatus = pDevice->GetMotiongateStatus();

	for (INT nIndex = 0; 16 > nIndex; nIndex++)
	{
		m_wndSignal[0].EnableSignal(nIndex, (dwMotiongateStatus >> nIndex) & 0x01);
	}

	this->DisplayName(pDevice);

	if (pDevice->IsTeachingPendantOnLine())
	{
		if (pDevice->IsMonitoringModeEnabled())
		{
			OnBnClickedBtnAdmin();
		}

		GetDlgItem(IDC_BTN_ADMIN);
	}
}


void CDlgMotionTest::DisplayName(CDeviceMotionGate* pDevice)
{

	//CString strDeviceName(pDevice->GetRealName()), strRegisterName(pDevice->GetSystemName());
	CString strSystemName(pDevice->GetSystemName());

	COLORREF m_colorBkName = RGB(0x0a, 0x0a, 0x50);
	COLORREF m_colorTxtName = RGB(0xc8, 0xc8, 0xc8);
	/*if (strDeviceName != strRegisterName)
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
	}*/
	CString strTemp;

	m_wndName.GetWindowText(strTemp);

	if (strTemp != strSystemName)
	{
		m_wndName.SetWindowText(strSystemName);
	}

	if (m_wndName.GetBkColor() != m_colorBkName)
	{
		m_wndName.SetBkColor(m_colorBkName);
	}

	if (m_wndName.GetTextColor() != m_colorTxtName)
	{
		m_wndName.SetTextColor(m_colorTxtName);
	}

}

void CDlgMotionTest::DisplayMotorStatus(CDeviceMotionGate* pDevice)
{
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
	}
}

BOOL CDlgMotionTest::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg);

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	{
		const INT nCtrlID = ::GetDlgCtrlID(pMsg->hwnd);
		switch (nCtrlID)
		{
		case IDC_BTN_MOVE_PT:
		case IDC_BTN_ORIGIN:
		case IDC_BTN_JOG_CCW:
		case IDC_BTN_JOG_CW:
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
		case IDC_BTN_MOVE_PT:
		case IDC_BTN_ORIGIN:
		case IDC_BTN_JOG_CCW:
		case IDC_BTN_JOG_CW:
		{
			::ReleaseCapture();
		}	break;
		}
	}	break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CDlgMotionTest::OnBnClickedBtnServoOn()
{
	CDeviceMotionGate* pDevice = this->GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	DWORD dwIndex = 0x00000000;
	for (INT nIndex = 1; nIndex <= 8; nIndex++)
	{
		if (pDevice->IsServoEnabled(nIndex))
		{
			dwIndex |= (0x1 << nIndex);
		}
	}

	dwIndex |= GetSelectedMotorIndex();

	pDevice->EnableServo(dwIndex);
}


void CDlgMotionTest::OnBnClickedBtnServoOff()
{
	CDeviceMotionGate* pDevice = this->GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	DWORD dwIndex = 0x00000000;
	for (INT nIndex = 1; nIndex <= 8; nIndex++)
	{
		if (pDevice->IsServoEnabled(nIndex))
		{
			dwIndex |= (0x1 << nIndex);
		}
	}

	dwIndex &= ~GetSelectedMotorIndex();

	pDevice->EnableServo(dwIndex);
}

void CDlgMotionTest::EnableControls(CDeviceMotionGate* pDevice)
{
	EnableMotorSelection(pDevice);
	EnableServoOn(pDevice);
	EnableReset(pDevice);
	EnableJogMotionControl(pDevice);
	EnableNormalMotionControl(pDevice);
	EnableStopMotionControl(pDevice);
}

void CDlgMotionTest::EnableMotorSelection(CDeviceMotionGate* pDevice)
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


void CDlgMotionTest::EnableServoOn(CDeviceMotionGate* pDevice)
{
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
		IDC_BTN_SERVO_ON,
		IDC_BTN_SERVO_OFF,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		if (bControlable != GetDlgItem(nResID[nIndex])->IsWindowEnabled())
		{
			GetDlgItem(nResID[nIndex])->EnableWindow(bControlable);
		}
	}
}


void CDlgMotionTest::EnableReset(CDeviceMotionGate* pDevice)
{
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


void CDlgMotionTest::EnableJogMotionControl(CDeviceMotionGate* pDevice)
{
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
		IDC_BTN_JOG_CCW,
		IDC_BTN_JOG_CW,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		GetDlgItem(nResID[nIndex])->EnableWindow(bControlable);
	}
}


void CDlgMotionTest::EnableNormalMotionControl(CDeviceMotionGate* pDevice)
{
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
		IDC_BTN_MOVE_PT,
		IDC_BTN_ORIGIN,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		GetDlgItem(nResID[nIndex])->EnableWindow(bControlable);
	}
}


void CDlgMotionTest::EnableStopMotionControl(CDeviceMotionGate* pDevice)
{
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


void CDlgMotionTest::OnBnClickedBtnSelAll()
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


void CDlgMotionTest::OnBnClickedBtnSelNot()
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


const DWORD CDlgMotionTest::GetSelectedMotorIndex()
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


void CDlgMotionTest::OnBnClickedBtnAdmin()
{
	CDeviceMotionGate* pMotionGate = GetMotiongateCtrl();
	if (NULL == pMotionGate)
	{
		return;
	}

	CRect rtButton, rtGroup1, rtGroup2;
	CWnd* pWnd = GetDlgItem(IDC_BTN_ADMIN);
	pWnd->GetWindowRect(rtButton);
	GetDlgItem(IDC_ST_GROUP1)->GetWindowRect(rtGroup1);
	GetDlgItem(IDC_ST_GROUP2)->GetWindowRect(rtGroup2);

	if (rtButton.right == rtGroup2.right)
	{
		if (pMotionGate->IsTeachingPendantOnLine())
		{
			return;
		}

		pWnd->SetWindowText(_T(">"));

		rtButton.right = rtGroup1.left;

		ShowControls(TRUE);

		GetDlgItem(IDC_CMB_PT)->ShowWindow(SW_SHOW);

		if (pMotionGate)
		{
			pMotionGate->EnableMonitoringMode(TRUE);
		}
	}
	else
	{
		pWnd->SetWindowText(_T("Enable Administration Mode"));

		rtButton.right = rtGroup2.right;

		ShowControls(FALSE);

		GetDlgItem(IDC_CMB_PT)->ShowWindow(SW_HIDE);

		pMotionGate->EnableMonitoringMode(FALSE);
	}

	pWnd->SetWindowPos(&wndTop, rtButton.left, rtButton.top, rtButton.Width(), rtButton.Height(), SWP_NOMOVE);
}


void CDlgMotionTest::ShowControls(const BOOL bEnable)
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
		IDC_BTN_SERVO_ON,
		IDC_BTN_SERVO_OFF,
		IDC_BTN_RESET,
		IDC_BTN_JOG_CCW,
		IDC_BTN_JOG_CW,
		IDC_CMB_PT,
		IDC_CMB_SPEED,
		IDC_PROGRESS_PT,
		IDC_BTN_MOVE_PT,
		IDC_PROGRESS_ORIGIN,
		IDC_BTN_ORIGIN,
		IDC_BTN_ESTOP,
		IDC_BTN_STOP,
		NULL,
	};

	for (INT nIndex = 0; nResID[nIndex]; nIndex++)
	{
		GetDlgItem(nResID[nIndex])->EnableWindow(bEnable);
		GetDlgItem(nResID[nIndex])->ShowWindow(bEnable ? SW_SHOW : SW_HIDE);
	}
}


void CDlgMotionTest::OnBnClickedBtnEstop()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->EmergencyStopIndex(pDevice->GetConnectedMotorStatus());	//	0번 모터가 빠져있으므로 1비트를 시프트한다.
	}
}


void CDlgMotionTest::OnBnClickedBtnStop()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->MoveStopIndex(GetSelectedMotorIndex());
	}
}


void CDlgMotionTest::OnBnClickedBtnReset()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->RescanSlave();
		pDevice->AlarmReset();
	}
}

void CDlgMotionTest::ShowMotorInformation(const INT nIndex, const INT nCmdShow)
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


void CDlgMotionTest::OnBnClickedOk()
{
	
}


void CDlgMotionTest::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
