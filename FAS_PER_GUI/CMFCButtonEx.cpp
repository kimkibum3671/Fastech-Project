// CMFCButtonEx.cpp: 구현 파일
//

#include "pch.h"
#include "CMFCButtonEx.h"

// CMFCButtonEx

IMPLEMENT_DYNAMIC(CMFCButtonEx, CMFCButton)

CMFCButtonEx::CMFCButtonEx()
{
	m_bEnableWindowTheme = TRUE;

	m_nBmpResId = \
		m_uiBmpHotResId = \
		m_uiBmpDsblResID = 0;

	m_bFontChange = TRUE;
}

CMFCButtonEx::~CMFCButtonEx()
{
}


BEGIN_MESSAGE_MAP(CMFCButtonEx, CMFCButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CMFCButtonEx 메시지 처리기
void CMFCButtonEx::OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState)
{
	CMFCButton::OnDrawBorder(pDC, rectClient, uiState);

	if (0 && m_bCheckButton)
	{
		if (this->m_bChecked)
		{
			CBrush brush;
			brush.CreateHatchBrush(HS_DIAGCROSS, RGB(0xff, 0x00, 0x00));
			pDC->FillRect(rectClient, &brush);
		}
	}

	if ( IsWindowEnabled() )
	{
		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		for (INT nIndex = 1; nIndex < 3; nIndex++)
		{
			rectClient.InflateRect(1, 1);
			pDC->Rectangle(rectClient);
		}

		pDC->SelectObject(pOldBrush);
	}
}


void CMFCButtonEx::OnDraw(CDC* pDC, const CRect& rect, UINT uiState)
{
	CRect rc;
	CPaintDC dc(this);
	INT x, y, width, height;

	rc = const_cast<LPRECT>(static_cast<LPCRECT>(rect));

	/*
	x = 0;
	y = 0;
	width = rect.Width();
	height = rect.Height();

	//x = (-1 == strText.Find('\n')) ? rectText.right / 4 : (rc.Width() - rectText.right) / 2 + 20;

	rc.SetRect(x, y, width, height);
	*/
	
	CMFCButton::OnDraw(pDC, rc, uiState);
}


void CMFCButtonEx::SetImageEx(const UINT uiBmpResId, const UINT uiBmpHotResId, const UINT uiBmpDsblResID)
{
	this->SetImage(m_nBmpResId = uiBmpResId, m_uiBmpHotResId = uiBmpHotResId, m_uiBmpDsblResID = uiBmpDsblResID);
	this->Invalidate(FALSE);
}


void CMFCButtonEx::GetImageEx(UINT* pUiBmpResId, UINT* pUiBmpHotResId, UINT* pUiBmpDsblResID)
{
	if (pUiBmpResId)
	{
		*pUiBmpResId = m_nBmpResId;
	}

	if (pUiBmpHotResId)
	{
		*pUiBmpHotResId = m_uiBmpHotResId;
	}

	if (pUiBmpDsblResID)
	{
		*pUiBmpDsblResID = m_uiBmpDsblResID;
	}
}


void CMFCButtonEx::OnDrawText(CDC* pDC, const CRect& rect, const CString& strText, UINT uiDTFlags, UINT uiState)
{
	
	CFont font, *pOldFont;
	
	LOGFONT lf = { 0, };
	afxGlobalData.fontRegular.GetLogFont(&lf);
	
	if (-1 == strText.Find('\n'))
	{
		//lf.lfHeight = -23;
		font.CreateFontIndirect(&lf);

		pOldFont = (CFont*)pDC->SelectObject(&font);

		BOOL bCheck = m_bFontChange;

		pDC->DrawText(strText, const_cast<LPRECT>(static_cast<LPCRECT>(rect)), DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		pDC->SelectObject(pOldFont);
	}
	else
	{
		//lf.lfHeight = -20;
		font.CreateFontIndirect(&lf);

		pOldFont = (CFont*)pDC->SelectObject(&font);

		CRect rc, rectText;
		rc = const_cast<LPRECT>(static_cast<LPCRECT>(rect));
		pDC->DrawText(strText, &rectText, DT_CALCRECT); // 출력될 텍스트의 너비와 높이를 리턴한다.

		INT x, y, width, height;

		x = (-1 == strText.Find('\n')) ? rectText.right / 2 : (rc.Width() - rectText.right) / 2 + 20;
		y = ((rc.Height() - rectText.Height()) / 2) + ((-1 == strText.Find('\n')) ? -3 : 0);
		width = rc.right;
		height = rc.bottom;

		rc.SetRect(x, y, width, height);

		pDC->DrawText(strText, (LPRECT)rc, DT_CENTER/* | DT_VCENTER*/);

		pDC->SelectObject(pOldFont);
	}
}


//void CMFCButtonEx::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
//					   // 그리기 메시지에 대해서는 CMFCButton::OnPaint()을(를) 호출하지 마십시오.
//
//	CRect rect;
//	GetClientRect(&rect);
//
//	CFont font;
//	font.CreatePointFont(400, _T("Arial"));
//	//dc.DrawText()
//	
//}
