// CSheetConfigDeviceSingle.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetConfigDeviceSingle.h"

#include "CDeviceList.h"
#include "CDataBaseManager.h"

#include "CSheetConfig.h"
#include "CPageConfigDeviceSocket.h"

// CSheetConfigDeviceSingle

IMPLEMENT_DYNAMIC(CSheetConfigDeviceSingle, CMFCPropertySheetEx)

CSheetConfigDeviceSingle::CSheetConfigDeviceSingle()
{
	SetLook(CMFCPropertySheet::PropSheetLook_List, 300);

	AddPage(&m_pageAddress);
	AddPage(&m_pageMotor);
	AddPage(&m_pageOrigin);
	AddPage(&m_pageImage);
}

CSheetConfigDeviceSingle::~CSheetConfigDeviceSingle()
{

}


BEGIN_MESSAGE_MAP(CSheetConfigDeviceSingle, CMFCPropertySheet)
END_MESSAGE_MAP()

// CSheetConfigDeviceSingle message handlers
void CSheetConfigDeviceSingle::Initialize(CDeviceMotionGate* pDevice)
{
	CSheetConfigDevice::Initialize(pDevice);
}

BOOL CSheetConfigDeviceSingle::OnInitDialog()
{
	BOOL bResult = CSheetConfigDevice::OnInitDialog();
	
	if (m_pDevice)
	{
		CString strTitle;
		strTitle.Format(_T("%s Configuration"), m_pDevice->GetSystemName());

		SetTitle(strTitle);
	}

	ShowWizardButton(IDCANCEL);

	RecalculateWizardButton();

	return bResult;
}

const INT	CSheetConfigDeviceSingle::CheckPropertyValues()
{
	INT nResult = CSheetConfigDevice::result_OK;

	if (CSheetConfigDevice::result_OK == nResult)
	{
		nResult = m_pageAddress.CheckSystemName();
	}

	ShowWizardButton(IDOK, CSheetConfigDevice::result_OK == nResult ? SW_SHOWNORMAL : SW_HIDE);

	RecalculateWizardButton();

	return nResult;
}

const BOOL CSheetConfigDeviceSingle::ScanAddress(const DWORD dwAddress)
{
	CWnd* pParent = GetParent();
	
	if (RUNTIME_CLASS(CSheetConfig) == pParent->GetRuntimeClass() )
	{
		CSheetConfig* pConfig = dynamic_cast<CSheetConfig*>(pParent);
		CDeviceMotionGate* pDevice = pConfig->ScanAddressFromSocket(dwAddress);

		return (NULL != pDevice) && (pDevice != m_pDevice);
	}

	return FALSE;
}

const BOOL CSheetConfigDeviceSingle::ScanSystemName(LPCTSTR lpszSystemName)
{
	CWnd* pParent = GetParent();

	if (RUNTIME_CLASS(CSheetConfig) == pParent->GetRuntimeClass())
	{
		CSheetConfig* pConfig = dynamic_cast<CSheetConfig*>(pParent);
		CDeviceMotionGate* pDevice = pConfig->ScanNameFromSocket(lpszSystemName);

		return (NULL != pDevice) && (pDevice != m_pDevice);
	}

	return FALSE;
}
