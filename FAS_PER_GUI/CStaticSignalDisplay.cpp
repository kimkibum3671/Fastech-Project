// CStaticSignalDisplay.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CStaticSignalDisplay.h"

#include "CStaticSignalDisplayGlobalForm.h"

// CStaticSignalDisplay

IMPLEMENT_DYNAMIC(CStaticSignalDisplay, CStatic)

CStaticSignalDisplay::CStaticSignalDisplay()
{
	m_nFormID = -1;
	m_pFormCtrl = NULL;
	m_nPointedItem = -1;
}

CStaticSignalDisplay::~CStaticSignalDisplay()
{
	RemoveAll();
}


BEGIN_MESSAGE_MAP(CStaticSignalDisplay, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CStaticSignalDisplay message handlers
CStaticSignalDisplayForm* CStaticSignalDisplay::GetFormCtrl()
{
	CStaticSignalDisplayForm* pResult = NULL;

	if (-1 != m_nFormID)
	{
		pResult = CStaticSignalDisplayGlobalForm::GetInstance()->GetForm(m_nFormID);
	}
	else
	{
		if (NULL == m_pFormCtrl)
		{
			m_pFormCtrl = new CStaticSignalDisplayForm;
		}

		pResult = m_pFormCtrl;
	}

	return pResult;
}

void CStaticSignalDisplay::SetFormID(const INT nFormID)
{
	if (m_nFormID != nFormID)
	{
		if (-1 != m_nFormID)
		{
			if (NULL != m_pFormCtrl)
			{
				delete m_pFormCtrl;
				m_pFormCtrl = NULL;
			}
		}

		m_nFormID = nFormID;
		Invalidate(FALSE);
	}
}

const INT CStaticSignalDisplay::GetFormID()
{
	return m_nFormID;
}

void CStaticSignalDisplay::RemoveAll()
{
	m_mapSignal.RemoveAll();
	if (m_pFormCtrl)
	{
		m_pFormCtrl->RemoveAll();
	}
}

const BOOL CStaticSignalDisplay::SetDefaultBkColor(const COLORREF color)
{
	return GetFormCtrl()->SetDefaultBkColor(color);
}

const BOOL CStaticSignalDisplay::SetDefaultBkColor(const COLORREF colorOn, const COLORREF colorOff)
{
	return GetFormCtrl()->SetDefaultBkColor(colorOn, colorOff);
}

void CStaticSignalDisplay::GetDefaultBkColor(COLORREF& colorOn, COLORREF& colorOff)
{
	GetFormCtrl()->GetDefaultBkColor(colorOn, colorOff);
}

const COLORREF CStaticSignalDisplay::GetDefaultBkColorOn()
{
	return GetFormCtrl()->GetDefaultBkColorOn();
}

const COLORREF CStaticSignalDisplay::GetDefaultBkColorOff()
{
	return GetFormCtrl()->GetDefaultBkColorOff();
}

const BOOL CStaticSignalDisplay::SetDefaultTextColor(const COLORREF color)
{
	return GetFormCtrl()->SetDefaultTextColor(color);
}

const BOOL CStaticSignalDisplay::SetDefaultTextColor(const COLORREF colorOn, const COLORREF colorOff)
{
	return GetFormCtrl()->SetDefaultTextColor(colorOn, colorOff);
}

void CStaticSignalDisplay::GetDefaultTextColor(COLORREF& colorOn, COLORREF& colorOff)
{
	GetFormCtrl()->GetDefaultTextColor(colorOn, colorOff);
}

const COLORREF CStaticSignalDisplay::GetDefaultTextColorOn()
{
	return GetFormCtrl()->GetDefaultTextColorOn();
}

const COLORREF CStaticSignalDisplay::GetDefaultTextColorOff()
{
	return GetFormCtrl()->GetDefaultTextColorOff();
}

const BOOL CStaticSignalDisplay::SetSize(const INT cx, const INT cy)
{
	const BOOL bRedraw = m_sizeCell != CSize(cx, cy);

	if (bRedraw)
	{
		m_sizeCell = CSize(cx, cy);
	}

	return bRedraw;
}

CSize CStaticSignalDisplay::GetSize()
{
	return m_sizeCell;
}

const BOOL CStaticSignalDisplay::SetItemText(const ULONG index, LPCTSTR lpszText)
{
	if (static_cast<ULONG>(static_cast<LONGLONG>(m_sizeCell.cx) * m_sizeCell.cy) <= index)
	{
		return FALSE;
	}

	BOOL bRedraw = this->GetFormCtrl()->SetItemText(index, lpszText);

	if (bRedraw)
	{
		Invalidate(FALSE);
	}

	return TRUE;
}

LPCTSTR CStaticSignalDisplay::GetItemText(const ULONG index)
{
	LPCTSTR lpszResult = _T("");

	return lpszResult;
}

const BOOL CStaticSignalDisplay::SetBkColor(const ULONG index, const COLORREF color)
{
	return GetFormCtrl()->SetBkColor(index, color);
}

const BOOL CStaticSignalDisplay::SetBkColor(const ULONG index, const COLORREF colorOn, const COLORREF colorOff)
{
	return GetFormCtrl()->SetBkColor(index, colorOn, colorOff);
}

void CStaticSignalDisplay::GetBkColor(const ULONG index, COLORREF& colorOn, COLORREF& colorOff)
{
	GetFormCtrl()->GetBkColor(index, colorOn, colorOff);
}

const COLORREF CStaticSignalDisplay::GetBkColorOn(const ULONG index)
{
	return GetFormCtrl()->GetBkColorOn(index);
}

const COLORREF CStaticSignalDisplay::GetBkColorOff(const ULONG index)
{
	return GetFormCtrl()->GetBkColorOff(index);
}

const BOOL CStaticSignalDisplay::SetTextColor(const ULONG index, const COLORREF color)
{
	return GetFormCtrl()->SetTextColor(index, color);
}

const BOOL CStaticSignalDisplay::SetTextColor(const ULONG index, const COLORREF colorOn, const COLORREF colorOff)
{
	return GetFormCtrl()->SetTextColor(index, colorOn, colorOff);
}

void CStaticSignalDisplay::GetTextColor(const ULONG index, COLORREF& colorOn, COLORREF& colorOff)
{
	GetFormCtrl()->GetTextColor(index, colorOn, colorOff);
}

const COLORREF CStaticSignalDisplay::GetTextColorOn(const ULONG index)
{
	return GetFormCtrl()->GetTextColorOn(index);
}

const COLORREF CStaticSignalDisplay::GetTextColorOff(const ULONG index)
{
	return GetFormCtrl()->GetTextColorOff(index);
}


void CStaticSignalDisplay::OnPaint()
{
	CPaintDC dc(this);

	CStaticSignalDisplayForm* pForm = GetFormCtrl();

	CRect rtClient;
	this->GetClientRect(rtClient);

	const INT nWidth = rtClient.Width();
	const INT nHeight = rtClient.Height();

	const INT cx = nWidth / m_sizeCell.cx;
	const INT cy = nHeight / m_sizeCell.cy;

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, nWidth, nHeight);
	CBitmap* pOldMemBitmap = memDC.SelectObject(&bitmap);

	if (1)
	{
		CRect rtWindow;
		GetWindowRect(rtWindow);
		CWnd* pParent = GetParent();
		pParent->GetParent()->ScreenToClient(rtWindow);
		CDC* pParentDC = pParent->GetWindowDC();
		memDC.BitBlt(0, 0, rtWindow.Width(), rtWindow.Height(), pParentDC, rtWindow.left, rtWindow.top, SRCCOPY);
	}

	memDC.SetBkMode(TRANSPARENT);
	CFont* pFont = memDC.SelectObject(&afxGlobalData.fontBold);

	const BOOL bEnabled = this->IsWindowEnabled();

	INT nIndex = 0;
	for (INT y = 0; y < m_sizeCell.cy; y++)
	{
		for (INT x = 0; x < m_sizeCell.cx; x++)
		{
			CRect rtItem(CPoint(x * cx, y * cy), CPoint((x + 1) * cx, (y + 1) * cy));

			BOOL bEnable = FALSE;
			m_mapSignal.Lookup(nIndex, bEnable);
			pForm->Draw(nIndex, &memDC, rtItem, bEnable);

			nIndex++;
		}
	}

	memDC.SelectObject(pFont);

	dc.BitBlt(0, 0, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldMemBitmap);
}


