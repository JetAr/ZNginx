// MySocket.cpp : implementation file
//
#include "stdafx.h"
#include "SClient.h"
#include "MySocket.h"
#include "SClientDlg.h"   //��ӵ����Ի�����ͷ�ļ�

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySocket

CMySocket::CMySocket()
{
}

CMySocket::~CMySocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMySocket, CSocket)
	//{{AFX_MSG_MAP(CMySocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CMySocket member functions

void CMySocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	char chMsg[5120], chMsgTemp[1024];
	UINT unRXCharNum;    //ÿ�ζ�ȡ���ַ���
	BOOL bEndFlag=0;   //������ϱ�־
	strcpy(chMsg,"");
	do
	{
		strcpy(chMsgTemp,"");
		unRXCharNum=Receive(chMsgTemp,1000);
		if(unRXCharNum>1000||unRXCharNum<=0)
		{
			AfxMessageBox("���������г���",MB_OK);
			return;
		}
		else if(unRXCharNum<1000 && unRXCharNum>0)
		{
            bEndFlag=1;
		}
		chMsgTemp[unRXCharNum]=0; //�����ַ�������λ
		strcat(chMsg,chMsgTemp);
	}while(bEndFlag==0);
	CSClientDlg* pDlg=(CSClientDlg*)AfxGetMainWnd(); //�õ������(�Ի���)ָ��
	pDlg->m_strRXDataTemp.Format("%s",chMsg);
	pDlg->UpdateRXData();   //�����Ի��Ľ��ձ༭������ʾ���յ�������
	//ע��һ�£����߻���������Ϣ������Ʒ��ͽ��յ�������
	//::SendMessage(pDlg->m_hWnd,WM_MYMESSAGE,0,0);
	CSocket::OnReceive(nErrorCode);
}
