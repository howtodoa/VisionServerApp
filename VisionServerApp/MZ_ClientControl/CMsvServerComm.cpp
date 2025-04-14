#include "pch.h"
#include "CMsvServerComm.h"
#include <process.h>
#include <thread>

CMsvServerComm::CMsvServerComm(vector<CommPorts> inputs)
{
	m_vecCommPorts = inputs;
}

CMsvServerComm::~CMsvServerComm(void)
{
}

int CMsvServerComm::StartWork()
{
	if (FALSE == InterlockedExchangeAdd((long*)&(m_bThreadRun), 0))
	{
		InterlockedExchange((long*)&m_bThreadRun, TRUE);
		ThreadParam m_paramThread;
		for (int i = 0; i < m_vecCommPorts.size(); ++i)
		{
			CRITICAL_SECTION templock;
			m_lockfuncs.push_back(templock);
			m_vecfuncs.push_back(m_vecCommPorts[i].m_Callbackfuncs);
			CRITICAL_SECTION templock2;
			m_lockPacetDataRecv.push_back(templock2);
			m_vecPacetDataRecv.push_back(vector<R_CPacketData>());
			CRITICAL_SECTION templock3;
			m_lockPacetDataSend.push_back(templock3);
			m_vecPacetDataSend.push_back(vector<R_CPacketData>());

			m_paramThread.ThreadName = m_vecCommPorts[i].PortName;
			m_paramThread.Socketsvr = InitSocks(m_vecCommPorts[i]);
			m_paramThread.nCameraID = i + 1;
			m_paramThread.pParam = this;
			m_paramThread.CommPort = m_vecCommPorts[i];
			m_paramThread.hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, ParamThreadRecvProc, &m_paramThread, 0, NULL);
			m_paramThread.hThreadSend = (HANDLE)_beginthreadex(NULL, 0, ParamThreadSendProc, &m_paramThread, 0, NULL);
			m_vecThreadParams.push_back(m_paramThread);
		}
	}
	return 0;
}

int CMsvServerComm::StopWork()
{
	if (InterlockedExchangeAdd((long*)&(m_bThreadRun), 0))
	{
		InterlockedExchange((long*)&m_bThreadRun, FALSE);
		ReleaseSocks();//释放套接字资源
		for (int i = 0; i < m_vecThreadParams.size(); ++i)
		{
			//关闭接收线程
			WaitForSingleObject(m_vecThreadParams[i].hThreadRecv, INFINITE);
			CloseHandle(m_vecThreadParams[i].hThreadRecv);
			m_vecThreadParams[i].hThreadRecv = NULL;
			//关闭发送线程
			WaitForSingleObject(m_vecThreadParams[i].hThreadSend, INFINITE);
			CloseHandle(m_vecThreadParams[i].hThreadSend);
			m_vecThreadParams[i].hThreadSend = NULL;
		}
		m_vecThreadParams.clear();
	}
	return 0;
}

int CMsvServerComm::DoActionFun(string portName, string funcName, HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg)
{
	int nIndex = -1;
	FindIndexByPortName(portName, nIndex);
	if (nIndex != -1)
	{
		PushPacketDataSend(portName,R_CPacketData(funcName, inputPrams,inputImages));
		R_CPacketData aMsg;
		if (GetPacketDataRecv(portName, aMsg))
		{
///TODO: 处理返回数据...
			string functionnamerev;
			AnasysPacket(aMsg, functionnamerev, outputPrams,outputImages, errcode, errmsg);
			return 0;
		}
		else
		{
			errcode = -1;
			errmsg = "No response data from server.";
			return -1;
		}
	}
	return -1;
}

Callbackfunc CMsvServerComm::FindActionFun(string portName, string funcName)
{
	int nIndex = -1;
	FindIndexByPortName(portName, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockfuncs[nIndex]);
		for (int i = 0; i < m_vecfuncs[nIndex].size(); ++i)
		{
			if (m_vecfuncs[nIndex][i].funcname == funcName)
			{
				LeaveCriticalSection(&m_lockfuncs[nIndex]);
				return m_vecfuncs[nIndex][i];
			}
		}
		LeaveCriticalSection(&m_lockfuncs[nIndex]);
	}
	return Callbackfunc();
}
int CMsvServerComm::AskActionFunList(string portName, vector<Callbackfunc>& funclist)
{
	int nIndex = -1;
	FindIndexByPortName(portName, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockfuncs[nIndex]);
		for (int i = 0; i < m_vecfuncs[nIndex].size(); ++i)
		{
			funclist.push_back(m_vecfuncs[nIndex][i]);
		}
		LeaveCriticalSection(&m_lockfuncs[nIndex]);
	}
	return 0;
}

