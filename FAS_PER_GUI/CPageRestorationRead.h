#pragma once

#include "CMFCPropertyPageEx.h"

#include "CMFCButtonEx.h"
#include "ColorStaticST.h"

// CPageRestorationRead dialog

class CPageRestorationRead : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageRestorationRead)

public:
	CPageRestorationRead(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageRestorationRead();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_RESTORATION_READ };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();

protected:
	CMFCButtonEx	m_btnPrev, m_btnNext;
	CColorStaticST	m_wndNotice, m_wndProgress;
	
	COLORREF m_colorBkComplete, m_colorTxtComplete;
	BOOL	m_bRun, m_bShow;

	CFont m_BigFont;

protected:
	typedef struct BACKUP_STRUCT
	{
		INT nBackupID;
		CString strTimeStamp;
		CString strComment;
	};
	BACKUP_STRUCT m_SelectedProject, m_ProcessProject;
	typedef enum _TIMER_ID
	{
		TIMER_POLLING = 0,
		TIMER_WATING_STOP,
	}	TIMER_ID;

	typedef enum _TIMER_ELPASE
	{
		ELAPSE_TIMER_POLLING = 100,
	}	TIMER_ELPASE;

protected:
	void DisplayComplete();
public:
	const BOOL IsComparing();
	void StartPolling();
	const BOOL IsShow();
	virtual BOOL OnSetActive();
	void SetProjectInfo(const INT nBackupID, LPCTSTR lpszTimeStamp, LPCTSTR lpszComment);
	const BOOL StopCompare();
	void ActiveComparing();
	void GetProcessInfo(INT& nBackupID, CString& strTime, CString& strComment);
	void InitControlTextSize();
};
