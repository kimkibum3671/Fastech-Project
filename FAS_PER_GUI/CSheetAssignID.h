#pragma once

#include "CMFCPropertySheetEx.h"

#include "CPageProject.h"
#include "CPageAssignSelect.h"
#include "CPageAssignConfirm.h"

// CSheetAssignID

class CSheetAssignID : public CMFCPropertySheetEx
{
	DECLARE_DYNAMIC(CSheetAssignID)

public:
	CSheetAssignID();
	virtual ~CSheetAssignID();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CPageProject		m_pageProject;
	CPageAssignSelect	m_pageSelect;
	CPageAssignConfirm	m_pageConfirm;

protected:
	INT m_nSelectedMotorID;

public:
	void		SetSelectedMotorID(const INT nMotorID);
	const INT	GetSelectedMotorID();
};
