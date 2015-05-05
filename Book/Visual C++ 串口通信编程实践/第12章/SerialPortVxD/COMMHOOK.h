// COMMHOOK.h - include file for VxD COMMHOOK
#ifndef _COMMHOOK_H
#define _COMMHOOK_H

#ifdef	DEVICE_MAIN
#include <vtoolscp.h>
#endif

typedef struct
	{
	DWORD		dwOpenCount;
	DWORD		dwCloseCount;
	DWORD		dwNotMineCloseCount;
	DWORD		dwReadCount;
	DWORD		dwReadBytes;
	DWORD		dwWriteCount;
	DWORD		dwWriteBytes;
	} _sAccessStats;

#define	_CommHook_DIOC_AccessStats		1
#define _CommHook_DIOC_ClearStats		2
#define _CommHook_DIOC_Handles			3
#define _CommHook_DIOC_SetTargetPort	4
#define _CommHook_DIOC_WriteToPort		5
#define _CommHook_DIOC_ReadFilteredData	6
#define _CommHook_DIOC_ReadTraceData	7





#ifdef	DEVICE_MAIN
#define DEVICE_CLASS		CommhookDevice
#define COMMHOOK_DeviceID		UNDEFINED_DEVICE_ID
#define COMMHOOK_Init_Order	UNDEFINED_INIT_ORDER
#define COMMHOOK_Major		1
#define COMMHOOK_Minor		0

class CDataBlock
	{
	private:
		CDataBlock() {}
	public:
		CDataBlock( unsigned char *pData, bool bWritten, DWORD dwLength )
			{
			m_pData = new unsigned char[ m_dwLength = dwLength ];
			memcpy( m_pData, pData, m_dwLength );
			m_bWritten = bWritten;
			m_pNextBlock = NULL;
			}
		~CDataBlock()
			{
			delete[] m_pData;
			}

		unsigned char	*m_pData;
		bool			m_bWritten;
		DWORD			m_dwLength;

		CDataBlock		*m_pNextBlock;
	};

class CommhookDevice : public VDevice
{
public:
	virtual BOOL OnSysCriticalInit(VMHANDLE hSysVM, PCHAR pszCmdTail, PVOID refData);
	virtual VOID OnSysCriticalExit();
	virtual BOOL OnSysDynamicDeviceInit();
	virtual BOOL OnSysDynamicDeviceExit();
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);

	VOID DoHooks( void );
	VOID DoUnhooks( void );
	void DeleteHandles( void );
	void SetupData( void );
	void ClearStats( void );
	void Cleanup( void );

	HANDLE			*pHandles;
	int				nNumHandles;
	_sAccessStats	sAccessStats;

	COMMPORTHANDLE	hPort;
	char			*cpTargetPortName;

	VMutex	*mutexWrite;

	unsigned char	FilteredData[0x400];
	DWORD			FilteredCount;
	DWORD			FilteredRead;
	DWORD			FilteredWrite;

	VMutex			*mutexTrace;
	DWORD			m_dwTraceSize;
	CDataBlock		*m_pFirstTraceBlock;
	CDataBlock		*m_pLastTraceBlock;
	void			AddTraceBlock( unsigned char *pData, bool bWritten, DWORD dwLength );
	CDataBlock		*GetNextTraceBlock();
};

class CommhookVM : public VVirtualMachine
{
public:
	CommhookVM(VMHANDLE hVM);
};

class CommhookThread : public VThread
{
public:
	CommhookThread(THREADHANDLE hThread);
};



#endif	// DEVICE_MAIN
#endif	// _COMMHOOK_H
