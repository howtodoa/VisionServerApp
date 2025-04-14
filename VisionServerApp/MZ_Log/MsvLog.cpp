#pragma once
#include "pch.h"
#include "MsvLog.h"
#include <windows.h>
#include <time.h>
#include  <comdef.h>   
#include <process.h>
#include <shlwapi.h>
#include <shlobj.h>
#pragma comment(lib,"shlwapi.lib")

int CMsvLog::s_Use_Count = 0;
CMsvLog * CMsvLog::m_pInstance[MAX_LOG_INSTANCE_NUM] = {0};
CMsvLog* CMsvLog::m_pInstanceSingle = NULL;
LogWrapper CMsvLog::s_Wrapper;


HANDLE								LogMessageData::s_Heap = NULL;
std::vector<LogMessageData* >		LogMessageData::s_IDLQue;
CRITICAL_SECTION					LogMessageData::s_IDLQueLock;
int LogMessageData::s_Count = 0;


CMsvLog::CMsvLog()
	: m_bRunningFlag(FALSE)
	, m_hTreadWrite(NULL)
	, m_strFileHeadName(_T("log"))
{
	if (m_strMoudlePath.empty())
	{
		TCHAR tmpBuff[512];

		GetModuleFileName(NULL, tmpBuff, _MAX_PATH);
		_tstring Path = tmpBuff;
		int nPos = Path.rfind(_T("\\"));
		Path = Path.substr(0,nPos);

		m_strMoudlePath = Path + _T("\\..\\var");

	}

	DoInitialize(m_strFileHeadName.c_str());

}

CMsvLog::CMsvLog(const TCHAR * pStrLogFolderName, const TCHAR * pFileName)
	: m_bRunningFlag(FALSE)
	, m_hTreadWrite(NULL)
	, m_strFileHeadName(pFileName)
{

	if (m_strMoudlePath.empty())
	{
		TCHAR tmpBuff[512];

		GetModuleFileName(NULL, tmpBuff, _MAX_PATH);
		_tstring Path = tmpBuff;
		int nPos = Path.rfind(_T("\\"));
		Path = Path.substr(0,nPos);

		m_strMoudlePath = Path + _T("\\..\\") + pStrLogFolderName;

	}

	DoInitialize(m_strFileHeadName.c_str());

}

CMsvLog::CMsvLog(const TCHAR * pStrLogPath)
	: m_strMoudlePath(pStrLogPath)
	, m_strFileHeadName(_T("log"))
	, m_bRunningFlag(FALSE)
	, m_hTreadWrite(NULL)
{
	DoInitialize(m_strFileHeadName.c_str());
}

void CMsvLog::DoInitialize(const TCHAR * pFileName)
{
	if (s_Use_Count >= MAX_LOG_INSTANCE_NUM)
	{
		return;
	}
	if (0 == s_Use_Count++)
	{
		LogMessageData::InitReource();

	}

	InitializeCriticalSection(&(m_secLog));

	if (!PathIsDirectory(m_strMoudlePath.c_str()))
	{
		int ne = SHCreateDirectoryEx(NULL,m_strMoudlePath.c_str(),NULL);
	}

	SetLogFile(pFileName);

	m_bRunningFlag = TRUE;
	m_hTreadWrite = (HANDLE)_beginthreadex(NULL, 0, ThreadWriteProc, this, 0, NULL);

	AddLog(_T("///////////////////////////////////////////////////////////////////////////////"));
	AddLog(_T("日志初始化完毕，开始记录"));
}

CMsvLog::~CMsvLog()
{
	m_bRunningFlag = FALSE;
	WaitForSingleObject(m_hTreadWrite, INFINITE);

	DeleteCriticalSection(&(m_secLog));

	s_Use_Count--;
	if (0 == s_Use_Count)
	{
		LogMessageData::ReleaseReource();
	}

}

CMsvLog & CMsvLog::GetInstance(int nIndex)
{
	int tmpIndex = nIndex;
	if (tmpIndex < 0 || tmpIndex >= MAX_LOG_INSTANCE_NUM)
		tmpIndex = 0;

	if ( NULL == m_pInstance[tmpIndex] )
	{
		_tostringstream  strmName;
		strmName << _T("camera(") << tmpIndex+1 << _T(")_") << std::ends;
		m_pInstance[tmpIndex] = new CMsvLog(_T("var"), strmName.str().c_str());
	}

	return *m_pInstance[tmpIndex];
}

CMsvLog& CMsvLog::GetInstance(TCHAR* pFileName)
{
	if (m_pInstanceSingle == NULL) {
		m_pInstanceSingle = new CMsvLog(_T("var"), pFileName);
	}
	return *m_pInstanceSingle;
}


