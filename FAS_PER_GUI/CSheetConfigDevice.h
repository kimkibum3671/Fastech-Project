#pragma once

#include "CMFCPropertySheetEx.h"

// CSheetConfigDevice

class CSheetConfigDevice : public CMFCPropertySheetEx
{
	DECLARE_DYNAMIC(CSheetConfigDevice)

public:
	CSheetConfigDevice();
	virtual ~CSheetConfigDevice();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CDeviceMotionGate* m_pDevice;

protected:
	virtual void Initialize(CDeviceMotionGate* pDevice = NULL);
	virtual BOOL OnInitDialog();

public:
	virtual void				SetMotiongateCtrl(CDeviceMotionGate* pDevice);
	virtual CDeviceMotionGate*	GetMotiongateCtrl();
	
public:
	typedef enum _CHECK_RESULT_CODE
	{
		result_OK = 0,
		result_error_invalid_address,
		result_error_exist_address,
		result_error_invalid_name,
		result_error_exist_name,
	}	CHECK_RESULT_CODE;

public:
	virtual const INT	CheckPropertyValues();

	virtual void		SetEditAddress(const DWORD dwAddrss, CMFCPropertyPage* pPage = NULL);
	virtual const DWORD	GetEditAddress(CMFCPropertyPage* pPage = NULL);

	virtual void		SetEditSystemName(LPCTSTR lpszSystemName, CMFCPropertyPage* pPage = NULL);
	virtual LPCTSTR		GetEditSystemName(CMFCPropertyPage* pPage = NULL);

	virtual void		SetEditMotorCount(const INT nCount, CMFCPropertyPage* pPage = NULL);
	virtual const INT	GetEditMotorCount(CMFCPropertyPage* pPage = NULL);

	virtual void		SetEditHomingTimeout(const INT nTimeout, const BOOL bSecond = TRUE, CMFCPropertyPage* pPage = NULL);
	virtual const INT	GetEditHomingTimeout(const BOOL bSecond = TRUE, CMFCPropertyPage* pPage = NULL);

	virtual HBITMAP	SetEditSystemImage(HBITMAP hBitmap, CMFCPropertyPage* pPage = NULL);
	virtual HBITMAP	GetEditSystemImage(CMFCPropertyPage* pPage = NULL);

	virtual void SetEditDeviceID(INT nDeviceID, CMFCPropertyPage* pPage = NULL);
	virtual const INT GetEditDeviceID(CMFCPropertyPage* pPage = NULL);
	
};
