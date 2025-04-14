#include "pch.h"
#include "MsvClientComm.h"
#include <process.h>
#include <string>
#include <iostream>
#include <Windows.h>


MsvClientComm::MsvClientComm(void)
	: m_bThreadRun(FALSE)
{
}


MsvClientComm::~MsvClientComm(void)
{
	StopWork();
}


int MsvClientComm::StartWork()
{
	if (FALSE == InterlockedExchangeAdd((long *)&(m_bThreadRun),0))
	{
		InitSocks(CommPorts());
		InterlockedExchange((long *)&m_bThreadRun, TRUE);
	}
	return 0;
}

int MsvClientComm::StopWork()
{
	if (InterlockedExchangeAdd((long *)&(m_bThreadRun),0))
	{
		InterlockedExchange((long *)&m_bThreadRun, FALSE);
		HANDLE hThread[10] = {0};
		ReleaseSocks();
	}
	return 0;
}

int MsvClientComm::InitSocks(CommPorts input)
{
	SysParamPort = new CTcpServer();
	SysParamPort->InitServer(input.localhost_IP,input.SysParamPort);

	DataPackSendPort = new CTcpServer();
	DataPackSendPort->InitServer(input.localhost_IP, input.SysParamPort);

	CameraStaSendPort = new CTcpServer();
	CameraStaSendPort->InitServer(input.localhost_IP, input.SysParamPort);

	AlarmMsgPort = new CTcpServer();
	AlarmMsgPort->InitServer(input.localhost_IP, input.SysParamPort);
	return 0;
}

int MsvClientComm::ReleaseSocks()
{
	SysParamPort->CloseClient();
	DataPackSendPort->CloseClient();
	CameraStaSendPort->CloseClient();
	AlarmMsgPort->CloseClient();
	return 0;
}

int MsvClientComm::DispatchParamData(const char * buf, int nLen)
{
	static char s_AnlyBuff[4096] = {0};
	static int s_offset = 0;

	//解包...
	memcpy(s_AnlyBuff+s_offset, buf, nLen);
	s_offset += nLen;

	while (s_offset >= sizeof(SC_PackHead))
	{
		int indexStart = -1;
		for (int i = 0; i < s_offset-1; ++i)
		{
			if ( (unsigned char)s_AnlyBuff[i] == (unsigned char)HEAD_LABEL_SC_1 && (unsigned char)s_AnlyBuff[i+1] == (unsigned char)HEAD_LABEL_SC_2)
			{
				indexStart = i;break;	
			}
		}

		if (indexStart < 0)
		{
			s_offset = 0;
			indexStart = 0;
			memset(s_AnlyBuff, 0, sizeof(s_AnlyBuff));
			SystemLog.Mz_AddLog(_T("参数解析到没有数据包头的数据，已丢弃"));
			return -3;
		}
		else if (indexStart > 0)
		{
			s_offset -= indexStart;
			memcpy(s_AnlyBuff, s_AnlyBuff+indexStart, s_offset);
			indexStart = 0;
			SystemLog.Mz_AddLog(_T("indexStart > 0"));
		}

		//
		SC_PackHead * pHead = (SC_PackHead *)s_AnlyBuff;
		int data_len = (int)ntohl(pHead->data_len);
		data_len = data_len + sizeof(SC_PackHead);
		if (s_offset < data_len)
		{
			return -1;
		}

		//相机参数...
		if (pHead->operate_type == C_OPERATE_TYPE)
		{
			HandleCameraParam(s_AnlyBuff, data_len);
		}

		//信号接口平台参数...
		else if (pHead->operate_type == M_OPERATE_TYPE)
		{
			HandleSigMnrParam(s_AnlyBuff, data_len);
		}

		//客户端-服务端参数...
		else if (pHead->operate_type == SC_OPERATE_TYPE)
		{
			HandleSystemParam(s_AnlyBuff, data_len);
		}

		//移动剩下的包数据
		s_offset -= data_len;
		memcpy(s_AnlyBuff, s_AnlyBuff+data_len, s_offset);
	}
	
	return 0;
}

unsigned int __stdcall MsvClientComm::ParamThreadProc(void * pParam)
{
	MsvClientComm * pThis = (MsvClientComm *)pParam;
	BOOL bConnectFlag = FALSE;//初始状态下默认连接断开
	char recvBuf[4096] = {0};
	while (TRUE)
	{
		if (FALSE == InterlockedExchangeAdd((long *)&(pThis->m_bThreadRun),0))
		{
			TCHAR errmsg[1024] = {0};
			_stprintf_s(errmsg, 1024,  _T("参数通信送线程退出"));
			pThis->SystemLog.Mz_AddLog(errmsg);
			break;
		}	

		if (!bConnectFlag)
		{
			int ret = pThis->SysParamPort->Accept();
			if (ret > 0)
			{
				bConnectFlag = TRUE;
				pThis->SystemLog.Mz_AddLog(_T("参数通信线程程接收到客户端连接"));
			}
			else
			{
				Sleep(50);
				continue;
			}
		}

		if (pThis->SysParamPort->m_connfd != INVALID_SOCKET)
		{
			int rcvLen = 0;
			pThis->SysParamPort->Recv(recvBuf, 4096, rcvLen);
			if (rcvLen > 0)
			{
				pThis->DispatchParamData(recvBuf, rcvLen);
			}
			else if (rcvLen < 0)
			{
				pThis->SysParamPort->CloseClient();
				pThis->SysParamPort->m_connfd = INVALID_SOCKET;
				bConnectFlag = FALSE;
				pThis->SystemLog.Mz_AddLog(_T("参数通信线程客户端断开连接"));
			}
		}
		Sleep(1);
	}
	return 0;
}
