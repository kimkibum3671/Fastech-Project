#pragma once

// CDocDevice document
class CDocDevice : public CDocument
{
	DECLARE_DYNCREATE(CDocDevice)

public:
	CDocDevice();
	virtual ~CDocDevice();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()

protected:
	DWORD	m_dwAddress;
	INT		m_nSelectedMotorIndex;
	CString m_strDeviceTitle;

public:
	void			SetAddress(const DWORD dwAddress);
	const DWORD		GetAddress();

	void			SelectMotorIndex(const INT nMotorIndex);
	const INT		GetSelectMotorIndex();

	void			SetDeviceTitle(LPCTSTR lpszTitle );
	void			CheckDeviceTitle();
};
