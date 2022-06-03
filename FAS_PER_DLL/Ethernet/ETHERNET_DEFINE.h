#pragma once

//----------------------------------------------------------------
//           DEFINITIONS
//#define	PACKET_SIZE			3

#define TCP_PORT_NO1		2001
#define TCP_PORT_NO2		2002

#define UDP_PORT_NO1		3001
#define UDP_PORT_NO2		3002

#define INSERT_AXIS_IN_PACKET

static const unsigned char	PACKET_HEADER		= 0xAA;

enum _PACKET_SENDOFFSET
{
	SENDOFFSET_HEADER = 0,
	SENDOFFSET_LENGTH,
	SENDOFFSET_SYNC,
#ifdef INSERT_AXIS_IN_PACKET
	SENDOFFSET_AXIS,
#endif
	SENDOFFSET_CMD,
	SENDOFFSET_DATA,
};

enum _PACKET_RECVOFFSET
{
	RECVOFFSET_HEADER = 0,
	RECVOFFSET_LENGTH,
	RECVOFFSET_SYNC,
#ifdef INSERT_AXIS_IN_PACKET
	RECVOFFSET_AXIS,
#endif
	RECVOFFSET_CMD,
	RECVOFFSET_STATUS,
	RECVOFFSET_DATA,
};

#define	MAX_BUFFER_SIZE			512
#define	MAX_SWINFO_LENGTH		250

enum _SOCKET {
	SOCKET_CONNECTED = 0,		// 연결되었음.. 잘 통신중..
	SOCKET_TRYCONN,				// 연결 시도 중... 결과가 나오겠지..
	SOCKET_UNPLUGED,			// 응답이 없음.
	SOCKET_DISCONN,				// Close 메세지를 받음.
	SOCKET_BEFORECONN,			// 연결 시도 전.
	SOCKET_CLOSE,				// shutdown 실행 함.
	SOCKET_EXIT					// 완전 종료.
};

//----------------------------------------------------------------
//           PING 

#define DEF_PACKET_SIZE 32
#define MAX_PACKET_SIZE 1024

#define ICMP_MIN 8
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
/*
/////////////////////////////////////////////////////////////////////////////
// IpHeader

typedef struct
{
	unsigned int h_len:4;          // length of the header
	unsigned int version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char ttl; 
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum
	unsigned int sourceIP;
	unsigned int destIP;
} IPHEADER;

/////////////////////////////////////////////////////////////////////////////
// IcmpHeader

typedef struct
{
	BYTE i_type;
	BYTE i_code;				   // type sub code
	USHORT i_cksum;
	USHORT i_id;
	USHORT i_seq;
} ICMPHEADER;

typedef struct
{
	ICMPHEADER ipHeader;
	char dataPart[MAX_PACKET_SIZE];
} PINGPACKET;
*/
