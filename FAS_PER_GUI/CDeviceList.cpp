#include "pch.h"
#include "CDeviceList.h"

#include "CDataBaseManager.h"

#include "MainFrm.h"

#include "CInterfaceManager.h"

CDeviceList::CDeviceList() : m_mutex()
{
	m_bRun = FALSE;
	m_bTerminate = FALSE;
	m_nThreadCount = 0;
	m_nDisplayMethod = display_address;

	m_deviceSerial.SetAddress(0xffffffff);

	Initialize();
}

CDeviceList::~CDeviceList()
{ 
	m_bTerminate = TRUE;
	StopMonitoring(TRUE);
	//this->RemoveAll();
}

CDeviceList* CDeviceList::GetInstance()
{
	static CDeviceList _instance;
	return &_instance;
}


UINT CDeviceList::StartingThread(LPVOID pParam)
{
	CDeviceList* pParent = reinterpret_cast<CDeviceList*>(pParam);

	for( INT nIndex = 0;; nIndex++)
	{
		if (FALSE == pParent->IsMonitoringEnabled())
		{
			break;
		}

		const DWORD dwAddress = pParent->GetAddressByIndex(nIndex);
		if( 0 == dwAddress)
		{
			break;
		}

		CDeviceMotionGate* pDevice = NULL;
		if (pParent->GetMotiongateByAddress(dwAddress, pDevice))
		{
			pDevice->StartThread();
			Sleep(50);
		}
	}

	pParent->m_pStartingThread = NULL;

	return 0;
}

const BOOL CDeviceList::LockMutex(const DWORD dwTimeout)
{
	return m_mutex.Lock(dwTimeout);
}

const BOOL CDeviceList::UnlockMutex()
{
	return m_mutex.Unlock();
}

void CDeviceList::RemoveAll()
{
	LockMutex();

	if (1)
	{
		POSITION pos = m_mapDeviceAddress.GetStartPosition();
		while (pos)
		{
			DWORD key = 0;
			CDeviceMotionGate* pDevice = NULL;
			m_mapDeviceAddress.GetNextAssoc(pos, key, pDevice);
			if (pDevice)
			{
				delete pDevice;
			}
		}

		m_mapDeviceAddress.RemoveAll();
	} 

	m_mapDevicePriorityToAddress.RemoveAll();
	m_mapDeviceAddressToPriority.RemoveAll();
	m_mapDeviceAddressToDeviceID.RemoveAll();
	m_mapDevicIDToAddress.RemoveAll();

	UnlockMutex();
}

void CDeviceList::IncreaseThreadCount()
{
	m_nThreadCount++;
}

void CDeviceList::DecreaseThreadCount()
{
	m_nThreadCount--;
}

void CDeviceList::ResetTimeStamp(CTime time)
{
	m_timeStamp = time;
}

CTime& CDeviceList::GetTimeStamp()
{
	return m_timeStamp;
}

void CDeviceList::ResetAddressIndex()
{
	m_mapDevicePriorityToAddress.RemoveAll();
}

void CDeviceList::AddAddress(const INT nPriority, const DWORD dwAddress)
{
	m_mapDevicePriorityToAddress.SetAt(nPriority, dwAddress);
}

void CDeviceList::AddDeviceID(const DWORD dwAddress, const INT nDeviceID)
{
	m_mapDeviceAddressToDeviceID.SetAt(dwAddress, nDeviceID);
	m_mapDevicIDToAddress.SetAt(nDeviceID, dwAddress);
}

void CDeviceList::ResetDeviceID()
{
	m_mapDeviceAddressToDeviceID.RemoveAll();
	m_mapDevicIDToAddress.RemoveAll();
}


