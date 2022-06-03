#include "pch.h"
#include "CModbusAcceptedClient.h"
#include "CDeviceList.h"
#include "CModbusServer.h"
#include "CDataBaseManager.h"

#define PT_ACCESS_ZERO 255

CModbusAcceptedClient::CModbusAcceptedClient(const INT nPortNo, SOCKET socket, SOCKADDR_IN& address)
{
	m_nPortNo = nPortNo;
	m_socket = socket;
	memcpy(&m_address, &address, sizeof(SOCKADDR_IN));
	m_bRun = TRUE;

	m_nRevSize = 0;
	m_nSndSize = 0;

	m_bResMotorSetting = false;

	m_pThread = AfxBeginThread(CModbusAcceptedClient::ThreadClient, this);
}

CModbusAcceptedClient::~CModbusAcceptedClient()
{

}

UINT CModbusAcceptedClient::ThreadClient(LPVOID pParam)
{
	UINT uResult = 0;

	CModbusAcceptedClient* pParent = reinterpret_cast<CModbusAcceptedClient*>(pParam);

	SOCKET sockClient = pParent->GetSocket();

	CHAR btBuffer[1024] = { 0, };

	while (0 == uResult && pParent->IsRun())
	{
		const INT nReceive = recv(sockClient, btBuffer, sizeof(btBuffer), 0);


		switch (nReceive)
		{
		case 0:
			//	상대방에서 통신을 종료한 경우
			uResult = 1;
			break;

		case SOCKET_ERROR:
			uResult = 2;
			break;

		default:
		{

			CString str;
			CString strMerge;

			for (INT n = 0; n < 12; n++)
			{
				str.Format(_T(" %02X / "), (unsigned char)btBuffer[n]);
				strMerge += str;
			}

			// TRACE(strMerge);
			pParent->OnReceive(btBuffer, nReceive);
		}	break;
		}

		Sleep(1);
	}

	shutdown(sockClient, SD_BOTH);

	Sleep(100);

	closesocket(sockClient);

	pParent->OnClose();

	return uResult;
}

const INT CModbusAcceptedClient::GetPortNo()
{
	return m_nPortNo;
}

SOCKET CModbusAcceptedClient::GetSocket()
{
	return m_socket;
}

const BOOL CModbusAcceptedClient::IsRun()
{
	return m_bRun;
}

void CModbusAcceptedClient::ChangeEndian(WORD* pwData)
{
	*pwData = MAKEWORD(HIBYTE(*pwData), LOBYTE(*pwData));
}

void CModbusAcceptedClient::BuildRequest(LPCVOID lpBuffer, const INT cbBuffer)
{
	memcpy(m_revBuffer, lpBuffer, cbBuffer);
	m_nRevSize = cbBuffer;

	PMODBUS_PROTOCOL pProtocol = reinterpret_cast<PMODBUS_PROTOCOL>(m_revBuffer);
		
	ChangeEndian(&pProtocol->header.wTransactionID);
	ChangeEndian(&pProtocol->header.wProtocolID);
	ChangeEndian(&pProtocol->header.wDataSize);
}

void CModbusAcceptedClient::BuildResponse(LPCVOID lpBuffer, const INT cbBuffer)
{
	memcpy(m_sndBuffer, lpBuffer, sizeof(MBAP_HEADER));
}

void CModbusAcceptedClient::CModbusAcceptedClient::Close()
{
	shutdown(m_socket, SD_BOTH);
}

