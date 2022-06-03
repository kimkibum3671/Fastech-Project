#pragma once

#include "CMFCPropertySheetEx.h"
#include "CPageProject.h"
#include "CPageBackup.h"

// CSheetBackup

class CSheetBackup : public CMFCPropertySheetEx
{
	DECLARE_DYNAMIC(CSheetBackup)

public:
	CSheetBackup();
	virtual ~CSheetBackup();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CPageProject	m_pageProject;
	CPageBackup		m_pageBackup;

protected:
	CString			m_strComment;

public:
	void			SetComment(LPCTSTR lpszComment);
	LPCTSTR			GetComment();

protected:
	BOOL			m_bNominal;

public:
	void			EnableNominalBackup(const BOOL bEnable);
	const BOOL		IsNominalBackupEnabled();
	virtual BOOL OnInitDialog();
};
