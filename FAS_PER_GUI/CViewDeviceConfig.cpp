// CViewDeviceConfig.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CViewDeviceConfig.h"

// CViewDeviceConfig

IMPLEMENT_DYNCREATE(CViewDeviceConfig, CViewDevice)

CViewDeviceConfig::CViewDeviceConfig()
	: CViewDevice(IDD_DEVICE_CONFIG)
{

}

CViewDeviceConfig::~CViewDeviceConfig()
{

}

void CViewDeviceConfig::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewDeviceConfig, CFormView)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_REGISTER, &CViewDeviceConfig::OnBnClickedRadioRegister)
	ON_BN_CLICKED(IDC_RADIO_DEVICE, &CViewDeviceConfig::OnBnClickedRadioDevice)
	ON_BN_CLICKED(IDC_RADIO_NEW, &CViewDeviceConfig::OnBnClickedRadioNew)
	ON_BN_CLICKED(IDC_BTN_HOMING_TIMEOUT, &CViewDeviceConfig::OnBnClickedBtnHomingTimeout)
	ON_BN_CLICKED(IDC_BTN_EXECUTE, &CViewDeviceConfig::OnBnClickedBtnExecute)
	ON_EN_CHANGE(IDC_EDIT_NEW, &CViewDeviceConfig::OnEnChangeEditNew)
END_MESSAGE_MAP()


// CViewDeviceConfig diagnostics

#ifdef _DEBUG
void CViewDeviceConfig::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewDeviceConfig::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewDeviceConfig message handlers
void CViewDeviceConfig::OnInitialUpdate()
{
	CViewDevice::OnInitialUpdate();

	if (1)
	{
		const INT nResID[] = {
			IDC_ST_REGISTER1,
			IDC_ST_REGISTER2,
			IDC_ST_REGISTER3,
			NULL
		};

		const COLORREF colorBk = RGB(0x00, 0xc8, 0x00);
		const COLORREF colorTxt = RGB(0x00, 0x00, 0x00);

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_wndColor[nIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndColor[nIndex].SetBkColor(colorBk);
			m_wndColor[nIndex].SetTextColor(colorTxt);
		}
	}

	if (1)
	{
		const INT nResID[] = {
		IDC_ST_DEVICE1,
		IDC_ST_DEVICE2,
		IDC_ST_DEVICE3,
		NULL
		};

		const COLORREF colorBk = RGB(0x0a, 0x0a, 0x50);
		const COLORREF colorTxt = RGB(0xc8, 0xc8, 0xc8);

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			const INT nItemIndex = nIndex + 3;
			m_wndColor[nItemIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndColor[nItemIndex].SetBkColor(colorBk);
			m_wndColor[nItemIndex].SetTextColor(colorTxt);
		}
	}

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
}


void CViewDeviceConfig::OnTimer(UINT_PTR nIDEvent)
{
	/*switch (nIDEvent)
	{
	case TIMER_INITIALIZE:
	{
		KillTimer(nIDEvent);

		CDeviceMotionGate* pDevice = GetMotiongateCtrl();
		if (NULL == pDevice)
		{
			break;
		}

		if (m_wndViewer.SubclassDlgItem(IDC_ST_IMAGE, this))
		{
			m_wndViewer.LoadMainImage(pDevice->GetSystemImage());
		}

		CString strProjectName(pDevice->GetSystemName());
		SetDlgItemText(IDC_ST_REGISTER2, strProjectName);

		CString strDeviceName(pDevice->GetRealName());
		SetDlgItemText(IDC_ST_DEVICE2, strDeviceName);

		DestroyDeviceWindow(this);

		if (pDevice->IsMotiongateConnected())
		{
			if (CheckNameMatched(pDevice))
			{
				const INT nResID[] = {
					IDC_RADIO_REGISTER,
					IDC_RADIO_DEVICE,
					NULL
				};

				for (INT nIndex = 0; nResID[nIndex]; nIndex++)
				{
					GetDlgItem(nResID[nIndex])->EnableWindow(FALSE);
				}
				
				static_cast<CButton*>(GetDlgItem(IDC_RADIO_NEW))->SetCheck(BST_CHECKED);
				
				CWnd* pWnd = GetDlgItem(IDC_EDIT_NEW);
				pWnd->SetFocus();
				GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow(pWnd->GetWindowTextLength() ? TRUE : FALSE);
			}
			else
			{
				if (1)
				{
					const INT nResID[] = {
					IDC_EDIT_TIMEOUT,
					IDC_BTN_HOMING_TIMEOUT,
					IDC_EDIT_NEW,
					NULL
					};

					for (INT nIndex = 0; nResID[nIndex]; nIndex++)
					{
						GetDlgItem(nResID[nIndex])->EnableWindow(FALSE);
					}
				}

				static_cast<CButton*>(GetDlgItem(IDC_RADIO_REGISTER))->SetCheck(BST_CHECKED);
				GetDlgItem(IDC_BTN_EXECUTE)->SetFocus();
			}
		}

		SetDlgItemInt(IDC_EDIT_TIMEOUT, pDevice->GetHomingTimeout(), FALSE);

		SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);
	}	break;

	case TIMER_POLLING:
	{

	}	break;
	}*/

	CViewDevice::OnTimer(nIDEvent);
}


