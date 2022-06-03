#include "pch.h"
#include "CBalloonMsg.h"

CEdit CBalloonMsg::m_wndEdit;

const BOOL CBalloonMsg::ShowBalloonMsg(HWND hWnd, LPCWSTR lpszTitle, LPCWSTR lpszText, const INT ttiIcon)
{
	if (::IsWindow(m_wndEdit.m_hWnd))
	{
		if (FALSE == m_wndEdit.DestroyWindow())
		{
			return FALSE;
		}
	}

	if( ::IsWindow(hWnd) )
	{
		CRect rect;
		::GetClientRect(hWnd, rect);

		CWnd* pParent = CWnd::FromHandle(hWnd);

		if (m_wndEdit.Create(WS_CHILD, rect, pParent, -1))
		{
			if (m_wndEdit.SetWindowPos(&CWnd::wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(), 0))
			{
				EDITBALLOONTIP bt = { 0, };

				//Add whatever characters you want to restrict here:
				bt.cbStruct = sizeof(EDITBALLOONTIP);
				bt.pszTitle = lpszTitle;
				bt.pszText = lpszText;
				bt.ttiIcon = ttiIcon;

				m_wndEdit.SendMessage(EM_SHOWBALLOONTIP, 0, (LPARAM)&bt);
			}

			return TRUE;
		}
	}

	return FALSE;
}
