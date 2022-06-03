// CViewBackup.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CViewBackup.h"

#include "CDocDevice.h"
#include "CDeviceList.h"
#include "CDeviceMotionGate.h"

#include "CDataBaseManager.h"

// CViewBackup

IMPLEMENT_DYNCREATE(CViewBackup, CViewDevice)

CViewBackup::CViewBackup() : CViewDevice(IDD_DEVICE_BACKUP)
{

}

CViewBackup::~CViewBackup()
{
}

void CViewBackup::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewBackup, CViewDevice)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_START, &CViewBackup::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CViewBackup::OnBnClickedBtnRandom)
	ON_BN_CLICKED(IDC_BTN_NOMINAL, &CViewBackup::OnBnClickedBtnNominal)
	ON_EN_CHANGE(IDC_EDIT, &CViewBackup::OnEnChangeEdit)
END_MESSAGE_MAP()


// CViewBackup diagnostics

#ifdef _DEBUG
void CViewBackup::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewBackup::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewBackup message handlers


void CViewBackup::OnInitialUpdate()
{
	CViewDevice::OnInitialUpdate();

	if (m_wndNameRegister.SubclassDlgItem(IDC_ST_NAME_REGISTER, this))
	{
		
	}

	if (m_wndNameDevice.SubclassDlgItem(IDC_ST_NAME_DEVICE, this))
	{
		
	}

	SetTimer(TIMER_INITIALIZE, ELAPSE_TIMER_INITIALIZE, NULL);
}


void CViewBackup::OnTimer(UINT_PTR nIDEvent)
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

		m_strProjectName = pDevice->GetSystemName();
//		CString strDeviceName(pDevice->GetRealName());

		m_wndNameRegister.SetWindowText(m_strProjectName);
//		m_wndNameDevice.SetWindowText(strDeviceName);

		DestroyDeviceWindow(this);

		if (pDevice->IsMotiongateConnected() && FALSE == CheckNameMatched(pDevice))
		{
			GetDlgItem(IDC_ST_GROUP_DEVICE)->ShowWindow(SW_SHOW);
			m_wndNameDevice.ShowWindow(SW_SHOW);
		}

		GetDlgItem(IDC_EDIT)->SetFocus();

		SetTimer(TIMER_POLLING, ELAPSE_TIMER_POLLING, NULL);
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

		this->DisplayNames(pDevice);

		SetTimer(nIDEvent, ELAPSE_TIMER_POLLING, NULL);
	}	break;
	}
	CViewDevice::OnTimer(nIDEvent);
}


void CViewBackup::OnBnClickedBtnStart()
{
	CString strBackupName;
	GetDlgItemText(IDC_EDIT, strBackupName);

	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	if (pDevice)
	{
		pDevice->SetProjectName( m_strProjectName );
		pDevice->SetBackupName(strBackupName);
		pDevice->StartBackup();

		DestroyDeviceWindow();
	}
}

void CViewBackup::DisplayNames(CDeviceMotionGate* pDevice)
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

	if (m_wndNameRegister.GetBkColor() != m_colorBkName)
	{
		m_wndNameRegister.SetBkColor(m_colorBkName);
	}

	if (m_wndNameRegister.GetTextColor() != m_colorTxtName)
	{
		m_wndNameRegister.SetTextColor(m_colorTxtName);
	}

	if (m_wndNameDevice.GetBkColor() != m_colorBkName)
	{
		m_wndNameDevice.SetBkColor(m_colorBkName);
	}

	if (m_wndNameDevice.GetTextColor() != m_colorTxtName)
	{
		m_wndNameDevice.SetTextColor(m_colorTxtName);
	}*/
}

void CViewBackup::OnBnClickedBtnRandom()
{
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();
	const INT nCount = pProject->GetProjectBackupListCount( m_strProjectName );

	CString strTemp;
	strTemp.Format(_T("%s_%d"), m_strProjectName, nCount);

	SetDlgItemText(IDC_EDIT, strTemp);
}


void CViewBackup::OnBnClickedBtnNominal()
{
	
}


void CViewBackup::OnEnChangeEdit()
{
	CString strTemp;
	GetDlgItemText(IDC_EDIT, strTemp);

	GetDlgItem(IDC_BTN_START)->EnableWindow( 0 < strTemp.GetLength() ? TRUE : FALSE);
}