void CViewDeviceConfig::OnBnClickedRadioRegister()
{
	GetDlgItem(IDC_EDIT_NEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow(TRUE);
}


void CViewDeviceConfig::OnBnClickedRadioDevice()
{
	GetDlgItem(IDC_EDIT_NEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow(TRUE);
}


void CViewDeviceConfig::OnBnClickedRadioNew()
{
	CWnd* pWnd = GetDlgItem(IDC_EDIT_NEW);
	pWnd->EnableWindow(TRUE);
	pWnd->SetFocus();

	GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow(pWnd->GetWindowTextLength() ? TRUE : FALSE);
}


void CViewDeviceConfig::OnBnClickedBtnHomingTimeout()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	pDevice->SetHomingTimeout(GetDlgItemInt(IDC_EDIT_TIMEOUT));
}


void CViewDeviceConfig::OnBnClickedBtnExecute()
{
//	CString strMessage, strCaption;
//	
//	const INT nResID[] = {
//		IDC_RADIO_REGISTER,
//		IDC_RADIO_DEVICE,
//		IDC_RADIO_NEW,
//		NULL
//	};
//
//	INT nIndex = 0;
//	for (; nResID[nIndex]; nIndex++)
//	{
//		if (BST_CHECKED == static_cast<CButton*>(GetDlgItem(nResID[nIndex]))->GetCheck())
//		{
//			break;
//		}
//	}
//
//	if (!nResID[nIndex])
//	{
//		//	들어올 수 없는 영역
//		return;
//	}
//
//	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
//	if (!pDevice)
//	{
//		return;
//	}
//
//	BYTE btOption = 0;
////	pDevice->SetMotiongateCommandOption(0);
//
//	CString strName;
//	BOOL bResult = TRUE;
//	switch (nResID[nIndex])
//	{
//	case IDC_RADIO_REGISTER:
//	{
//		btOption = CDeviceMotionGate::option_change_name_register;
//		strName = pDevice->GetSystemName();
//	}	break;
//
//	case IDC_RADIO_DEVICE:
//	{
//		btOption = CDeviceMotionGate::option_change_name_device;
//		strName = pDevice->GetRealName();
//	}	break;
//
//	case IDC_RADIO_NEW:
//	{
//		btOption = CDeviceMotionGate::option_change_name_register | CDeviceMotionGate::option_change_name_device;
//		GetDlgItemText(IDC_EDIT_NEW, strName);
//	}	break;
//	}
//		
//	if( btOption & CDeviceMotionGate::option_change_name_device )
//	{
//		strMessage.LoadString(IDS_FORMAT_CHANGE_NAME);
//		strCaption.LoadString(IDS_CAPTION_CONFIRM);
//
//		const INT nMessageID = MessageBox(strMessage, strCaption, MB_YESNOCANCEL | MB_ICONQUESTION);
//		switch(nMessageID)
//		{
//		case IDYES:
//		{
//			btOption |= CDeviceMotionGate::option_change_name_database;
//		}	break;
//
//		case IDNO:
//		{
//
//		}	break;
//
//		case IDCANCEL:
//		{
//			return;
//		}	break;
//		}
//	}
//	
//	switch (pDevice->ChangeName(strName, btOption))
//	{
//	case 0:
//	{
//
//	}	break;
//
//	case 1:
//	{
//
//	}	break;
//	}
//
//	bResult = TRUE;
//	if (bResult)
//	{
//		DestroyDeviceWindow();
//	}
//	else
//	{
//		strMessage.LoadString(IDS_FAIL_CHANGE_NAME);
//		strCaption.LoadString(IDS_CAPTION_ERROR);
//
//		MessageBox(strMessage, strCaption, MB_ICONERROR);
//	}
}


void CViewDeviceConfig::OnEnChangeEditNew()
{
	GetDlgItem(IDC_BTN_EXECUTE)->EnableWindow( GetDlgItem(IDC_EDIT_NEW)->GetWindowTextLength() ? TRUE : FALSE );
}

