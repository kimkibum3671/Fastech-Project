#pragma once


// CStaticImageViewer

class CStaticImageViewer : public CStatic
{
	DECLARE_DYNAMIC(CStaticImageViewer)

public:
	CStaticImageViewer();
	virtual ~CStaticImageViewer();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();

protected:
	CBitmap m_bmpMain, m_bmpMainBackground;
	BOOL	m_btAlphaBlendRatio;

protected:
	CSize GetImageSize(CBitmap* pImage);

public:
	const BOOL LoadMainIcon(const INT nResID);

	const BOOL LoadMainImage(const INT nResID);
	const BOOL LoadMainImage(CBitmap* pBitmap);
	const BOOL LoadMainImage(LPCTSTR lpszFilename);

	const BOOL	SetAlphaBlend(const BOOL btRatio);
};
