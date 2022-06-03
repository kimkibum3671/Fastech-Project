#pragma once

#include "..\Common_Define\Define_Structure.h"
#include "ReferD.h"
class CDeviceMotionGate
{
public:
	CDeviceMotionGate();
	~CDeviceMotionGate();

protected:
	typedef struct _DATABASE_DEVICE_INFORMATION
	{	//	Project DB에 저장되는 ProjectTable의 내용
		CString strName;
		DWORD	dwAddress;
		INT		nMotorCount;
		INT		nHomingTimeout;
		INT		nNominalID;
		INT		nRestoredID;
		CBitmap	bmpImage;
		INT		nDeviceID;
		BOOL	bDeleted;
	}	DATABASE_DEVICE_INFORMATION;

	typedef struct _REAL_DEVICE_INFORMATION
	{
		CString strInformation;
		CString strName;
		INT		nMotorCount;
		INT		nHomingTimeout;
		INT		nDeviceID;
		DWORD	dwWarningLifeCount;
		DWORD	dwWarningLifeTickCount;
	}	CONNECTED_DEVICE_INFORMATION;

	enum _RESPONSE_STRUCT
	{
		REQ0,
		REQ1,
		EE_NO,
		PT_MOTOR_NO,
		ERR_CODE,
		POS_LO,
		POS_HI,
		SPEED,
		MAX
	};

protected:
	DATABASE_DEVICE_INFORMATION		m_stDatabaseDeviceInfo, m_stDatabaseEditDeviceInfo;
	CONNECTED_DEVICE_INFORMATION	m_stConnectedDeviceInfo;

protected:
	BOOL	m_bConnected, m_bAutoDatabaseUpdate, m_bDeleted;
	CString	m_strProjectName, m_strBackupName;
	INT		m_nRetryCount, m_nLogLevel;

	CTime	m_timeOperationTimeStamp;

	BOOL		m_bRun;
	BOOL		m_bInformRequest;
	CWinThread* m_pThread;

	DWORD	m_dwTickCount;
	CString m_strDeviceCheck;
public:
	void		EnableAutoDatabaseUpdate(const BOOL bEnable = TRUE);
	const BOOL	IsAutoDatabaseUpdateEnabled();

public:
	typedef union _MOTOR_DETAIL_STATUS
	{
		DWORD dwValue;

		struct
		{
			BYTE bitErrorAll : 1;
			BYTE bitHardwarePositiveLimit : 1;
			BYTE bitHardwareNegativeLimit : 1;
			BYTE bitSoftwarePositiveLimit : 1;
			BYTE bitSoftwareNegativeLimit : 1;
			BYTE bitReserved1 : 1;
			BYTE bitReserved2 : 1;
			BYTE bitErrorPositionOverflow : 1;
			BYTE bitErrorOverCurrent : 1;
			BYTE bitErrorOverSpeed : 1;
			BYTE bitErrorPositionTracking : 1;
			BYTE bitErrorOverLoad : 1;
			BYTE bitErrorOverHeat : 1;
			BYTE bitErrorBackEMF : 1;
			BYTE bitErrorMotorPower : 1;
			BYTE bitErrorInposition : 1;
			BYTE bitEmergencyStop : 1;
			BYTE bitSlowStop : 1;
			BYTE bitOriginReturning : 1;
			BYTE bitInposition : 1;
			BYTE bitServoOn : 1;
			BYTE bitAlarmReset : 1;
			BYTE bitPositionTableStop : 1;
			BYTE bitOriginSensor : 1;
			BYTE bitPhaseZ : 1;
			BYTE bitOriginComplete : 1;
			BYTE bitMotionDirection : 1;	// High = Directoin -
			BYTE bitMotioning : 1;
			BYTE bitMotionPause : 1;
			BYTE bitMotionAccel : 1;
			BYTE bitMotionDecel : 1;
			BYTE bitMotionConst : 1;
		};
	} MOTOR_DETAIL_STATUS;

	typedef union _MOTOR_STATUS
	{
		WORD wValue;
		struct
		{
			BYTE bitServoOn : 1;
			BYTE bitMovingForward : 1;
			BYTE bitMovingBackward : 1;
			BYTE bitInpsotion : 1;
			BYTE bitOriginRun : 1;
			BYTE bitOriginOK : 1;
			BYTE bitAlarm : 1;
			BYTE bitSoftwareLimitMinus : 1;
			BYTE bitSoftwareLimitPlus : 1;
			BYTE bitPtError : 1;
		};
	}	MOTOR_STATUS;



	typedef union _MOTOR_OPERATION_STATUS
	{
		WORD wValue;
		struct
		{
			BYTE bitHoming : 1;
			BYTE bitHomingOK : 1;
			BYTE bitHomingNG : 1;
		};
	}	MOTOR_OPERATION_STATUS;

protected:
	typedef struct _AXIS_INFORMAION
	{
		WORD					wPosInteger;
		WORD					wPosDecimal;
		MOTOR_STATUS			wStatus;
		WORD					wPositionTableNo;
		BYTE					btErrorCode;
		BYTE					btReserved;
	}	AXIS_INFORMAION;

	typedef union _GATE_STATUS
	{
		WORD wValue;
		struct
		{
			BYTE bitAlarm : 1;
			BYTE bitReady : 1;
			BYTE bitPtNumError : 1;
			BYTE bitGripperOn : 1;
			BYTE bitMonitorGrant : 1;
			BYTE bitMonitorOn : 1;
			BYTE bitPtRun : 1;
			BYTE bitHoming : 1;
			BYTE bitNetworkOn : 1; // Master 연결상태 확인
			BYTE bitTeachingPendant : 1;
		};
	}	GATE_STATUS;

	typedef struct _SYSTEM_STATUS
	{
		//	MotionGate에서 사용하는 통신프레임
		GATE_STATUS				wStatus;
		BYTE					btStartMotorConnectionStatus;
		BYTE					btCurrentMotorConnectionStatus;
		AXIS_INFORMAION			stMotorStatus;
	}	SYSTEM_STATUS;

