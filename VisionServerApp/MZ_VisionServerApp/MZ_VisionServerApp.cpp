// MZ_VisionServerApp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "stdafx.h"
#include "DumpFile.h"
#include "MsvKernelManager.h"


TCHAR szServiceName[] = _T("Mz_VisionServerInspection");
BOOL bInstall = FALSE;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;

BOOL g_bServicRunning = FALSE;
HANDLE g_hExitEvent;

void InitService();
void StartWorkMoudle();
void StopWorkMoudle();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void WINAPI ServiceMain(DWORD   dwNumServicesArgs, LPTSTR* lpServiceArgVectors);
void WINAPI ServiceStrl(DWORD dwOpcode);



int _tmain(int argc, _TCHAR* argv[])
{
	//初始化网络通信
	WORD VersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(VersionRequested, &wsaData);	// 启动winsock服务
	if (wsaData.wVersion != VersionRequested)
	{
		SystemLog.AddLog(_T("Wrong version or WinSock not loaded"));
	}

	//
	MsvInitLib();

	//初始COM库
	::CoInitialize(NULL);

	SystemLog.AddLog(_T("程序启动"));

#ifdef _DEBUG
	StartWorkMoudle();
	StopWorkMoudle();
#else
	InitService();
#endif

	MsvCloseLib();
	::CoUninitialize();
	WSACleanup();//终止Winsock 2 DLL (Ws2_32.dll) 的使用-

	return 0;
}


//*********************************************************
//Functiopn:			InitService
//Description:			初始化服务
//			
//*********************************************************
void InitService()
{
	hServiceStatus = NULL;
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	status.dwWin32ExitCode = 0;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;

	SERVICE_TABLE_ENTRY st[] =
	{
		{ szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
	if (!::StartServiceCtrlDispatcher(st))
	{
		SystemLog.AddLog(_T("Register Service Main Function Error!"));
		return;
	}
	g_hExitEvent = CreateEvent(0, FALSE, FALSE, 0);

	if (!g_hExitEvent)
	{
		DWORD dwError = 0;
		dwError = GetLastError();
		char chMsg[1024] = { 0 };
		sprintf_s(chMsg, 1024, "CreateEvent Failed ErrorCode:%lu", dwError);
	}

}


//*********************************************************
//Functiopn:			ServiceMain
//Description:			服务主函数，这在里进行控制对服务控制的注册
//			
//*********************************************************
void WINAPI ServiceMain(DWORD   dwNumServicesArgs,
	LPTSTR* lpServiceArgVectors)
{
	// Register the control request handler
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	//注册服务控制
	hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
	if (hServiceStatus == NULL)
	{
		SystemLog.AddLog(_T("Handler not installed"));
		return;
	}
	SetServiceStatus(hServiceStatus, &status);

	status.dwWin32ExitCode = S_OK;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hServiceStatus, &status);

	//////////////////////////////////////////////////////////////////////////
	StartWorkMoudle();

}

//*********************************************************
//Functiopn:			StartWorkMoudle
//Description:			启动服务实际处理工作
//			
//*********************************************************
void StartWorkMoudle()
{
	//////////////////////////////////////////////////////////////////////////
	//工作代码
	DeclareDumpFile();

	MsvKernelManager::MsvInitResource();
	g_msvKernelMng = new MsvKernelManager;
	if (g_msvKernelMng == NULL)
	{
		SystemLog.AddLog(_T("MsvKernelManager init error"));
		return;
	}
	g_msvKernelMng->StartWork();

	g_bServicRunning = TRUE;
	SystemLog.AddLog(_T("成功启动服务"));
	std::cout << "成功启动服务" << std::endl;
	//////////////////////////////////////////////////////////////////////////

	int nCnt = 0;
	while (InterlockedExchangeAdd((long*)&g_bServicRunning, 0))
	{
#ifdef _DEBUG
		char code[1024] = { 0 };
		scanf_s("%s", code);
		if (strcmp(code, "stop") == 0)
		{
			break;
		}
#else
		Sleep(300);
#endif

	}

	SystemLog.AddLog(_T("退出循环"));
	SetEvent(g_hExitEvent);
	Sleep(200);
}

//*********************************************************
//Functiopn:			StartWorkMoudle
//Description:			停止服务实际处理工作
//			
//*********************************************************
void StopWorkMoudle()
{
	if (NULL != g_msvKernelMng)
	{
		g_msvKernelMng->StopWork();

		delete g_msvKernelMng;
		g_msvKernelMng = NULL;
	}
}


//*********************************************************
//Functiopn:			ServiceStrl
//Description:			服务控制主函数，这里实现对服务的控制，
//						当在服务管理器上停止或其它操作时，将会运行此处代码
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:				dwOpcode：控制服务的状态
//Output:
//Return:
//Others:
//History:
//			
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:

		InterlockedExchange((long*)&g_bServicRunning, FALSE);
		StopWorkMoudle();

		WaitForSingleObject(g_hExitEvent, INFINITE);

		SystemLog.AddLog(_T("退出服务"));

		status.dwWin32ExitCode = NO_ERROR;
		status.dwCurrentState = SERVICE_STOPPED;
		status.dwCheckPoint = 0;
		status.dwWaitHint = 0;
		break;

	case SERVICE_CONTROL_PAUSE:
		status.dwCurrentState = SERVICE_CONTROL_PAUSE;
		break;

	case SERVICE_CONTROL_CONTINUE:
		status.dwCurrentState = SERVICE_CONTROL_CONTINUE;
		break;

	case SERVICE_CONTROL_INTERROGATE:
		status.dwCurrentState = SERVICE_CONTROL_INTERROGATE;
		break;

	case SERVICE_CONTROL_SHUTDOWN:
		status.dwCurrentState = SERVICE_CONTROL_SHUTDOWN;
		break;

	default:
		status.dwCurrentState = SERVICE_NO_CHANGE;
		SystemLog.AddLog(_T("Bad service request"));

	}
	SetServiceStatus(hServiceStatus, &status);
}

//*********************************************************
//Functiopn:			IsInstalled
//Description:			判断服务是否已经被安装
//			
//*********************************************************
BOOL IsInstalled()
{
	BOOL bResult = FALSE;

	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM != NULL)
	{
		//打开服务
		SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			bResult = TRUE;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bResult;
}

//*********************************************************
//Functiopn:			Install
//Description:			安装服务函数
//			
//*********************************************************
BOOL Install()
{
	if (IsInstalled())
		return TRUE;

	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
		return FALSE;
	}

	// Get the executable file path
	TCHAR szFilePath[MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	//创建服务
	SC_HANDLE hService = ::CreateService(hSCM, szServiceName, szServiceName,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szFilePath, NULL, NULL, _T(""), NULL, NULL);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		MessageBox(NULL, _T("Couldn't create service"), szServiceName, MB_OK);
		return FALSE;
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);
	return TRUE;
}

//*********************************************************
//Functiopn:			Uninstall
//Description:			删除服务函数
//			
//*********************************************************
BOOL Uninstall()
{
	if (!IsInstalled())
		return TRUE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
	{
		MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		MessageBox(NULL, _T("Couldn't open service"), szServiceName, MB_OK);
		return FALSE;
	}
	SERVICE_STATUS status;
	::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	//删除服务
	BOOL bDelete = ::DeleteService(hService);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	if (bDelete)
		return TRUE;

	return FALSE;
}