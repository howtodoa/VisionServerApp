#include "pch.h"
#include "MZ_ADOConn.h"
#include "MsvDatabaseSave.h"

namespace Mz_DB
{
MsvDatabaseSave* m_dabase = NULL;

int DBOperation::Mz_ConnectDB(wchar_t* connectstr)
{
	if (m_dabase != NULL)
	{
		delete m_dabase;
		m_dabase = NULL;
	}

	m_dabase = new MsvDatabaseSave();
	return m_dabase->ConnectDB(connectstr);
}

int DBOperation::Mz_CloseDB()
{
	if (m_dabase != NULL)
	{
		m_dabase->CloseDB();
	}
	return 0;
}

int DBOperation::Mz_OpenRecord(wchar_t* connectstr)
{
	if (m_dabase != NULL)
	{
		return m_dabase->OpenRecordset(connectstr);
	}
	return 0;
}

int DBOperation::Mz_CloseRecord()
{
	if (m_dabase != NULL)
	{
		return m_dabase->CloseRecordset();
	}
	return 0;
}

int DBOperation::Mz_GetRecordeValue(LPCTSTR strKey, int& Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->GetRecordValue(strKey, Value);
	}
	return 0;
}

int DBOperation::Mz_SetRecordeValue(LPCTSTR strKey, int Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->SetRecordValue(strKey, Value);
	}
	return 0;
}

int DBOperation::Mz_GetRecordeValue(LPCTSTR strKey, double& Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->GetRecordValue(strKey, Value);
	}
	return 0;

}
int DBOperation::Mz_SetRecordeValue(LPCTSTR strKey, double Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->SetRecordValue(strKey, Value);
	}
	return 0;
}

int DBOperation::Mz_GetRecordeValue(LPCTSTR strKey, float& Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->GetRecordValue(strKey, Value);
	}
	return 0;
}
int DBOperation::Mz_SetRecordeValue(LPCTSTR strKey, float Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->SetRecordValue(strKey, Value);
	}
	return 0;
}

int DBOperation::Mz_GetRecordeValue(LPCTSTR strKey, long& Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->GetRecordValue(strKey, Value);
	}
	return 0;
}
int DBOperation::Mz_SetRecordeValue(LPCTSTR strKey, long Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->SetRecordValue(strKey, Value);
	}
	return 0;
}

int DBOperation::Mz_GetRecordeValue(LPCTSTR strKey, _bstr_t& Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->GetRecordValue(strKey, Value);
	}
	return 0;
}
int DBOperation::Mz_SetRecordeValue(LPCTSTR strKey, _bstr_t Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->SetRecordValue(strKey, Value);
	}
	return 0;
}

int DBOperation::Mz_GetRecordeValue(LPCTSTR strKey, _SYSTEMTIME& Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->GetRecordValue(strKey, Value);
	}
	return 0;
}
int DBOperation::Mz_SetRecordeValue(LPCTSTR strKey, _SYSTEMTIME Value)
{
	if (m_dabase != NULL)
	{
		return m_dabase->SetRecordValue(strKey, Value);
	}
	return 0;
}

int DBOperation::Mz_UpdataRecorde()
{
	if (m_dabase != NULL)
	{
		return m_dabase->UpdateRecordset();
	}
	return 0;
}

int DBOperation::Mz_AddNewRecorde()
{
	if (m_dabase != NULL)
	{
		return m_dabase->AddRecordset();
	}
	return 0;
}
}
