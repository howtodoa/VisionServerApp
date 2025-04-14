#include "pch.h"
#include "MsvAdo.h"



/////////////////////////////////////////////////////////////////////////////
// CADODatabase class

BOOL CADODatabase::Open(LPCTSTR lpstrConnection)
{
	if (m_pConnection==NULL)
	{
		return FALSE;
	}

	//超时设置
	m_pConnection->ConnectionTimeout =30;   //连接超时时间 
	m_pConnection->CommandTimeout =30;   //查询等命令超时时间 

	HRESULT hr = S_OK;

	if(IsOpen())
		Close();

	if(_tcscmp(lpstrConnection, _T("")) != 0)
		m_strConnection = (_variant_t)lpstrConnection;

	try
	{
		hr = m_pConnection->Open(_bstr_t(m_strConnection), "", "", NULL);
		return hr == S_OK;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
	}
	return FALSE;
}
BOOL CADODatabase::Execute(LPCTSTR lpstrExec)
{
	if(m_pConnection == NULL)
		return -1;

	try
	{
		m_pConnection->Execute(_bstr_t(lpstrExec), NULL, adExecuteNoRecords);
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
	}
	return TRUE;	
}

void CADODatabase::dump_com_error(_com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	_bstr_t bstrConn = m_strConnection.bstrVal;

	TCHAR ErrorStr[1024] = {0};
	_stprintf_s( ErrorStr, 1024, _T("Connection String = %s\n CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n"),
		(LPCTSTR)bstrConn, e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription );

	SystemLog.Mz_AddLog(ErrorStr);

	m_strLastError = ErrorStr;
}

BOOL CADODatabase::IsOpen()
{	
	try
	{
		return (m_pConnection != NULL && (m_pConnection->State & adStateOpen));
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
return FALSE;
}

void CADODatabase::Close()
{
	try
	{	
		if (IsOpen()) 
			m_pConnection->Close();		
	}
	catch (_com_error e)
	{
		dump_com_error(e);
	} 
}

long CADODatabase::BeginTransaction()
{
	if(m_pConnection == NULL)
		return -1;

	try
	{
		return m_pConnection->BeginTrans();
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return -1;
	} 
	return -1;
}

BOOL CADODatabase::CommitTransaction()
{
	if(m_pConnection == NULL)
		return -1;

	try
	{
		return SUCCEEDED(m_pConnection->CommitTrans());
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
	return FALSE;
}

BOOL CADODatabase::RollbackTransaction()
{
	if(m_pConnection == NULL)
		return -1;

	try
	{
		return SUCCEEDED(m_pConnection->RollbackTrans());
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CADORecordset class

//构造函数
CADORecordset::CADORecordset(CADODatabase* pAdoDatabase)
{
	m_pRecordset = NULL;
	m_strQuery = _T("");
	m_pRecordset.CreateInstance(__uuidof(Recordset));
	m_nSearchDirection = CADORecordset::searchForward;

	m_pConnection = pAdoDatabase->GetActiveConnection();
}

//打开记录集
BOOL CADORecordset::Open(_ConnectionPtr mpdb, LPCTSTR lpstrExec, int nOption)
{	
	if (IsOpen())
		Close();

	if(_tcscmp(lpstrExec, _T("")) != 0)
		m_strQuery = lpstrExec;

	try
	{
		m_pRecordset->CursorLocation = adUseClient;//当前位置
		if(nOption == openQuery)//SQL语句
		{
			m_pRecordset->Open((_variant_t)m_strQuery, _variant_t((IDispatch*)mpdb, TRUE), 
				adOpenStatic, adLockOptimistic, adCmdText);
		}
		else if(nOption == openTable)
			m_pRecordset->Open((_variant_t)m_strQuery, _variant_t((IDispatch*)mpdb, TRUE),
			adOpenDynamic, adLockOptimistic, adCmdTable);
		else if(nOption == openStoredProc)
		{
			m_pRecordset->Open((_variant_t)m_strQuery, _variant_t((IDispatch*)mpdb, TRUE),
				adOpenStatic, adLockOptimistic, adCmdStoredProc);
		}
		else
		{
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
	return m_pRecordset != NULL;
}

BOOL CADORecordset::Open(LPCTSTR lpstrExec, int nOption)
{
	if(m_pConnection == NULL)
		return FALSE;

	return Open(m_pConnection, lpstrExec, nOption);
}

BOOL CADORecordset::GetFieldValue(LPCTSTR lpFieldName, double& dbValue)
{	
	double val = (double)NULL;
	_variant_t vtFld;
	try
	{	
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL)
			val = vtFld.dblVal;
		dbValue = val;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		dbValue=0;
		return FALSE;
	}
}

BOOL CADORecordset::GetFieldValue(int nIndex, double& lValue)
{	
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL)
			lValue = vtFld.dblVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		lValue=0;
		return FALSE;
	}	

}

BOOL CADORecordset::GetFieldValue(LPCTSTR lpFieldName, long& lValue)
{
	_variant_t vtFld;
	try
	{	
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL)
			lValue = vtFld.lVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		lValue=0;
		return FALSE;
	}
}

BOOL CADORecordset::GetFieldValue(int nIndex, long& lValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL)
			lValue = vtFld.lVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		lValue=0;
		return FALSE;
	}	
}

