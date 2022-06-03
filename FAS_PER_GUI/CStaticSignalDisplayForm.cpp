#include "pch.h"
#include "CStaticSignalDisplayForm.h"

CStaticSignalDisplayForm::CStaticSignalDisplayForm()
{
	m_colorDefaultBkOn = m_colorDefaultBkOff = RGB(0x00, 0x00, 0x00);
	m_colorDefaultTextOn = m_colorDefaultTextOff = RGB(0xff, 0xff, 0xff);
}


CStaticSignalDisplayForm::~CStaticSignalDisplayForm()
{
	this->RemoveAll();
}


CStaticSignalDisplayForm* CStaticSignalDisplayForm::GetInstance()
{
	static CStaticSignalDisplayForm _instance;

	return &_instance;
}


STRUCT_FORM_SIGNAL_INFO* CStaticSignalDisplayForm::GetFormStructure(const ULONG index)
{
	STRUCT_FORM_SIGNAL_INFO* pData = NULL;

	if ( FALSE == m_mapData.Lookup(index, pData))
	{
		pData = new STRUCT_FORM_SIGNAL_INFO;
		pData->colorBkOn		= m_colorDefaultBkOn;
		pData->colorBkOff		= m_colorDefaultBkOff;
		pData->colorTextOn		= m_colorDefaultTextOn;
		pData->colorTextOff		= m_colorDefaultTextOff;
		pData->m_nTooltipType	= TTI_NONE;
		pData->m_nTooltipID		= -1;

		m_mapData.SetAt(index, pData);
	}

	return pData;
}


void CStaticSignalDisplayForm::RemoveAll()
{
	POSITION pos = m_mapData.GetStartPosition();
	while (pos)
	{
		ULONG key = 0;
		STRUCT_FORM_SIGNAL_INFO* pData = NULL;

		m_mapData.GetNextAssoc(pos, key, pData);
		delete pData;
	}
	m_mapData.RemoveAll();
}


const BOOL CStaticSignalDisplayForm::SetDefaultBkColor(const COLORREF color)
{
	const BOOL bRedraw = m_colorDefaultBkOn != color || m_colorDefaultBkOff != color;
	
	if (bRedraw)
	{
		m_colorDefaultBkOn = m_colorDefaultBkOff = color;
	}
	
	return bRedraw;
}


const BOOL CStaticSignalDisplayForm::SetDefaultBkColor(const COLORREF colorOn, const COLORREF colorOff)
{
	const BOOL bRedraw = m_colorDefaultBkOn != colorOn || m_colorDefaultBkOff != colorOff;

	if (bRedraw)
	{
		m_colorDefaultBkOn = colorOn;
		m_colorDefaultBkOff = colorOff;
	}

	return bRedraw;
}


void CStaticSignalDisplayForm::GetDefaultBkColor(COLORREF& colorOn, COLORREF& colorOff)
{
	colorOn = m_colorDefaultBkOn;
	colorOff = m_colorDefaultBkOff;
}


const COLORREF CStaticSignalDisplayForm::GetDefaultBkColorOn()
{
	return m_colorDefaultBkOn;
}


const COLORREF CStaticSignalDisplayForm::GetDefaultBkColorOff()
{
	return m_colorDefaultBkOff;
}


const BOOL CStaticSignalDisplayForm::SetDefaultTextColor(const COLORREF color)
{
	const BOOL bRedraw = m_colorDefaultTextOn != color || m_colorDefaultTextOff != color;
	
	if (bRedraw)
	{
		m_colorDefaultTextOn = m_colorDefaultTextOff = color;
	}	

	return bRedraw;
}


const BOOL CStaticSignalDisplayForm::SetDefaultTextColor(const COLORREF colorOn, const COLORREF colorOff)
{
	const BOOL bRedraw = m_colorDefaultTextOn != colorOn || m_colorDefaultTextOff != colorOff;

	if (bRedraw)
	{
		m_colorDefaultTextOn = colorOn;
		m_colorDefaultTextOff = colorOff;
	}

	return bRedraw;
}


void CStaticSignalDisplayForm::GetDefaultTextColor(COLORREF& colorOn, COLORREF& colorOff)
{
	colorOn = m_colorDefaultTextOn;
	colorOff = m_colorDefaultTextOff;
}


const COLORREF CStaticSignalDisplayForm::GetDefaultTextColorOn()
{
	return m_colorDefaultTextOn;
}


const COLORREF CStaticSignalDisplayForm::GetDefaultTextColorOff()
{
	return m_colorDefaultTextOff;
}


