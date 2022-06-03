// CPageConfigDeviceAll.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceAll.h"
#include "afxdialogex.h"


// CPageConfigDeviceAll dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceAll, CMFCPropertyPageEx)

CPageConfigDeviceAll::CPageConfigDeviceAll(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_CONFIG_DEVICE_ALL)
{

}

CPageConfigDeviceAll::~CPageConfigDeviceAll()
{
}

void CPageConfigDeviceAll::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceAll, CMFCPropertyPageEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPageConfigDeviceAll message handlers


BOOL CPageConfigDeviceAll::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	SubclassWizardButton(IDB_BACK, IDB_NEXT);

	return TRUE;
}


BOOL CPageConfigDeviceAll::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_CHAR:
	{
		if (IDC_IPADDRESS == ::GetDlgCtrlID(pMsg->hwnd))
		{
			//	여기서 바로 처리하면 IP Address가 제대로 동작하지 않는다.
			SetTimer(TIMER_IP_ADDRESS, 1, NULL);
		}
	}	break;
	}

	return CMFCPropertyPageEx::PreTranslateMessage(pMsg);
}


void CPageConfigDeviceAll::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_IP_ADDRESS:
	{
		CIPAddressCtrl* pAddressCtrl = static_cast<CIPAddressCtrl*>(GetDlgItem(IDC_IPADDRESS));
		if (pAddressCtrl->GetSafeHwnd())
		{
			pAddressCtrl->GetAddress(m_dwAddress);

			const BOOL bEnable = !(
				0x00 == (0xff000000 & m_dwAddress) ||
				0x00 == (0x000000ff & m_dwAddress) ||
				0xff == (0x000000ff & m_dwAddress));

			CWnd* pWnd = GetDlgItem(IDC_BTN_NEXT);

			if (bEnable != pWnd->IsWindowEnabled())
			{
				pWnd->EnableWindow(bEnable);
			}
		}
	}	break;
	}

	CMFCPropertyPageEx::OnTimer(nIDEvent);
}
