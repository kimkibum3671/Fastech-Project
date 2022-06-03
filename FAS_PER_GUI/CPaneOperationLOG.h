#pragma once

// CPaneOperationLOG

#include "CMFCListCtrlEX.h"
#include "ReferD.h"
#include <stdio.h>
#include <time.h>

class CLogData
{
public:
	CLogData( LPVOID pParam);
	~CLogData();

protected:
	INT			m_nMsgType;
	SYSTEMTIME	m_timeStamp;
	CString		m_strAddress;
	CString		m_strName;
	CString		m_strMessage;

public:
	void GetTimeStamp(CString& strTimeStamp);
	void GetDeviceInformation(CString& strDeviceInfo);
	void GetInformation(CString& strInformation);
};

class CPaneOperationLog : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneOperationLog)

public:
	CPaneOperationLog();
	virtual ~CPaneOperationLog();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMFCToolBar m_toolbar;//CPaneToolbar 
	//CToolBar m_toolbar;
	CMFCListCtrlEx m_listCtrl;

	CList<CLogData*, CLogData*> m_lstLogData;
protected:
	void InsertMessageToListCtrl(LPVOID pParam);

public:
	typedef enum _TIMER_TYPE
	{
		TIMER_REALL = 0,
		TIMER_SELECT
	}	_TIMER_TYPE;

	typedef struct _TIMER_STATUS
	{
		BOOL bReAllList;
		BOOL bSelectList;
	}	TIMER_STATUS;

	TIMER_STATUS m_timer;

	//typedef enum _LOG_STATUS
	//{
	//	// Filtering Status
	//	LOG_ALL = 0,
	//	LOG_WARNING,
	//	LOG_ERROR,
	//	// Show Current Device 
	//	LOG_SELECT,
	//	// Show Device Name, IP, Name + IP
	//	LOG_NAME,
	//	LOG_IP,
	//	LOG_MIX,
	//	// Tree Select
	//	LOG_TREE_SELECT
	//}	LOG_STATUS;

	//typedef struct _OPERATIONLOG_INFORMATION
	//{	//	Log DB에 저장할 내용
	//	INT		nLevel;
	//	CString strTime;
	//	CString strDeviceName;
	//	CString strDeviceIP;
	//	CString strInform;
	//}	OPERATIONLOG_INFORMATION, *PST_OPERATIONLOG_INFORMATION;

	CList<OPERATIONLOG_INFORMATION, OPERATIONLOG_INFORMATION&> m_lstSystemLog;

	OPERATIONLOG_INFORMATION m_opLog;
	void InsertMessage(LPVOID pParam);	// Parameter is CMainFrame::PST_OPERAION_MESSAGE
public:
	POSITION m_pos;
	CTime m_time;
	const BOOL SaveLogList(INT nLevel, CString strTime,	CString strDeviceName,	CString strDeviceIP, CString strInform, CTime time);
	//const BOOL SaveLogListTest(OPERATIONLOG_INFORMATION inform);
	const BOOL ShowLogList();
	void SetCurrentLogStatus(INT nRequest);
	void SetReShowAllLogList();
	INT m_nLogStatus;
	INT m_nLogDeviceNaming;
	BOOL m_nLogSelect;
	void SetReShowSelectedLogList();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void SaveLogData();
	void ReShowAllLogList();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ReShowSelectedLogList();
	void RemoveList();
	void SetLogTime();
};
