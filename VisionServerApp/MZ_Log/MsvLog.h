#pragma once
#include <fstream>
#include <strstream>
#include <sstream>
#include <vector>
#include <tchar.h>
#include <windows.h>



#if defined UNICODE || defined _UNICODE

typedef std::wstring			_tstring;				
typedef std::wostringstream		_tostringstream;		
typedef std::wofstream			_tofstream;		

#else

typedef std::string				_tstring;				
typedef std::ostringstream		_tostringstream;			
typedef std::ofstream			_tofstream;		

#endif 

#define MAX_LOG_MSG_LEN			1100
#define MAX_HEAP_VEC_NUM		80000
#define MAX_LOG_INSTANCE_NUM	40

class LogMessageData
{

public:
	TCHAR m_pData[MAX_LOG_MSG_LEN];			//���ݻ�����
	int m_nLen;									//���ݵĳ���

	LogMessageData()
	{
		ZeroMemory(m_pData, sizeof(m_pData));
		m_nLen = 0;
	}

	void* operator new(size_t nSize)
	{
		void* pContext = NULL;

		try
		{
			EnterCriticalSection(&s_IDLQueLock);
			if (NULL == s_Heap)
			{
				throw ((long)(__LINE__));
			}

			std:: vector<LogMessageData* >::iterator iter = s_IDLQue.begin();
			if (s_IDLQue.end() != iter)
			{
				pContext = *iter;
				s_IDLQue.erase(iter);
			}
			else
			{
				pContext = HeapAlloc(s_Heap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, nSize);

				if (NULL != pContext)
					s_Count++;
			}			

			if (NULL == pContext)
			{
				DWORD dwError = GetLastError();
				throw ((long)(__LINE__));

			}
			LeaveCriticalSection(&s_IDLQueLock);
		}
		catch (const long& iErrCode)
		{
			pContext = NULL;
			LeaveCriticalSection(&s_IDLQueLock);
			char errmsg[1024];
			sprintf_s(errmsg, 1024,  "\nnew REAL_UDP_CONTEXT error Exp : %s--%ld", __FILE__, iErrCode);
		}

		return pContext;
	}

	void operator delete(void* p)
	{
		if (p)
		{
			EnterCriticalSection(&s_IDLQueLock);
			const DWORD QUE_SIZE = (DWORD)(s_IDLQue.size());
			LogMessageData* const pContext = (LogMessageData*)p;

			if (QUE_SIZE <= MAX_IDL_DATA)
			{
				s_IDLQue.push_back(pContext);
			}
			else
			{
				HeapFree(s_Heap, HEAP_NO_SERIALIZE, p);
			}
			LeaveCriticalSection(&s_IDLQueLock);		
		}

		return;
	}

	static void InitReource()
	{
		if (NULL == s_Heap)
		{
			s_Heap = HeapCreate(0, S_PAGE_SIZE, RCV_HEAP_SIZE);

			InitializeCriticalSection(&(s_IDLQueLock));

			s_IDLQue.reserve(MAX_IDL_DATA );
		}
	}

	static void ReleaseReource()
	{
		if (NULL != s_Heap)
		{
			HeapDestroy(s_Heap);
			s_Heap = NULL;

			EnterCriticalSection(&(s_IDLQueLock));
			s_IDLQue.clear();
			LeaveCriticalSection(&(s_IDLQueLock));

			DeleteCriticalSection(&(s_IDLQueLock));
		}
	}

	enum
	{
		S_PAGE_SIZE = (MAX_LOG_MSG_LEN+4),
		MAX_IDL_DATA = MAX_HEAP_VEC_NUM,									//���д�С
		RCV_HEAP_SIZE = S_PAGE_SIZE * MAX_IDL_DATA,			//s_Heap�ѵĴ�С	
	};

private:
	static std::vector<LogMessageData* > s_IDLQue;
	static CRITICAL_SECTION s_IDLQueLock;
	static HANDLE s_Heap;			//RCV_DATA�Ķ�
	static int s_Count;
};


class CMsvLog
{
	
	friend class LogWrapper;
public:

	static CMsvLog &GetInstance()
	{
		static  CMsvLog instance;
		return  instance;
	}

	static CMsvLog & GetInstance(int nIndex);
	static CMsvLog& GetInstance(TCHAR *pFileName);

	CMsvLog(void);
	CMsvLog(const TCHAR * pStrLogPath);
	CMsvLog(const TCHAR * pStrLogFolderName, const TCHAR * pFileName);
	~CMsvLog(void);


public:
	int	 SetLogFile(const TCHAR * pFileName = _T("log"));	//��ʼ����־�ļ�
	int	 AddLog(const TCHAR *_logStr);	//����־�ļ��������־���ݣ��ַ����飩
	int	 CheckDateTime();				//��־һ��һ��  ��һ������дһ���ļ�

	BOOL m_bRunningFlag;

private:
	//����ļ��������汾��������־
	static CMsvLog * m_pInstance[MAX_LOG_INSTANCE_NUM];
	static CMsvLog* m_pInstanceSingle;

	_tofstream			m_logFile;					//��־�ļ�
	
	_tstring m_strMoudlePath;			//��־�ļ�����Ŀ¼

	_tstring m_strFileHeadName;

	SYSTEMTIME m_timeStartUp;
	
	enum { MAX_VEC_LENGTH = (LogMessageData::MAX_IDL_DATA)/MAX_LOG_INSTANCE_NUM - 10};

	CRITICAL_SECTION m_secLog;
	std::vector<LogMessageData *> m_vecLogMsg;
	int PushLogMsg(const TCHAR * pStrLog);

	void DoInitialize(const TCHAR * pFileName = _T("log"));

	static LogWrapper s_Wrapper;
	static int s_Use_Count;

	HANDLE m_hTreadWrite;
	static unsigned int __stdcall ThreadWriteProc(void * pParam);
};

class LogWrapper
{
public:
	LogWrapper(){}
	~LogWrapper()
	{
		for (int i = 0; i < MAX_LOG_INSTANCE_NUM; ++i)
		{
			delete CMsvLog::m_pInstance[i];
			CMsvLog::m_pInstance[i] = NULL;
		}
		CMsvLog::m_pInstanceSingle = NULL;
	}
};
