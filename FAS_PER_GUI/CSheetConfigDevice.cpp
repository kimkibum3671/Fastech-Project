// CSheetConfigDevice.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetConfigDevice.h"

#include "CDataBaseManager.h"

// CSheetConfigDevice

IMPLEMENT_DYNAMIC(CSheetConfigDevice, CMFCPropertySheetEx)

CSheetConfigDevice::CSheetConfigDevice()
{
	m_pDevice = NULL;
}

CSheetConfigDevice::~CSheetConfigDevice()
{
	
}


BEGIN_MESSAGE_MAP(CSheetConfigDevice, CMFCPropertySheet)
END_MESSAGE_MAP()

// CSheetConfigDevice message handlers
void CSheetConfigDevice::Initialize(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;
}

void CSheetConfigDevice::SetMotiongateCtrl(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;
}

CDeviceMotionGate* CSheetConfigDevice::GetMotiongateCtrl()
{
	return m_pDevice;
}

//void CSheetConfigDevice::SetAddress(const DWORD dwAddress)
//{
//	if (m_stMotiongateInfo.dwAddress != dwAddress)
//	{
//		m_stMotiongateInfo.dwAddress = dwAddress;
//	}	
//}
//
//
//const DWORD	CSheetConfigDevice::GetAddress()
//{
//	return m_stMotiongateInfo.dwAddress;
//}
//
//void CSheetConfigDevice::SetMotorCount(const INT nCount)
//{
//	if (m_stMotiongateInfo.nMotorCount = nCount)
//	{
//		m_stMotiongateInfo.nMotorCount = nCount;
//	}
//}
//
//const INT CSheetConfigDevice::GetMotorCount()
//{
//	return m_stMotiongateInfo.nMotorCount;
//}
//
//void CSheetConfigDevice::SetTimeout(const INT nTimeout)
//{
//	if (m_stMotiongateInfo.nHomingTimeout != nTimeout)
//	{
//		m_stMotiongateInfo.nHomingTimeout = nTimeout;
//	}
//}
//
//const INT CSheetConfigDevice::GetTimeout()
//{
//	return m_stMotiongateInfo.nHomingTimeout;
//}
//
//void CSheetConfigDevice::SetSystemName(LPCTSTR lpszSystemName)
//{
//	if (m_stMotiongateInfo.strSystemName != lpszSystemName)
//	{
//		m_stMotiongateInfo.strSystemName = lpszSystemName;
//	}
//}
//
//LPCTSTR	CSheetConfigDevice::GetSystemName()
//{
//	return m_stMotiongateInfo.strSystemName;
//}
//
//CBitmap* CSheetConfigDevice::SetSystemImage(HBITMAP hBitmap)
//{
//	if (m_stMotiongateInfo.bmpImage.GetSafeHandle())
//	{
//		m_stMotiongateInfo.bmpImage.Detach();
//	}
//
//	if (NULL == hBitmap)
//	{
//		return NULL;
//	}
//
//	m_stMotiongateInfo.bmpImage.Attach(::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0));
//
//	return &m_stMotiongateInfo.bmpImage;
//}
//
//CBitmap* CSheetConfigDevice::GetSystemImage()
//{
//	return &m_stMotiongateInfo.bmpImage;
//}
//

BOOL CSheetConfigDevice::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheetEx::OnInitDialog();

	return bResult;
}

const INT	CSheetConfigDevice::CheckPropertyValues()
{
	return result_OK;
}

void CSheetConfigDevice::SetEditAddress(const DWORD dwAddrss, CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	
	ASSERT(pDevice);
	
	pDevice->SetEditAddress(dwAddrss);
}

const DWORD CSheetConfigDevice::GetEditAddress(CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	return pDevice->GetEditAddress();
}

void CSheetConfigDevice::SetEditSystemName(LPCTSTR lpszSystemName, CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();
	
	ASSERT(pDevice);
	
	pDevice->SetEditSystemName(lpszSystemName);
}


void CSheetConfigDevice::SetEditDeviceID(INT nDeviceID, CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	pDevice->SetEditDeviceID(nDeviceID);
}

LPCTSTR CSheetConfigDevice::GetEditSystemName(CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	return pDevice->GetEditSystemName();
}


const INT CSheetConfigDevice::GetEditDeviceID(CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);
	return pDevice->GetEditDeviceID();
}


void CSheetConfigDevice::SetEditMotorCount(const INT nCount, CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	pDevice->SetEditMotorCount(nCount);
}

const INT CSheetConfigDevice::GetEditMotorCount(CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	return pDevice->GetEditMotorCount();
}

void CSheetConfigDevice::SetEditHomingTimeout(const INT nTimeout, const BOOL bSecond, CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	pDevice->SetEditHomingTimeout(nTimeout, bSecond);
}

const INT CSheetConfigDevice::GetEditHomingTimeout(const BOOL bSecond, CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	return pDevice->GetEditHomingTimeout(bSecond);
}

HBITMAP CSheetConfigDevice::SetEditSystemImage(HBITMAP hBitmap, CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	CBitmap* pBitmap = pDevice->SetEditSystemImage(hBitmap);

	return *pBitmap;
}

HBITMAP	CSheetConfigDevice::GetEditSystemImage(CMFCPropertyPage* pPage)
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	ASSERT(pDevice);

	CBitmap* pBitmap = pDevice->GetEditSystemImage();

	return *pBitmap;
}