//	Todo : Serial 관련 동작은 어떻게 할 것인지 고민
void CDeviceList::RebuildAddressIndex( const BOOL bApplyDatabase )
{
	CMap<DWORD, DWORD, INT, INT> mapIndex;

	if (1)
	{	//	현재의 AddressToPriority를 만든다.

		POSITION pos = m_mapDevicePriorityToAddress.GetStartPosition();
		while (pos)
		{
			INT nPriority = -1;
			DWORD dwAddress = 0;
			m_mapDevicePriorityToAddress.GetNextAssoc(pos, nPriority, dwAddress);
			mapIndex.SetAt(dwAddress, nPriority);
		}
	}

	if (1)
	{
		//	기존 AddressToPriority에서 현재 AddressToPriority가 없으면 기존 AddressToPriority 삭제

		POSITION pos = m_mapDeviceAddressToPriority.GetStartPosition();
		while (pos)
		{
			INT nPriority = -1;
			DWORD dwAddress = 0;
			m_mapDeviceAddressToPriority.GetNextAssoc(pos, dwAddress, nPriority);
			if (FALSE == mapIndex.Lookup(dwAddress, nPriority))
			{
				m_mapDeviceAddressToPriority.RemoveKey(dwAddress);
			}
		}
	}

	if (1)
	{	//	현재 AddressToPriority의 내용을 기존 AddressToPriority에 추가 혹은 갱신
		CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
		BOOL bDatabaseMode = FALSE;
		if (bApplyDatabase)
		{	//	Database를 연다.
			bDatabaseMode = pProjectDB->OpenDataBase();
			if (bDatabaseMode)
			{
				pProjectDB->Begin();
				pProjectDB->DeleteAddressIndexAll();
			}
			else
			{	//	문제가 있다.
				ASSERT(0);
			}
		}

		POSITION pos = mapIndex.GetStartPosition();
		while (pos)
		{
			INT nPriority = -1;
			DWORD dwAddress = 0;
			mapIndex.GetNextAssoc(pos, dwAddress, nPriority);
			m_mapDeviceAddressToPriority.SetAt(dwAddress, nPriority);
			
			CDeviceMotionGate* pDevice = NULL;
			if (FALSE == m_mapDeviceAddress.Lookup(dwAddress, pDevice))
			{// Device 생성..
				pDevice = new CDeviceMotionGate;
				pDevice->SetAddress(dwAddress);
				m_mapDeviceAddress.SetAt(dwAddress, pDevice);

				//	Run 상태라면 바로 실행
				if (m_bRun)
				{
					pDevice->StartThread();
				}
			}

			if (bDatabaseMode)
			{	//	데이터를 추가한다.
				if ( FALSE == pProjectDB->InsertAddressIndex(dwAddress, nPriority))
				{	//	오면 안되지만... 예외 처리
					pProjectDB->CancelCommit();
					bDatabaseMode = FALSE;
				}
			}
		}

		if (bDatabaseMode)
		{	//	Database를 닫는다.
			pProjectDB->Commit();
			pProjectDB->CloseDataBase();
		}
	}
}

const BOOL CDeviceList::StartMonitoring()
{
	if (m_bRun == FALSE)
	{
		m_bRun = TRUE;
		
		switch (CDataBaseConfig::GetInstance()->GetInterface())
		{
		case CDataBaseConfig::interface_Serial:
		{
			m_deviceSerial.StartThread();
		}	break;

		default:
		{
			if( NULL == m_pStartingThread )
			{
				m_pStartingThread = AfxBeginThread(&CDeviceList::StartingThread, this);
			}
			/*
			POSITION pos = m_mapDeviceAddress.GetStartPosition();
			while (pos)
			{
				DWORD key;
				CDeviceMotionGate* pDevice = NULL;
				m_mapDeviceAddress.GetNextAssoc(pos, key, pDevice);

				pDevice->StartThread();
			}
			*/
		}	break;
		}
	}

	return TRUE;
}

const BOOL CDeviceList::IsMonitoringEnabled(const DWORD dwAddress)
{
	if (0 == dwAddress)
	{
		return m_bRun;
	}

	switch (CDataBaseConfig::GetInstance()->GetInterface())
	{
	case CDataBaseConfig::interface_Serial:
	{
		if (FALSE == m_deviceSerial.IsRun())
		{
			return FALSE;
		}
	}	break;

	default:
	{
		CDeviceMotionGate* pDevice = NULL;
		if (FALSE == m_mapDeviceAddress.Lookup(dwAddress, pDevice))
		{
			return FALSE;
		}

		if (FALSE == pDevice->IsRun())
		{
			CString str;
			pDevice->GetAddress(str);
			return FALSE;
		}

		//// KKB 추가
		//else if(TRUE == pDevice->IsRun())
		//{
		//	return TRUE;
		//}
		
	}	break;
	}

	return m_bRun;
}