const INT CModbusAcceptedClient::OnReceive(LPCVOID lpBuffer/*입력 데이터*/, const INT cbBuffer/*입력 데이터 사이즈*/)
{ 
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	WORD wStandardAdd = pConfig->GetModBusWriteAddressTypeWORD();

	INT nResult = 0;

	BuildRequest(lpBuffer, cbBuffer); // 입력받은 데이터를 m_revBuffer 에 저장한다.

	PMODBUS_PROTOCOL pReqProtocol = reinterpret_cast<PMODBUS_PROTOCOL>(m_revBuffer);

	switch (pReqProtocol->header.btCommand)
	{
	// Function Code : 
	case 0x03: // [Read]Input Registers
	case 0x04: // [Read]Holding Registers
	{
		PMODBUS_PROTOCOL_REQ_READ_REGISTER pRevData = reinterpret_cast<PMODBUS_PROTOCOL_REQ_READ_REGISTER>(&pReqProtocol->data);
		ChangeEndian(&pRevData->wAddress); // Port의 시작번지
		ChangeEndian(&pRevData->wLength); // 시작번지 부터 읽을 워드의 개수

		BuildResponse(lpBuffer, cbBuffer); // m_sndBuffer 입력 데이터 저장.

		PMODBUS_PROTOCOL pResProtocol = reinterpret_cast<PMODBUS_PROTOCOL>(m_sndBuffer);

		pResProtocol->header.wDataSize = 3 + pRevData->wLength * sizeof(WORD);
		ChangeEndian(&pResProtocol->header.wDataSize);

		PMODBUS_PROTOCOL_RES_READ_REGISTER pSndData = reinterpret_cast<PMODBUS_PROTOCOL_RES_READ_REGISTER>(&pResProtocol->data);

		pSndData->btCount = pRevData->wLength * sizeof(WORD);
		WORD* pwData = &pSndData->wData;
		/*여기서 데이터를 받아온다.*/
		//	Find Correct Motionage
		//	Read Motiongate Memory
		//	PreqpareBuffer

		ReadyResponseData();

		for (INT nIndex = 0; nIndex < pRevData->wLength; nIndex++)
		{
			*pwData = m_wData[pRevData->wAddress - wStandardAdd + nIndex];
			/*받아온 데이터를 저장한다.*/
			ChangeEndian(pwData);
			pwData++;
			/*Index를 넘긴다*/
		}

		nResult = send(m_socket, reinterpret_cast<const CHAR*>(pResProtocol), sizeof(MBAP_HEADER) + 1 + pSndData->btCount, 0);
		
	}	break;

	case 0x06:  // [Write]
	{
		PMODBUS_PROTOCOL_REQ_WRITE_REGISTER pData = reinterpret_cast<PMODBUS_PROTOCOL_REQ_WRITE_REGISTER>(&pReqProtocol->data);
		ChangeEndian(&pData->wAddress);
		ChangeEndian(&pData->wData);

		m_wData[pData->wAddress - wStandardAdd] = pData->wData;

		BuildResponse(lpBuffer, cbBuffer); // m_sndBuffer 입력 데이터 저장.

		PMODBUS_PROTOCOL pResProtocol = reinterpret_cast<PMODBUS_PROTOCOL>(m_sndBuffer);
		
		pResProtocol->header.wDataSize = 2 + sizeof(MODBUS_PROTOCOL_RES_WRITE_MULTIPLE_REGISTER);
		ChangeEndian(&pResProtocol->header.wDataSize);

		PMODBUS_PROTOCOL_RES_WRITE_SINGLE_REGISTER pSndData = reinterpret_cast<PMODBUS_PROTOCOL_RES_WRITE_SINGLE_REGISTER>(&pResProtocol->data);
		pSndData->wAddress = pData->wAddress;
		pSndData->wData = pData->wData;
		
		RequireWriteCmd(pSndData->wAddress, pSndData->wData, wStandardAdd); // KKB, Write Require Data

		ChangeEndian(&pSndData->wAddress);
		ChangeEndian(&pSndData->wData);

		nResult = send(m_socket, reinterpret_cast<const CHAR*>(pResProtocol), sizeof(MBAP_HEADER) + sizeof(MODBUS_PROTOCOL_RES_WRITE_SINGLE_REGISTER), 0);	//	요청한 명령을 그대로 반환
	}	break;

	case 0x10:
	{
		PMODBUS_PROTOCOL_WRITE_MULTIPLE_REGISTER pReqData = reinterpret_cast<PMODBUS_PROTOCOL_WRITE_MULTIPLE_REGISTER>(&pReqProtocol->data);
		ChangeEndian(&pReqData->wAddress);
		ChangeEndian(&pReqData->wLength);

		const INT nCount = pReqData->btCount / 2;
		WORD* pwData = &pReqData->wData;

		CString str;
		CString strMerge = _T("");

		for (INT nIndex = 0; nIndex < nCount; nIndex++)
		{
			ChangeEndian(pwData);
			m_wData[pReqData->wAddress - wStandardAdd + nIndex] = *pwData;
			str.Format(_T("%02X /"), *pwData);
			strMerge += str;
			RequireWriteCmd(pReqData->wAddress + nIndex, *pwData, wStandardAdd);
			pwData++;
		}
		//TRACE(strMerge);
		BuildResponse(lpBuffer, cbBuffer);

		PMODBUS_PROTOCOL pResProtocol = reinterpret_cast<PMODBUS_PROTOCOL>(m_sndBuffer);
		
		pResProtocol->header.wDataSize = 2 + sizeof(MODBUS_PROTOCOL_RES_WRITE_MULTIPLE_REGISTER);
		ChangeEndian(&pResProtocol->header.wDataSize);

		PMODBUS_PROTOCOL_RES_WRITE_MULTIPLE_REGISTER pSndData = reinterpret_cast<PMODBUS_PROTOCOL_RES_WRITE_MULTIPLE_REGISTER>(&pResProtocol->data);

		pSndData->wAddress = pReqData->wAddress;
		pSndData->wLength = pReqData->wLength;

		ChangeEndian(&pSndData->wAddress);
		ChangeEndian(&pSndData->wLength);

		nResult = send(m_socket, reinterpret_cast<const CHAR*>(pResProtocol), sizeof(MBAP_HEADER) + sizeof(MODBUS_PROTOCOL_RES_WRITE_MULTIPLE_REGISTER), 0);	//	요청한 명령을 그대로 반환
		TRACE(_T(""));
	}	break;
	}

	return nResult;
}