	typedef struct _SYSTEM_STATUS_ALL
	{
		GATE_STATUS				wStatus;
		BYTE					btSystemAlarmStatus;
		BYTE					btStartMotorConnectionStatus;
		BYTE					btCurrentMotorConnectionStatus;
		AXIS_INFORMAION			stMotorStatus[9];
		MOTOR_OPERATION_STATUS	opStatus[9];
		DWORD					dwLifeCount[9];
		BOOL					bMotorLifeDetected[9];
		BOOL					bMotorLifeCycleDetected[9];
		// KKB 추가.
		BYTE					btMotorAlarm[9];
		MOTOR_DETAIL_STATUS     dwDetailStatus[9];
		CString					stMotorLog[9];
	}	SYSTEM_STATUS_ALL;

	SYSTEM_STATUS_ALL	m_stSystemStatus;

	WORD			m_wPrevAxisConnection, m_wScanAxisConnection, m_wCurAxisConnection;
	WORD			m_wCurAxisOrganization, m_wOriginAxisOrganization;

public:
	typedef struct _MOTOR_PARAMETER_DATA
	{
		LONG lParameter[40];
		::ITEM_NODE nodeItem[32];
	}	MOTOR_PARAMETER_DATA;

protected:
	MOTOR_PARAMETER_DATA m_stMotorInfo[9];

protected:
	static UINT PollingThread(LPVOID pParam);

public:
	const BOOL	LoadFromDatabase(LPVOID pDatabaseRecordData);
	const BOOL	SaveToDatabase(const INT nPriority);
	const BOOL	UpdateDatabase();

	void		ResetOperationTimeStamp(CTime time);
	CTime&		GetOpertionTimeStamp();

	const BOOL	StartThread();
	void		StopThread();
	const BOOL	IsRun();
	const BOOL	IsRunning();

	void		SetAddress(const DWORD dwAddress);
	const DWORD	GetAddress();
	void		GetAddress(CString& strAddress);
	
	void		SetEditAddress(const DWORD dwAddress);
	const DWORD	GetEditAddress();
	void		GetEditAddress(CString& strAddress);

	const BOOL	SetSystemName(LPCTSTR lpszName);
	LPCTSTR		GetSystemName();

	const BOOL	SetEditSystemName(LPCTSTR lpszName);
	LPCTSTR		GetEditSystemName();

	LPCTSTR		GetDatabaseSystemName();

	void		SetNominalBackupID(const INT nBackupID = -1);
	const INT	GetNominalBackupID();

	void		SetRestoredBackupID(const INT nBackupID = -1);
	const INT	GetRestoredBackupID();

	CBitmap*	SetSystemImage(HBITMAP hBitmap);
	CBitmap*	GetSystemImage();

	CBitmap*	SetEditSystemImage(HBITMAP hBitmap);
	CBitmap*	GetEditSystemImage();

	void		SetMotorCount(const INT nCount);
	const INT	GetMotorCount();

	const INT	GetDatabaseMotorCount();
	
	void		SetEditMotorCount(const INT nCount);
	const INT	GetEditMotorCount();

	const INT	GetConnectedMotorCount();
	const INT	GetContinuouslyConnectedMotorCount(const INT nExceptMotorIndex = 0);
	const BOOL	IsMotorConnectionContinuity( const INT nExceptMotorIndex = 0);

	const BOOL	IsTeachingPendantOnLine();

	const DWORD	GetStatus();

public:
	typedef enum __STATE_MACHINE
	{
		state_stop = 0,
		state_connect,
		state_reboot,
		state_motion_gate_get_system_status_all,
		state_motion_gate_command,
		state_backup,
		state_restoration,
		state_factory_reset,
		state_assign_motor_id,
		state_home_all,
		state_change_information,
	}	STATE_MACHINE;

	typedef enum _OPCODE_STATUS_MACHINE
	{
		opCode_stop = 0,
		opCode_connect,
		opCode_disconenct,				// Cannot connect to conveter
		opCode_no_response,
		opCode_MotiongateBusy,			//	Teaching Pendant가 연결되었을 때
		opCode_read_motiongate_info,
		opCode_scan_device,
		opCode_read_motiongate_status,
		opCode_read_life_counter,
		opCode_scan_next_device_ok,
		opCode_scan_next_device_ng,
		opCode_standby_motiongate,
		opCode_backup_read_parameter,
		opCode_backup_read_position_table,
		opCode_enter_monitor_mode,
		opCode_exit_monitor_mode,
		opCode_check_monitor_mode_on,
		opCode_check_monitor_mode_off,
		opCode_notify_backup_complete,
		opCode_check_device_name,
		opCode_change_device_info,
		opCode_write_main_parameter,
		opCode_write_position_table,
		opCode_save_main_parameter,
		opCode_save_position_table,
		opCode_confirm_main_parameter,
		opCode_load_position_table,
		opCode_confirm_position_table,
		opCode_change_motiongate_id,
		opCode_reboot_motiongate,
		opCode_homing_servo_on,
		opCode_homing_start,
		opCode_homing_wait,
		opCode_homing_stop,
	}	OPCODE_STATUS_MACHINE;

protected:
	UINT			m_nOpCode;
	STATE_MACHINE	m_stateMachine;
	INT				m_nMotorIndex;
	INT				m_nParameterIndex;
//	INT				m_nProgressCounter;
	INT				m_nBackupID;
	INT				m_nAssignSourceID, m_nAssignTargetID;
public:
		void		StopSequence();
		

protected:
	void		SendLogMessage(const INT nLevel, LPCTSTR lpszFormat, ...);
	void		SendLogMessage(const INT nLevel, const INT nMsgID, const LPARAM lParam = 0);
	void		Initialize(const BOOL bFirstLaunch = TRUE);
	

	void		SetOpCode(const UINT nStatusCode);

	

	const INT	DoNextStateMachine();

	void		SetConnectionStatus(const BOOL bConnected);

