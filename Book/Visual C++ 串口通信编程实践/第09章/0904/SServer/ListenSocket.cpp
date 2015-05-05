// ListenSocket.cpp : implementation file
//

#include "stdafx.h"
#include "SServer.h"
#include "ListenSocket.h"
#include "AcceptSocket.h"  //添加CAcceptSocket类头文件
#include "SServerDlg.h"    //添加的主对话框类头文件

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListenSocket

CListenSocket::CListenSocket()
{
	
}

CListenSocket::~CListenSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CListenSocket, CSocket)
	//{{AFX_MSG_MAP(CListenSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CListenSocket member functions

void CListenSocket::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CAcceptSocket *pSocket=new CAcceptSocket;
	CSServerDlg* pDlg=(CSServerDlg*)AfxGetMainWnd(); //得到主框口(对话框)指针

	if(Accept(*pSocket))
	{
		pDlg->m_pAcceptList.AddTail(pSocket);
		CString strAddr,strAddr1;
		UINT unPort,unPort1;
		pSocket->GetPeerName(strAddr1,unPort1); //得到远程IP地址和端口号
		pSocket->GetSockName(strAddr,unPort);   //得到本地IP地址和端口号
		pDlg->m_strNetMsg.Format("本地IP%s端口%d连接上远程客户IP%s端口%d",
			strAddr,unPort,strAddr1,unPort1);
		pDlg->UpdateMsgData();
	}
	else
	{
		delete pSocket;
	}
	CSocket::OnAccept(nErrorCode);
}
