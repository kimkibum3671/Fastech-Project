// CSheetFactoryReset.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetFactoryReset.h"


// CSheetFactoryReset

IMPLEMENT_DYNAMIC(CSheetFactoryReset, CMFCPropertySheetEx)

CSheetFactoryReset::CSheetFactoryReset()
{
	SetLook(CMFCPropertySheet::PropSheetLook_OneNoteTabs);

	SetImageList(IDB_SHEET32_RESTORATION0);

	AddPage(&m_pageProject);
	AddPage(&m_pageSelect);	

	m_nSelectedMotorID = -1;
}

CSheetFactoryReset::~CSheetFactoryReset()
{
}


BEGIN_MESSAGE_MAP(CSheetFactoryReset, CMFCPropertySheet)
END_MESSAGE_MAP()

// CSheetFactoryReset message handlers
void CSheetFactoryReset::SetSelectedMotorID(const INT nMotorID)
{
	if (-1 == m_nSelectedMotorID)
	{
		AddPage(&m_pageConfirm);
	}

	m_nSelectedMotorID = nMotorID;
}

const INT CSheetFactoryReset::GetSelectedMotorID()
{
	return m_nSelectedMotorID;
}