int CMsvServerComm::RegisterActionFun(string portName, string funcName, Callbackfunc func)
{
	int nIndex = -1;
	FindIndexByPortName(portName, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockfuncs[nIndex]);
		m_vecfuncs[nIndex].push_back(func);
		LeaveCriticalSection(&m_lockfuncs[nIndex]);
	}
	return 0;
}

//初始化套接字资源
void* CMsvServerComm::InitSocks(CommPorts input)
{
	if (input.isActAsServer == 1)
	{
		CTcpServer* tempSer = new CTcpServer();
		tempSer->InitServer(input.localhost_IP.c_str(), input.nLocalPort);
		return (void*)tempSer;
	}
	else
	{
		CTcpClient* tempCli = new CTcpClient();
		tempCli->ConnectToServer(input.remote_IP.c_str(), input.nRemotePort);
		return (void*)tempCli;
	}
	return NULL;
}

//释放套接字资源
int CMsvServerComm::ReleaseSocks()
{
	for (int i = 0; i < m_vecThreadParams.size(); ++i)
	{
		if(m_vecThreadParams[i].isActasServer == 1)
		{
			if (m_vecThreadParams[i].Socketsvr != NULL)
			{
				CTcpServer* tempSer = (CTcpServer*)m_vecThreadParams[i].Socketsvr;
				tempSer->CloseClient();
				tempSer->CloseListen();
				delete tempSer;
				tempSer = NULL;
			}
		}
		else
		{
			if (m_vecThreadParams[i].Socketsvr != NULL)
			{
				CTcpClient* tempSer = (CTcpClient*)m_vecThreadParams[i].Socketsvr;
				tempSer->Close();
				delete tempSer;
				tempSer = NULL;
			}
		}
	}
	return 0;
}

int CMsvServerComm::PushPacketDataSend(string comportname, const R_CPacketData& aPacket)
{
	int nRet = 0;
	int nIndex = -1;
	FindIndexByPortName(comportname, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockPacetDataSend[nIndex]);
		if ((int)m_vecPacetDataSend[nIndex].size() >= MAX_PACKETDATA_NUM)
		{
			m_vecPacetDataSend[nIndex].erase(m_vecPacetDataSend[nIndex].begin());
		}
		m_vecPacetDataSend[nIndex].push_back(aPacket);
		nRet = m_vecPacetDataSend[nIndex].size();
		LeaveCriticalSection(&m_lockPacetDataSend[nIndex]);
	}
	return nRet;
}

BOOL CMsvServerComm::GetPacketDataSend(string comportname, R_CPacketData& aPacket)
{
	BOOL bRet = FALSE;
	int nIndex = -1;
	FindIndexByPortName(comportname, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockPacetDataSend[nIndex]);
		if (m_vecPacetDataSend[nIndex].size() > 0)
		{
			aPacket = m_vecPacetDataSend[nIndex][0];
			m_vecPacetDataSend[nIndex].erase(m_vecPacetDataSend[nIndex].begin());
			bRet = TRUE;
		}
		LeaveCriticalSection(&m_lockPacetDataSend[nIndex]);
	}
	return bRet;
}

int CMsvServerComm::PushPacketDataRecv(string comportname, const R_CPacketData& aPacket)
{
	int nRet = 0;
	int nIndex = -1;
	FindIndexByPortName(comportname, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockPacetDataRecv[nIndex]);
		if ((int)m_vecPacetDataRecv[nIndex].size() >= MAX_PACKETDATA_NUM)
		{
			m_vecPacetDataRecv[nIndex].erase(m_vecPacetDataRecv[nIndex].begin());
		}
		m_vecPacetDataRecv[nIndex].push_back(aPacket);
		nRet = m_vecPacetDataRecv[nIndex].size();
		LeaveCriticalSection(&m_lockPacetDataRecv[nIndex]);
	}
	return nRet;
}

