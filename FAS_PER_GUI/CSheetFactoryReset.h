#pragma once

#include "CMFCPropertySheetEx.h"

#include "CPageProject.h"
#include "CPageFactoryResetSelect.h"
#include "CPageFactoryResetConfirm.h"

// CSheetFactoryReset

class CSheetFactoryReset : public CMFCPropertySheetEx
{
	DECLARE_DYNAMIC(CSheetFactoryReset)

public:
	CSheetFactoryReset();
	virtual ~CSheetFactoryReset();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CPageProject				m_pageProject;
	CPageFactoryResetSelect		m_pageSelect;
	CPageFactoryResetConfirm	m_pageConfirm;

protected:
	INT m_nSelectedMotorID;

public:
	void		SetSelectedMotorID(const INT nMotorID);
	const INT	GetSelectedMotorID();
};
