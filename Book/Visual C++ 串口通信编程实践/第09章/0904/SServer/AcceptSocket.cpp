// AcceptSocket.cpp : implementation file
//

#include "stdafx.h"
#include "SServer.h"
#include "AcceptSocket.h"
#include "SServerDlg.h"    //��ӵ����Ի�����ͷ�ļ�

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAcceptSocket

CAcceptSocket::CAcceptSocket()
{
}

CAcceptSocket::~CAcceptSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CAcceptSocket, CSocket)
	//{{AFX_MSG_MAP(CAcceptSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CAcceptSocket member functions


void CAcceptSocket::OnReceive(int nErrorCode) 
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
	CSServerDlg* pDlg=(CSServerDlg*)AfxGetMainWnd(); //�õ������(�Ի���)ָ��
	pDlg->m_strNetMsg.Format("���յ���%s",chMsg);
	pDlg->UpdateMsgData();   //�����Ի��Ľ��ձ༭������ʾ����˿ڽ��յ�������
	CString strtemp;
	strtemp.Format("%s",chMsg);
	pDlg->SerialSendData(strtemp);
	strtemp="���������յ�" + strtemp; //��ɻش���Ϣ
	Send(strtemp,strtemp.GetLength(),0); //���ͻش���Ϣ
 	CSocket::OnReceive(nErrorCode);
}
