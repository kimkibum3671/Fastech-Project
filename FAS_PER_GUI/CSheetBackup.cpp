// CSheetBackup.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetBackup.h"
// CSheetBackup

IMPLEMENT_DYNAMIC(CSheetBackup, CMFCPropertySheetEx)

CSheetBackup::CSheetBackup()
{
	SetLook(CMFCPropertySheet::PropSheetLook_OneNoteTabs);

	BOOL b32BitIcons = afxGlobalData.m_nBitsPerPixel < 16 ? FALSE : TRUE;
	SetImageList(IDB_SHEET32_BACKUP);

	m_bNominal = FALSE;

	AddPage(&m_pageProject);
	AddPage(&m_pageBackup);
}

CSheetBackup::~CSheetBackup()
{

}


BEGIN_MESSAGE_MAP(CSheetBackup, CMFCPropertySheet)
END_MESSAGE_MAP()

// CSheetBackup message handlers
void CSheetBackup::SetComment(LPCTSTR lpszComment)
{
	m_strComment = lpszComment;
}

LPCTSTR CSheetBackup::GetComment()
{
	return m_strComment;
}

void CSheetBackup::EnableNominalBackup(const BOOL bEnable)
{
	m_bNominal = bEnable;
}

const BOOL CSheetBackup::IsNominalBackupEnabled()
{
	return m_bNominal;
}


BOOL CSheetBackup::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheetEx::OnInitDialog();

	return bResult;
}
