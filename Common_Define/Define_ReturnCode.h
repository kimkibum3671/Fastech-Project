#pragma once

#ifndef FMM_RETURN_CODE
#define FMM_RETURN_CODE

/**
* --------------------------------------------------------------------------------
*                         Motor Return Code Defines.
* --------------------------------------------------------------------------------
*/	

typedef enum _FMM_ERROR
{
	FMM_OK = 0,

	FMM_NOT_OPEN,
	FMM_INVALID_DEVICE,
	FMM_INVALID_PORT_NUM,
	FMM_INVALID_SLAVE_NUM,

	FMC_DISCONNECTED = 5,
	FMC_TIMEOUT_ERROR,				//	1회 송수신 과정에서 응답이 없는 경우
	FMC_CRCFAILED_ERROR,
	FMC_RECVPACKET_ERROR,			// PACKET SIZE ERROR

	FMM_POSTABLE_ERROR,

	FMP_FRAMETYPEERROR = 0x80,
	FMP_DATAERROR,
	FMP_PACKETERROR,
	FMP_RUNFAIL = 0x85,
	FMP_RESETFAIL,
	FMP_SERVOONFAIL1,
	FMP_SERVOONFAIL2,
	FMP_SERVOONFAIL3,
	FMP_SERVOOFF_FAIL,
	FMP_ROMACCESS,

	FMP_PACKETCRCERROR = 0xAA,

	FMM_MOTION_GATE_BUSY = 0xE2,

	FMM_UNKNOWN_ERROR = 0xFF,


	FMC_UNKNOWN_ERROR = 0x1000,
	FMM_INVALID_PORT,
	FMC_NO_RESPONSE,				//	TCP 연결은 이루어졌으나 디바이스로부터 응답이 없을 때
	FMC_REFUSED,					//	상대방에서 연결을 거부할 때
	FMC_SOCKET_ERROR,				//	Socket을 제어하는 과정에서 Error가 발생
	FMC_NOT_COMPLPETE,				//	FMC_NOT_COMPLPETE
	FMC_INSUFFICIENT_BUFFER,		//	수신 버퍼가 부족하다
	FMC_INSUFFICIENT_BUFFER2,		//	데이터 저장 용량이 부족하다
//	FMC_INVALID_DATASIZE,			//	요청한 데이터와 크기가 다르다
	FMC_DATA_ERROR,					//	정상적인 Data가 아님
	FMC_DATA_COMPARE_ERROR,			//	비교한 데이터가 정상이 아님.

}	FMM_ERROR;
#endif
