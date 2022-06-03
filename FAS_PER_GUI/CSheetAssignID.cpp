// CSheetAssignID.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetAssignID.h"


// CSheetAssignID

IMPLEMENT_DYNAMIC(CSheetAssignID, CMFCPropertySheetEx)

CSheetAssignID::CSheetAssignID()
{
	SetLook(CMFCPropertySheet::PropSheetLook_OneNoteTabs);

	BOOL b32BitIcons = afxGlobalData.m_nBitsPerPixel < 16 ? FALSE : TRUE;
	SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION0 : IDB_SHEET32_RESTORATION0, 32, RGB(0xc0, 0xc0, 0xc0));

	AddPage(&m_pageProject);
	AddPage(&m_pageSelect);

	m_nSelectedMotorID = -1;
}

CSheetAssignID::~CSheetAssignID()
{

}


BEGIN_MESSAGE_MAP(CSheetAssignID, CMFCPropertySheet)
END_MESSAGE_MAP()

// CSheetAssignID message handlers
void CSheetAssignID::SetSelectedMotorID(const INT nMotorID)
{
	if (-1 == m_nSelectedMotorID)
	{
		AddPage(&m_pageConfirm);
	}

	m_nSelectedMotorID = nMotorID;
}

const INT CSheetAssignID::GetSelectedMotorID()
{
	return m_nSelectedMotorID;
}
