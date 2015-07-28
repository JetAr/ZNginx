// SmsTraffic.cpp: implementation of the CSmsTraffic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsTest.h"
#include "SmsTraffic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmsTraffic::CSmsTraffic()
{
    m_nSendIn = 0;
    m_nSendOut = 0;
    m_nRecvIn = 0;
    m_nRecvOut = 0;

    m_hKillThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hThreadKilledEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    InitializeCriticalSection(&m_csSend);
    InitializeCriticalSection(&m_csRecv);

    // �������߳�
    AfxBeginThread(SmThread, this, THREAD_PRIORITY_NORMAL);
}

CSmsTraffic::~CSmsTraffic()
{
    SetEvent(m_hKillThreadEvent);			// �����ر����̵߳��ź�
    WaitForSingleObject(m_hThreadKilledEvent, INFINITE);	// �ȴ����̹߳ر�

    DeleteCriticalSection(&m_csSend);
    DeleteCriticalSection(&m_csRecv);

    CloseHandle(m_hKillThreadEvent);
    CloseHandle(m_hThreadKilledEvent);
}

// ��һ������Ϣ���뷢�Ͷ���
void CSmsTraffic::PutSendMessage(SM_PARAM* pparam)
{
    EnterCriticalSection(&m_csSend);

    memcpy(&m_SmSend[m_nSendIn], pparam, sizeof(SM_PARAM));

    m_nSendIn++;
    if (m_nSendIn >= MAX_SM_SEND)  m_nSendIn = 0;

    LeaveCriticalSection(&m_csSend);
}

// �ӷ��Ͷ�����ȡһ������Ϣ
BOOL CSmsTraffic::GetSendMessage(SM_PARAM* pparam)
{
    BOOL fSuccess = FALSE;

    EnterCriticalSection(&m_csSend);

    if (m_nSendOut != m_nSendIn)
    {
        memcpy(pparam, &m_SmSend[m_nSendOut], sizeof(SM_PARAM));

        m_nSendOut++;
        if (m_nSendOut >= MAX_SM_SEND)  m_nSendOut = 0;

        fSuccess = TRUE;
    }

    LeaveCriticalSection(&m_csSend);

    return fSuccess;
}

// ������Ϣ������ն���
void CSmsTraffic::PutRecvMessage(SM_PARAM* pparam, int nCount)
{
    EnterCriticalSection(&m_csRecv);

    for (int i = 0; i < nCount; i++)
    {
        memcpy(&m_SmRecv[m_nRecvIn], pparam, sizeof(SM_PARAM));

        m_nRecvIn++;
        if (m_nRecvIn >= MAX_SM_RECV)  m_nRecvIn = 0;

        pparam++;
    }

    LeaveCriticalSection(&m_csRecv);
}

// �ӽ��ն�����ȡһ������Ϣ
BOOL CSmsTraffic::GetRecvMessage(SM_PARAM* pparam)
{
    BOOL fSuccess = FALSE;

    EnterCriticalSection(&m_csRecv);

    if (m_nRecvOut != m_nRecvIn)
    {
        memcpy(pparam, &m_SmRecv[m_nRecvOut], sizeof(SM_PARAM));

        m_nRecvOut++;
        if (m_nRecvOut >= MAX_SM_RECV)  m_nRecvOut = 0;

        fSuccess = TRUE;
    }

    LeaveCriticalSection(&m_csRecv);

    return fSuccess;
}