void CModbusAcceptedClient::OnClose()
{
	CModbusServer::GetInstance()->OnClientClose(this);
}


const BOOL CModbusAcceptedClient::RequireWriteCmd(WORD wAddress, WORD wValue, WORD wStandardAddr)
{
	INT nIndex = (INT)(wAddress - wStandardAddr);

	switch (nIndex)
	{
	case 0:
	{
		m_ReqData.wEENo = wValue;
	}	break;

	case 1:
	{
		// Motor / PT No 설정을 실행하지 않는다면...  Response 에서 PT_ACCESS_ZERO 를 반환하기에 초기 값과 다른값이 나옴..
		m_bResMotorSetting = true;

		BYTE byHIBYTE = HIBYTE(wValue);
		BYTE byLOBYTE = LOBYTE(wValue);

		m_ReqData.byMotorNo = HIBYTE(wValue);
		m_ReqData.byPTNo = LOBYTE(wValue) > 0 ? LOBYTE(wValue) -1 : PT_ACCESS_ZERO;
		// 0번 포지션 테이블 값은 사용하지 않음..
		// 따라서 ReadParameter때 1번 포지션 값이 m_stMotorInfo[nMotor].nodeItem[0]번 인덱스에 저장됨.
		
		
	}	break;
	default:
		break;
	}

	return TRUE;
}

const BOOL CModbusAcceptedClient::ReadyResponseData()
{
	for (INT nIndex = 0; nIndex < MAX; nIndex++)
	{
		m_wData[nIndex] = ResponseCmd(nIndex);
	}
	return TRUE;
}


const WORD CModbusAcceptedClient::ResponseCmd(WORD wAddress)
{
	WORD wResult = 0;

	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;
	pDeviceList->GetMotionGateByDeviceID((INT)m_ReqData.wEENo, pDevice);
	INT nIndex = (INT)(wAddress);
	BOOL bEnable = (31 > m_ReqData.byPTNo && 9 > m_ReqData.byMotorNo);
	if (NULL != pDevice)
	{
		switch (nIndex)
		{
		case EE_NO:
		{
			wResult = m_ReqData.wEENo;
		}	break;
		case PT_MOTOR_NO:
		{
			if (m_bResMotorSetting)
			{
				wResult = MergByte(m_ReqData.byMotorNo, m_ReqData.byPTNo == PT_ACCESS_ZERO ? 0 : m_ReqData.byPTNo + 1);
			}
			else
			{
				wResult = 0;
			}

		}	break;

		case ERR_CODE:
		{   
			wResult = MergByte((BYTE)pDevice->GetMotorError(m_ReqData.byMotorNo, m_ReqData.byPTNo), (BYTE)pDevice->GetMotionGateErrorCode());
		}	break;

		case POS_LO:
		{
			if (bEnable)
			{
				m_node = pDevice->GetPLCReqNodeItem(m_ReqData.byPTNo, m_ReqData.byMotorNo);
				wResult = pDevice->GetModBusResPonseCmd(m_ReqData.byMotorNo, m_ReqData.byPTNo, POS_LO, m_node.lPosition);
			}
		}	break;

		case POS_HI:
		{
			if (31 > m_ReqData.byPTNo && 9 > m_ReqData.byMotorNo)
			{
				m_node = pDevice->GetPLCReqNodeItem(m_ReqData.byPTNo, m_ReqData.byMotorNo);
				wResult = pDevice->GetModBusResPonseCmd(m_ReqData.byMotorNo, m_ReqData.byPTNo, POS_HI, m_node.lPosition);
			}
		}	break;

		case SPEED:
		{
			if (bEnable)
			{
				m_node = pDevice->GetPLCReqNodeItem(m_ReqData.byPTNo, m_ReqData.byMotorNo);
				wResult = pDevice->GetModBusResPonseCmd(m_ReqData.byMotorNo, m_ReqData.byPTNo, SPEED, m_node.dwMoveSpd);
			}
		}	break;

		default:
			break;
		}
	}

	return wResult;
}


WORD CModbusAcceptedClient::MergByte(BYTE byte1, BYTE byte2)
{
	return ((WORD)(byte1 << 8)) | ((WORD)(byte2));
}