int CStaticSignalDisplay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void CStaticSignalDisplay::PreSubclassWindow()
{
	if (NULL == m_tipCtrl.GetSafeHwnd())
	{
		m_tipCtrl.Create(this);
//		m_tipCtrl.SetTitle(TTI_NONE, _T(""));
//		m_tipCtrl.SetTitle(TTI_WARNING, _T("21345"));

		m_tipCtrl.SetDelayTime(TTDT_AUTOPOP, INFINITE);
		m_tipCtrl.SetDelayTime(TTDT_INITIAL, 0);
		m_tipCtrl.SetDelayTime(TTDT_RESHOW, 0);

		SetTimer(TIMER_POLLING, 100, NULL);
	}

	CStatic::PreSubclassWindow();
}


BOOL CStaticSignalDisplay::PreTranslateMessage(MSG* pMsg)
{
	m_tipCtrl.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}

const BOOL CStaticSignalDisplay::EnableSignal(const ULONG index, const BOOL bEnable)
{
	BOOL bResult = FALSE;
	BOOL bStatus = FALSE;
	if (FALSE == m_mapSignal.Lookup(index, bStatus) || bStatus != bEnable)
	{
		m_mapSignal.SetAt(index, bEnable);
		Invalidate(FALSE);
		bResult = TRUE;
	}

	return bResult;
}