BOOL CADORecordset::GetFieldValue(LPCTSTR lpFieldName, int& nValue)
{
	_variant_t vtFld;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL)
			nValue = vtFld.lVal;
		return TRUE;		
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		nValue=0;
		return FALSE;
	}
}

BOOL CADORecordset::GetFieldValue(int nIndex, int& nValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL)
			nValue = vtFld.lVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		nValue=0;
		return FALSE;
	}

}

BOOL CADORecordset::GetFieldValue(LPCTSTR lpFieldName, float& nValue)
{
	_variant_t vtFld;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL)
			nValue = vtFld.fltVal;
		return TRUE;		
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		nValue=0;
		return FALSE;
	}
}

BOOL CADORecordset::GetFieldValue(int nIndex, float& nValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL)
			nValue = vtFld.fltVal;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		nValue=0;
		return FALSE;
	}
}

BOOL CADORecordset::GetFieldValue(LPCTSTR lpFieldName, _bstr_t& strValue)
{
	_variant_t vtFld;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt != VT_NULL)
			strValue = _bstr_t(vtFld);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		strValue= _T("");
		return FALSE;
	}
}

BOOL CADORecordset::GetFieldValue(int nIndex, _bstr_t & strValue)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
		if(vtFld.vt != VT_NULL)
			strValue = _bstr_t(vtFld);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		strValue= _T("");
		return FALSE;
	}	
}

BOOL CADORecordset::GetFieldValue(LPCTSTR lpFieldName, _SYSTEMTIME  & time)
{
	_variant_t vtFld;

	try
	{
		vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
		if(vtFld.vt == VT_DATE)
			VariantTimeToSystemTime(vtFld.date, &time);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}	
}

BOOL CADORecordset::GetFieldValue(int nIndex, _SYSTEMTIME  & time)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		vtFld = m_pRecordset->Fields->GetItem(nIndex)->Value;
		if(vtFld.vt == VT_DATE)
			VariantTimeToSystemTime(vtFld.date, &time);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}	
}

BOOL CADORecordset::IsFieldNull(LPCTSTR lpFieldName)
{
	_variant_t vtFld;

	vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
	return vtFld.vt == VT_NULL;
}

BOOL CADORecordset::IsFieldNull(int nIndex)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
	return vtFld.vt == VT_NULL;
}

BOOL CADORecordset::IsFieldEmpty(LPCTSTR lpFieldName)
{
	_variant_t vtFld;

	vtFld = m_pRecordset->Fields->GetItem(lpFieldName)->Value;
	return vtFld.vt == VT_EMPTY || vtFld.vt == VT_NULL;
}

BOOL CADORecordset::IsFieldEmpty(int nIndex)
{
	_variant_t vtFld;
	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	vtFld = m_pRecordset->Fields->GetItem(vtIndex)->Value;
	return vtFld.vt == VT_EMPTY || vtFld.vt == VT_NULL;
}

