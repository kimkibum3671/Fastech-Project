// CPageConfigDeviceInterface.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceInterface.h"
#include "afxdialogex.h"

#include "CDataBaseManager.h"

////////////////////////////////////////
//	For scan serial port
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")

#include <Ntddser.h>

// CPageConfigDeviceInterface dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceInterface, CMFCPropertyPageEx)

CPageConfigDeviceInterface::CPageConfigDeviceInterface(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_CONFIG_DEVICE_GENERAL)
{

}

CPageConfigDeviceInterface::~CPageConfigDeviceInterface()
{
}

void CPageConfigDeviceInterface::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceInterface, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_TCP, &CPageConfigDeviceInterface::OnBnClickedRadioTcp)
	ON_BN_CLICKED(IDC_RADIO_SERIAL, &CPageConfigDeviceInterface::OnBnClickedRadioSerial)
	ON_BN_CLICKED(IDC_BTN_SCAN, &CPageConfigDeviceInterface::OnBnClickedBtnScan)
	ON_BN_CLICKED(IDC_RADIO_MG_YES, &CPageConfigDeviceInterface::OnBnClickedRadioMgYes)
	ON_BN_CLICKED(IDC_RADIO_MG_NO, &CPageConfigDeviceInterface::OnBnClickedRadioMgNo)
	ON_CBN_SELCHANGE(IDC_CMB_SERIAL, &CPageConfigDeviceInterface::OnCbnSelchangeCmbSerial)
	ON_CBN_SELCHANGE(IDC_CMB_CONTINUE, &CPageConfigDeviceInterface::OnCbnSelchangeCmbContinue)
END_MESSAGE_MAP()


// CPageConfigDeviceInterface message handlers


BOOL CPageConfigDeviceInterface::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	m_nInterface = pConfigDB->GetInterface();

	switch (m_nInterface)
	{
	case CDataBaseConfig::interface_TCP:
	{
		static_cast<CButton*>(GetDlgItem(IDC_RADIO_TCP))->SetCheck(BST_CHECKED);
		GetDlgItem(IDC_CMB_SERIAL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_SCAN)->EnableWindow(FALSE);
	}	break;

	default:
	{
		static_cast<CButton*>(GetDlgItem(IDC_RADIO_SERIAL))->SetCheck(BST_CHECKED);
	}	break;
	}

	if (1)
	{
		m_nSerialPort = pConfigDB->GetSerialPort();
		ScanSerialPort(m_nSerialPort);
	}

	if (1)
	{
		m_bEnableMotionGate = pConfigDB->IsMotionGateEnabled();
		static_cast<CButton*>(GetDlgItem(m_bEnableMotionGate ? IDC_RADIO_MG_YES : IDC_RADIO_MG_NO))->SetCheck(BST_CHECKED);
//		GetDlgItem(IDC_CMB_CONTINUE)->EnableWindow(m_bEnableMotionGate);
		GetDlgItem(IDC_CMB_CONTINUE)->EnableWindow(FALSE);
	}

	if (1)
	{
		m_bConfirm = pConfigDB->IsConfirmToContinueEnabled();

		static_cast<CComboBox*>(GetDlgItem(IDC_CMB_CONTINUE))->SetCurSel(m_bConfirm ? 1 : 0);
	}	

	return TRUE;
}


void CPageConfigDeviceInterface::OnBnClickedRadioTcp()
{
	m_nInterface = CDataBaseConfig::interface_TCP;
	
	GetDlgItem(IDC_CMB_SERIAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SCAN)->EnableWindow(FALSE);
}


void CPageConfigDeviceInterface::OnBnClickedRadioSerial()
{
	m_nInterface = CDataBaseConfig::interface_Serial;
	
	GetDlgItem(IDC_CMB_SERIAL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SCAN)->EnableWindow(TRUE);
}


void CPageConfigDeviceInterface::OnBnClickedBtnScan()
{
	ScanSerialPort(m_nSerialPort);
}


void CPageConfigDeviceInterface::OnCbnSelchangeCmbSerial()
{
	CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_SERIAL));

	CString strPortNo;	pCmbCtrl->GetLBText(pCmbCtrl->GetCurSel(), strPortNo);
	strPortNo.MakeUpper();
	const INT nIndex = strPortNo.Find(_T("(COM")) + 4;
	m_nSerialPort = _ttoi(static_cast<LPCTSTR>(strPortNo) + nIndex);
}


