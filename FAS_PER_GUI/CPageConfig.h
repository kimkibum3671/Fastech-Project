#pragma once

#include "CMFCPropertyPageEx.h"

// CPageConfig dialog

class CPageConfig : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageConfig)

public:
	CPageConfig();
	CPageConfig(UINT nIDTemplate, UINT nIDCaption = 0);
	CPageConfig(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

	virtual ~CPageConfig();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = 1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	typedef enum _OP_MODE
	{
		op_socket = 0,	//	for device list configuration (default)
		op_serial,		//	for serial configuration
		op_default,		//	for default configuration
	}	OP_MODE;

protected:
	void Initialize();

protected:
	INT m_nOpMode;

public:
	void		SetOpCode(const INT nOpMode);
	const INT	GetOpMode();
};
