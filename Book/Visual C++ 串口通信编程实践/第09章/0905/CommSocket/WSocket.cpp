//////////////////////////////////////////////////////////////////////
//WYD's Socket
//类作者SoFantasy  EMail: SoFantasy@21cn.com
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WSocket.h"
#include <TIME.H>
#include <stdio.h> 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWSocket::CWSocket()
{
	WSADATA wsaData;

	m_sSocket = NULL;
	m_bConnected = FALSE;
	m_sServer = NULL;
	m_bListened = FALSE;
	m_nServerPort = -1;
	m_hServerThread = NULL;

	m_bNeedClose = FALSE;
	m_bNeedCloseServer = FALSE;
	
	if(WSAStartup(0x202, &wsaData) == 0)
	{
		m_bWSAStartup = TRUE;
		err = 0;
	}
	else
	{
		m_bWSAStartup = FALSE;
		err = WSAGetLastError();
	}
}

CWSocket::CWSocket(SOCKET s)
{
	m_sSocket = NULL;
	m_bWSAStartup = FALSE;
	m_sServer = NULL;
	m_bListened = FALSE;
	m_nServerPort = -1;
	m_hServerThread = NULL;

	m_bNeedClose = FALSE;
	m_bNeedCloseServer = FALSE;
	
	m_bConnected = IsSockConnected(s);
	if(m_bConnected) m_sSocket = s;

	if(!m_bConnected)
	{
		WSADATA wsaData;
		if(WSAStartup(0x202, &wsaData) == 0)
		{
			m_bWSAStartup = TRUE;
			err = 0;
		}
		else
		{
			m_bWSAStartup = FALSE;
			err = WSAGetLastError();
		}
	}
}

CWSocket::operator=(SOCKET s)
{	
	if(IsSockConnected(s))
	{
		if(m_bConnected) Close();
		m_sSocket = s;
		m_bConnected = TRUE;
		m_bNeedClose = FALSE;
	}
}

CWSocket::~CWSocket()
{	
	if(m_bNeedClose) Close();
	if(m_bNeedCloseServer) CloseServer();
	if(m_bWSAStartup) WSACleanup();
}

BOOL CWSocket::Connection(LPCSTR pstrHost, int nPort)
{
	if(m_bConnected) Close();

	LPHOSTENT lpHost;	
	struct sockaddr_in server;	

	//Lookup host
	lpHost = gethostbyname(pstrHost);
	if(lpHost == NULL) return FALSE;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = *((u_long FAR *)(lpHost->h_addr));
	server.sin_port=htons(nPort);

	m_sSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(m_sSocket <= 0)
	{
		err = WSAGetLastError();
		return FALSE;
	}

	if(connect(m_sSocket, (LPSOCKADDR)&server, sizeof(SOCKADDR)) == SOCKET_ERROR) 
	{
		err = WSAGetLastError();
		closesocket(m_sSocket);
		m_sSocket = NULL;
		return FALSE;
	}
	
	m_bNeedClose = TRUE;
	m_bConnected = TRUE;
	return TRUE;
}

void CWSocket::Close()
{
	if(!m_bConnected) return;
	if(m_sSocket == NULL) return;

	shutdown(m_sSocket, SD_RECEIVE);
	Sleep(50);
	closesocket(m_sSocket);
	m_sSocket = NULL;
	m_bConnected = FALSE;
}

void CWSocket::CloseServer()
{
	if(!m_bListened) return;
	if(m_sServer == NULL) return;

	shutdown(m_sServer, SD_RECEIVE);
	Sleep(50);
	closesocket(m_sServer);
	m_sServer = NULL;
	m_bListened = FALSE;
}


DWORD CALLBACK CWSocket::TimeoutControl(LPVOID lpParm)
{
	TPARA* para = (TPARA*)lpParm;
	time_t stime = time(NULL);
	BOOL bTimeover = FALSE;

	while(!bTimeover)
	{
		if(para->bExit)
		{
			para->IsExit = TRUE;
			return 0;
		}

		Sleep(1);

		time_t ntime = time(NULL);
		if((ntime - stime) > para->OutTime) bTimeover = TRUE;
	}

	if(para->bExit)
	{
		para->IsExit = TRUE;
		return 0;
	}

	if(para->s != NULL)
	{
		para->pbConnected[0] = FALSE;
		shutdown(para->s, SD_RECEIVE);
		Sleep(5);
		closesocket(para->s);
	}

	para->IsExit = TRUE;
	return 0;
}

int CWSocket::ReadData(char FAR* buf, int len, int timeout)
{
	if(!m_bConnected || m_sSocket == NULL)
	{
		err = -1;
		return -1;
	}

	HANDLE hThread;
	DWORD dwThreadId;
	TPARA para;

	para.OutTime = timeout;
	para.s = m_sSocket;
	para.bExit = FALSE;
	para.IsExit = FALSE;
	para.pbConnected = &m_bConnected;
	hThread = CreateThread(NULL, NULL, TimeoutControl, (LPVOID)(&para), 0, &dwThreadId);
	if (hThread == NULL) return -1;

	int nRet = recv(m_sSocket, buf, len, 0);
	if(nRet == SOCKET_ERROR) err = WSAGetLastError();

	para.bExit = TRUE;
	while(!para.IsExit) Sleep(1);

	return nRet;
}