	const INT	OpConnectDevice();
	const INT	OpRebootDevice();
	const INT	OpReadMotiongateStatus();
	const INT	OpWriteUserRequest();
	const INT	OpReadParameters();
	const INT	OpRestoreParameter();
	const INT	OpFactoryReset();
	const INT	OpAssignMotorID();
	const INT	OpHomingAll();
	const INT	OpChangeDeviceInformation();

protected:
	typedef enum _COMMAND_CODE
	{
		cmd_empty = 0,
		cmd_change_information,
		cmd_enter_monitoring_mode,
		cmd_exit_monitoring_mode,
		cmd_enable_servo,
		cmd_disable_servo,
		cmd_alarm_reset,
		cmd_restart_system,
		cmd_rescan_slave,
		cmd_normal_stop,
		cmd_emergency_stop,
		cmd_jog_cw,
		cmd_jog_ccw,
		cmd_position_table,
		cmd_origin,
		cmd_start_read_parameter,
		cmd_start_restoration,
		cmd_start_factory_reset,
		cmd_start_assign_id,
		cmd_motiongate_alarm_reset,
		cmd_read_motiongate,
	}	COMMAND_CODE;

	typedef union _GATE_COMMAND_FLAG
	{
		DWORD dwFlag;

		struct
		{
			//	BYTE1
			BYTE index;
			
			//	BYTE2
			BYTE bitChangeInformation : 1;
			BYTE bitMonitoringMode : 1;
			BYTE bitAlarmReset : 1;
			BYTE bitStopExecution : 1;
			BYTE bitBackup : 1;
			BYTE bitBackupNominal : 1;
			BYTE bitRestoration : 1;
			BYTE bitFactoryReset : 1;

			//	BYTE3
			BYTE bitAssginID : 1;
			BYTE bitRescanSlave : 1;
			BYTE bitReadParameter : 1;
			BYTE bitReboot : 1;
			BYTE bitMotionAlarmReset : 1;
		};
		
	}	GATE_COMMAND_FLAG;

	typedef union _MOTOR_COMMAND_FLAG
	{
		ULONGLONG dwFlag;

		struct
		{
			BYTE		btIndex;
			BYTE		bitServoOn : 1;
			BYTE		bitServoOff : 1;
			BYTE		bitStop : 1;
			BYTE		bitEStop : 1;
			BYTE		bitJogSpeedHigh : 1;
			BYTE		bitJogPlus : 1;
			BYTE		bitJogMinus : 1;
			BYTE		bitOrigin : 1;
			BYTE		bitPtOn : 1;
		};
	}	MOTOR_COMMAND_FLAG;

	typedef struct _MOTOR_COMMAND_TIMER
	{
		WORD servoOnDelay;
		WORD servoOffDelay;
		WORD homeFail;
		WORD originDelay;
	}	MOTOR_COMMAND_TIMER;

	typedef struct _MOTOR_COMMAND
	{
		MOTOR_COMMAND_FLAG	command;
		MOTOR_COMMAND_TIMER	timer;
	}	MOTOR_COMMAND;

	typedef struct _COMMAND_INFO
	{
		ULONGLONG			timeStamp[9];
		GATE_COMMAND_FLAG	cmdGate;
		MOTOR_COMMAND		cmdMotor[9];
	}	COMMAND_INFO;

	COMMAND_INFO m_stCommand;

protected:
	const DWORD		GetCommand(const INT iAxisNo);
	const INT		FindNextMotorIndex( const INT nIndex = 1);
	const BOOL		SaveCurrentParameterToDataBase(const BOOL bNominalData = FALSE);
	const BOOL		LoadCurrentParameterFromDataBase();
	
protected:
	const INT		DoCmdConnect();
	const INT		DoCmdClose();
	const INT		DoCmdScanSlave();
	const INT		DoCmdGetSlaveInfoQuick();
	const INT		DoCmdGetLifeCounter(const INT nMotorIndex);
	const INT		DoCmdGetAxisStatus(const INT nMotorIndex);
//	const INT		DoCmdGetStatusAll(const INT nMotorIndex);
	const INT		DoCmdGetMotiongateSystemStatus(const INT nMotorIndex);
	const INT		DoCmdResetMotiongateAlarm();
	const INT		DoCmdMoveStop(const INT nMotorIndex);
	const INT		DoCmdEnableServo(const INT nMotorIndex, const BOOL bEnable);
	const INT		DoCmdMoveVelocity(const INT nMotorIndex, const INT nSpeed, const BOOL bIncDirection);
	const INT		DoCmdEnableAdministrationMode(const BOOL bEnable);
	const INT		DoCmdChangeMotiongateInformation();
//	const INT		DoCmdChangeMotiongateName(LPCTSTR lpszDeviceName);
	const INT		DoCmdReadMotiongateInfo();
	const INT		DoCmdSetParameter(const INT nMotorIndex, const INT nParameterIndex, const LONG lValue);
	const INT		DoCmdGetParameter(const INT nMotorIndex, const INT nParameterIndex, LONG* pValue);
	const INT		DoCmdGetSavedParameter(const INT nMotorIndex, const INT nParameterIndex, LONG* pValue);
	const INT		DoCmdExecutePositionTable(const INT nMotorIndex, const INT nTableIndex);
	const INT		DoCmdSetPositionTableItem(const INT nMotorIndex, const INT nTableIndex, ITEM_NODE* pItem);
	const INT		DoCmdGetPositionTableItem(const INT nMotorIndex, const INT nTableIndex, ITEM_NODE* pItem);
	const INT		DoCmdComparePositionTableItem(const INT nMotorIndex, const INT nTableIndex, ITEM_NODE* pItem);
	const INT		DoCmdSaveAllParameters(const INT nMotorIndex);
	const INT		DoCmdSavePositionTableItems(const INT nMotorIndex);
	const INT		DoCmdLoadPositionTableItems(const INT nMotorIndex);
	const INT		DoCmdChangeSlaveID(const INT nCurMotorIndex, const INT nNewMotorIndex);
	const INT		DoCmdRebootMotiongate();
	const INT		DoCmdSearchOrigin(const INT nMotorIndex);
	const INT		DoCmdNotifyBackupDone();

/// <summary>
/// For Simple Status
/// </summary>
protected:
//	CMutex	m_mutexSimpleStatus;
//	TCHAR	m_strSimpleStatus[MAX_PATH];

protected:
	void	BuildSimpleStatus(const INT nMsgID, const LRESULT lParam = -1);

public:
//	LPCTSTR			GetSimpleStatus();
	void			GetSimpleStatus(CString& strStatus);

/// <summary>
/// For Detail Status
/// </summary>
protected:
	CMutex	m_mutexDetailStatus;
	TCHAR	m_strDetailStatus[MAX_PATH];

protected:
	void	BuildDetailStatus(const INT nMsgID, const LPARAM lParam);

public:
	LPCTSTR	GetDetailStatus(INT* pPos = NULL, INT* pMin = NULL, INT* pMax = NULL);

protected:
	INT		m_nDetailProgressPos, m_nDetailProgressMin, m_nDetailProgressMax;

protected:
	void	BuildDetailProgress(const INT nMsgID, const LPARAM lParam);

public:
	const UINT		GetOpCode();
	const DWORD		GetMotiongateStatus();
	const DWORD		GetConnectedMotorStatus();
	const DWORD		GetMotorStatus(const INT nAxisNo);
	const DOUBLE	GetMotorCurPosition(const INT nAxisNo);
	const DWORD		GetMotorCurPositionTable(const INT nAxisNo);
	const DWORD		GetMotorErrorCode(const INT nAxisNo);

