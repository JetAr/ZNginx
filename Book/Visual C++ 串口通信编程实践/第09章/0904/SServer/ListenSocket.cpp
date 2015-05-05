// ListenSocket.cpp : implementation file
//

#include "stdafx.h"
#include "SServer.h"
#include "ListenSocket.h"
#include "AcceptSocket.h"  //���CAcceptSocket��ͷ�ļ�
#include "SServerDlg.h"    //��ӵ����Ի�����ͷ�ļ�

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
	CSServerDlg* pDlg=(CSServerDlg*)AfxGetMainWnd(); //�õ������(�Ի���)ָ��

	if(Accept(*pSocket))
	{
		pDlg->m_pAcceptList.AddTail(pSocket);
		CString strAddr,strAddr1;
		UINT unPort,unPort1;
		pSocket->GetPeerName(strAddr1,unPort1); //�õ�Զ��IP��ַ�Ͷ˿ں�
		pSocket->GetSockName(strAddr,unPort);   //�õ�����IP��ַ�Ͷ˿ں�
		pDlg->m_strNetMsg.Format("����IP%s�˿�%d������Զ�̿ͻ�IP%s�˿�%d",
			strAddr,unPort,strAddr1,unPort1);
		pDlg->UpdateMsgData();
	}
	else
	{
		delete pSocket;
	}
	CSocket::OnAccept(nErrorCode);
}
