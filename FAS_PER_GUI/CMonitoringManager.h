#pragma once
class CMonitoringManager
{
protected:
	CMonitoringManager();

public:
	~CMonitoringManager();

public:
	static CMonitoringManager* GetInstance();

public:
	typedef enum _MONITORING_COMMAND
	{
		CMD_START = 0,
		CMD_STOP,
	}	MONITORING_COMMAND;

	typedef enum _MONITORING_STATUS
	{
		STS_STOP = 0, 
		STS_HOLDING,
		STS_RUN,
	}	MONITORING_STATUS;

	typedef enum _THREAD_STATUS
	{
		THD_IDLE = 0x1000,
		THD_FAIL,			//	30초마다 Reconnect?
		THD_CONNECTING,		//	Connecting
		THD_SCANNING,		//	Scan items
		THD_POLLING,		//	Gateway monitoring	// Polling cycle time은 어떻게 할까?
		THD_BACKUP,			//	Backup Plus-R Data
		THD_RESTORATION,	//	Restore Plus-R Data
		THD_ASSIGNMENT,		//	Assign Plus-R ID
		THD_RESET,			//	Inialize Plus-R data and assign Plus-R ID to 0
	}	THREAD_STATUS;

protected:
	INT m_nMonitoringStatus, m_nOperationStatus;

public:
	const INT RequestCommand(const INT nCmd, const WPARAM wParam = 0, const LPARAM lParam = 0);

	const INT GetMonitoringStatus();
	const INT GetThreadCount();
	const INT GetThreadStatus( const INT nID );
};
