// MySocket.cpp : implementation file
//
#include "stdafx.h"
#include "SClient.h"
#include "MySocket.h"
#include "SClientDlg.h"   //添加的主对话框类头文件

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
	UINT unRXCharNum;    //每次读取的字符数
	BOOL bEndFlag=0;   //接收完毕标志
	strcpy(chMsg,"");
	do
	{
		strcpy(chMsgTemp,"");
		unRXCharNum=Receive(chMsgTemp,1000);
		if(unRXCharNum>1000||unRXCharNum<=0)
		{
			AfxMessageBox("接收数据中出错",MB_OK);
			return;
		}
		else if(unRXCharNum<1000 && unRXCharNum>0)
		{
            bEndFlag=1;
		}
		chMsgTemp[unRXCharNum]=0; //加上字符串结束位
		strcat(chMsg,chMsgTemp);
	}while(bEndFlag==0);
	CSClientDlg* pDlg=(CSClientDlg*)AfxGetMainWnd(); //得到主框口(对话框)指针
	pDlg->m_strRXDataTemp.Format("%s",chMsg);
	pDlg->UpdateRXData();   //在主对话的接收编辑框中显示接收到的数据
	//注意一下：读者还可以用消息处理机制发送接收到的数据
	//::SendMessage(pDlg->m_hWnd,WM_MYMESSAGE,0,0);
	CSocket::OnReceive(nErrorCode);
}
