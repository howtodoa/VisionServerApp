#pragma once
#include <tchar.h>
namespace Mz_Log
{
	class __declspec(dllexport) COperation
	{
		//��ʼ����
	public:
		int	Mz_Realse();
		int	Mz_GetInstance(const wchar_t* filename);
		int	Mz_SetLogFile(const wchar_t* pFileName);	//��ʼ����־�ļ�
		int	Mz_AddLog(const wchar_t* _logStr);	//����־�ļ��������־���ݣ��ַ����飩
		int	Mz_CheckDateTime();				//��־һ��һ��  ��һ������дһ���ļ�
	};
}