const BOOL CDeviceList::IsTerminateEnabled()
{
	return m_bTerminate;
}

const BOOL CDeviceList::StopMonitoring(const BOOL bWait)
{
	// KKB 추가
	for (INT i = 0; i < GetAddressCount(); i++)
	{ // 디바이스마다 진행상황 체크중..
		CDeviceMotionGate* pDevice = NULL;
		const DWORD dwAddress = GetAddressByIndex(i);
		GetMotiongateByAddress(dwAddress, pDevice);

		if (true)
		{
			pDevice->StopExecution();
			pDevice->StopSequence();
			pDevice->StopThread();
			pDevice->CmdConnectionClose();
			
		}
	}

	m_bRun = FALSE;

	if (bWait)
	{
		do
		{
			Sleep(100);
		} while (m_nThreadCount);
	}

	return FALSE;
}

const INT CDeviceList::GetRegisteredDeviceCount()
{
	//	Querty 사용하는 방법이 바뀐다.

	return 0;
//	return m_mapDevicePriority.GetCount();
}

CDeviceMotionGate* CDeviceList::GetMotiongateByPriority(const INT nIndex)
{
	return NULL;

	/*CDeviceMotionGate* pDevice = NULL;

	m_mapDevicePriority.Lookup(nIndex, pDevice);

	return pDevice;*/
}

void CDeviceList::RegisterDevice(CDeviceMotionGate* pDevice)
{
	/*
	CDeviceMotionGate* pTemp = NULL;
	if (FALSE == m_mapDeviceMemory.Lookup(pDevice, pTemp))
	{
		m_mapDeviceMemory.SetAt(pDevice, pDevice);
	}
	*/
}

void CDeviceList::AddMotionGate(const DWORD dwAddress, LPCTSTR lpszName, const INT nMotorCount, const INT nHomingTimeout)
{
	//m_lstTempMotionGate.AddTail(new CDeviceMotionGate(dwAddress, lpszName, nMotorCount, nHomingTimeout));
}

void CDeviceList::ApplyMotionGate()
{/*
	LockMutex();

	m_lstMotionGate.RemoveAll();

	CTime time = CTime::GetCurrentTime();

	this->ResetTimeStamp(time);

	POSITION pos = m_lstTempMotionGate.GetHeadPosition();
	while (pos)
	{
		CDeviceMotionGate* pDeivceTemp = m_lstTempMotionGate.GetNext(pos);

		CDeviceMotionGate* pDeivce = NULL;
		const DWORD dwAddress = pDeivceTemp->GetAddress();
		if (m_mapDeviceAddress.Lookup(dwAddress, pDeivce))
		{
			pDeivce->SetSystemName(pDeivceTemp->GetSystemName());
			pDeivce->SetMotorCount(pDeivceTemp->GetMotorCount());

			delete pDeivceTemp;
		}
		else
		{
			pDeivce = pDeivceTemp;
			m_mapDeviceAddress.SetAt(dwAddress, pDeivce);

			if (m_bRun)
			{
				pDeivce->StartThread();
			}
		}

		pDeivce->ResetOperationTimeStamp(time);
		m_lstMotionGate.AddTail(pDeivce);
	}

	m_lstTempMotionGate.RemoveAll();

	UnlockMutex();*/
}

const BOOL CDeviceList::RemoveMotionGate(const DWORD dwAddress)
{
	BOOL bResult = FALSE;

	/*if (this->IsTerminateEnabled())
	{
		LockMutex();

		CDeviceMotionGate* pDevice = NULL;
		bResult = m_mapDeviceAddress.Lookup(dwAddress, pDevice);
		if (bResult)
		{
			delete pDevice;
			m_mapDeviceAddress.RemoveKey(dwAddress);
		}

		UnlockMutex();
	}*/

	return bResult;
}

