#pragma once

#include "CMFCPropertySheetEx.h"

#include "CPageProject.h"
#include "CPageRestorationMethod.h"
#include "CPageRestorationConfirm.h"
#include "CPageRestorationSelect.h"
#include "CPageRestorationCompare.h"
#include "CPageRestorationRead.h"
#include "CPageRestorationSelectedMethod.h"

// CSheetRestoration

class CSheetRestoration : public CMFCPropertySheetEx
{
	DECLARE_DYNAMIC(CSheetRestoration)

public:
	CSheetRestoration();
	virtual ~CSheetRestoration();	

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	
	virtual BOOL OnInitDialog();

protected:
	CPageProject					m_pageProject;
	CPageRestorationMethod			m_pageRestorationMethod;
	CPageRestorationSelectedMethod	m_pageRestorationSelectMethod; // 사용X
	CPageRestorationConfirm			m_pageRestorationConfirm; // Compare 모드 사용 X
	CPageRestorationSelect			m_pageRestorationSelect; // 사용X
	CPageRestorationCompare			m_pageRestorationCompare;
	CPageRestorationRead			m_pageRestorationRead;

public:
	typedef enum _RESTORE_METHOD
	{
		method_nothing = -1,
		method_direct_restore = 0,
		method_compare_backup,
		method_compare_motor,
	}	RESTORE_METHOD;

protected:
	INT				m_nBackupID, m_nCompareID, m_nDataBaseMotorCount;
	CString			m_strProjectName, m_strComment, m_strBackupTimeStamp;
	CString			m_strCompareTimeStamp, m_strNominalTimeStamp, m_strRestoredTimeStamp;
	INT				m_nMethod, m_nNominalID, m_nRestoredID;
	BOOL			m_bNominalData, m_bAutoHomingMode;
	COleDateTime	m_timeParameter;	// Parameter를 읽었을 때의 시간을 비교하기 위해
	BOOL			m_bSystemCtrl;
	BOOL			m_bReading;

public:
	virtual void ChangeMethodSubPages();

public:
	LPCTSTR		GetProjectName();
	
	const INT	GetNominalBackupID();
	LPCTSTR		GetNominalTimeStamp();


	const INT	GetLatestRestoredID();
	LPCTSTR		GetLatestRestoredTimeStamp();
	const BOOL	IsBackupEqualRestoredID();

	void		SetProjectInfo(const INT nBackupID = -1, LPCTSTR lpszTimeStamp = NULL, LPCTSTR lpszComment = NULL);
	const INT	GetProjectBackupID();
	LPCTSTR		GetProjectComment();
	LPCTSTR		GetProjectTimeStamp();
	const BOOL	IsProjectNominalData();

	void		SetCompareBackupID(const INT nCompareID = -1, LPCTSTR lpszTimeStamp = NULL );
	const INT	GetCompareBackupID();
	LPCTSTR		GetCompareTimeStamp();

	void		SetMethod( const INT nMethod);
	const INT	GetMethod();

	void		EnableAutoHomingMode(const BOOL bEnable = TRUE);
	const BOOL	IsAutoHomingModeEnabled();

	void		CheckParameterReaded();
	const BOOL	IsParameterReaded();	
	void SetProjectName(CString strName);
	void SetSystemControl(BOOL bCtrl = FALSE);
	BOOL IsSystemControl();
	void InitPage();
	void SetDataBaseMotorCount(INT nMotor);
	INT GetDataBaseMotorCount();
	void SetDataBaseProjectName(CString strName);
	CString GetDataBaseProjectName();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	const BOOL StopCompare();
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	void SetTimeParameter(COleDateTime time);
	const BOOL CheckInCorrectCompareData();
	void SetReading(const BOOL bReading);
	const BOOL IsReading();
	const BOOL IsRequestFromRibbon();
	void RequestFromRibbon(BOOL bRequest);
	BOOL m_bRequestFromRibbon;
};
