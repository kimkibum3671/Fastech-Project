// CSheetRestoration.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CSheetRestoration.h"

#include "CDeviceList.h"
#include "CDataBaseManager.h"
#include "MainFrm.h"

// CSheetRestoration

IMPLEMENT_DYNAMIC(CSheetRestoration, CMFCPropertySheetEx)

CSheetRestoration::CSheetRestoration() :
	m_timeParameter(COleDateTime::GetCurrentTime())
{
	SetLook(CMFCPropertySheet::PropSheetLook_OneNoteTabs);

	BOOL b32BitIcons = afxGlobalData.m_nBitsPerPixel < 16 ? FALSE : TRUE;
	SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION0 : IDB_SHEET32_RESTORATION0, 32, RGB(0xc0, 0xc0, 0xc0));

	InitPage();

	m_nMethod = method_nothing;

	m_nNominalID = -1;
	m_nRestoredID = -1;
	m_nCompareID = -1;
	m_nDataBaseMotorCount = 0;
	m_bReading = FALSE;
	m_bRequestFromRibbon = FALSE;
}

CSheetRestoration::~CSheetRestoration()
{

}


BEGIN_MESSAGE_MAP(CSheetRestoration, CMFCPropertySheetEx)
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()



// CSheetRestoration message handlers
LPCTSTR	CSheetRestoration::GetProjectName()
{
	return m_strProjectName;
}

const INT	CSheetRestoration::GetNominalBackupID()
{
	return m_nNominalID;
}

LPCTSTR CSheetRestoration::GetNominalTimeStamp()
{
	return m_strNominalTimeStamp;
}

const INT	CSheetRestoration::GetLatestRestoredID()
{
	return m_nRestoredID;
}

LPCTSTR CSheetRestoration::GetLatestRestoredTimeStamp()
{
	return m_strRestoredTimeStamp;
}

const BOOL	CSheetRestoration::IsBackupEqualRestoredID()
{
	return m_nBackupID == m_nRestoredID;
}

void CSheetRestoration::SetProjectInfo(const INT nBackupID, LPCTSTR lpszTimeStamp, LPCTSTR lpszComment)
{
	m_nBackupID = nBackupID;
	m_strBackupTimeStamp = lpszTimeStamp;
	m_strComment = lpszComment;	
	m_bNominalData = (-1 != nBackupID) ? m_nNominalID == nBackupID : FALSE;

	ChangeMethodSubPages();
}

const INT CSheetRestoration::GetProjectBackupID()
{
	return m_nBackupID;
}

LPCTSTR CSheetRestoration::GetProjectComment()
{
	return m_strComment;
}

LPCTSTR CSheetRestoration::GetProjectTimeStamp()
{
	return m_strBackupTimeStamp;
}

const BOOL CSheetRestoration::IsProjectNominalData()
{
	return m_bNominalData;
}

void CSheetRestoration::SetCompareBackupID(const INT nCompareID, LPCTSTR lpszTimeStamp)
{
	m_nCompareID = nCompareID;
	m_strCompareTimeStamp = lpszTimeStamp;

	ChangeMethodSubPages();
}

const INT CSheetRestoration::GetCompareBackupID()
{
	return m_nCompareID;
}

LPCTSTR CSheetRestoration::GetCompareTimeStamp()
{
	return m_strCompareTimeStamp;
}

void CSheetRestoration::SetMethod(const INT nMethod)
{
	if (m_nMethod != nMethod ||
		nMethod == 1)
	{
		if (method_compare_motor == m_nMethod &&
			method_compare_motor != nMethod)
		{
			if (TRUE == IsReading())
			{
				GetMotiongateCtrl()->StopExecution();
			}
			
		}

		m_nMethod = nMethod;

		this->ChangeMethodSubPages();
	}
}

const INT CSheetRestoration::GetMethod()
{
	return m_nMethod;
}

void CSheetRestoration::EnableAutoHomingMode(const BOOL bEnable)
{
	m_bAutoHomingMode = bEnable;
}

const BOOL CSheetRestoration::IsAutoHomingModeEnabled()
{
	return m_bAutoHomingMode;
}


BOOL CSheetRestoration::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheetEx::OnInitDialog();

	CDeviceMotionGate* pDevice = NULL;
	if (CDeviceList::GetInstance()->GetMotiongateByAddress(m_dwAddress, pDevice) && FALSE == IsSystemControl())
	{
		SetProjectName(pDevice->GetSystemName());
	}

	m_bAutoHomingMode = FALSE;

	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	CString strSystemName(IsSystemControl() ? GetProjectName() : pDevice->GetSystemName());

	pProjectDB->GetNominalBackupInfo(strSystemName, &m_nNominalID, &m_strNominalTimeStamp);
	pProjectDB->GetLatestRestoredInfo(strSystemName, &m_nRestoredID, &m_strRestoredTimeStamp);
	m_bNominalData = FALSE;

	m_nCompareID = GetNominalBackupID();

	m_pageRestorationMethod.SetNominalID(m_nNominalID);

	return bResult;
}

