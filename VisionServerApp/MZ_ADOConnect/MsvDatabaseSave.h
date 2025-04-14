#pragma once
#include "MsvAdo.h"
class MsvDatabaseSave
{
public:
	MsvDatabaseSave(void);
	~MsvDatabaseSave(void);
	int ConnectDB(wchar_t* connectString);
	int CloseDB();

	//查询记录
	int OpenRecordset(wchar_t* excuseSql);
	int CloseRecordset();
	int GetRecordValue(LPCTSTR KeyValue, double& Value);
	int GetRecordValue(LPCTSTR KeyValue, long& Value);
	int GetRecordValue(LPCTSTR KeyValue, int& Value);
	int GetRecordValue(LPCTSTR KeyValue, float& Value);
	int GetRecordValue(LPCTSTR KeyValue, _bstr_t& Value);
	int GetRecordValue(LPCTSTR KeyValue, _SYSTEMTIME& Value);

	//更新记录
	int SetRecordValue(LPCTSTR KeyValue, double Value);
	int SetRecordValue(LPCTSTR KeyValue, long Value);
	int SetRecordValue(LPCTSTR KeyValue, int Value);
	int SetRecordValue(LPCTSTR KeyValue, float Value);
	int SetRecordValue(LPCTSTR KeyValue, _bstr_t Value);
	int SetRecordValue(LPCTSTR KeyValue, _SYSTEMTIME Value);
	int UpdateRecordset();

	int AddRecordset();

protected:

	//数据集对象
	CADORecordset * m_pRS;
	//数据库对象
	CADODatabase * m_pDB;
	HANDLE m_hThread;
};

