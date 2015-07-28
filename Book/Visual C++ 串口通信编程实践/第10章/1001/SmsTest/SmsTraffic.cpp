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

    // 启动子线程
    AfxBeginThread(SmThread, this, THREAD_PRIORITY_NORMAL);
}

CSmsTraffic::~CSmsTraffic()
{
    SetEvent(m_hKillThreadEvent);			// 发出关闭子线程的信号
    WaitForSingleObject(m_hThreadKilledEvent, INFINITE);	// 等待子线程关闭

    DeleteCriticalSection(&m_csSend);
    DeleteCriticalSection(&m_csRecv);

    CloseHandle(m_hKillThreadEvent);
    CloseHandle(m_hThreadKilledEvent);
}

// 将一条短消息放入发送队列
void CSmsTraffic::PutSendMessage(SM_PARAM* pparam)
{
    EnterCriticalSection(&m_csSend);

    memcpy(&m_SmSend[m_nSendIn], pparam, sizeof(SM_PARAM));

    m_nSendIn++;
    if (m_nSendIn >= MAX_SM_SEND)  m_nSendIn = 0;

    LeaveCriticalSection(&m_csSend);
}

// 从发送队列中取一条短消息
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

// 将短消息放入接收队列
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

// 从接收队列中取一条短消息
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
    int nMsg;				// 收到短消息条数
    int nDelete;			// 目前正在删除的短消息编号
    SM_BUFF buff;			// 接收短消息列表的缓冲区
    SM_PARAM param[256];	// 发送/接收短消息缓冲区
    CTime tmOrg, tmNow;		// 上次和现在的时间，计算超时用
    enum
    {
        stBeginRest,				// 开始休息/延时
        stContinueRest,				// 继续休息/延时
        stSendMessageRequest,		// 发送短消息
        stSendMessageResponse,		// 读取短消息列表到缓冲区
        stSendMessageWaitIdle,		// 发送不成功，等待GSM就绪
        stReadMessageRequest,		// 发送读取短消息列表的命令
        stReadMessageResponse,		// 读取短消息列表到缓冲区
        stDeleteMessageRequest,		// 删除短消息
        stDeleteMessageResponse,	// 删除短消息
        stDeleteMessageWaitIdle,	// 删除不成功，等待GSM就绪
        stExitThread				// 退出
    } nState;				// 处理过程的状态

    // 初始状态
    nState = stBeginRest;

    // 发送和接收处理的状态循环
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
                nState = stSendMessageRequest;	// 有待发短消息，就不休息了
            }
            else if (tmNow - tmOrg >= 5)		// 待发短消息队列空，休息5秒
            {
                nState = stReadMessageRequest;	// 转到读取短消息状态
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
                if (tmNow - tmOrg >= 10)		// 10秒超时
                {
//							TRACE("  Timeout!\n");
                    nState = stSendMessageWaitIdle;
                }
            }
            break;

        case stSendMessageWaitIdle:
            Sleep(500);
            nState = stSendMessageRequest;		// 直到发送成功为止
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
                if (tmNow - tmOrg >= 15)		// 15秒超时
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
                if (tmNow - tmOrg >= 5)		// 5秒超时
                {
//							TRACE("  Timeout!\n");
                    nState = stBeginRest;
                }
            }
            break;

        case stDeleteMessageWaitIdle:
//				TRACE("State=stDeleteMessageWaitIdle\n");
            Sleep(500);
            nState = stDeleteMessageRequest;		// 直到删除成功为止
            break;
        }

        // 检测是否有关闭本线程的信号
        DWORD dwEvent = WaitForSingleObject(p->m_hKillThreadEvent, 20);
        if (dwEvent == WAIT_OBJECT_0)  nState = stExitThread;
    }

    // 置该线程结束标志
    SetEvent(p->m_hThreadKilledEvent);

    return 9999;
}