void CSheetRestoration::ChangeMethodSubPages()
{
	RemoveSubPages(GetActiveIndex() + 1);
	if (-1 == m_nBackupID)
	{
		return;
	}

	BOOL b32BitIcons = afxGlobalData.m_nBitsPerPixel < 16 ? FALSE : TRUE;

	m_pageRestorationConfirm.m_bCompareType = true;

	
	switch (m_nMethod)
	{
	case method_direct_restore:
	{	
		SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION0 : IDB_SHEET32_RESTORATION0, 32, RGB(0xc0, 0xc0, 0xc0));

		AddSubPage(&m_pageRestorationConfirm);
	}	break;

	case method_compare_backup:
	{
		SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION1 : IDB_SHEET32_RESTORATION1, 32, RGB(0xc0, 0xc0, 0xc0));

		if ( -1 != m_nCompareID && m_nBackupID != m_nCompareID || IsSystemControl()/* 추가*/)
		{
			AddSubPage(&m_pageRestorationCompare);
//			AddSubPage(&m_pageRestorationConfirm);
		}
 	}	break;

	case method_compare_motor:
	{
		SetImageList(b32BitIcons ? IDB_SHEET32_RESTORATION2 : IDB_SHEET32_RESTORATION2, 32, RGB(0xc0, 0xc0, 0xc0));
		AddSubPage(&m_pageRestorationRead);
		CheckParameterReaded();
		
		/*if (!m_pageRestorationRead.IsShow())
		{
			AddSubPage(&m_pageRestorationRead);
		}
		else
		{

		}
		CheckParameterReaded();*/

		/*if (CheckInCorrectCompareData())
		{
			AddSubPage(&m_pageRestorationRead);
			m_pageRestorationRead.SetProjectInfo(GetProjectBackupID(), GetProjectTimeStamp(), GetProjectComment());
			StopCompare();
			m_timeParameter = COleDateTime::GetCurrentTime();
		}*/
		
	}	break;
	                   
	default:
	{

	}	break;
	}
}

const BOOL CSheetRestoration::IsParameterReaded()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	if( NULL == pDevice)
	{
		return FALSE;
	}

	return m_timeParameter <= pDevice->GetLastReadParameter();
}

void CSheetRestoration::CheckParameterReaded()
{
	if (IsParameterReaded())
	{
		if (-1 == GetPageIndex(&m_pageRestorationCompare))
		{
			AddSubPage(&m_pageRestorationCompare);
		}

	/*	if (-1 == GetPageIndex(&m_pageRestorationConfirm) )
		{
			AddSubPage(&m_pageRestorationConfirm);
		}*/
	}
}


void CSheetRestoration::OnDestroy()
{
	CMFCPropertySheetEx::OnDestroy();
}


void CSheetRestoration::SetProjectName(CString strName)
{
	m_strProjectName = strName;
}


void CSheetRestoration::SetSystemControl(BOOL bCtrl)
{
	m_bSystemCtrl = bCtrl;
}


BOOL CSheetRestoration::IsSystemControl()
{
	m_bSystemCtrl = ((CMainFrame*)AfxGetMainWnd())->IsSystemCompare();
	return m_bSystemCtrl;
}


void CSheetRestoration::InitPage()
{
	BOOL bSystemCtrl = ((CMainFrame*)AfxGetMainWnd())->IsSystemCompare();
	m_pageProject.SetSystemControl(bSystemCtrl);

	AddPage(&m_pageProject);
	AddPage(&m_pageRestorationMethod);
}


void CSheetRestoration::SetDataBaseMotorCount(INT nMotor)
{
	m_nDataBaseMotorCount = nMotor;
}


INT CSheetRestoration::GetDataBaseMotorCount()
{
	return m_nDataBaseMotorCount;
}


void CSheetRestoration::SetDataBaseProjectName(CString strName)
{
	// TODO: 여기에 구현 코드 추가.
}


CString CSheetRestoration::GetDataBaseProjectName()
{
	// TODO: 여기에 구현 코드 추가.
	return CString();
}


BOOL CSheetRestoration::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CMFCPropertySheetEx::OnCommand(wParam, lParam);
}


const BOOL CSheetRestoration::StopCompare()
{
	CDeviceMotionGate* pDevice = GetMotiongateCtrl();

	if (NULL == pDevice)
	{
		return FALSE;
	}

	if (pDevice->IsBackupState())
	{
		pDevice->StopExecution();
		SetReading(FALSE);
	}

	return TRUE;
}


//BOOL CSheetRestoration::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//
//	switch (pMsg->message)
//	{
//	case WM_LBUTTONDOWN:
//	{
//		TRACE(_T("BUTTON DOWN!! \r\n"));
//	}	break;
//
//	case TVN_SELCHANGED:
//	{
//		TRACE(_T("TVN_SELCHANGED !! \r\n"));
//	}	break;
//
//	default:
//		break;
//	}
//
//	return CMFCPropertySheetEx::PreTranslateMessage(pMsg);
//}


//void CSheetRestoration::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
//{
//	CMFCPropertySheetEx::OnActivate(nState, pWndOther, bMinimized);
//
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//}


void CSheetRestoration::SetTimeParameter(COleDateTime time)
{
	m_timeParameter = time;
}


const BOOL CSheetRestoration::CheckInCorrectCompareData()
{
	BOOL bResult = FALSE;
	INT nBackupID;
	CString strTime;
	CString strComment;

	m_pageRestorationRead.GetProcessInfo(nBackupID, strTime, strComment);
	
	if (m_nBackupID != nBackupID ||
		m_strBackupTimeStamp != strTime ||
		m_strComment != strComment)
	{
		bResult = TRUE;
	}

	return bResult;
}


void CSheetRestoration::SetReading(const BOOL bReading)
{
	m_bReading = bReading;
}


const BOOL CSheetRestoration::IsReading()
{
	return m_bReading;
}


const BOOL CSheetRestoration::IsRequestFromRibbon()
{ // 사용 X
	return m_bRequestFromRibbon;
}


void CSheetRestoration::RequestFromRibbon(BOOL bRequest)
{ // 사용 X
	m_bRequestFromRibbon = bRequest;
}
