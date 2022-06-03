#pragma once

#include "CMFCPropertyPageEx.h"

#include "CPageProject.h"
#include "CPageRestorationMethod.h"
#include "CPageRestorationConfirm.h"
#include "CPageRestorationSelect.h"
#include "CPageRestorationCompare.h"
#include "CPageRestorationRead.h"
#include "CPageRestorationSelectedMethod.h"
// CSheetSelectRestoration

class CSheetSelectRestoration : public CMFCPropertySheetEx
{
	DECLARE_DYNAMIC(CSheetSelectRestoration)

public:
	CSheetSelectRestoration();
	virtual ~CSheetSelectRestoration();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();

	virtual BOOL OnInitDialog();

protected:
	CPageProject					m_pageProject;
	CPageRestorationSelectedMethod	m_pageRestorationSelectMethod; // ���� ȭ��
	CPageRestorationConfirm			m_pageRestorationConfirm; // Information about restoration
	
	//CPageRestorationSelect			m_pageRestorationSelect;
	//CPageRestorationCompare			m_pageRestorationCompare;
	//CPageRestorationRead			m_pageRestorationRead;

public:
	typedef enum _RESTORE_METHOD
	{
		method_nothing = -1,
		method_direct_restore = 0,
		method_compare_backup,
		method_compare_motor,
	}	RESTORE_METHOD;

protected:
	INT				m_nBackupID, m_nCompareID;
	CString			m_strProjectName, m_strComment, m_strBackupTimeStamp;
	CString			m_strCompareTimeStamp, m_strNominalTimeStamp, m_strRestoredTimeStamp;
	INT				m_nMethod, m_nNominalID, m_nRestoredID;
	BOOL			m_bNominalData, m_bAutoHomingMode;
	COleDateTime	m_timeParameter;	// Parameter�� �о��� ���� �ð��� ���ϱ� ����

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

	void		SetCompareBackupID(const INT nCompareID = -1, LPCTSTR lpszTimeStamp = NULL);
	const INT	GetCompareBackupID();
	LPCTSTR		GetCompareTimeStamp();

	void		SetMethod(const INT nMethod);
	const INT	GetMethod();

	void		EnableAutoHomingMode(const BOOL bEnable = TRUE);
	const BOOL	IsAutoHomingModeEnabled();

	void		CheckParameterReaded();
	const BOOL	IsParameterReaded();
};


