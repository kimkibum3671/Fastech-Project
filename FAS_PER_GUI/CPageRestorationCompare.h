#pragma once

#include "CMFCPropertyPageEx.h"

#include "CMFCListCtrlEX.h"
#include "ColorStaticST.h"
#include "CMFCButtonEx.h"

// CPageRestorationCompare dialog

class CPageRestorationCompare : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageRestorationCompare)

public:
	CPageRestorationCompare(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageRestorationCompare();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_RESTORATION_COMPARE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()	

	afx_msg void OnDestroy();
	afx_msg void OnTcnSelchangeTabMotor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();

protected:
	INT m_nRestorationID, m_nCompareID, m_nMotorIndex;
	CFont m_BigFont;
	CMFCButtonEx m_btnPrev, m_btnNext;
	CMFCListCtrlEx m_wndList;
	CImageList m_imgList, m_imgTab;	
	CMFCTabCtrl m_wndTab;

	CDeviceMotionGate::MOTOR_PARAMETER_DATA m_stParamBackup[8], m_stParamCompare[8];

protected:
	typedef enum _COLOR_STATIC_ID
	{
		wndTotalAll = 0,
		wndCommandAll,
		wndPositionAll,
		wndSpeedAll,
		wndTotalOne,
		wndCommandOne,
		wndPositionOne,
		wndSpeedOne,
		wndRestorationID,
		wndComparingID,
		wndCaptionCommandAll,
		wndCaptionPositionAll,
		wndCaptionSpeedAll,
		wndCaptionCommandOne,
		wndCaptionPositionOne,
		wndCaptionSpeedOne,
		wndCaptionRestorationID,
		wndCaptionComparingID,
		wndCaptionComparingInform,
		wndMax,
	}	COLOR_STATIC_ID;

	CColorStaticST m_wndColor[wndMax], m_wndAlarmColor[3];

	typedef enum _COMPARE_RESULT
	{
		diff_none = -1,
		diff_speed = 1,
		diff_positoin = 2,
		diff_command = 3,
	}	COMPARE_RESULT;

	typedef enum _COMMAND_RESULT
	{
		sts_abs_lowspeed = 0,
		sts_abs_highspeed,
		sts_abs_highspeed_decel,
		sts_abs_normotion,
		sts_inc_lowspeed,
		sts_inc_highspeed,
		sts_inc_highspeed_decel,
		sts_inc_normotion,
		sts_move_origin,
		sts_clear_position,
		sts_reset = 0xffff,
		
	} COMMAND_RESULT;
	
	typedef enum _COMMAND_CHECK
	{
		sts_normal_motion, // sts_abs_normotion 경우만 normal Command으로 취급
		sts_not_normal_motion
	} COMMAND_CHECK;

protected:
	void InitializeListCtrl();
	void LoadMotorCompareData(const INT nMotorIndex);
	const INT CompareBackupData(const INT nMotorIndex, const INT nIndex);

	const DOUBLE	ConvertPulseToMillimeter(CDeviceMotionGate::MOTOR_PARAMETER_DATA* pstParameter, const LONG lPosition);
	const LONG		ConvertMillimeterToPulse(CDeviceMotionGate::MOTOR_PARAMETER_DATA* pstParameter, const DOUBLE dbDistance);
public:
	const INT CompareBackupPostion(const INT nMotorIndex, const INT nIndex);
	const INT CompareBackupSpeed(const INT nMotorIndex, const INT nIndex);
	const BOOL IsSystemControl();
	const INT GetParamBackupCommandStatus(INT nMotorIndex, INT nIndex);
	const INT GetParamCompareCommandStatus(INT nMotorIndex, INT nIndex);
	const INT CompareBackupCommand(const INT nMotorIndex, const INT nIndex);
	void SetErrorDataBkColor(INT nStyleErrorCnt, INT nPositionErrorCnt, INT nSpeedErrorCnt, BOOL bTotal = FALSE);
	void InitControlTextSize();
};
