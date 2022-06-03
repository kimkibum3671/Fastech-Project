#pragma once

#include "CViewDevice.h"

#include "CMFCListCtrlEX.h"

// CViewSchedule form view

class CViewSchedule : public CViewDevice
{
	DECLARE_DYNCREATE(CViewSchedule)

protected:
	CViewSchedule();           // protected constructor used by dynamic creation
	virtual ~CViewSchedule();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_SCHEDULE };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedChkSun();
	afx_msg void OnBnClickedChkMon();
	afx_msg void OnBnClickedChkTue();
	afx_msg void OnBnClickedChkWed();
	afx_msg void OnBnClickedChkThu();
	afx_msg void OnBnClickedChkFri();
	afx_msg void OnBnClickedChkSat();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	typedef enum _TIMER_ID
	{
		TIMER_INITIALIZE = 0,
	}	TIMER_ID;

	typedef enum _TIMER_ELPASE
	{
		ELAPSE_TIMER_INITIALIZE = 1,
	}	TIMER_ELPASE;

protected:
	void IntializeListCtrl();

	void DisplayRepeatCondition();
	void SetRepeatCode(const DWORD dwRepeatCode );
	const DWORD GetRepeatCode();
	void ConvertRepeatCodeToString(const DWORD dwRepeatCode, CString& strRepeatCode);
	void EnableEditControls(const BOOL bEnable);

	const INT FindBackupReservation(COleDateTime timeClock);
	const INT FindBackupReservation(COleDateTime timeDate, COleDateTime timeClock);

protected:
	CMFCListCtrlEx m_wndList;
};