	const BOOL		IsMotiongateConnected();
	const BOOL		IsMotiongateControllable( const BOOL bCheckGrant = TRUE, const BOOL bCheckCondition = TRUE );
//	const BOOL		IsDeviceNameValid();
	const BOOL		IsDeviceInformationValid();

	const BOOL		EnableMonitoringMode(const BOOL bEnable);
	const BOOL		IsMonitoringModeEnabled();
	const BOOL		IsMonitoringModeActivated();

	const BOOL		IsMotorScanned(const INT nAxisNo);
	const BOOL		IsMotorConnected(const INT nAxisNo);
	const BOOL		IsBareMotorConnected();
	const BOOL		IsInvalidMotorConnected();

	const DWORD		EnableServo(const DWORD dwIndex);
	const DWORD		EnableServo(const INT nAxisNo, const BOOL bEnable);
	const BOOL		IsServoEnabled(const INT nAxisNo);

	const BOOL		JogPlus(const DWORD dwIndex, const BOOL bHighSpeed = FALSE);
	const BOOL		JogPlus(const INT nAxisNo, const BOOL bHighSpeed = FALSE);

	const BOOL		JogMinus(const DWORD dwIndex, const BOOL bHighSpeed = FALSE);
	const BOOL		JogMinus(const INT nAxisNo, const BOOL bHighSpeed = FALSE);

	const BOOL		EmergencyStopIndex(const DWORD dwIndex);
	const BOOL		EmergencyStop(const INT nAxisNo);

	const BOOL		MoveStopIndex(const DWORD dwIndex);
	const BOOL		MoveStop(const INT nAxisNo);

	const BOOL		ExecutePositionTableIndex(const DWORD dwIndex, const INT nTableNo);
	const BOOL		ExecutePositionTablex(const INT nAxisNo, const INT nTableNo);

	const BOOL		ExecuteOriginSerachIndex(const DWORD dwIndex);
	const BOOL		ExecuteOriginSerach(const INT nAxisNo);

	const BOOL		IsMotiongateAlarmEnabled();
	const BOOL		AlarmResetIndex(const DWORD dwIndex);
	const BOOL		AlarmReset(const INT nAxisNo);
	const BOOL		AlarmReset();
	const BOOL		RescanSlave();
	
	const BOOL		IsHoming(const INT nAxisNo);
	const BOOL		IsHomingOK(const INT nAxisNo);
	const BOOL		IsHomingNG(const INT nAxisNo);

	const BOOL		StopExecution();
	const BOOL		StartBackup();
	const BOOL		StartReadParameter();

	void			SetProjectName(LPCTSTR lpszProjectName);
	LPCTSTR			GetProjectName();
//	const INT		ChangeName(LPCTSTR lpszName, const BYTE btOption);

	void			SetBackupName(LPCTSTR lpszBackupName);
	LPCTSTR			GetBackupName();

	void			SetNominalBackup(const BOOL bActivate = TRUE);

	const BOOL		IsBusy();
	const BOOL		IsGrantedToStop();

	void			SelectBackupID(const INT nBackupID);
	const INT		GetSelectedBackupID();
	
	const BOOL		StartRestoration();
	const BOOL		StartFactoryReset(const INT nMotorIndex);
	
	void			SelectAssignID(const INT nTargetID, const INT nSourceID = 0);
	const BOOL		StartAssignID();
	
	void			SetHomingTimeout(const INT nTime, const BOOL bSecond = TRUE);	// Unit : bSecond = TRUE, second // bSecond = FALSE, milisecond
	const INT		GetHomingTimeout(const BOOL bSecond = TRUE);
	const INT		GetDatabaseHomingTimeout(const BOOL bSecond = TRUE);

	void			SetEditHomingTimeout(const INT nTime, const BOOL bSecond = TRUE);	// Unit : bSecond = TRUE, second // bSecond = FALSE, milisecond
	const INT		GetEditHomingTimeout(const BOOL bSecond = TRUE);

protected:
	BOOL m_bAutoHoming, m_bAutoRestoration;

public:
	void			EnableAutoHoming(const BOOL bEnable = TRUE);
	const BOOL		IsAutoHomingEnabled();

	void			EnableAutoRestoration(const BOOL bEnable = TRUE);
	const BOOL		IsAutoRestorationEnabled();

	const BOOL		GetAutoBackupDatetime( COleDateTime& dateTime );

protected:
	INT m_nLastFasResult;

public:
	const INT GetLastFasResult();


public:
	typedef enum _MOTOR_TOTAL_STATUS
	{
		motor_status_unknown = 0,	//	통신 안함
		motor_status_caution,		//	주의		//	주로 관리자 모드 관련 동작에서 활성화
		motor_status_warning,		//	경고		//	Disconnect에서 사용함
		motor_status_error,			//	에러
		motor_status_initialize,	//	초기화
		motor_status_ok,			//	문제 없음
	}	MOTOR_TOTAL_STATUS;

public :
	typedef enum _MOTION_ALARM_STATUS
	{
		motion_status_push_detection_fail = 20,
		motion_status_eot_not_match = 25,
		motion_status_teaching_pendant_connected = 29,
		motion_status_motor_disconnected = 30,
		motion_status_motor_port_disconnected,
	}MOTION_ALARM_STATUS;


public:
	const INT GetMotorsTotalStatus( const INT nMotorIndex );

//public:
//	typedef enum _OPTION_CHANGE_NAME
//	{
//		option_change_name_register = 0x01,	//	device 이름을 register 이름과 일치시킨다.
//		option_change_name_device	= 0x02,	//	register 이름을 device 이름과 일치시킨다.
//		option_change_name_database = 0x10,	//	바뀌는 Register에 맞춰서 Data 모두를 변경한다.
//	}	OPTION_CHANGE_NAME;

protected:
	BOOL				m_bBackupReservation;
	INT					m_nPauseBackupCounter;
	COleDateTime		m_timeAutoBackup, m_timeAutoBackupTickTock, m_timeLastReadParameter;
	COleDateTimeSpan	m_timespanAutoBackup;

public:
	void			CheckAutoBackupSchedule();
	const BOOL		IsAutoBackupScheduleExist();

