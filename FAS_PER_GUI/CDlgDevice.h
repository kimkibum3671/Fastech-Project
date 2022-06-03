#pragma once

#include "CDeviceMotionGate.h"

// CDlgDevice dialog

class CDlgDevice : public CDialog
{
	DECLARE_DYNAMIC(CDlgDevice)

public:
	CDlgDevice();   // standard constructor
	virtual ~CDlgDevice();

public:
	explicit CDlgDevice(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	explicit CDlgDevice(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = 0 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	DWORD m_dwAddress;

protected:
	CDeviceMotionGate* GetMotiongateCtrl();
	const BOOL CheckNameMatched(CDeviceMotionGate* pDevice);

	void		SetRadioIndex(const INT* pResArray, const INT nIndex, const INT cbArray = -1);
	const INT	GetRadioIndex(const INT* pResArray, const INT cbArray = -1);

public:
	void		SetAddress(const DWORD dwAddress);
	const DWORD	GetAddress();
};