/////////////////////////////////////////////////////////////////////////////
//获得记录数目
DWORD CADORecordset::GetRecordCount()
{
	DWORD nRows = 0;
	try{	
		nRows = m_pRecordset->GetRecordCount();
		//如果提供者不支持计算记录个数,则通过遍历计算
		if(nRows == -1)
		{
			nRows = 0;
			if(m_pRecordset->adoEOF != VARIANT_TRUE)
				m_pRecordset->MoveFirst();
			while(m_pRecordset->adoEOF != VARIANT_TRUE)
			{
				nRows++;
				m_pRecordset->MoveNext();
			}
			if(nRows > 0)
				m_pRecordset->MoveFirst();
		}
	}
	catch (_com_error& e)
	{
		dump_com_error(e);
		return -1;
	}	
	return nRows;
}

BOOL CADORecordset::IsOpen()
{
	try
	{
		if(m_pRecordset)
		{
			return m_pRecordset->GetState() != adStateClosed;
		}	
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
	return FALSE;
}

void CADORecordset::Close()
{	
	try
	{
		if (m_pRecordset != NULL && m_pRecordset->State != adStateClosed)
			m_pRecordset->Close();
	}
	catch (_com_error& e)
	{dump_com_error(e);}	
}

void CADORecordset::dump_com_error(_com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	_bstr_t bstrQuery = m_strQuery.bstrVal;

	TCHAR ErrorStr[1024] = {0};
	_stprintf_s( ErrorStr, 1024, _T("Query = %s\n CADORecordset Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n"),
		(LPCTSTR)bstrQuery, e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription );

	SystemLog.Mz_AddLog(ErrorStr);

	m_strLastError = ErrorStr;
}

long CADORecordset::GetAbsolutePosition()
{
	if (m_pRecordset == NULL)
		return -1;	
	try
	{
		return m_pRecordset->GetAbsolutePosition();
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return -1;
	}
}

/////////////////////////////////////////////////////////////////////////////
//添加记录、更新记录
BOOL CADORecordset::AddNew()
{
	try
	{
		if(m_pRecordset->AddNew() != S_OK)
			return FALSE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
	return TRUE;
}

BOOL CADORecordset::Update()
{
	try
	{
		if(m_pRecordset->Update() != S_OK)
			return FALSE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
	return TRUE;
}

BOOL CADORecordset::SetFieldValue(int nIndex, LPCTSTR strValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_BSTR;
	vtFld.bstrVal = _bstr_t(strValue);

	_variant_t vtIndex;	
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		m_pRecordset->Fields->GetItem(vtIndex)->Value = _bstr_t(vtFld);//_bstr_t(strValue);
		return TRUE;
	}	
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}	
}

BOOL CADORecordset::SetFieldValue(LPCTSTR lpFieldName, LPCTSTR strValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_BSTR;
	vtFld.bstrVal = _bstr_t(strValue);
	try
	{
		m_pRecordset->Fields->GetItem(lpFieldName)->Value = _bstr_t(vtFld);
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}		
}

BOOL CADORecordset::SetFieldValue(int nIndex, int nValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.intVal = nValue;

	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		m_pRecordset->Fields->GetItem(vtIndex)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}		
}

BOOL CADORecordset::SetFieldValue(LPCTSTR lpFieldName, int nValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.intVal = nValue;
	try
	{
		m_pRecordset->Fields->GetItem(lpFieldName)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}		
}

BOOL CADORecordset::SetFieldValue(int nIndex, long lValue)
{	
	long val = (long)NULL;
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal = lValue;

	_variant_t vtIndex;
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		m_pRecordset->Fields->GetItem(vtIndex)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		lValue=0;
		return FALSE;
	}	
}

BOOL CADORecordset::SetFieldValue(LPCTSTR lpFieldName, long lValue)
{
	long val = (long)NULL;
	_variant_t vtFld;
	vtFld.vt = VT_I4;
	vtFld.lVal = lValue;
	try
	{	
		m_pRecordset->Fields->GetItem(lpFieldName)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		lValue=0;
		return FALSE;
	}	
}

BOOL CADORecordset::SetFieldValue(int nIndex, double dblValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_R8;
	vtFld.dblVal = dblValue;

	_variant_t vtIndex;

	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;
	try
	{
		m_pRecordset->Fields->GetItem(vtIndex)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}		
}

BOOL CADORecordset::SetFieldValue(LPCTSTR lpFieldName, double dblValue)
{
	_variant_t vtFld;
	vtFld.vt = VT_R8;
	vtFld.dblVal = dblValue;
	try
	{
		m_pRecordset->Fields->GetItem(lpFieldName)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}	
}

