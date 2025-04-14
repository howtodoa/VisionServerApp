#include "pch.h"
#include "Mz_LogConn.h"
#include "MsvLog.h"

namespace Mz_Log
{
	static CMsvLog* m_log = NULL;
	int COperation:: Mz_Realse()
	{
		if(m_log != NULL)
			delete m_log;
		return 0;
	}

	int COperation:: Mz_GetInstance(const wchar_t* filename)
	{
		if (m_log == NULL)//change by tjq
		{
			m_log = new CMsvLog(_T("var"), filename);
		}
		return 0;
	}


	int COperation:: Mz_SetLogFile(const wchar_t* pFileName)
	{
		if (m_log != NULL)
			m_log->SetLogFile(pFileName);
		return 0;
	}

	int COperation:: Mz_AddLog(const wchar_t* _logStr)
	{
		if (m_log != NULL)
			m_log->AddLog(_logStr);
		return 0;
	}

	int COperation:: Mz_CheckDateTime()
	{
		if (m_log != NULL)
			m_log->CheckDateTime();
		return 0;
	}
}
