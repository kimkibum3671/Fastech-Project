#pragma once

#include "CSheetConfigDevice.h"

#include "CPageConfigDeviceAddress.h"
#include "CPageConfigDeviceMotor.h"
#include "CPageConfigDeviceOrigin.h"
#include "CPageConfigDeviceImage.h"

#include "CDeviceList.h"

// CSheetConfigDeviceSingle

class CSheetConfigDeviceSingle : public CSheetConfigDevice
{
	DECLARE_DYNAMIC(CSheetConfigDeviceSingle)

public:
	CSheetConfigDeviceSingle();
	virtual ~CSheetConfigDeviceSingle();

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void Initialize(CDeviceMotionGate* pDevice = NULL);

protected:
	CPageConfigDeviceAddress	m_pageAddress;
	CPageConfigDeviceMotor		m_pageMotor;
	CPageConfigDeviceOrigin		m_pageOrigin;
	CPageConfigDeviceImage		m_pageImage;

public:
	virtual const INT	CheckPropertyValues();

	virtual const BOOL	ScanAddress(const DWORD dwAddress);
	virtual const BOOL	ScanSystemName(LPCTSTR lpszSystemName);
};
