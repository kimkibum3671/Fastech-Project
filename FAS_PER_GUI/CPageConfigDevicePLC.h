#pragma once


// CPageConfigDevicePLC

class CPageConfigDevicePLC : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigDevicePLC)

public:
	CPageConfigDevicePLC(CWnd* pParent = nullptr);
	virtual ~CPageConfigDevicePLC();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_PLC };
#endif
public:
	CRichEditCtrl m_edit;

	INT m_nModBusWriteAddress;
	WORD m_wModBusWriteAddress;

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	void SaveIndexData();
	void DeleteIndexData();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	
	const BOOL SetTextData(INT nStartIndex, CString str, COLORREF crText = RGB(0,0,0));
	CListCtrl m_ListCtrl;
	static CPageConfigDevicePLC* GetInstance();

	typedef enum _COMM_PLC
	{
		cmd_req_eeno,
		cmd_req_pt_mt_no,
		cmd_res_eeno,
		cmd_res_pt_mt_no,
		cmd_errcode,
		cmd_pos,
		cmd_speed,
		cmd_max
	}	COMM_PLC;

	void Initialize();
	afx_msg void OnEnChangeEditIndex();
	unsigned char GetState(CString str);
	WORD GetStringToWord(CString str);

	virtual void OnOK();
	afx_msg void OnBnClickedButtonSave();
};


