//////////////////////////////////////////////////////////////////////
//WYD's Socket
//Àà×÷ÕßSoFantasy  EMail: SoFantasy@21cn.com
////////////////////////////////////////////////////////////////////////

#if !defined(_1974__WSOCKET_H__09_29_)
#define _1974__WSOCKET_H__09_29_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "winsock2.h"

typedef void (CALLBACK* DEALPROC)(SOCKET s, int ServerPort, const char *ClientIP);

class CWSocket  
{
public:
	
//variable
	int		err;

//Function
	int GetError();
	SOCKET GetSocket();
	SOCKET GetServerSocket();
	BOOL StopServer();
	BOOL StartServer(DEALPROC lpDealFunc);
	SOCKET Listen(char *ClientIP = NULL);
	BOOL CreateServer(int port, int backlog);
	int ReadData(char FAR* buf, int len, int timeout);
	int SendData(const char FAR* buf, int len, int timeout);
	BOOL IsSockConnected(SOCKET s);
	BOOL Connection(LPCSTR pstrHost, int nPort);	
	void Close();
	void CloseServer();
	operator=(SOCKET s);
	CWSocket(SOCKET s);
	CWSocket();
	virtual ~CWSocket();

protected:
	
//variable
	SOCKET  m_sSocket;
	BOOL	m_bConnected;
	BOOL	m_bNeedClose;

	SOCKET  m_sServer;
	BOOL	m_bListened;
	int		m_nServerPort;
	BOOL	m_bNeedCloseServer;

	BOOL	m_bWSAStartup;

	HANDLE	m_hServerThread;

//Function
	static DWORD CALLBACK TimeoutControl(LPVOID lpParm);
	static DWORD CALLBACK ServerProc(LPVOID lpParm);
	static DWORD CALLBACK DealProc(LPVOID lpParm);

 	struct TPARA
	{
		int		OutTime;
		SOCKET	s;
		BOOL	*pbConnected;
		BOOL bExit;
		BOOL IsExit;
	};

	struct SERVERPARA
	{
		SOCKET		s;
		int			port;
		DEALPROC	lpDealFunc;
	};

	struct DEALPARA
	{
		SOCKET		s;
		int			port;
		char		IP[32];
		DEALPROC	lpDealFunc;
	};
};


#endif // !defined(_1974__WSOCKET_H__09_29_)
