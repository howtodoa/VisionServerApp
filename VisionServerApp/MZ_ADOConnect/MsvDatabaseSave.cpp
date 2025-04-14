#include "pch.h"
#include "MsvDatabaseSave.h"
#include <process.h>

MsvDatabaseSave::MsvDatabaseSave(void)
	: m_pDB(NULL)
	, m_pRS(NULL)
{
}


MsvDatabaseSave::~MsvDatabaseSave(void)
{
	
}

int MsvDatabaseSave::ConnectDB(wchar_t *connectString)
{
// 	TCHAR connectString[1024] = {0};
// 	_stprintf_s(connectString, 1024, _T("%s"), _T("Provider=SQLOLEDB.1;Server=127.0.0.1,1433;DATABASE=GlassDetect;UID=sa;PWD=123456"));
// 	SystemLog.AddLog(_T("�����ļ����Ҳ������ݿ������ֶ�DB������ΪĬ�ϣ�Provider=SQLOLEDB.1;Server=127.0.0.1,1433;DATABASE=GlassDetect;UID=sa;PWD=123456"));
	if (m_pRS)
	{
		return 1;
	}
	try
	{
		//���ݿ����ĳ�ʼ��
		m_pDB = new CADODatabase;
		BOOL isopen = m_pDB->Open(connectString);
		if (isopen)
		{
			m_pRS = new CADORecordset(m_pDB);
		}
		else
		{
			SystemLog.Mz_AddLog(_T("���ݿⲻ���ڻ����Ӳ��ϣ�"));
			return -1;
		}
	}
	catch (_com_error e)
	{
		TCHAR errormessage[1024] = {0};
		_stprintf_s(errormessage, 1024, _T("�������ݿ�ʧ��!\r\n������Ϣ:%s"), e.ErrorMessage());
		SystemLog.Mz_AddLog(errormessage);
		return -1;
	}

	return 0;
}

int MsvDatabaseSave::CloseDB()
{
	if (m_pRS && m_pRS->IsOpen())
		m_pRS->Close();
	if (m_pDB && m_pDB->IsOpen())
		m_pDB->Close();
	delete m_pDB; m_pDB = NULL;
	delete m_pRS; m_pRS = NULL;

	return 0;
}

int MsvDatabaseSave::OpenRecordset(wchar_t* excuseSql)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->Open(excuseSql, CADORecordset::openQuery))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::CloseRecordset()
{
	if (m_pRS != NULL)
	{
		m_pRS->Close();
	}
	return 0;
}

int MsvDatabaseSave::GetRecordValue(LPCTSTR KeyValue, double& Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->GetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::GetRecordValue(LPCTSTR KeyValue, long& Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->GetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::GetRecordValue(LPCTSTR KeyValue, int& Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->GetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::GetRecordValue(LPCTSTR KeyValue, float& Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->GetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::GetRecordValue(LPCTSTR KeyValue, _bstr_t& Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->GetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::GetRecordValue(LPCTSTR KeyValue, _SYSTEMTIME& Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->GetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::SetRecordValue(LPCTSTR KeyValue, double Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->SetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::SetRecordValue(LPCTSTR KeyValue, long Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->SetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::SetRecordValue(LPCTSTR KeyValue, int Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->SetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::SetRecordValue(LPCTSTR KeyValue, float Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->SetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::SetRecordValue(LPCTSTR KeyValue, _bstr_t Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->SetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::SetRecordValue(LPCTSTR KeyValue, _SYSTEMTIME Value)
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->SetFieldValue(KeyValue, Value))
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::UpdateRecordset()
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->Update())
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}

int MsvDatabaseSave::AddRecordset()
{
	if (m_pRS != NULL)
	{
		if (!m_pRS->AddNew())
		{
			SystemLog.Mz_AddLog(_T("���ݿ����ʧ��"));
			return 3;
		}
	}
	return 0;
}