int CWSocket::SendData(const char FAR *buf, int len, int timeout)
{
	if(!m_bConnected || m_sSocket == NULL)
	{
		err = -1;
		return -1;
	}

	if(!IsSockConnected(m_sSocket))
	{
		m_bConnected = FALSE;
		err = -1;
		return -1;
	}

	int nRet = send(m_sSocket, buf, len, 0);

	if(nRet == SOCKET_ERROR) err = WSAGetLastError();

	return nRet;
}

BOOL CWSocket::IsSockConnected(SOCKET s)
{
	int nRet = 0;
	struct fd_set Fd_Recv;
    struct timeval Time_Recv;

	memset(&Fd_Recv, 0, sizeof(struct fd_set));
    FD_CLR(s, &Fd_Recv); 
    FD_SET(s, &Fd_Recv); 
    Time_Recv.tv_sec = 0;
    Time_Recv.tv_usec = 0;

	nRet = select(s, &Fd_Recv, NULL, NULL, &Time_Recv);

    return (nRet == 0);
}

BOOL CWSocket::CreateServer(int port, int backlog)
{
	if(m_bListened == TRUE) CloseServer();

	struct	sockaddr_in local;

	//创建监听socket
    m_sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(m_sServer == SOCKET_ERROR)
    {
        err = WSAGetLastError();
        return FALSE;
    }

	//添参数	
	local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(port);

	if(bind(m_sServer, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
    {
		err = WSAGetLastError();
		closesocket(m_sServer);
        return FALSE;
    }

	//开始侦听
    if(listen(m_sServer, backlog) != 0)
	{
		err = WSAGetLastError();
		closesocket(m_sServer);
        return FALSE;
    }

	m_nServerPort = port;
	m_bListened = TRUE;
	m_bNeedCloseServer = TRUE;

	return TRUE;
}

SOCKET CWSocket::Listen(char *ClientIP)
{
	if(!m_bListened) return -1;
	if(m_sServer == NULL) return -1;

	SOCKET	sClient;	
	int		iAddrSize;
	struct	sockaddr_in addr;

	iAddrSize = sizeof(addr);

	sClient = accept(m_sServer, (struct sockaddr *)&addr, &iAddrSize);
	
	if(sClient == SOCKET_ERROR)
	{
		err = WSAGetLastError();
        return SOCKET_ERROR;
    }

	if(ClientIP != NULL)
	{
		sprintf(ClientIP, "%d.%d.%d.%d", addr.sin_addr.S_un.S_un_b.s_b1, 
										 addr.sin_addr.S_un.S_un_b.s_b2,
										 addr.sin_addr.S_un.S_un_b.s_b3,
										 addr.sin_addr.S_un.S_un_b.s_b4);
	}

	return sClient;
}

BOOL CWSocket::StartServer(DEALPROC lpDealFunc)
{
	if(!m_bListened) return -1;
	if(m_sServer == NULL) return -1;

	DWORD dwThreadId;
	SERVERPARA *para;

	para = new SERVERPARA;
	para->s = m_sServer;
	para->port = m_nServerPort;
	para->lpDealFunc = lpDealFunc;

	m_hServerThread = CreateThread(NULL, 0, ServerProc, (LPVOID)(para), 0, &dwThreadId);

	if(m_hServerThread == NULL)
	{
		delete para;
		err = WSAGetLastError();
        return FALSE;
	}

	return TRUE;
}

BOOL CWSocket::StopServer()
{
	CloseServer();

	return TRUE;
}

DWORD CALLBACK CWSocket::ServerProc(LPVOID lpParm)
{
	SERVERPARA *para = (SERVERPARA*)lpParm;
	
	if(para == NULL) return -1;

	SOCKET		s = para->s;
	int			port = para->port;
	DEALPROC	lpDealFunc = para->lpDealFunc;
	delete para;

	SOCKET	sClient;	
	int		iAddrSize;
	struct	sockaddr_in addr;
	char	IP[32];
	HANDLE	hThread;
    DWORD	dwThreadId;
	DEALPARA *parac;

	iAddrSize = sizeof(addr);

	while(1)
	{
		sClient = accept(s, (struct sockaddr *)&addr, &iAddrSize);
		
		if(sClient == SOCKET_ERROR) break;

		sprintf(IP, "%d.%d.%d.%d", addr.sin_addr.S_un.S_un_b.s_b1, 
									   addr.sin_addr.S_un.S_un_b.s_b2,
									   addr.sin_addr.S_un.S_un_b.s_b3,
									   addr.sin_addr.S_un.S_un_b.s_b4);

		parac = new DEALPARA;
		memset(parac->IP, 0, sizeof(parac->IP));
		parac->s = sClient;
		parac->port = port;
		parac->lpDealFunc = lpDealFunc;
		memcpy(parac->IP, IP, strlen(IP));	

		//侦听到连接，开一个线程		
        hThread = CreateThread(NULL, 0, DealProc, (LPVOID)(parac), 0, &dwThreadId);

		if(hThread == NULL) delete parac;
	}

	return 0;
}

DWORD CALLBACK CWSocket::DealProc(LPVOID lpParm)
{
	DEALPARA *para = (DEALPARA*)lpParm;
	
	if(para == NULL) return -1;

	SOCKET		s = para->s;
	int			port = para->port;
	DEALPROC	lpDealFunc = para->lpDealFunc;
	char		IP[32];
	memcpy(IP, para->IP, sizeof(IP));
	delete para;

	try{lpDealFunc(s, port, IP);}catch(...){return -1;}

	return 0;
}

SOCKET CWSocket::GetSocket()
{
	return m_sSocket;
}

SOCKET CWSocket::GetServerSocket()
{
	return m_sServer;
}

int CWSocket::GetError()
{
	return err;
}