	void			PauseAutoBackup(const BOOL bPause = TRUE);
	
	const BOOL		GetParameter(const INT nMotorIndex, MOTOR_PARAMETER_DATA* pParameter);
	COleDateTime&	GetLastReadParameter();
	const DOUBLE	GetAutoBackupLeftSeconds();

protected:
	typedef enum _STATUS_TIME_STAMP
	{
		sts_connection,
		sts_alarm,
		sts_error,
		sts_backup,
		sts_restore,
		sts_reset,
		sts_assign,
		sts_max,
	}	STATUS_TIME_STAMP;

	typedef enum _STATUS_FLAG_CONNECTION
	{
		sts_flag_connection_stop				= 0x00000001,
		sts_flag_connection_start				= 0x00000002,
		sts_flag_connection_disconnected		= 0x00000004,
		sts_flag_connection_connected			= 0x00000008,
		sts_flag_connection_off_line			= 0x00000010,
		sts_flag_connection_check_motiongate	= 0x00000020,
		sts_flag_connection_check_slave			= 0x00000040,
		sts_flag_connection_system_ready		= 0x00000080,
		sts_flag_connection_reboot				= 0x00000100,
	}	STATUS_FLAG_CONNECTION;

	typedef enum _STATUS_FLAG_ALARM
	{
		sts_flag_alarm_motor0					= 0x00000001,
		sts_flag_alarm_motor_count				= 0x00000002,
		sts_flag_alarm_restoration				= 0x00000004,
		sts_flag_alarm_all_homing				= 0x00000008,
	}	STATUS_FLAG_ALARM;

	typedef enum _STATUS_FLAG_ERROR
	{
		sts_flag_error_none						= 0x00000001,
		sts_flag_error_no_response_motion_gate	= 0x00000002,
		sts_flag_error_no_response_motor0		= 0x00000010,
		sts_flag_error_no_response_motor1		= 0x00000020,
		sts_flag_error_no_response_motor2		= 0x00000040,
		sts_flag_error_no_response_motor3		= 0x00000080,
		sts_flag_error_no_response_motor4		= 0x00000100,
		sts_flag_error_no_response_motor5		= 0x00000200,
		sts_flag_error_no_response_motor6		= 0x00000400,
		sts_flag_error_no_response_motor7		= 0x00000800,
		sts_flag_error_no_response_motor8		= 0x00001000,
		
		sts_flag_error_motiongate				= 0x00010000,
		sts_flag_error_motor0					= 0x00100000,
		sts_flag_error_motor1					= 0x00200000,
		sts_flag_error_motor2					= 0x00400000,
		sts_flag_error_motor3					= 0x00800000,
		sts_flag_error_motor4					= 0x01000000,
		sts_flag_error_motor5					= 0x02000000,
		sts_flag_error_motor6					= 0x04000000,
		sts_flag_error_motor7					= 0x08000000,
		sts_flag_error_motor8					= 0x10000000,

		sts_flag_error_no_motor					= 0x20000000,
	}	STATUS_FLAG_ERROR;

	typedef enum _STATUS_FLAG_BACKUP
	{
		sts_flag_backup_run						= 0x00000001,
		sts_flag_backup_stop					= 0x00000002,
		sts_flag_backup_good					= 0x00000004,
		sts_flag_backup_bad						= 0x00000008,
		sts_flag_comapare_start					= 0x00000010,
		sts_flag_comapare_stop					= 0x00000020,
		sts_flag_comapare_bad					= 0x00000040,
		sts_flag_comapare_end					= 0x00000080,
	}	STATUS_FLAG_BACKUP;

	typedef enum _STATUS_FLAG_RESTORE
	{
		sts_flag_restore_run					= 0x00000001,
		sts_flag_restore_stop					= 0x00000002,
		sts_flag_restore_good					= 0x00000004,
		sts_flag_restore_bad					= 0x00000008,
		sts_flag_restore_all_home_run			= 0x00000010,
		sts_flag_restore_all_home_stop			= 0x00000020,
		sts_flag_restore_all_home_good			= 0x00000040,
		sts_flag_restore_all_home_bad			= 0x00000080,
		sts_flag_restore_all_home_finish		= 0x00000100,
	}	STATUS_FLAG_RESTORE;

	typedef enum _STATUS_FLAG_RESET
	{
		sts_flag_reset_run						= 0x00000001,
		sts_flag_reset_stop						= 0x00000002,
		sts_flag_reset_good						= 0x00000004,
		sts_flag_reset_bad						= 0x00000008,
	}	STATUS_FLAG_RESET;

	typedef enum _STATUS_FLAG_ASSIGN
	{
		sts_flag_assign_run						= 0x00000001,
		sts_flag_assign_stop					= 0x00000002,
		sts_flag_assign_good					= 0x00000004,
		sts_flag_assign_bad						= 0x00000008,
	}	STATUS_FLAG_ASSIGN;

	COleDateTime	m_dateTimeStatus[sts_max];
	DWORD			m_dwStatusFlag[sts_max];

protected:
	const BOOL	SetStatusConnectionStop();
	const BOOL	SetStatusConnectionStart();
	const BOOL	SetStatusConnectionDisconnected();
	const BOOL	SetStatusConnectionConnected();
	const BOOL	SetStatusConnectionBusy();
	const BOOL	SetStatusConnectionCheckMotiongate();
	const BOOL	SetStatusConnectionCheckSlave();
	const BOOL	SetStatusConnectionSystemReady();
	const BOOL	SetStatusConnectionReboot();
	