//根据路径和文件名，初始化日志文件
int CMsvLog::SetLogFile(const TCHAR * pFileName)
{
	GetLocalTime(&m_timeStartUp);

	_tostringstream  strmLogFileName;
	strmLogFileName << m_strMoudlePath << _T("\\") << pFileName;
	strmLogFileName << m_timeStartUp.wYear << _T("_") << m_timeStartUp.wMonth << _T("_") << m_timeStartUp.wDay;
	strmLogFileName << _T(".txt") << std::ends;

	std::locale loc = std::locale::global(std::locale("")); //要打开的文件路径含中文，设置全局locale为本地环境

	if (m_logFile.is_open())
	{
		m_logFile.flush();
		m_logFile.close();
	}

	m_logFile.open(strmLogFileName.str().c_str(), std::ios_base::out | std::ios_base::app);
	if (!m_logFile.is_open())
	{
		m_logFile.open(strmLogFileName.str().c_str(), std::ios_base::out | std::ios_base::trunc);
	}

	std::locale::global(loc);     

	return 0;
}
//向日志文件中添加日志内容（字符数组）
int	CMsvLog::AddLog(const TCHAR *_logStr)
{
	if (s_Use_Count < MAX_LOG_INSTANCE_NUM)
	{
		//保存到文件中
		TCHAR date[20];
		TCHAR time[20];

		_tstrtime_s(time);
		_tstrdate_s(date);

		_tostringstream str;
		str << date << _T(" ") << time << _T(": ") << _logStr << std::ends;

		PushLogMsg(str.str().c_str());
	}

	return 0;
}


int CMsvLog::PushLogMsg(const TCHAR * pStrLog)
{
	LogMessageData * pLogData = new LogMessageData();

	if (NULL != pLogData)
	{
		_tcscpy_s( pLogData->m_pData, MAX_LOG_MSG_LEN, pStrLog );

		EnterCriticalSection(&m_secLog);

		if ( m_vecLogMsg.size() >= MAX_VEC_LENGTH )
		{
			std::vector<LogMessageData *>::iterator iter = m_vecLogMsg.begin();
			delete *iter;
			m_vecLogMsg.erase(iter);
		}

		m_vecLogMsg.push_back(pLogData);

		LeaveCriticalSection(&m_secLog);
	}

	return 0;
}

int	 CMsvLog::CheckDateTime()
{
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);

	if (m_timeStartUp.wDay != curTime.wDay)
	{
		SetLogFile(m_strFileHeadName.c_str());
	}

	return 0;
}

unsigned int __stdcall CMsvLog::ThreadWriteProc(void * pParam)
{
	CMsvLog * pThis = (CMsvLog *)pParam;

	int nMaxWriteBufLen = MAX_LOG_MSG_LEN;

	char * pWriteBuf = new char[nMaxWriteBufLen];

	BOOL bExistMsgFlag = FALSE;

	while (pThis->m_bRunningFlag)
	{
		pThis->CheckDateTime();

		EnterCriticalSection(&pThis->m_secLog);

		std::vector<LogMessageData *>::iterator iter = pThis->m_vecLogMsg.begin();

		if (iter != pThis->m_vecLogMsg.end())
		{
			bExistMsgFlag = TRUE;

			LogMessageData * pLogData = *iter;

#ifdef UNICODE 

			int nLength = ::WideCharToMultiByte( CP_ACP, 0, pLogData->m_pData, -1, NULL, 0, NULL, NULL );
			nLength += 2;

			if (nLength > nMaxWriteBufLen)
			{
				delete[] pWriteBuf;
				pWriteBuf = new char[nLength];
				nMaxWriteBufLen = nLength;
			}

			if (NULL != pWriteBuf)
			{
				ZeroMemory(pWriteBuf, sizeof(char) * nMaxWriteBufLen);
				int code = ::WideCharToMultiByte(CP_ACP,0,pLogData->m_pData,-1,pWriteBuf,nLength,NULL,NULL);	
			}
#else
			_tcscpy_s(pWriteBuf, MAX_LOG_MSG_LEN, pLogData->m_pData);
#endif

			pThis->m_logFile << pWriteBuf << std::endl << std::flush;

			delete *iter; *iter = NULL; pLogData = NULL;
			pThis->m_vecLogMsg.erase(iter);
		}
		else
		{
			bExistMsgFlag = FALSE;
		}


		LeaveCriticalSection(&pThis->m_secLog);

		if (bExistMsgFlag)
			Sleep(0);
		else
			Sleep(10);
	}

	delete[] pWriteBuf; pWriteBuf = NULL;

	return 0;
}

