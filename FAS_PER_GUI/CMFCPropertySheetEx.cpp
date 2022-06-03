// CMFCPropertySheetEx.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CMFCPropertySheetEx.h"

#include "CDeviceList.h"
// CMFCPropertySheetEx

IMPLEMENT_DYNAMIC(CMFCPropertySheetEx, CMFCPropertySheet)

CMFCPropertySheetEx::CMFCPropertySheetEx()
{
	m_bShowWizardButton = FALSE;
	m_ptStart.x = 0;
	m_ptStart.y = 0;
	m_offset.cx = 0;
	m_offset.cy = 0;	
}

CMFCPropertySheetEx::~CMFCPropertySheetEx()
{

}


BEGIN_MESSAGE_MAP(CMFCPropertySheetEx, CMFCPropertySheet)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CMFCPropertySheetEx message handlers
BOOL CMFCPropertySheetEx::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheet::OnInitDialog();

	SetWindowText( m_strCaption);

	HICON icon = ::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(icon, FALSE);
	this->SetIcon(icon, TRUE);

	const INT nCmdShow = m_bShowWizardButton ? SW_SHOWNORMAL : SW_HIDE;
	
	ShowWizardButton(IDHELP, nCmdShow);
	ShowWizardButton(ID_APPLY_NOW, nCmdShow);
	ShowWizardButton(IDCANCEL, nCmdShow);
	ShowWizardButton(IDOK, nCmdShow);

	CRect rtApply, rtCancel;
	GetDlgItem(ID_APPLY_NOW)->GetWindowRect(rtApply);
	ScreenToClient(rtApply);
	m_ptStart = rtApply.TopLeft();
	m_offset.cx = rtApply.Width() + 6;


	return bResult;
}

const BOOL CMFCPropertySheetEx::SetImageList(const INT nResID, const INT cx, const COLORREF crMask)
{	
	if (m_Icons.GetSafeHandle() && m_Icons.GetImageCount())
	{
		if (FALSE == m_Icons.DeleteImageList())
		{
			return FALSE;
		}
	}

	if (m_Icons.Create(nResID, cx, 0, crMask))
	{
		return GetTab().SetImageList(m_Icons.m_hImageList);
	}
	
	return FALSE;
}

void CMFCPropertySheetEx::SetAddress(const DWORD dwAddress)
{
	m_dwAddress = dwAddress;
}

const DWORD CMFCPropertySheetEx::GetAddress()
{
	return m_dwAddress;
}

void CMFCPropertySheetEx::SetCaption(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;
}

LPCTSTR CMFCPropertySheetEx::GetCaption()
{
	return m_strCaption;
}

CDeviceMotionGate* CMFCPropertySheetEx::GetMotiongateCtrl()
{
	CDeviceMotionGate* pDevice = NULL;
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	pDeviceList->GetMotiongateByAddress(m_dwAddress, pDevice);

	return pDevice;
}


const BOOL CMFCPropertySheetEx::SetActiveSafePage(const INT nPage)
{
	return this->SetActivePage(nPage);
}

void CMFCPropertySheetEx::AddSubPage(CMFCPropertyPage* pPage)
{
	if (FALSE == pPage->GetSafeHwnd())
	{
		AddPage(pPage);
	}
}

void CMFCPropertySheetEx::RemoveSubPages(const INT nDelimeter)
{
	const INT nPageCount = GetPageCount();

	CMFCTabCtrl& tabCtrl = GetTab();

	for (INT nIndex = nPageCount; nDelimeter < nIndex; nIndex--)
	{
		const INT nPageIndex = nIndex - 1;
		RemovePage(nPageIndex);
		tabCtrl.RemoveTab(nPageIndex, FALSE);
	}

	tabCtrl.RecalcLayout();
}


BOOL CMFCPropertySheetEx::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if( VK_RETURN == pMsg->wParam )
		{
			return TRUE;
		}
	}
	return CMFCPropertySheet::PreTranslateMessage(pMsg);
}

const BOOL CMFCPropertySheetEx::ShowWizardButton(const INT nResID, const INT nCmdShow)
{
	CWnd* pWnd = GetDlgItem(nResID);
	if (FALSE == pWnd->GetSafeHwnd())
	{
		return FALSE;
	}

	return pWnd->ShowWindow(nCmdShow);
}

const BOOL CMFCPropertySheetEx::DestroyWizardButton(const INT nResID)
{
	CWnd* pWnd = GetDlgItem(nResID);
	if (FALSE == pWnd->GetSafeHwnd())
	{
		return FALSE;
	}

	return pWnd->DestroyWindow();
}

void CMFCPropertySheetEx::ShowWizardButtons(const BOOL bShow )
{
	m_bShowWizardButton = bShow;
}

const BOOL CMFCPropertySheetEx::IsWizardButtonsVisible()
{
	return m_bShowWizardButton;
}

void CMFCPropertySheetEx::RecalculateWizardButton()
{
	CPoint ptButton = m_ptStart;

	if( 1 )
	{
		CWnd* pWnd = GetDlgItem(ID_APPLY_NOW);
		if (pWnd && pWnd->IsWindowEnabled())
		{
			pWnd->SetWindowPos(&wndTop, ptButton.x, ptButton.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			ptButton.x -= m_offset.cx;
			pWnd->ShowWindow(FALSE);
		}
	}

	if (1)
	{
		CWnd* pWnd = GetDlgItem(IDCANCEL);
		if (pWnd && pWnd->IsWindowEnabled())
		{
			pWnd->SetWindowPos(&wndTop, ptButton.x, ptButton.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			ptButton.x -= m_offset.cx;
		}
	}

	if (1)
	{
		CWnd* pWnd = GetDlgItem(IDOK);
		if (pWnd && pWnd->IsWindowEnabled())
		{
			pWnd->SetWindowPos(&wndTop, ptButton.x, ptButton.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			ptButton.x -= m_offset.cx;
		}
	}
}


void CMFCPropertySheetEx::InitTabTextSize()
{
	/*CFont font;
	
	// getlogfont

	/*CTabCtrl* pTab = GetTabControl();
	pTab->SetFont(&font);*/
	//CFont font;
	
	CFont font;

	LOGFONT lf = { 0, };
	afxGlobalData.fontRegular.GetLogFont(&lf);

	lf.lfHeight = -23;
	font.CreateFontIndirect(&lf);

	//m_wndTab.SetFont(&font);

	/*
	if (true)
	{
		CTabCtrl* ptab = GetTabControl();
		//font.CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);
		ptab->SetFont(&font);
		font.Detach();
	}
	*/
	

	/*
	if (true)
	{
		//font.CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);
		CMFCTabCtrl& tabCtrl = GetTab();
		tabCtrl.SetFont(&font);
		font.Detach();
	}
	*/
	
	
	

	//if (true)
	//{
	//	//font.CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);
	//	m_wndTab.SetFont(&font);
	//	font.Detach();
	//}


}


HRESULT CMFCPropertySheetEx::accDoDefaultAction(VARIANT varChild)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CMFCPropertySheet::accDoDefaultAction(varChild);
}


int CMFCPropertySheetEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
