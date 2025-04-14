#pragma once
#include "MsvAdo.h"
class MsvDatabaseSave
{
public:
	MsvDatabaseSave(void);
	~MsvDatabaseSave(void);
	int ConnectDB(wchar_t* connectString);
	int CloseDB();

	//��ѯ��¼
	int OpenRecordset(wchar_t* excuseSql);
	int CloseRecordset();
	int GetRecordValue(LPCTSTR KeyValue, double& Value);
	int GetRecordValue(LPCTSTR KeyValue, long& Value);
	int GetRecordValue(LPCTSTR KeyValue, int& Value);
	int GetRecordValue(LPCTSTR KeyValue, float& Value);
	int GetRecordValue(LPCTSTR KeyValue, _bstr_t& Value);
	int GetRecordValue(LPCTSTR KeyValue, _SYSTEMTIME& Value);

	//���¼�¼
	int SetRecordValue(LPCTSTR KeyValue, double Value);
	int SetRecordValue(LPCTSTR KeyValue, long Value);
	int SetRecordValue(LPCTSTR KeyValue, int Value);
	int SetRecordValue(LPCTSTR KeyValue, float Value);
	int SetRecordValue(LPCTSTR KeyValue, _bstr_t Value);
	int SetRecordValue(LPCTSTR KeyValue, _SYSTEMTIME Value);
	int UpdateRecordset();

	int AddRecordset();

protected:

	//���ݼ�����
	CADORecordset * m_pRS;
	//���ݿ����
	CADODatabase * m_pDB;
	HANDLE m_hThread;
};