	const BOOL	SetStatusAlarmMotor0(const BOOL bEnable = TRUE);
	const BOOL	SetStatusAlarmMotorCount(const BOOL bEnable = TRUE);
	const BOOL	SetStatusAlarmAllHoming(const BOOL bEnable = TRUE);

	const BOOL	SetStatusNoResponseMotiongate(const BOOL bEnable = TRUE);
	const BOOL	SetStatusNoResponseMotor(const INT nMotorID, const BOOL bEnable = TRUE);
	const BOOL	SetStatusErrorMotiongate( const BOOL bError = FALSE );
	const BOOL	SetStatusErrorMotor(const INT nMotorID, const BOOL bError = FALSE);

	const BOOL	SetStatusBackupRun();
	const BOOL	SetStatusBackupStop();
	const BOOL	SetStatusBackupGood();
	const BOOL	SetStatusBackupBad();

	const BOOL	SetStatusRestoreRun();
	const BOOL	SetStatusRestoreStop();
	const BOOL	SetStatusRestoreGood();
	const BOOL	SetStatusRestoreBad();

	const BOOL	SetStatusAllHomeRun();
	const BOOL	SetStatusAllHomeStop();
	const BOOL	SetStatusAllHomeGood();
	const BOOL	SetStatusAllHomeBad();

	const BOOL	SetStatusResetRun();
	const BOOL	SetStatusResetStop();
	const BOOL	SetStatusResetGood();
	const BOOL	SetStatusResetBad();

	const BOOL	SetStatusAssignRun();
	const BOOL	SetStatusAssignStop();
	const BOOL	SetStatusAssignGood();
	const BOOL	SetStatusAssignBad();

	const DWORD	ScanFastStatus(const DWORD dwFlag);

public:
	const INT	GetLastSimpleStatus();
	const DWORD	GetLastSimpleStatusFlag(const INT nSimpleStatus);
	void		GetLastSimpleStatus(const INT nSimpleStatus, CString& strStatus);
	void		GetLastSimpleStatus(CString& strStatus);

protected:
	BOOL	m_bEditMode;

public:
	void	EnterEditMode();
	void	ExitEditMode(const BOOL bApplyEditProperty = TRUE);

	const BOOL	RebootSystem();

protected:
	virtual const BOOL Copy(CDeviceMotionGate* pTarget, CDeviceMotionGate* pSource);

public:
	CDeviceMotionGate* Clone();
	const BOOL	ApplyProperty(CDeviceMotionGate* pSource);

public:
	const BOOL	CompareMotiongateInfo();

public:
	const BOOL	ChangeSystemInformation();

public:
	const BOOL IsSystemStatusBusy();
	const BOOL IsSystemStatusWarning();
	const BOOL IsSystemStatusError();

protected:	//	OpCode Based functions
	
	//	State Code도 조금 더 Detail하게 만들어야 할텐데?
	//	MainState / SubState로 가야하나?	
	//	Return Code는 무엇으로?

	const DWORD OpFunc_MainBifurcation();			//	여기서 관련 예하 함수들로 분기		//	Operation Stop도 여기서?
	const DWORD OpFunc_Connect();
	const DWORD OpFunc_ReConnect();					//	Reboot를 통한 재접속을 위한
	const DWORD OpFunc_ReadMotiongateInfo();
	const DWORD OpFunc_ScanSlave();
	const DWORD OpFunc_ReadMotiongateStatus();
	const DWORD OpFunc_ManualBifurcation();			//	여기서 수동 제어 예하 함수들로 분기
	const DWORD OpFunc_ReadAdminitrationMode();
	const DWORD OpFunc_WriteAdminitrationModeOn();
	const DWORD OpFunc_WriteAdminitrationModeOff();
	const DWORD OpFunc_ControlServoOn();
	const DWORD OpFunc_ControlServoOff();
	const DWORD OpFunc_ControlStop();
	const DWORD OpFunc_ControlEStop();				//	Servo Off가 E-Stop의 기능을 대체한다.
	const DWORD OpFunc_ControlJog();
	const DWORD OpFunc_ControlHoming();
	const DWORD OpFunc_ControlPositionTable();
	const DWORD OpFunc_ControlReboot();
	const DWORD OpFunc_WaitServoOn();				//	동작 안정화를 위한 대기
	const DWORD OpFunc_WaitServoOff();				//	동작 안정화를 위한 대기
	const DWORD OpFunc_WaitTimeout();				//	동작 완료 혹은 지정 시간 초과를 위한 대기
	const DWORD OpFunc_ReadParameter();
	const DWORD OpFunc_ReadFlashParameter();
	const DWORD OpFunc_ReadPositionTable();
	const DWORD OpFunc_WriteParameter();
	const DWORD OpFunc_WritePositionTable();
	const DWORD OpFunc_SaveParameter();
	const DWORD OpFunc_SavePositionTable();
	const DWORD OpFunc_LoadPositionTable();
	const DWORD OpFunc_WriteDeviceID();
public:
	void SetEditDeviceID(INT nID);
	const BOOL DoMGFactoryReset(bool bAct = TRUE);
	const INT GetEditDeviceID();
	const INT GetDatabaseDeviceID();

public:

	typedef union _PLC_REQ_INFORM
	{
		WORD wFlag[12];

		struct
		{
			//	WORD0
			WORD wEEno;
			
			//	WORD1
			BYTE byPTNo;
			BYTE byMotorNo;

			//	WORD
			WORD wErrorCode;

			// WORD 
			LONG lPos;

			//	WORD
			WORD wSpd;
		};
		
	}	PLC_REQ_INFORM;

	PLC_REQ_INFORM* m_pPLCReqInform;

	CMap<DWORD, DWORD, PLC_REQ_INFORM*, PLC_REQ_INFORM*>	m_mapPLCReqInform;

	BOOL m_bCheckInform;
	BOOL m_bEndTimeCheck;
	INT m_nPrevCount;
	DWORD	m_dwProgressStartTickCount;
	DWORD	m_dwProgressStopTickCount;
	DWORD	m_dwProgressEndTime;
	BOOL	m_bProgressTimer;
	BOOL	m_bLogChanged;
	BOOL	m_bAllalarm;
	
