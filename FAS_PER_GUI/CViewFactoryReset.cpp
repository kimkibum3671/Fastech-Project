// CViewFactoryReset.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CViewFactoryReset.h"

#include "CDocDevice.h"
#include "CDeviceList.h"

// CViewFactoryReset

IMPLEMENT_DYNCREATE(CViewFactoryReset, CViewDevice)

CViewFactoryReset::CViewFactoryReset()
	: CViewDevice(IDD_FACTORY_RESET)
{
	m_nSelectedMotorIndex = 0;
}

CViewFactoryReset::~CViewFactoryReset()
{
}

void CViewFactoryReset::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewFactoryReset, CFormView)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SELECT1, &CViewFactoryReset::OnBnClickedBtnSelect1)
	ON_BN_CLICKED(IDC_BTN_SELECT2, &CViewFactoryReset::OnBnClickedBtnSelect2)
	ON_BN_CLICKED(IDC_BTN_SELECT3, &CViewFactoryReset::OnBnClickedBtnSelect3)
	ON_BN_CLICKED(IDC_BTN_SELECT4, &CViewFactoryReset::OnBnClickedBtnSelect4)
	ON_BN_CLICKED(IDC_BTN_SELECT5, &CViewFactoryReset::OnBnClickedBtnSelect5)
	ON_BN_CLICKED(IDC_BTN_SELECT6, &CViewFactoryReset::OnBnClickedBtnSelect6)
	ON_BN_CLICKED(IDC_BTN_SELECT7, &CViewFactoryReset::OnBnClickedBtnSelect7)
	ON_BN_CLICKED(IDC_BTN_SELECT8, &CViewFactoryReset::OnBnClickedBtnSelect8)
	ON_BN_CLICKED(IDC_BTN_START, &CViewFactoryReset::OnBnClickedBtnStart)
END_MESSAGE_MAP()


// CViewFactoryReset diagnostics

#ifdef _DEBUG
void CViewFactoryReset::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewFactoryReset::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewFactoryReset message handlers


void CViewFactoryReset::OnInitialUpdate()
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
			IDC_ST_SIGNAL_ZERO, 1,
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
			IDC_ST_ERROR_ZERO,
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
			m_wndColor[nIndex].SubclassDlgItem(nResID[nIndex], this);

			m_wndColor[nIndex].SetBkColor(m_colorDefaultBk);
			m_wndColor[nIndex].SetTextColor(m_colorDefaultText);
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

		//	활성화 되어있는 장치중 제일 빠른 녀석을 찾아서 활성화
		ChangeSelection(0);
	}


	m_wndName.SubclassDlgItem(IDC_ST_NAME, this);

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
}


void CViewFactoryReset::OnTimer(UINT_PTR nIDEvent)
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

		SetTimer(nIDEvent, ELAPSE_TIMER_POLLING, NULL);
	}	break;
	}	//	end of 'switch (nIDEvent)'

	CViewDevice::OnTimer(nIDEvent);
}

void CViewFactoryReset::DisplayMotionGateStatus(CDocument* pDoc)
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
		FALSE == pDevice->IsBareMotorConnected())
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

void CViewFactoryReset::DisplayName(CDeviceMotionGate* pDevice)
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

void CViewFactoryReset::DisplayMotorStatus(CDocument* pDoc)
{
	const DWORD dwAddrss = static_cast<CDocDevice*>(pDoc)->GetAddress();
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == pDeviceList->GetMotiongateByAddress(dwAddrss, pDevice))
	{
		return;
	}

	CString strTemp, strValue;
	for (INT iAxisNo = 0; iAxisNo <= 8; iAxisNo++)
	{
		DWORD dwMotorStatus = pDevice->GetMotorStatus(iAxisNo);
		for (INT nIndex = 0; 16 > nIndex; nIndex++)
		{
			if (iAxisNo)
			{
				m_wndSignal[iAxisNo].EnableSignal(nIndex, dwMotorStatus & 0x01);

				const BOOL bConnected = pDevice->IsMotorConnected(iAxisNo);
				if (bConnected != m_btnSelection[iAxisNo].IsWindowEnabled())
				{
					m_btnSelection[iAxisNo - 1].EnableWindow(bConnected);
				}
			}
			else
			{
				m_wndSignal[9].EnableSignal(nIndex, dwMotorStatus & 0x01);
			}

			dwMotorStatus >>= 1;
		}

		if (1)
		{
			INT nResID[] = {
				IDC_ST_ERROR_ZERO,
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


void CViewFactoryReset::OnBnClickedBtnSelect1()
{
	this->ChangeSelection(0);
}


void CViewFactoryReset::OnBnClickedBtnSelect2()
{
	this->ChangeSelection(1);
}


void CViewFactoryReset::OnBnClickedBtnSelect3()
{
	this->ChangeSelection(2);
}


void CViewFactoryReset::OnBnClickedBtnSelect4()
{
	this->ChangeSelection(3);
}


void CViewFactoryReset::OnBnClickedBtnSelect5()
{
	this->ChangeSelection(4);
}


void CViewFactoryReset::OnBnClickedBtnSelect6()
{
	this->ChangeSelection(5);
}


void CViewFactoryReset::OnBnClickedBtnSelect7()
{
	this->ChangeSelection(6);
}


void CViewFactoryReset::OnBnClickedBtnSelect8()
{
	this->ChangeSelection(7);
}


void CViewFactoryReset::ChangeSelection(const INT nSelectedIndex)
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
		strSelect = bCheck ? _T("Reset") : _T("-");
		if (strSelect != strTemp)
		{
			m_btnSelection[nIndex].SetWindowText(strSelect);
		}
	}

	m_nSelectedMotorIndex = nSelectedIndex;
}


void CViewFactoryReset::OnBnClickedBtnStart()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->SetProjectName(_T("Fastech_Motiongate"));
	if (pDevice->StartFactoryReset(m_nSelectedMotorIndex + 1))
	{
		DestroyDeviceWindow();
	}
}
