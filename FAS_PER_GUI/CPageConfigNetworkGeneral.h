#pragma once

#include "CMFCListCtrlEX.h"
#include "ClientSocket.h"
#include "ColorStaticST.h"
// CPageConfigNetworkGeneral dialog

class CPageConfigNetworkGeneral : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigNetworkGeneral)

public:
	CPageConfigNetworkGeneral(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigNetworkGeneral();
	 
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_NETWORK_GENERAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnLvnItemchangedListNetwork(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnEndlabeleditListNetwork(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnAdd();

public:
	virtual BOOL OnInitDialog();

protected:
	CMFCListCtrlEx	m_listCtrl;
	BOOL			m_bEsc;
	INT				m_nOctetThird;
	CColorStaticST	m_stColor;
protected:
	void ExchangeItemData(const INT nIdxItem1, const INT nIdxItem2);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnMod();
	afx_msg void OnBnClickedBtnDel();
	virtual void OnOK();
	afx_msg void OnBnClickedBtnUp();
	afx_msg void OnBnClickedBtnDown();
	afx_msg void OnBnClickedBtnModDevID();

protected:
#define	SIZE_SOCKRECVBUFFER		4096
#define	SIZE_SOCKSENDBUFFER		256
#define MAX_IP 255

	typedef struct _tagPROTOCOL {
		ClientSocket* pSock;
		CCriticalSection	cs;
		bool	bConnect;
		bool	bInitial;
		int		nRi;
		int		nConCycle;
		unsigned char	sRbuf[SIZE_SOCKRECVBUFFER];
		char	sSbuf[SIZE_SOCKSENDBUFFER];
	} tagPROTOCOL;

	//tagPROTOCOL m_protocol[MAX_IP];
public:
	afx_msg void OnBnClickedButtonSearching();
	const BOOL SetNetworkSearchingAddress(CString strAddress);
	const DWORD StringToAddress(CString strValue);
	afx_msg void OnIpnFieldchangedIpaddressStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEditOctet3();
	void InitControlTextSize();
protected:
	CFont m_BigFont;
};