	typedef struct _DEVICE_LOG_STATUS_STRING
	{
		CString strNormal;
		CString strWarning;
		CString strDeviceError;
		CString strMotorError[9];
	}	DEVICE_LOG_STATUS_STRING;

	DEVICE_LOG_STATUS_STRING m_CurLog;
	DEVICE_LOG_STATUS_STRING m_PrevLog;
	
	typedef struct _ST_OPERAION_LOG_INFO
	{
		CDeviceMotionGate* pDevice;
		INT					nLevel;
		INT					nMsgID;
		BOOL				bEnableLog;
	}	ST_OPERAION_MESSAGE, * PST_OPERAION_MESSAGE;

	ST_OPERAION_MESSAGE m_Msg;
	
	ITEM_NODE GetPLCReqNodeItem(const INT nPTNo, const INT nMotorNo);
	WORD GetModBusResPonseCmd(const INT nMotorNo, const INT nPTNo, const INT nCmd, const LONG lPos);
	DOUBLE ConvertPulseToMillimeter(MOTOR_PARAMETER_DATA* pstParameter, const LONG lPosition);
	BYTE GetMotionGateErrorCode(BYTE byPTNo = 0);

	typedef enum _STATUS_MOTIONGATE_ERROR_CODE
	{
		// status
		sts_status_normal = 0,
		sts_status_stop,
		sts_status_connect,
		// busy
		sts_status_reboot, // (3)
		sts_busy_backup,
		sts_busy_resoration,
		sts_busy_factory_reset,
		sts_busy_id_assign,
		sts_busy_homing,
		sts_busy_change_inform,
		// warning
		sts_warning_homing = 0x40,
		sts_warning_teaching_pendant,
		// error
		sts_error_stop_motion_while_try_connect = 0x80,
		sts_error_motion_busy_while_try_connect,
		sts_error_read_motion_while_try_connect,
		sts_error_read_param_while_try_connect,
		sts_error_read_pt_while_try_connect,
		sts_error_enter_monitormode_while_try_connect,
		sts_error_exit_monitormode_while_try_connect,
		sts_error_check_monitor_on_while_try_connect,
		sts_error_check_monitor_off_while_try_connect,
		sts_error_check_device_name_while_try_connect,
		sts_error_change_device_info_while_try_connect,
		sts_error_write_param_while_try_connect,
		sts_error_write_pt_while_try_connect,
		sts_error_save_param_while_try_connect,
		sts_error_save_pt_while_try_connect,
		sts_error_confirm_param_while_try_connect,
		sts_error_load_pt_while_try_connect, // 0x90
		sts_error_confirm_pt_while_try_connect,
		sts_error_change_motiongate_while_try_connect,
		sts_error_reboot_motiongate_while_try_connect,
		sts_error_homing_servo_on_while_try_connect,
		sts_error_homing_start_while_try_connect,
		sts_error_homing_wait_while_try_connect,
		sts_error_homing_stop_while_try_connect,
		sts_error_discord_current_init_motor_status_while_try_connect,
		sts_error_discord_current_connected_motor_count_while_try_connect,
		sts_error_discord_motorcount_in_db_while_try_connect,
		sts_error_system_no_name,
		sts_error_no_response = 0xfe, // motiongate
		sts_error_disconnect	// converter
	}	STATUS_MOTIONGATE_ERROR_CODE;

	typedef enum _STATUS_MOTOR_ERROR_CODE
	{
		sts_error_motor_none = 0,
		sts_error_motor_overcurrent = 1, // [과전류 이상]
		sts_error_motor_overspeed, // [과속도 이상], 속도가 3,000 rpm을 초과할경우
		sts_error_motor_position_tracking, // [위치 추종 이상], 모터회전중 위치 명령값과 실제 위치값의 차이가 180도 이상일 경우
		sts_error_motor_overload, // [과부하 이상], 모터의 최대 토크를 초과하는 부하가 5초이상 가해졌을경우
		sts_error_motor_overheat = 5, // [과열 이상], 드라이브의 내부온도가 85도 이상 초과하는 경우
		sts_error_motor_over_regeratived_voltage, // [회생 전압 이상], 모터의 역기전력 전압이 한계를 초과하였을경우
		sts_error_motor_connection, // [모터 접속 이상], 드라이브와 모터의 연결에 이상이 있을경우
		sts_error_motor_encoder_connection, // [엔코더 접속 이상], 드라이브와 모터의 연결에 이상이 있을경우
		sts_error_motor_motor_voltate_error, // 
		sts_error_motor_inposition_error = 10, // [인포지션 이상], 운전 완료후 1펄스 이상의 위치 오차가 3초이상 발생하였을경우
		sts_error_motor_drive_system_error, // [시스템 이상], 드라이브 시스템에 이상이 발생하였을경우
		sts_error_motor_drive_rom_error = 12, // [ROM 이상], 파라미터 저장 장치(ROM)에 이상이 발생하였을경우
		sts_error_motor_drive_voltage_error = 14,
		sts_error_motor_position_overflow, // [위치 오차 이상], 모터 정지 상태에서 180도 이상의 위치 오차가 발생한 경우
		sts_error_motor_position_change, // [new. 차종 위치 변동]
		sts_error_motor_break_short, // [new. 브레이크 단선]
		sts_error_motor_struct_postion_change, // [new, 기구 위치 변동]
		sts_error_motor_no_clam_load = 20, // 20
		sts_error_access_motor_not_exist,
		sts_error_access_motor_out_of_range, // [모터 접근 범위 초과 이상], 1 ~ 8 번모터 외 접근
		sts_error_access_pt_not_exist, // [포지션 테이블 0번 접근 이상]
		sts_error_access_pt_out_of_range, // [포지션 테이블 접근 범위 초과 이상], PT 번호가 30번 이상 접근할경우
	}	STATUS_MOTOR_ERROR_CODE;

	


	OPERATIONLOG_INFORMATION m_opLogInform;

	BYTE GetSystemStatusError();
	BYTE GetSystemStatusWarning();
	BYTE GetSystemStatus();
	BYTE GetMotorError(INT nAxis, INT nPTNum);
	const BOOL UpdateDeleteValue(BOOL bDelete = false);
	const BOOL UpdateRestoreBackupListTable();
	const BOOL CopyList(CString strOrigin, CString strTarget);
	const INT GetDeletedStatus();
	void MotionGateInformRequest(BOOL bRun);

	
	const BOOL CheckRunState();
	const BOOL GetMotionGateInformRequest();
	
