// CSheetSelectRestoration.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetSelectRestoration.h"

#include "CDeviceList.h"
#include "CDataBaseManager.h"

// CSheetSelectRestoration

IMPLEMENT_DYNAMIC(CSheetSelectRestoration, CMFCPropertySheetEx)

CSheetSelectRestoration::CSheetSelectRestoration() :
	m_timeParameter(COleDateTime::GetCurrentTime())
{
	SetLook(CMFCPropertySheet::PropSheetLook_OneNoteTabs);

	BOOL b32BitIcons = afxGlobalData.m_nBitsPerPixel < 16 ? FALSE : TRUE;
	SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION0 : IDB_SHEET32_RESTORATION0, 32, RGB(0xc0, 0xc0, 0xc0));

	AddPage(&m_pageProject);
	AddPage(&m_pageRestorationSelectMethod);
	
	m_nMethod = method_nothing;

	m_nNominalID = -1;
	m_nRestoredID = -1;
	m_nCompareID = -1;
}

CSheetSelectRestoration::~CSheetSelectRestoration()
{

}


BEGIN_MESSAGE_MAP(CSheetSelectRestoration, CMFCPropertySheetEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CSheetSelectRestoration message handlers
LPCTSTR	CSheetSelectRestoration::GetProjectName()
{
	return m_strProjectName;
}

const INT	CSheetSelectRestoration::GetNominalBackupID()
{
	return m_nNominalID;
}

LPCTSTR CSheetSelectRestoration::GetNominalTimeStamp()
{
	return m_strNominalTimeStamp;
}

const INT	CSheetSelectRestoration::GetLatestRestoredID()
{
	return m_nRestoredID;
}

LPCTSTR CSheetSelectRestoration::GetLatestRestoredTimeStamp()
{
	return m_strRestoredTimeStamp;
}

const BOOL	CSheetSelectRestoration::IsBackupEqualRestoredID()
{
	return m_nBackupID == m_nRestoredID;
}

void CSheetSelectRestoration::SetProjectInfo(const INT nBackupID, LPCTSTR lpszTimeStamp, LPCTSTR lpszComment)
{
	m_nBackupID = nBackupID;
	m_strBackupTimeStamp = lpszTimeStamp;
	m_strComment = lpszComment;
	m_bNominalData = (-1 != nBackupID) ? m_nNominalID == nBackupID : FALSE;

	ChangeMethodSubPages();
}

const INT CSheetSelectRestoration::GetProjectBackupID()
{
	return m_nBackupID;
}

LPCTSTR CSheetSelectRestoration::GetProjectComment()
{
	return m_strComment;
}

LPCTSTR CSheetSelectRestoration::GetProjectTimeStamp()
{
	return m_strBackupTimeStamp;
}

const BOOL CSheetSelectRestoration::IsProjectNominalData()
{
	return m_bNominalData;
}

void CSheetSelectRestoration::SetCompareBackupID(const INT nCompareID, LPCTSTR lpszTimeStamp)
{
	m_nCompareID = nCompareID;
	m_strCompareTimeStamp = lpszTimeStamp;

	ChangeMethodSubPages();
}

const INT CSheetSelectRestoration::GetCompareBackupID()
{
	return m_nCompareID;
}

LPCTSTR CSheetSelectRestoration::GetCompareTimeStamp()
{
	return m_strCompareTimeStamp;
}

void CSheetSelectRestoration::SetMethod(const INT nMethod)
{
	if (m_nMethod != nMethod)
	{
		if (method_compare_motor == m_nMethod &&
			method_compare_motor != nMethod)
		{
			GetMotiongateCtrl()->StopExecution();
		}

		m_nMethod = nMethod;

		this->ChangeMethodSubPages();
	}
}

const INT CSheetSelectRestoration::GetMethod()
{
	return m_nMethod;
}

void CSheetSelectRestoration::EnableAutoHomingMode(const BOOL bEnable)
{
	m_bAutoHomingMode = bEnable;
}

const BOOL CSheetSelectRestoration::IsAutoHomingModeEnabled()
{
	return m_bAutoHomingMode;
}


BOOL CSheetSelectRestoration::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheetEx::OnInitDialog();

	CDeviceMotionGate* pDevice = NULL;
	if (CDeviceList::GetInstance()->GetMotiongateByAddress(m_dwAddress, pDevice))
	{
		m_strProjectName = pDevice->GetSystemName();
	}

	m_bAutoHomingMode = FALSE;

	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	CString strAddress;	pDevice->GetAddress(strAddress);
	/*기존..
	pProjectDB->GetNominalBackupInfo(strAddress, &m_nNominalID, &m_strNominalTimeStamp);
	pProjectDB->GetLatestRestoredInfo(strAddress, &m_nRestoredID, &m_strRestoredTimeStamp);
	*/
	
	pProjectDB->GetNominalBackupInfo(m_strProjectName, &m_nNominalID, &m_strNominalTimeStamp);
	pProjectDB->GetLatestRestoredInfo(m_strProjectName, &m_nRestoredID, &m_strRestoredTimeStamp);
	m_bNominalData = FALSE;
	
	return bResult;
}

void CSheetSelectRestoration::ChangeMethodSubPages()
{
	RemoveSubPages(GetActiveIndex() + 1);
	if (-1 == m_nBackupID)
	{


		return;
	}

	BOOL b32BitIcons = afxGlobalData.m_nBitsPerPixel < 16 ? FALSE : TRUE;

	m_pageRestorationConfirm.m_bCompareType = false;

	switch (m_nMethod)
	{
	case method_direct_restore:
	{
		SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION0 : IDB_SHEET32_RESTORATION0, 32, RGB(0xc0, 0xc0, 0xc0));
		
		AddSubPage(&m_pageRestorationConfirm);
	}	break;

	/*case method_compare_backup:
	{
		SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION1 : IDB_SHEET32_RESTORATION1, 32, RGB(0xc0, 0xc0, 0xc0));

		AddSubPage(&m_pageRestorationSelect);

		if (-1 != m_nCompareID && m_nBackupID != m_nCompareID)
		{
			AddSubPage(&m_pageRestorationCompare);
			AddSubPage(&m_pageRestorationConfirm);
		}
	}	break;

	case method_compare_motor:
	{
		SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION2 : IDB_SHEET32_RESTORATION2, 32, RGB(0xc0, 0xc0, 0xc0));

		AddSubPage(&m_pageRestorationRead);

		CheckParameterReaded();
	}	break;*/

	default:
	{

	}	break;
	}
}

const BOOL CSheetSelectRestoration::IsParameterReaded()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	if (NULL == pDevice)
	{
		return FALSE;
	}

	return m_timeParameter <= pDevice->GetLastReadParameter();
}

void CSheetSelectRestoration::CheckParameterReaded()
{
	if (IsParameterReaded())
	{
		/*if (-1 == GetPageIndex(&m_pageRestorationCompare))
		{
			AddSubPage(&m_pageRestorationCompare);
		}

		if (-1 == GetPageIndex(&m_pageRestorationConfirm))
		{
			AddSubPage(&m_pageRestorationConfirm);
		}*/
	}
}


void CSheetSelectRestoration::OnDestroy()
{
	CMFCPropertySheetEx::OnDestroy();
}