void CPageConfigDeviceInterface::OnBnClickedRadioMgYes()
{
	m_bEnableMotionGate = TRUE;
	GetDlgItem(IDC_CMB_CONTINUE)->EnableWindow();
}


void CPageConfigDeviceInterface::OnBnClickedRadioMgNo()
{
	m_bEnableMotionGate = FALSE;
	GetDlgItem(IDC_CMB_CONTINUE)->EnableWindow(FALSE);
}


void CPageConfigDeviceInterface::OnCbnSelchangeCmbContinue()
{
	CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_CONTINUE));
	
	m_bConfirm = pCmbCtrl->GetCurSel();
}

void CPageConfigDeviceInterface::ScanSerialPort(const INT nSelectedPort)
{
	CComboBox* pComboBox = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_SERIAL));

	pComboBox->ResetContent();

	GUID* guidDev = (GUID*)&GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR;
	HDEVINFO hDevInfo = SetupDiGetClassDevs(guidDev, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return;
	}

	INT nIndexSel = -1;

	for (int cnt = 0;; cnt++)
	{
		SP_DEVICE_INTERFACE_DATA ifcData = { 0, };
		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		SP_DEVICE_INTERFACE_DETAIL_DATA stDetData = { 0, };
		stDetData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guidDev, cnt, &ifcData))
		{
			//	Got a device.Get the details.
			SP_DEVINFO_DATA devdata = { NULL, };
			devdata.cbSize = sizeof(SP_DEVINFO_DATA);
			SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifcData, &stDetData, stDetData.cbSize, NULL, &devdata);

			wchar_t fname[256] = { 0 };
			wchar_t desc[256] = { 0 };
			SetupDiGetDeviceRegistryProperty(hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL, (PBYTE)fname, 256, NULL);

			CString strPortNo(fname);
			strPortNo.MakeUpper();
			const INT nIndex = strPortNo.Find(_T("(COM")) + 4;
			const INT nPortNo = _ttoi(static_cast<LPCTSTR>(strPortNo) + nIndex);

			const INT nCount = pComboBox->AddString(fname);

			if (nSelectedPort == nPortNo)
			{
				nIndexSel = nCount;
			}
		}
		else
		{
			break;
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	const INT nResult = pComboBox->GetCount();

	if (nResult && -1 != nIndexSel)
	{
		pComboBox->SetCurSel(nIndexSel);
	}
	else
	{
		m_nSerialPort = -1;
	}
}


void CPageConfigDeviceInterface::OnOK()
{
	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	const INT nOldInterface = pConfigDB->GetInterface();
	INT nNewInterface = 0;

	//	Set Interface
	if (static_cast<CButton*>(GetDlgItem(IDC_RADIO_TCP))->GetCheck())
	{
		nNewInterface = CDataBaseConfig::interface_TCP;
	}
	else if( static_cast<CButton*>(GetDlgItem(IDC_RADIO_SERIAL))->GetCheck() )
	{
		nNewInterface = CDataBaseConfig::interface_Serial;
	}
	else
	{
		ASSERT(0);
	}

	if (nOldInterface != nNewInterface)
	{
		pConfigDB->SetInterface(nNewInterface);

		CString strContent(_T(""));
		CString strMainInstruction("Changes to the interface setting will not take effect until\nthe software is restarted.");
		CString strTitle(_T("Need to restart software"));

		CTaskDialog taskDialog(strContent, strMainInstruction, strTitle, TDCBF_CLOSE_BUTTON);

		taskDialog.AddCommandControl(IDYES, _T("OK, I understand."));

		taskDialog.SetMainIcon(TD_INFORMATION_ICON);

		taskDialog.SetDialogWidth(250);

		taskDialog.DoModal();
	}

	//	Set Port No
	pConfigDB->SetSerialPort(m_nSerialPort);

	//	Enable Motiongate
	pConfigDB->EnableMotionGate(m_bEnableMotionGate);

	//	Enable Continue
	pConfigDB->ConfirmToContinue(m_bConfirm);

	CMFCPropertyPageEx::OnOK();
}