	void SetRun(BOOL bRun);

	void StartProgressTimer(BOOL bStart);
	void StopProgressTimer(BOOL bStart);
	DWORD GetProgressTime();
	void SetEndTimeCheck(BOOL bEnd);
	BOOL GetEndTimeCheck();
	void SetProgressEndTime(DWORD dwTime);
	DWORD GetProgressEndTime();
	void GetMotorErrorStatus(CString& strError, INT nIndex);
	BOOL IsMotorExist(INT nAxis);
	
	void InsertLog(INT nLevel, CString strInform);
	CString GetCurrentSystemStatus(INT nLevel);
	BOOL LogDataChanged();
	BOOL SetOperationLogAll(INT nCursel);
	void SaveLogStatus(const INT nMsgID, const CString strDetail);
	void SaveLogMotionGateStatus(const INT nMsgID, CString strDetail);
	void BuildSubSystemStatusWarning();
	void SaveSubSystemStatus(const INT nMsgID = 0);
	void SaveLogMotorStatus();
	CBitmap* LoadImageFromDatabase(LPVOID pDatabaseRecordData, CString strProjectName);
	const BOOL IsMotorBusy(INT nAxis);
	CString GetMotorErrorText(INT nIndex);
	void ServoControl(INT nIndex, BOOL bEnable);
	void InitMotorState();
	
	void SetCommandTestResult(CString strCommand, INT nResult);
	CString m_strCmdTest;
	CString GetCommandTestResult();
	void ActiveOperationLog(INT nState, CString strMessage);
	const BOOL GetMasterConnectStatus();
	const DWORD GetMotorLifeCount(const INT nAxisNo);
	void SetPTLifeAlarmDetected(INT nAxis, BOOL bAlarm = FALSE);
	void SetPTLifePeriodAlarmDetected(INT nAxis ,INT nPortion, BOOL bAlarm= FALSE);
	BOOL GetPTLifeAlarmDetected(INT nAxis);
	DWORD GetLifePeriodAlarmCount(INT nAxis);
	BOOL m_bMotorError[9];
	BOOL m_bLifeCountAlarm;
	BOOL m_bLifeCountPeriodAlarm;
	BOOL m_bMotorChangeDetected;
	BYTE m_byMotorChange;
	BOOL m_bAutoBackupError;
	BOOL m_bKeepStopAutoBackup;
	BOOL m_bHomingStop;
	BOOL m_bSetHomingClose;
	BOOL m_bSendDoneMessage;
	void IncreseMotorLifeCount(INT nValue);
	void SetLifeCountStandard(DWORD dwValue);
	void SetLifeCountCycle(DWORD dwValue);
	DWORD GetLifeCountCycle();
	DWORD GetLifeCountStandard();
	INT CmdConnectionClose();
	BOOL GetMotionGateConnectedStatus();
	CString GetLastNomalStatus();
	CString GetLastWarningStatus();
	CString GetLastDeviceErrorStatus();
	CString GetLastMotorErrorStatus(INT index);
	INT GetBackupState();
	CBitmap* GetSystemEditImage();
//	CBitmap GetSystemDatabaseImage();
	BOOL GetSystemImageBitmap(CBitmap* pBitmap);
	const BOOL CompareNoticeInfo();
	void SetAutoBackupReservation(BOOL bEnable);
	const BOOL CompareMotionGateInfoEtc();
	const BOOL IsChangedMotorConnection();
	const BOOL SetStatusCompareStart();
	const BOOL SetStatusCompareEnd();
	void ResetMotionGateAlarm();
	const INT DoCmdMotionGateAlarmReset();
	const BOOL SetStatusCompareStop();
	const BOOL SetStatusCompareBad();
	const BOOL IsBackupState();
	void SetPTLifeCycleAlarmDetected(INT nAxis, BOOL bAlarm);
	const BOOL GetPTLifeCycleAlarmDetected(INT nAxis);
	const BOOL CompareChangeDatabase();
	const BOOL IsStableMotorStruct();
	const BOOL IsAutoBackupError();
	void SetAutoBackupDeviceError(const BOOL bEnable);
	void ActiveHoming();
	void KeepStopAutoBackup(BOOL bStop);
	const BOOL IsKeepStopAutoBackup();
	void CheckAutoBackupEnable();
	void SetHomingStop(BOOL bStop);
	const BOOL IsUserHomingStop();
	const BOOL MatchToDevice();
	void InitDatabase();
	const BOOL CompareNameDatabase();
	const BOOL InsertProjectDatabase();
	const BOOL IsDataBaseExist(CString strName);
	const BOOL InsertNewProjectDatabase();
	void InitEditData();
	const BOOL UpdateChangeProject();
	const BOOL IsAllalarm();
	const INT GetMotorsAlarmCount();
	INT DoCmdGetAlarmType(DWORD dwAddress, INT nMotorIndex);
	INT DoCmdGetMotionGateAlarmStatus(DWORD dwAddress);
	const BOOL IsSystemHoming();
	const BOOL IsMotorAlarmDetected(INT nMotorIndex);
	const BOOL IsMotorServoOff(INT nMotorIndex);
	const BOOL IsDefaultState();
	const INT DoCmdGetMotorOrganization();
	DWORD GetOriginAxisOrganization();
	DWORD GetCurAxisOrganization();
	BOOL IsActiveHomingAll();
	void SetActiveHomingAll(const BOOL bActive);
	BOOL m_bActiveHomingAll;
	void SetInitHomingState(INT nAxis);
	BOOL IsMotorStopped(INT nAxis);
	INT OpHomingMotion(int nMotor);
	INT CmdMotorMoveStop(INT nAxis);
	void SetHomingClose(BOOL bClose);
	const BOOL IsHomingClose();
	const BOOL IsAllMotorServoOff();
	const BOOL IsMotorOriginOK(INT nIndex);
	void SendHomingDoneMessage(BOOL bClose);
	const BOOL SetStatusAllHomeFinish();
	const BOOL SetStatusNoMotorExist(const BOOL bEnable = TRUE);
	const BOOL IsMotionGateMotorExist();
};