void CStaticSignalDisplayForm::Draw(const ULONG index, CDC* pDC, CRect rect, const BOOL bOn)
{
	STRUCT_FORM_SIGNAL_INFO* pData = GetFormStructure(index);

	CBrush brush;
	brush.CreateSolidBrush( bOn ? pData->colorBkOn : pData->colorBkOff);
	pDC->FillRect(rect, &brush);

	if (pData->strText.GetLength())
	{
		pDC->SetTextColor(bOn ? pData->colorTextOn : pData->colorTextOff);
		pDC->DrawText(pData->strText, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	pDC->DrawEdge(rect, BDR_SUNKENOUTER, BF_RECT);
}

void CStaticSignalDisplayForm::DrawOn(const ULONG index, CDC* pDC, CRect rect)
{
	this->Draw(index, pDC, rect, TRUE);
}

void CStaticSignalDisplayForm::DrawOff(const ULONG index, CDC* pDC, CRect rect)
{
	this->Draw(index, pDC, rect, TRUE);
}

const BOOL CStaticSignalDisplayForm::SetItemText(const ULONG iBitNo, LPCTSTR lpszText)
{
	GetFormStructure(iBitNo)->strText = lpszText;
	
	return TRUE;
}
	
LPCTSTR	 CStaticSignalDisplayForm::GetItemText(const ULONG iBitNo)
{
	return GetFormStructure(iBitNo)->strText;
}

const BOOL CStaticSignalDisplayForm::SetBkColor(const ULONG iBitNo, const COLORREF color)
{
	STRUCT_FORM_SIGNAL_INFO* pData = GetFormStructure(iBitNo);

	const BOOL bRedraw = pData->colorBkOn != color || pData->colorBkOff != color;
	if (bRedraw)
	{
		pData->colorBkOn = pData->colorBkOff = color;
	}

	return bRedraw;
}

const BOOL CStaticSignalDisplayForm::SetBkColor(const ULONG iBitNo, const COLORREF colorOn, const COLORREF colorOff)
{
	STRUCT_FORM_SIGNAL_INFO* pData = GetFormStructure(iBitNo);

	const BOOL bRedraw = pData->colorBkOn != colorOn || pData->colorBkOff != colorOff;
	if (bRedraw)
	{
		pData->colorBkOn = colorOn;
		pData->colorBkOff = colorOff;
	}

	return bRedraw;
}

void CStaticSignalDisplayForm::GetBkColor(const ULONG iBitNo, COLORREF& colorOn, COLORREF& colorOff)
{
	STRUCT_FORM_SIGNAL_INFO* pData = GetFormStructure(iBitNo);

	colorOn = pData->colorBkOn;
	colorOff = pData->colorBkOff;
}

const COLORREF CStaticSignalDisplayForm::GetBkColorOn(const ULONG iBitNo)
{
	return GetFormStructure(iBitNo)->colorBkOn;
}

const COLORREF CStaticSignalDisplayForm::GetBkColorOff(const ULONG iBitNo)
{
	return GetFormStructure(iBitNo)->colorBkOff;
}

const BOOL CStaticSignalDisplayForm::SetTextColor(const ULONG iBitNo, const COLORREF color)
{
	STRUCT_FORM_SIGNAL_INFO* pData = GetFormStructure(iBitNo);

	const BOOL bRedraw = pData->colorTextOn != color || pData->colorTextOff != color;
	if (bRedraw)
	{
		pData->colorTextOn = pData->colorTextOff = color;
	}

	return bRedraw;
}

const BOOL CStaticSignalDisplayForm::SetTextColor(const ULONG iBitNo, const COLORREF colorOn, const COLORREF colorOff)
{
	STRUCT_FORM_SIGNAL_INFO* pData = GetFormStructure(iBitNo);

	const BOOL bRedraw = pData->colorTextOn != colorOn || pData->colorTextOff != colorOff;
	if (bRedraw)
	{
		pData->colorTextOn = colorOn;
		pData->colorTextOff = colorOff;
	}

	return bRedraw;
}

void CStaticSignalDisplayForm::GetTextColor(const ULONG iBitNo, COLORREF& colorOn, COLORREF& colorOff)
{
	STRUCT_FORM_SIGNAL_INFO* pData = GetFormStructure(iBitNo);

	colorOn = pData->colorTextOn;
	colorOff = pData->colorTextOff;
}

const COLORREF CStaticSignalDisplayForm::GetTextColorOn(const ULONG iBitNo)
{
	return GetFormStructure(iBitNo)->colorTextOn;
}

const COLORREF	CStaticSignalDisplayForm::GetTextColorOff(const ULONG iBitNo)
{
	return GetFormStructure(iBitNo)->colorTextOff;
}


void CStaticSignalDisplayForm::SetTooltipTitle(LPCTSTR lpszTitle)
{
	m_strTooltipTitle = lpszTitle;
}


LPCTSTR CStaticSignalDisplayForm::GetTooltipTitle()
{
	return m_strTooltipTitle;
}


void CStaticSignalDisplayForm::SetTooltipType(const ULONG iBitNo, const UINT nType)
{
	GetFormStructure(iBitNo)->m_nTooltipType = nType;
}


const UINT CStaticSignalDisplayForm::GetTooltipType(const ULONG iBitNo)
{
	return GetFormStructure(iBitNo)->m_nTooltipType;
}


void CStaticSignalDisplayForm::SetTooltipID(const ULONG iBitNo, const UINT nResID)
{
	GetFormStructure(iBitNo)->m_nTooltipID = nResID;
}


const UINT CStaticSignalDisplayForm::GetTooltipID(const ULONG iBitNo)
{
	return GetFormStructure(iBitNo)->m_nTooltipID;
}