const BOOL CStaticSignalDisplay::IsSignalEnabled(const ULONG index)
{
	BOOL bResult = FALSE;
	m_mapSignal.Lookup(index, bResult);

	return bResult;
}


void CStaticSignalDisplay::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_POLLING:
	{
		TrackMousePos();
	}	break;
	}

	CStatic::OnTimer(nIDEvent);
}

void CStaticSignalDisplay::TrackMousePos()
{
	POINT ptCursor;
	if (FALSE == GetCursorPos(&ptCursor))
	{
		return;
	}

	CRect rtWindow;
	GetWindowRect(rtWindow);

	if ( this->IsWindowEnabled() && this->IsWindowVisible() && rtWindow.PtInRect(ptCursor) )
	{
		const INT nItemIndex = CalcSelectedItem(ptCursor, rtWindow);

		if (nItemIndex != m_nPointedItem)
		{
			if (m_tipCtrl.GetToolCount())
			{
				m_tipCtrl.DelTool(this);
			}
			
			const UINT nToolTipID = GetFormCtrl()->GetTooltipID(nItemIndex);

			if (-1 != nItemIndex)
			{
				CString strToolTip, strTemp;
				strTemp.LoadString(-1 != nToolTipID ? nToolTipID : IDS_TOOLTIP_RESERVED);
				strToolTip.Format(_T("Bit%02d - %s"), nItemIndex, strTemp );
				m_tipCtrl.SetTitle(GetFormCtrl()->GetTooltipType(nItemIndex), GetFormCtrl()->GetTooltipTitle());
				m_tipCtrl.AddTool(this, strToolTip);
			}

			m_nPointedItem = nItemIndex;
		}
	}

	m_ptMouse = ptCursor;
}

const INT CStaticSignalDisplay::CalcSelectedItem(CPoint ptMouse, CRect rtWindow)
{
	const INT cx = rtWindow.Width() / m_sizeCell.cx;
	const INT cy = rtWindow.Height() / m_sizeCell.cy;

	const INT nCount = cx * cy;

	CRect rtItem(0, 0, cx, cy);

	rtItem.OffsetRect(rtWindow.TopLeft());

	for (INT y = 0; y < m_sizeCell.cy; y++)
	{
		for (INT x = 0; x < m_sizeCell.cx; x++)
		{
			if (rtItem.PtInRect(ptMouse))
			{
				return y * m_sizeCell.cx + x;
			}

			rtItem.OffsetRect(cx, 0);
		}

		rtItem.OffsetRect(-rtItem.left + rtWindow.left, cy);
	}

	return -1;
}
