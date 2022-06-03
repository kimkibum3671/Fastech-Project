// CMFCCaptionBarEx.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CMFCCaptionBarEx.h"


// CMFCCaptionBarEx

IMPLEMENT_DYNAMIC(CMFCCaptionBarEx, CMFCCaptionBar)

CMFCCaptionBarEx::CMFCCaptionBarEx()
{

}

CMFCCaptionBarEx::~CMFCCaptionBarEx()
{
}


BEGIN_MESSAGE_MAP(CMFCCaptionBarEx, CMFCCaptionBar)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMFCCaptionBarEx message handlers
void CMFCCaptionBarEx::RecalcLayout()
{
	__super::RecalcLayout();

	// 없을 수는 있는데 크기나 위치를 바꿀 수 없다.
	m_rectClose.SetRectEmpty();
}

BOOL CMFCCaptionBarEx::CreateEx(DWORD dwStyle, CWnd* pParentWnd, UINT uID, int nHeight, BOOL bIsMessageBarMode)
{
	ENSURE(AfxIsExtendedFrameClass(pParentWnd) || pParentWnd->IsKindOf(RUNTIME_CLASS(CDialog)));

	SetPaneStyle(CBRS_ALIGN_BOTTOM);
	m_nDefaultHeight = nHeight;

	if (bIsMessageBarMode)
	{
		m_dwStyle |= CBRS_HIDE_INPLACE;
	}

	if (!CPane::Create(NULL, dwStyle, CRect(0, 0, 0, 0), pParentWnd, uID, 0))
	{
		return FALSE;
	}

	// register with parent frames' dock manager!!!
	if (pParentWnd->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
	{
		((CFrameWndEx*)pParentWnd)->AddPane(this);
	}
	else if (pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		((CMDIFrameWndEx*)pParentWnd)->AddPane(this);
	}
	else if (pParentWnd->IsKindOf(RUNTIME_CLASS(COleIPFrameWndEx)))
	{
		((COleIPFrameWndEx*)pParentWnd)->AddPane(this);
	}
	else if (pParentWnd->IsKindOf(RUNTIME_CLASS(COleDocIPFrameWndEx)))
	{
		((COleIPFrameWndEx*)pParentWnd)->AddPane(this);
	}
	else if (pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWndEx)))
	{
		((CMDIChildWndEx*)pParentWnd)->AddPane(this);
	}
	else if (pParentWnd->IsKindOf(RUNTIME_CLASS(CDialog)))
	{
		if (pParentWnd->GetSafeHwnd() == AfxGetMainWnd()->GetSafeHwnd())
		{
			afxGlobalUtils.m_bDialogApp = TRUE;
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	m_bIsMessageBarMode = bIsMessageBarMode;

	if (m_bIsMessageBarMode)
	{
		m_nBorderSize = 0;
	}

	return TRUE;
}

void CMFCCaptionBarEx::SetIcon(LPCTSTR lpIconName, COLORREF clrTransparent, BOOL bStretch, BarElementAlignment bmpAlignment)
{
	HICON hIcon = LoadIcon(NULL, lpIconName);
	if (hIcon)
	{
		ICONINFO info = { NULL, };
		::GetIconInfo(hIcon, &info);
		SetBitmap(info.hbmColor, clrTransparent, bStretch, bmpAlignment);
	}
}

void CMFCCaptionBarEx::SetIcon(UINT uiBmpResID, COLORREF clrTransparent, BOOL bStretch, BarElementAlignment bmpAlignment)
{
	this->SetIcon(MAKEINTRESOURCE(uiBmpResID), clrTransparent, bStretch, bmpAlignment);
}

void CMFCCaptionBarEx::EnableAutoClose(const BOOL bEnable, const INT nSecond)
{
	if (bEnable)
	{
		m_tickCount = ::GetTickCount64();
		m_timeAutoClose = nSecond;

		SetTimer(TIMER_POLLING, 0, NULL);
	}
	else
	{
		if (m_strBtnText != _T("Close"))
		{
			this->SetButton(_T("Close"), this->GetDlgCtrlID(), m_btnAlignnment, m_bBtnHasDropDownArrow);
			static_cast<CMDIFrameWndEx*>(GetParent())->RecalcLayout(FALSE);
			this->Invalidate(FALSE);
		}

		KillTimer(bEnable);

	}
}


void CMFCCaptionBarEx::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_POLLING:
	{
		KillTimer(nIDEvent);
		const ULONGLONG timer = ::GetTickCount64() - m_tickCount;
		if (timer >= m_timeAutoClose)
		{
			this->ShowWindow(SW_HIDE);
			static_cast<CMDIFrameWndEx*>(GetParent())->RecalcLayout(FALSE);
			return;
		}

		CString strCloseTime;
		strCloseTime.Format(_T("Close in %d Sec"), static_cast<INT>((m_timeAutoClose - timer) / 1000) + 1);
		if (m_strBtnText != strCloseTime)
		{
			this->SetButton(strCloseTime, this->GetDlgCtrlID(), m_btnAlignnment, m_bBtnHasDropDownArrow);
			static_cast<CMDIFrameWndEx*>(GetParent())->RecalcLayout(FALSE);
			this->Invalidate(FALSE);
		}

		SetTimer(nIDEvent, ELAPSE_TIMER_POLLING, NULL);
	}	break;
	}

	CMFCCaptionBar::OnTimer(nIDEvent);
}
