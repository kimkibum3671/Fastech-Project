#pragma once

#include "CDeviceMotionGate.h"

class CDeviceList
{
protected:
	CDeviceList();

public:
	~CDeviceList();

public:
	static CDeviceList* GetInstance();

protected:
	CMap<INT, INT, DWORD, DWORD>	m_mapDevicePriorityToAddress;	//	IP Address Priority로 Address를 검색
	CMap<DWORD, DWORD, INT, INT>	m_mapDeviceAddressToPriority;	//	IP Address Address로 Priority를 검색

	CMap<DWORD, DWORD, CDeviceMotionGate*, CDeviceMotionGate*>	m_mapDeviceAddress;		//	IP Address 연결 정보

	CMap<DWORD, DWORD, INT, INT>	m_mapDeviceAddressToDeviceID;
	CMap<INT, INT, DWORD, DWORD>	m_mapDevicIDToAddress;

	CMutex m_mutex;
	BOOL m_bRun, m_bTerminate;
	CTime m_timeStamp;
	INT m_nThreadCount;	

protected:
	void RemoveAll();

public:
	void ResetTimeStamp( CTime time = CTime::GetCurrentTime() );
	CTime& GetTimeStamp();

	void IncreaseThreadCount();
	void DecreaseThreadCount();

	void ResetAddressIndex();
	void AddAddress(const INT nPriority, const DWORD dwAddress);
	void RebuildAddressIndex(const BOOL bApplyDatabase = FALSE);
	
	const INT	GetAddressCount(const BOOL bListMode = FALSE);
	const DWORD GetAddressByIndex(INT nIndex, const BOOL bListMode = FALSE);

	void RebuildMotionGateIndex();

	const BOOL StartMonitoring();
	const BOOL IsMonitoringEnabled( const DWORD dwAddress = 0);
	const BOOL IsTerminateEnabled();
	const BOOL StopMonitoring( const BOOL bWait = FALSE);

	const BOOL	LockMutex(const DWORD dwTimeout = INFINITE );
	const BOOL	UnlockMutex();

	const INT GetRegisteredDeviceCount();
	CDeviceMotionGate* GetMotiongateByPriority(const INT nIndex);

	void RegisterDevice(CDeviceMotionGate* pDevice);

	//	Temp가 기본임
	void AddMotionGate( const DWORD dwAddress, LPCTSTR lpszName, const INT nMotorCount, const INT nHomingTimeout);
	void ApplyMotionGate();
	const BOOL RemoveMotionGate(const DWORD dwAddress);
	
	const BOOL GetMotiongateByAddress(const DWORD dwAddress, CDeviceMotionGate*& pDevice);
	const BOOL GetMotiongateByName(LPCTSTR lpszSystemName, CDeviceMotionGate*& pDevice);

public:
	typedef enum _DISPLAY_METHOD
	{
		display_address = 0,
		display_name,
	}	DISPLAY_METHOD;

protected:
	INT m_nDisplayMethod;

public:
	void		SetDisplayMethod(const INT nMethod);
	const INT	GetDisplayMethod();

protected:
	void Initialize();

public:
	void ResetDevicePriority();
	void UpdateDeviceInformation();
	const BOOL ChangeAddress(const DWORD dwOrgAddress, const DWORD dwNextAddress);
	
	void DeleteObject(CDeviceMotionGate* pDevice);

protected:
	CDeviceMotionGate m_deviceSerial;

public:
	CDeviceMotionGate* GetMotiongateForSerial();

protected:
	CWinThread* m_pStartingThread;

	static UINT StartingThread(LPVOID pParam);
public:
	void AddDeviceID(const DWORD dwAddress, const INT nDeviceID);
	const BOOL GetMotionGateByDeviceID(const INT nIndex, CDeviceMotionGate*& pDevice);
	void ResetDeviceID();
	const BOOL RemoveDeviceIDKey(const DWORD dwAddress);
	const BOOL IsStartMonitoring();
};
