#pragma once

#include "CSheetConfigDevice.h"

#include "CPageConfigDeviceInterface.h"
#include "CPageConfigDeviceDatabase.h"

#include "CPageConfigNetworkGeneral.h"

#include "CPageConfigDeviceImage.h"

#include "CPageConfigDevicePLC.h"

#include "CPageConfigDeviceTest.h"

#include "CPageConfigProjectManagement.h"

#include "CPageConfigDeviceControlAuthorize.h"

#include "CPageConfigDeviceLifeCount.h"

//	For Serial

// CSheetConfig

class CSheetConfig : public CSheetConfigDevice
{
	DECLARE_DYNAMIC(CSheetConfig)

public:
	CSheetConfig();
	virtual ~CSheetConfig();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CPageConfigDeviceInterface		m_pageDeviceInterface;
	CPageConfigDeviceDatabase		m_pageDeviceDatabase;

	CPageConfigNetworkGeneral		m_pageNetworkGeneral;
	
//	CPageConfigUserList				m_pageUserList;
	CPageConfigDeviceImage			m_pageDefaultImg;

	CPageConfigDevicePLC			m_pageDevicePLC;

	CPageConfigProjectManagement	m_pageProjectManagement;
	
	CPageConfigDeviceLifeCount		m_pageDeviceLifeCount;
	
	CPageConfigDeviceControlAuthorize	m_pageDeviceOthers;

	CFont m_BigFont;

	//CPageConfigDeviceTest			m_pageDeviceTest;

protected:
	virtual void Initialize(CDeviceMotionGate* pDevice = NULL);

public:
	virtual BOOL OnInitDialog();
	virtual CDeviceMotionGate* GetMotiongateCtrl();

	CDeviceMotionGate* ScanAddressFromSocket(const DWORD dwAddress);
	CDeviceMotionGate* ScanNameFromSocket(LPCTSTR lpszSystemName);

	virtual void		SetEditAddress(const DWORD dwAddrss, CMFCPropertyPage* pPage = NULL);
	virtual const DWORD	GetEditAddress(CMFCPropertyPage* pPage = NULL);

	virtual void		SetEditSystemName(LPCTSTR lpszSystemName, CMFCPropertyPage* pPage = NULL);
	virtual LPCTSTR		GetEditSystemName(CMFCPropertyPage* pPage = NULL);

	virtual void		SetEditMotorCount(const INT nCount, CMFCPropertyPage* pPage = NULL);
	virtual const INT	GetEditMotorCount(CMFCPropertyPage* pPage = NULL);

	virtual void		SetEditHomingTimeout(const INT nTimeout, const BOOL bSecond = TRUE, CMFCPropertyPage* pPage = NULL);
	virtual const INT	GetEditHomingTimeout(const BOOL bSecond = TRUE, CMFCPropertyPage* pPage = NULL);


	virtual HBITMAP		SetEditSystemImage(HBITMAP hBitmap, CMFCPropertyPage* pPage = NULL);
	virtual HBITMAP		GetEditSystemImage(CMFCPropertyPage* pPage = NULL);
};