UINT CSmsTraffic::SmThread(LPVOID lParam)
{
    CSmsTraffic* p=(CSmsTraffic *)lParam;	// this
    int nMsg;				// �յ�����Ϣ����
    int nDelete;			// Ŀǰ����ɾ���Ķ���Ϣ���
    SM_BUFF buff;			// ���ն���Ϣ�б�Ļ�����
    SM_PARAM param[256];	// ����/���ն���Ϣ������
    CTime tmOrg, tmNow;		// �ϴκ����ڵ�ʱ�䣬���㳬ʱ��
    enum
    {
        stBeginRest,				// ��ʼ��Ϣ/��ʱ
        stContinueRest,				// ������Ϣ/��ʱ
        stSendMessageRequest,		// ���Ͷ���Ϣ
        stSendMessageResponse,		// ��ȡ����Ϣ�б�������
        stSendMessageWaitIdle,		// ���Ͳ��ɹ����ȴ�GSM����
        stReadMessageRequest,		// ���Ͷ�ȡ����Ϣ�б������
        stReadMessageResponse,		// ��ȡ����Ϣ�б�������
        stDeleteMessageRequest,		// ɾ������Ϣ
        stDeleteMessageResponse,	// ɾ������Ϣ
        stDeleteMessageWaitIdle,	// ɾ�����ɹ����ȴ�GSM����
        stExitThread				// �˳�
    } nState;				// ������̵�״̬

    // ��ʼ״̬
    nState = stBeginRest;

    // ���ͺͽ��մ����״̬ѭ��
    while (nState != stExitThread)
    {
        switch(nState)
        {
        case stBeginRest:
//				TRACE("State=stBeginRest\n");
            tmOrg = CTime::GetCurrentTime();
            nState = stContinueRest;
            break;

        case stContinueRest:
//				TRACE("State=stContinueRest\n");
            Sleep(300);
            tmNow = CTime::GetCurrentTime();
            if (p->GetSendMessage(&param[0]))
            {
                nState = stSendMessageRequest;	// �д�������Ϣ���Ͳ���Ϣ��
            }
            else if (tmNow - tmOrg >= 5)		// ��������Ϣ���пգ���Ϣ5��
            {
                nState = stReadMessageRequest;	// ת����ȡ����Ϣ״̬
            }
            break;

        case stSendMessageRequest:
//				TRACE("State=stSendMessageRequest\n");
            gsmSendMessage(&param[0]);
            memset(&buff, 0, sizeof(buff));
            tmOrg = CTime::GetCurrentTime();
            nState = stSendMessageResponse;
            break;

        case stSendMessageResponse:
//				TRACE("State=stSendMessageResponse\n");
            Sleep(100);
            tmNow = CTime::GetCurrentTime();
            switch (gsmGetResponse(&buff))
            {
            case GSM_OK:
//						TRACE("  GSM_OK %d\n", tmNow - tmOrg);
                nState = stBeginRest;
                break;
            case GSM_ERR:
//						TRACE("  GSM_ERR %d\n", tmNow - tmOrg);
                nState = stSendMessageWaitIdle;
                break;
            default:
//						TRACE("  GSM_WAIT %d\n", tmNow - tmOrg);
                if (tmNow - tmOrg >= 10)		// 10�볬ʱ
                {
//							TRACE("  Timeout!\n");
                    nState = stSendMessageWaitIdle;
                }
            }
            break;

        case stSendMessageWaitIdle:
            Sleep(500);
            nState = stSendMessageRequest;		// ֱ�����ͳɹ�Ϊֹ
            break;

        case stReadMessageRequest:
//				TRACE("State=stReadMessageRequest\n");
            gsmReadMessageList();
            memset(&buff, 0, sizeof(buff));
            tmOrg = CTime::GetCurrentTime();
            nState = stReadMessageResponse;
            break;

        case stReadMessageResponse:
//				TRACE("State=stReadMessageResponse\n");
            Sleep(100);
            tmNow = CTime::GetCurrentTime();
            switch (gsmGetResponse(&buff))
            {
            case GSM_OK:
//						TRACE("  GSM_OK %d\n", tmNow - tmOrg);
                nMsg = gsmParseMessageList(param, &buff);
                if (nMsg > 0)
                {
                    p->PutRecvMessage(param, nMsg);
                    nDelete = 0;
                    nState = stDeleteMessageRequest;
                }
                else
                {
                    nState = stBeginRest;
                }
                break;
            case GSM_ERR:
//						TRACE("  GSM_ERR %d\n", tmNow - tmOrg);
                nState = stBeginRest;
                break;
            default:
//						TRACE("  GSM_WAIT %d\n", tmNow - tmOrg);
                if (tmNow - tmOrg >= 15)		// 15�볬ʱ
                {
//							TRACE("  Timeout!\n");
                    nState = stBeginRest;
                }
            }
            break;

        case stDeleteMessageRequest:
//				TRACE("State=stDeleteMessageRequest\n");
            if (nDelete < nMsg)
            {
                gsmDeleteMessage(param[nDelete].index);
                memset(&buff, 0, sizeof(buff));
                tmOrg = CTime::GetCurrentTime();
                nState = stDeleteMessageResponse;
            }
            else
            {
                nState = stBeginRest;
            }
            break;

        case stDeleteMessageResponse:
//				TRACE("State=stDeleteMessageResponse\n");
            Sleep(100);
            tmNow = CTime::GetCurrentTime();
            switch (gsmGetResponse(&buff))
            {
            case GSM_OK:
//						TRACE("  GSM_OK %d\n", tmNow - tmOrg);
                nDelete++;
                nState = stDeleteMessageRequest;
                break;
            case GSM_ERR:
//						TRACE("  GSM_ERR %d\n", tmNow - tmOrg);
                nState = stDeleteMessageWaitIdle;
                break;
            default:
//						TRACE("  GSM_WAIT %d\n", tmNow - tmOrg);
                if (tmNow - tmOrg >= 5)		// 5�볬ʱ
                {
//							TRACE("  Timeout!\n");
                    nState = stBeginRest;
                }
            }
            break;

        case stDeleteMessageWaitIdle:
//				TRACE("State=stDeleteMessageWaitIdle\n");
            Sleep(500);
            nState = stDeleteMessageRequest;		// ֱ��ɾ���ɹ�Ϊֹ
            break;
        }

        // ����Ƿ��йرձ��̵߳��ź�
        DWORD dwEvent = WaitForSingleObject(p->m_hKillThreadEvent, 20);
        if (dwEvent == WAIT_OBJECT_0)  nState = stExitThread;
    }

    // �ø��߳̽�����־
    SetEvent(p->m_hThreadKilledEvent);

    return 9999;
}