BOOL CADORecordset::SetFieldValue(int nIndex, SYSTEMTIME  time)
{
	variant_t vtIndex;
	vtIndex.vt = VT_I2;
	vtIndex.iVal = nIndex;

	_variant_t vtFld;
	vtFld.vt = VT_DATE;
	if ( SystemTimeToVariantTime(&time, &vtFld.date) == 0)
		return FALSE;

	try
	{
		m_pRecordset->Fields->GetItem(vtIndex)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}		
}

BOOL CADORecordset::SetFieldValue(LPCTSTR lpFieldName, SYSTEMTIME  time)
{
	_variant_t vtFld;
	vtFld.vt = VT_DATE;
	if ( SystemTimeToVariantTime(&time, &vtFld.date) == 0)
		return FALSE;

	try
	{
		m_pRecordset->Fields->GetItem(lpFieldName)->Value = vtFld;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}		
}

BOOL CADORecordset::SetBookmark()
{
	if(m_varBookmark.vt != VT_EMPTY)
	{
		m_pRecordset->Bookmark = m_varBookmark;
		return TRUE;
	}
	return FALSE;
}

void CADORecordset::GetBookmark()
{
	try
	{
		if (IsOpen())		
			m_varBookmark = m_pRecordset->GetBookmark();			
	}
	catch (_com_error e)
	{dump_com_error(e);}
}

BOOL CADORecordset::Delete()
{
	if(m_pRecordset->Delete(adAffectCurrent) != S_OK)
		return FALSE;

	if(m_pRecordset->Update() != S_OK)
		return FALSE;

	return TRUE;
}

BOOL CADORecordset::Find(LPCTSTR lpFind, int nSearchDirection)
{
	m_strFind = lpFind;
	m_nSearchDirection = nSearchDirection;
	try
	{
		if(m_nSearchDirection == searchForward)
		{
			m_pRecordset->Find(_bstr_t(m_strFind), 0, adSearchForward, "");
			if(!IsEOF())
			{
				m_varBookFind = m_pRecordset->Bookmark;
				return TRUE;
			}
		}
		else if(m_nSearchDirection == searchBackward)
		{	
			m_pRecordset->Find(_bstr_t(m_strFind), 0, adSearchBackward, "");
			if(!IsBOF())
			{
				m_varBookFind = m_pRecordset->Bookmark;
				return TRUE;
			}				
		}
		else
		{
			m_nSearchDirection = searchForward;
		}
		return FALSE;
	}
	catch(_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 	
}

BOOL CADORecordset::IsBOF()
{
	if (m_pRecordset == NULL)
		return FALSE;
	try
	{
		return m_pRecordset->adoBOF;
	}
	catch(_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
	return FALSE;
}

BOOL CADORecordset::IsEOF()
{
	if (m_pRecordset == NULL)
		return FALSE;
	try
	{
		return ((m_pRecordset->adoEOF)==VARIANT_TRUE);
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	}
}

BOOL CADORecordset::MoveFirst()
{
	if (m_pRecordset == NULL)
		return FALSE;
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveFirst());
		}
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
	return	FALSE;
}

BOOL CADORecordset::MoveNext()
{
	if (m_pRecordset == NULL)
		return FALSE;
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveNext());
		}
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	}
	return FALSE;
}

BOOL CADORecordset::MovePrevious()
{
	if (m_pRecordset == NULL)
		return FALSE;
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MovePrevious());
		}
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	}
	return FALSE;	
}

BOOL CADORecordset::MoveLast()
{
	if (m_pRecordset == NULL)
		return FALSE;
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveLast());
		}
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
	return	FALSE;
}

BOOL CADORecordset::Requery(long Options)
{
	if (m_pRecordset == NULL)
		return FALSE;
	try
	{
		if (m_pRecordset != NULL) 
		{
			return (m_pRecordset->Requery(Options) == S_OK);
		}
	}
	catch (_com_error e)
	{
		dump_com_error(e);
		return FALSE;
	} 
	return	FALSE; 
}

_RecordsetPtr CADORecordset::GetRecordsetPtr()
{
	return m_pRecordset;

}

