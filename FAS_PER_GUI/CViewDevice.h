#pragma once

// CViewDevice form view

#include "CDeviceMotionGate.h"

class CViewDevice : public CFormView
{
	DECLARE_DYNAMIC(CViewDevice)

protected:
	CViewDevice(LPCTSTR lpszTemplateName);
	CViewDevice(UINT nIDTemplate);

	virtual ~CViewDevice();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CDeviceMotionGate* GetMotiongateCtrl();

	void DestroyDeviceWindow(CFormView* pExceptionView = NULL);

	const BOOL CheckNameMatched(CDeviceMotionGate* pDevice);

public:
	virtual void PrepareDestroy();

	void		SetRadioIndex(const INT* pResArray, const INT nIndex, const INT cbArray = -1);
	const INT	GetRadioIndex(const INT* pResArray, const INT cbArray = -1);
	virtual void OnInitialUpdate();
	virtual void PostNcDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