BOOL CMsvServerComm::GetPacketDataRecv(string comportname, R_CPacketData& aPacket)
{
	BOOL bRet = FALSE;
	int nIndex = -1;
	FindIndexByPortName(comportname, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockPacetDataRecv[nIndex]);
		if (m_vecPacetDataRecv[nIndex].size() > 0)
		{
			aPacket = m_vecPacetDataRecv[nIndex][0];
			m_vecPacetDataRecv[nIndex].erase(m_vecPacetDataRecv[nIndex].begin());
			bRet = TRUE;
		}
		LeaveCriticalSection(&m_lockPacetDataRecv[nIndex]);
	}
	return bRet;
}




int CMsvServerComm::DispatchParamData(string comportname, const char* buf, int nLen)
{
	static char s_AnlyBuff[4096] = { 0 };
	static int s_offset = 0;

	//解包...
	memcpy(s_AnlyBuff + s_offset, buf, nLen);
	s_offset += nLen;

	while (s_offset >= sizeof(SC_PackHead))
	{
		int indexStart = -1;
		for (int i = 0; i < s_offset - 1; ++i)
		{
			if ((unsigned char)s_AnlyBuff[i] == (unsigned char)HEAD_LABEL_SC_1 && (unsigned char)s_AnlyBuff[i + 1] == (unsigned char)HEAD_LABEL_SC_2)
			{
				indexStart = i; break;
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
			memcpy(s_AnlyBuff, s_AnlyBuff + indexStart, s_offset);
			indexStart = 0;
			SystemLog.Mz_AddLog(_T("indexStart > 0"));
		}

		//
		SC_PackHead* pHead = (SC_PackHead*)s_AnlyBuff;
		int data_len = (int)ntohl(pHead->data_len);
		data_len = data_len + sizeof(SC_PackHead);
		if (s_offset < data_len)
		{
			return -1;
		}

		//解析参数
		PushPacketDataRecv(comportname, R_CPacketData((char*)pHead->label_1,pHead->data_len));

		//移动剩下的包数据
		s_offset -= data_len;
		memcpy(s_AnlyBuff, s_AnlyBuff + data_len, s_offset);
	}

	return 0;
}

void CMsvServerComm::AnasysPacket(R_CPacketData inputpacket,string& outputfuncname, HValues	&outputParams, HImages& outputImages, int& errcode, string& errmsg)
{
//解析数据包
	SC_PackHead *tempHead = (SC_PackHead*)(inputpacket.buffer);
	outputfuncname = tempHead->user_define.func_name;
	int inputparams = tempHead->inputParamsnums;
	int inputimages = tempHead->inputImagesnums;
	for (int i = 0; i < inputparams; ++i)
	{
		HValue tempValue;
		tempValue = *(HValue*)(inputpacket.buffer + sizeof(SC_PackHead) + i * sizeof(HValue));
		outputParams.m_Values.push_back(tempValue);
	}
	for (int i = 0; i < inputimages; ++i)
	{
		HImage tempImage;
		tempImage = *(HImage*)(inputpacket.buffer + sizeof(SC_PackHead) + inputparams * sizeof(HValue) + i * tempImage.getdatelength());
		outputImages.m_Images.push_back(tempImage);
	}
}
void CMsvServerComm::FindIndexByPortName(string portname, int& index)
{
	for (int i = 0; i < m_vecCommPorts.size(); ++i)
	{
		if (m_vecCommPorts[i].PortName == portname)
		{
			index = i;
			return;
		}
	}
}
void CMsvServerComm::Pushfuncs(string potrname, Callbackfunc func)
{
	int m_nIndex = -1;
	FindIndexByPortName(potrname, m_nIndex);
	if(m_nIndex != -1)
	{
		EnterCriticalSection(&m_lockfuncs[m_nIndex]);
		m_vecfuncs[m_nIndex].push_back(func);
		LeaveCriticalSection(&m_lockfuncs[m_nIndex]);
	}
}

bool CMsvServerComm::Getfunc(string potrname, string funcname, Callbackfunc& func)
{
	int m_nIndex = -1;
	FindIndexByPortName(potrname, m_nIndex);
	if (m_nIndex != -1)
	{
		EnterCriticalSection(&m_lockfuncs[m_nIndex]);
		for (int i = 0; i < m_vecfuncs[m_nIndex].size(); ++i)
		{
			if (m_vecfuncs[m_nIndex][i].funcname == funcname)
			{
				func = m_vecfuncs[m_nIndex][i];
				LeaveCriticalSection(&m_lockfuncs[m_nIndex]);
				return true;
			}
		}
		LeaveCriticalSection(&m_lockfuncs[m_nIndex]);
		return false;
	}
	return false;
}

unsigned int __stdcall ParamThreadRecvProc(void* pParam)
{
	ThreadParam* pThis = (ThreadParam*)pParam;
	CMsvServerComm *m_pServerComm = (CMsvServerComm*)pThis->pParam;
	BOOL bConnectFlag = FALSE;//初始状态下默认连接断开
	char recvBuf[4096] = { 0 };
	while (TRUE)
	{
		if (FALSE == InterlockedExchangeAdd((long*)&(m_pServerComm->m_bThreadRun), 0))
		{
			TCHAR errmsg[1024] = { 0 };
			_stprintf_s(errmsg, 1024, _T("%s线程退出"), pThis->ThreadName.c_str());
			m_pServerComm->SystemLog.Mz_AddLog(errmsg);
			break;
		}

		if(pThis->isActasServer == 1)//服务端
		{
			CTcpServer *pServer = (CTcpServer*)pThis->Socketsvr;
			if (!bConnectFlag)
			{
				int ret = pServer->Accept();
				if (ret > 0)
				{
					bConnectFlag = TRUE;
					TCHAR errmsg[1024] = { 0 };
					_stprintf_s(errmsg, 1024, _T("%s线程接收到客户端连接"), pThis->ThreadName.c_str());
					m_pServerComm->SystemLog.Mz_AddLog(errmsg);
				}
				else
				{
					Sleep(50);
					continue;
				}
			}

			if (pServer->m_connfd != INVALID_SOCKET)
			{
				int rcvLen = 0;
				pServer->Recv(recvBuf, 4096, rcvLen);
				if (rcvLen > 0)
				{
					char* Revbuf = NULL;
					int Revlen = 0;
					m_pServerComm->DispatchParamData(pThis->CommPort.PortName,recvBuf, rcvLen);
					if (Revbuf != NULL && Revlen > 0)
					{
						pServer->Send(Revbuf);
						delete[] Revbuf;
						Revbuf = NULL;
						Revlen = 0;
					}
				}
				else if (rcvLen < 0)
				{
					pServer->CloseClient();
					bConnectFlag = FALSE;
					TCHAR errmsg[1024] = { 0 };
					_stprintf_s(errmsg, 1024, _T("%s线程客户端断开连接"), pThis->ThreadName.c_str());
					m_pServerComm->SystemLog.Mz_AddLog(errmsg);
				}
			}
		}
		else
		{
			CTcpClient *pClient = (CTcpClient*)pThis->Socketsvr;
			if (!bConnectFlag)
			{
				int ret = pClient->ConnectToServer(pThis->CommPort.remote_IP.c_str(), pThis->CommPort.nRemotePort);
				if (ret == 0)
				{
					bConnectFlag = TRUE;
					TCHAR errmsg[1024] = { 0 };
					_stprintf_s(errmsg, 1024, _T("%s线程连接到服务器"), pThis->ThreadName.c_str());
					m_pServerComm->SystemLog.Mz_AddLog(errmsg);
				}
				else
				{
					Sleep(50);
					continue;
				}
			}

			if (pClient->m_sockfd != INVALID_SOCKET)
			{
				int rcvLen = 0;
				pClient->Recv(recvBuf, 4096,500);
				if (rcvLen > 0)
				{
					char* Revbuf = NULL;
					int Revlen = 0;
					m_pServerComm->DispatchParamData(pThis->CommPort.PortName,recvBuf, rcvLen);
					if (Revbuf != NULL && Revlen > 0)
					{
						pClient->Send(Revbuf, Revlen);
						delete[] Revbuf;
						Revbuf = NULL;
						Revlen = 0;
					}
				}
				else if (rcvLen < 0)
				{
					pClient->Close();
					bConnectFlag = FALSE;
					TCHAR errmsg[1024] = { 0 };
					_stprintf_s(errmsg, 1024, _T("%s线程客户端断开连接"), pThis->ThreadName.c_str());
					m_pServerComm->SystemLog.Mz_AddLog(errmsg);
				}
			}
		}
		Sleep(1);
	}
	return 0;
}

unsigned int __stdcall ParamThreadSendProc(void* pParam)
{
	ThreadParam* pThis = (ThreadParam*)pParam;
	CMsvServerComm* m_pServerComm = (CMsvServerComm*)pThis->pParam;
	BOOL bConnectFlag = FALSE;//初始状态下默认连接断开
	R_CPacketData alarmdata;
	while (TRUE)
	{
		if (FALSE == InterlockedExchangeAdd((long*)&(m_pServerComm->m_bThreadRun), 0))
		{
			TCHAR errmsg[1024] = { 0 };
			_stprintf_s(errmsg, 1024, _T("%s线程退出"), pThis->ThreadName.c_str());
			m_pServerComm->SystemLog.Mz_AddLog(errmsg);
			break;
		}

		if(pThis->isActasServer == 1)
		{
			CTcpServer *pServer = (CTcpServer*)pThis->Socketsvr;
			if (!bConnectFlag)
			{
				int ret = pServer->Accept();
				if (ret > 0)
				{
					bConnectFlag = TRUE;
					TCHAR errmsg[1024] = { 0 };
					_stprintf_s(errmsg, 1024, _T("%s线程接收到客户端连接"), pThis->ThreadName.c_str());
					m_pServerComm->SystemLog.Mz_AddLog(errmsg);
				}
			}

			if (m_pServerComm->GetPacketDataSend(pThis->CommPort.PortName,alarmdata))
			{
				if (bConnectFlag)
				{
					//do send
					int nRet = 0;
					int tmp_count = 0;
					do
					{
						nRet = pServer->Send(alarmdata.buffer, alarmdata.bufferLength);
						if (nRet < 0)
						{
							pServer->CloseClient();
							bConnectFlag = FALSE;
							TCHAR errmsg[1024] = { 0 };
							_stprintf_s(errmsg, 1024, _T("%s线程客户端断开连接"), pThis->ThreadName.c_str());
							m_pServerComm->SystemLog.Mz_AddLog(errmsg);
						}
					} while (nRet == 0 && tmp_count++ < TIME_OUT_COUNT);

					if (tmp_count >= TIME_OUT_COUNT)
					{
						pServer->CloseClient();
						bConnectFlag = FALSE;
						TCHAR errmsg[1024] = { 0 };
						_stprintf_s(errmsg, 1024, _T("%s发送-连接未断开，但发送超时，主动断开"), pThis->ThreadName.c_str());
						m_pServerComm->SystemLog.Mz_AddLog(errmsg);
					}
				}
			}
		}
		else
		{
			CTcpClient *pClient = (CTcpClient*)pThis->Socketsvr;
			if (!bConnectFlag)
			{
				int ret = pClient->ConnectToServer(pThis->CommPort.remote_IP.c_str(), pThis->CommPort.nRemotePort);
				if (ret == 0)
				{
					bConnectFlag = TRUE;
					TCHAR errmsg[1024] = { 0 };
					_stprintf_s(errmsg, 1024, _T("%s线程连接到服务器"), pThis->ThreadName.c_str());
					m_pServerComm->SystemLog.Mz_AddLog(errmsg);
				}
			}

			if (m_pServerComm->GetPacketDataSend(pThis->CommPort.PortName,alarmdata))
			{
				if (bConnectFlag)
				{
					//do send
					int nRet = 0;
					int tmp_count = 0;
					do
					{
						nRet = pClient->Send(alarmdata.buffer, alarmdata.bufferLength);
						if (nRet < 0)
						{
							pClient->Close();
							bConnectFlag = FALSE;
							TCHAR errmsg[1024] = { 0 };
							_stprintf_s(errmsg, 1024, _T("%s线程客户端断开连接"), pThis->ThreadName.c_str());
							m_pServerComm->SystemLog.Mz_AddLog(errmsg);
						}
					} while (nRet == 0 && tmp_count++ < TIME_OUT_COUNT);

					if (tmp_count >= TIME_OUT_COUNT)
					{
						pClient->Close();
						bConnectFlag = FALSE;
						TCHAR errmsg[1024] = { 0 };
						_stprintf_s(errmsg, 1024, _T("%s发送-连接未断开，但发送超时，主动断开"), pThis->ThreadName.c_str());
						m_pServerComm->SystemLog.Mz_AddLog(errmsg);
					}
				}
			}
		}

		Sleep(1);
	}

	return 0;
}



