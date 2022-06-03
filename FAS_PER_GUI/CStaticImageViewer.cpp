// CStaticImageViewer.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CStaticImageViewer.h"


// CStaticImageViewer

IMPLEMENT_DYNAMIC(CStaticImageViewer, CStatic)

CStaticImageViewer::CStaticImageViewer()
{
	m_btAlphaBlendRatio = 0;
}

CStaticImageViewer::~CStaticImageViewer()
{

}


BEGIN_MESSAGE_MAP(CStaticImageViewer, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CStaticImageViewer message handlers
void CStaticImageViewer::OnPaint()
{
	CPaintDC dc(this);
	
	if (NULL == m_bmpMain.GetSafeHandle())
	{
		return;
	}

	CRect rect;
	this->GetClientRect(rect);

	const INT nWidth = rect.Width();
	const INT nHeight = rect.Height();

	CSize cbMain = GetImageSize(&m_bmpMain);

	CDC memDC, memDC2;
	memDC.CreateCompatibleDC(&dc);
	memDC2.CreateCompatibleDC(&dc);

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, nWidth, nHeight);

	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.Rectangle(rect);

	if (1)
	{
		CBitmap* pOldBitmap2 = memDC2.SelectObject(&m_bmpMain);

		const DOUBLE dbWidthRatio = static_cast<DOUBLE>(nWidth) / cbMain.cx;
		const DOUBLE dbHeightRatio = static_cast<DOUBLE>(nHeight) / cbMain.cy;

		const INT nFinalWidth = static_cast<INT>(dbHeightRatio < dbWidthRatio ? dbHeightRatio * cbMain.cx : dbWidthRatio * cbMain.cx);
		const INT nFinalHeight = static_cast<INT>(dbHeightRatio < dbWidthRatio ? dbHeightRatio * cbMain.cy : dbWidthRatio * cbMain.cy);

		const INT nOffsetX = static_cast<INT>(static_cast<DOUBLE>(nWidth - nFinalWidth) / 2);
		const INT nOffsetY = static_cast<INT>(static_cast<DOUBLE>(nHeight - nFinalHeight) / 2);

		memDC.StretchBlt(nOffsetX, nOffsetY, nFinalWidth, nFinalHeight, &memDC2, 0, 0, cbMain.cx, cbMain.cy, SRCCOPY);

		memDC2.SelectObject(pOldBitmap2);
	}
	
	if (1)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));
		CPen* pOldPen = memDC.SelectObject(&pen);

		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);
		CBrush* pOldBrush = memDC.SelectObject(&brush);

		memDC.Rectangle(0, 0, rect.Width(), rect.Height());

		memDC.SelectObject(pOldPen);
		memDC.SelectObject(pOldBrush);
	}
	
	dc.BitBlt(0, 0, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
}

CSize CStaticImageViewer::GetImageSize(CBitmap* pImage)
{
	CSize result;

	BITMAP bmp = { 0, };
	pImage->GetBitmap(&bmp);
	result.cx = bmp.bmWidth;
	result.cy = bmp.bmHeight;

	return result;
}

const BOOL CStaticImageViewer::LoadMainIcon(const INT nResID)
{
	return FALSE;
}

const BOOL CStaticImageViewer::LoadMainImage(const INT nResID)
{
	if (FALSE == m_bmpMain.LoadBitmap(nResID))
	{
		return FALSE;
	}

	Invalidate(FALSE);

	return TRUE;
}

const BOOL CStaticImageViewer::LoadMainImage(CBitmap* pBitmap)
{
	if (NULL == pBitmap->GetSafeHandle())
	{
		return FALSE;
	}	

	BITMAP bitmap = { 0, };
	pBitmap->GetBitmap(&bitmap);

	if (NULL != m_bmpMain.GetSafeHandle())
	{
		m_bmpMain.DeleteObject();
	}

	m_bmpMain.Attach(::CopyImage(static_cast<HBITMAP>(*pBitmap), IMAGE_BITMAP, 0, 0, 0));

	Invalidate(FALSE);

	return TRUE;
}

const BOOL CStaticImageViewer::LoadMainImage(LPCTSTR lpszFilename)
{
	if (IS_INTRESOURCE(lpszFilename))
	{
		return this->LoadMainImage(reinterpret_cast<INT>(lpszFilename));
	}

	return FALSE;
}

const BOOL CStaticImageViewer::SetAlphaBlend(const BOOL btRatio)
{
	if (m_btAlphaBlendRatio != btRatio)
	{
		m_btAlphaBlendRatio = btRatio;

		Invalidate(FALSE);
	}

	return FALSE;
}