const BOOL CDeviceList::GetMotiongateByAddress(const DWORD dwAddress, CDeviceMotionGate*& pDevice)
{
	if ( 0xffffffff == dwAddress)
	{
		pDevice = &m_deviceSerial;
		return TRUE;
	}

	return m_mapDeviceAddress.Lookup(dwAddress, pDevice);
}


const BOOL CDeviceList::GetMotionGateByDeviceID(const INT nIndex, CDeviceMotionGate*& pDevice)
{
	DWORD dwAddress = 0;
	m_mapDevicIDToAddress.Lookup(nIndex, dwAddress);

	return m_mapDeviceAddress.Lookup(dwAddress, pDevice);
}


const BOOL CDeviceList::GetMotiongateByName(LPCTSTR lpszSystemName, CDeviceMotionGate*& pDevice)
{
	/*BOOL bResult = FALSE;
	POSITION pos = m_mapDeviceAddress.GetStartPosition();
	while (pos)
	{
		DWORD dwAddress = 0;
		CDeviceMotionGate* pCurDevice = NULL;
		m_mapDeviceAddress.GetNextAssoc(pos, dwAddress, pCurDevice);
		if (0 == _tcscmp(pCurDevice->GetSystemName(), lpszSystemName))
		{
			bResult = TRUE;
			pDevice = pCurDevice;
			break;
		}
	}*/
	CDeviceMotionGate* pCurDevice = NULL;
	
	CString strStandard = lpszSystemName;
	BOOL bResult = FALSE;

	for (INT i = 0; i < GetAddressCount(); i++)
	{ // 디바이스마다 진행상황 체크중..
		const DWORD dwAddress = GetAddressByIndex(i);
		GetMotiongateByAddress(dwAddress, pCurDevice);
		
		if (0 == strStandard.CompareNoCase(pCurDevice->GetProjectName()))
		{
			bResult = TRUE;
			pDevice = pCurDevice;
			break;
		}
		else
		{
			pDevice = NULL;
		}
	}

	return bResult;
}

void CDeviceList::SetDisplayMethod(const INT nMethod)
{
	m_nDisplayMethod = nMethod;
}

const INT CDeviceList::GetDisplayMethod()
{
	return m_nDisplayMethod;
}

void CDeviceList::Initialize()
{
	m_pStartingThread = NULL;

	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	
	ResetAddressIndex();
	ResetDeviceID();

	if (pProjectDB->LoadAddressList() && pProjectDB->SeekToBegin())
	{
		pProjectDB->Lock();

		while (CDataBaseRecordData* pRecord = pProjectDB->SeekToNext())
		{
			CString strAddress;
			if (pRecord->GetTextData(_T("Address"), strAddress))
			{
				const DWORD dwAddress = pProjectDB->StringToAddress(strAddress);

				INT nPriority = 0;
				if (pRecord->GetIntergerData(_T("Priority"), nPriority))
				{
					AddAddress(nPriority, dwAddress);
				}
			}
		}
		pProjectDB->UnLock();
	}

	RebuildAddressIndex();
}

void CDeviceList::RebuildMotionGateIndex()
{

	//m_mapDevicePriority.RemoveAll();

	//POSITION pos = m_mapDeviceMemory.GetStartPosition();
	//while (pos)
	//{
	//	LPVOID key = NULL;
	//	CDeviceMotionGate* pDevice1 = NULL, * pDevice2 = NULL;

	//	m_mapDeviceMemory.GetNextAssoc(pos, key, pDevice1);
	//	const DWORD dwAddress = pDevice1->GetAddress();

	//	if (-1 != pDevice1->GetPriority())
	//	{
	//		if (FALSE == m_mapDeviceAddress.Lookup(dwAddress, pDevice2))
	//		{
	//			m_mapDeviceAddress.SetAt(dwAddress, pDevice1);
	//		}
	//		else if (pDevice1 != pDevice2)
	//		{
	//			//	문제가 있다.
	//			int i = 0;
	//		}

	//		m_mapDevicePriority.SetAt(pDevice1->GetPriority(), pDevice1);

	//		if (m_bRun && FALSE == pDevice1->IsRun())
	//		{
	//			pDevice1->StartThread();
	//		}
	//	}
	//	else if (m_mapDeviceAddress.Lookup(dwAddress, pDevice2))
	//	{
	//		m_mapDeviceAddress.RemoveKey(dwAddress);
	//	}
	//}
}

