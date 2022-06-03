// CDlgDevice.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgDevice.h"
#include "afxdialogex.h"

#include "CDeviceList.h"

// CDlgDevice dialog

IMPLEMENT_DYNAMIC(CDlgDevice, CDialog)

CDlgDevice::CDlgDevice() : CDialog()
{
	m_dwAddress = 0;
}

CDlgDevice::CDlgDevice(LPCTSTR lpszTemplateName, CWnd* pParentWnd ) : CDialog(lpszTemplateName, pParentWnd)
{
	m_dwAddress = 0;
}

CDlgDevice::CDlgDevice(UINT nIDTemplate, CWnd* pParentWnd) : CDialog(nIDTemplate, pParentWnd)
{
	m_dwAddress = 0;
}

CDlgDevice::~CDlgDevice()
{
}

void CDlgDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDevice, CDialog)
END_MESSAGE_MAP()


// CDlgDevice message handlers
CDeviceMotionGate* CDlgDevice::GetMotiongateCtrl()
{
	if (0 == m_dwAddress)
	{
		return NULL;
	}

	CDeviceMotionGate* pDevice = NULL;	
	CDeviceList* pDeviceList = CDeviceList::GetInstance();	
	pDeviceList->GetMotiongateByAddress(m_dwAddress, pDevice);
	
	return pDevice;
}

const BOOL CDlgDevice::CheckNameMatched(CDeviceMotionGate* pDevice)
{
	//	이제 사용하지 않는다.
	return TRUE;
}

void CDlgDevice::SetRadioIndex(const INT* pResArray, const INT nIndex, const INT cbArray)
{
	for (INT i = 0; (-1 == cbArray) ? pResArray[i] : i < cbArray; i++)
	{
		CButton* pBtn = static_cast<CButton*>(GetDlgItem(pResArray[i]));
		const BOOL bEnable = i == nIndex;
		if (bEnable != pBtn->GetCheck())
		{
			pBtn->SetCheck(bEnable);
		}
	}
}

const INT CDlgDevice::GetRadioIndex(const INT* pResArray, const INT cbArray)
{
	for (INT i = 0; (-1 == cbArray) ? pResArray[i] : i < cbArray; i++)
	{
		if (BST_CHECKED == static_cast<CButton*>(GetDlgItem(pResArray[i]))->GetCheck())
		{
			return pResArray[i];
		}
	}

	return -1;
}

void CDlgDevice::SetAddress(const DWORD dwAddress)
{
	m_dwAddress = dwAddress;
}

const DWORD CDlgDevice::GetAddress()
{
	return m_dwAddress;
}