void CDeviceList::ResetDevicePriority()
{
	/*
	POSITION pos = m_mapDeviceMemory.GetStartPosition();
	while (pos)
	{
		LPVOID key = NULL;
		CDeviceMotionGate* pDevice = NULL;
		m_mapDeviceMemory.GetNextAssoc(pos, key, pDevice);
		pDevice->EnableAutoDatabaseUpdate(FALSE);
		pDevice->SetPriority();
		pDevice->EnableAutoDatabaseUpdate();
	}
	*/
}

void CDeviceList::UpdateDeviceInformation()
{
	/*
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	if (pProjectDB->OpenDataBase())
	{
		pProjectDB->Begin();
		pProjectDB->DeleteProjectTable();

		POSITION pos = m_mapDeviceMemory.GetStartPosition();
		while (pos)
		{
			LPVOID key = NULL;
			CDeviceMotionGate* pDevice = NULL;
			m_mapDeviceMemory.GetNextAssoc(pos, key, pDevice);

			if (-1 != pDevice->GetPriority())
			{
				pDevice->UpdateDeviceInformation();
			}
			else
			{
				pDevice->StopThread();
			}
		}

		pProjectDB->Commit();
		pProjectDB->CloseDataBase();
	}

	RebuildMotionGateIndex();
	*/
}

const BOOL CDeviceList::ChangeAddress(const DWORD dwOrgAddress, const DWORD dwNextAddress)
{
	/*
	CDeviceMotionGate* pDevice = NULL;
	if (m_mapDeviceAddress.Lookup(dwNextAddress, pDevice))
	{
		return FALSE;
	}

	if (FALSE == m_mapDeviceAddress.Lookup(dwOrgAddress, pDevice))
	{
		return FALSE;
	}

	m_mapDeviceAddress.SetAt(dwNextAddress, pDevice);
	m_mapDeviceAddress.RemoveKey(dwOrgAddress);
	*/

	return TRUE;
}

void CDeviceList::DeleteObject(CDeviceMotionGate* pDeleteDevice)
{
	DecreaseThreadCount();
}

CDeviceMotionGate* CDeviceList::GetMotiongateForSerial()
{
	return &m_deviceSerial;
}

const INT CDeviceList::GetAddressCount(const BOOL bListMode)
{
	INT nResult = 0;
	switch (CDataBaseConfig::GetInstance()->GetInterface())
	{
	case CDataBaseConfig::interface_Serial:
	{
		nResult = bListMode ? m_mapDevicePriorityToAddress.GetCount() : 1;
	}	break;

	default:
	{
		nResult = m_mapDevicePriorityToAddress.GetCount();
	}	break;
	}

	return nResult;
}

const DWORD CDeviceList::GetAddressByIndex(INT nIndex, const BOOL bListMode)
{
	DWORD dwResult = 0;
	switch (CDataBaseConfig::GetInstance()->GetInterface())
	{
	case CDataBaseConfig::interface_Serial:
	{
		if (bListMode)
		{
			m_mapDevicePriorityToAddress.Lookup(nIndex, dwResult);
		}
		else
		{
			dwResult = 0xffffffff;
		}
	}	break;

	default:
	{
		m_mapDevicePriorityToAddress.Lookup(nIndex, dwResult);
	}	break;
	}

	return dwResult;
}



const BOOL CDeviceList::RemoveDeviceIDKey(const DWORD dwAddress)
{
	INT nValue;
	if (m_mapDeviceAddressToDeviceID.Lookup(dwAddress, nValue))
	{
		m_mapDevicIDToAddress.RemoveKey(nValue);
		m_mapDeviceAddressToDeviceID.RemoveKey(dwAddress);
		
		return TRUE;
	}

	return FALSE;
}


const BOOL CDeviceList::IsStartMonitoring()
{
	return m_bRun;
}
