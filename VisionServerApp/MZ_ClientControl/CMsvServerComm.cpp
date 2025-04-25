#include "pch.h"
#include "CMsvServerComm.h"
#include <process.h>
#include <iostream>
#include <thread>

CMsvServerComm* m_pServerComm = NULL;
//char转wstring
wstring AnsiToUnicode(const char* str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t* buf = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, str, -1, buf, len);
	wstring result(buf);
	delete[] buf;
	return result;
}

//wstring转char
string UnicodeToAnsi(const wstring& str)
{
	int len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	char* buf = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, buf, len, NULL, NULL);
	string result(buf);
	delete[] buf;
	return result;
}
//通过SOCKET获取IP地址
wstring GetIP(SOCKET sock)
{
	sockaddr_in addr;
	int len = sizeof(addr);
	if (getpeername(sock, (sockaddr*)&addr, &len) == 0)
	{
		char ip[16];
		inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
		return AnsiToUnicode(ip);
	}
	return L"";
}
string GetIP_s(SOCKET sock)
{
	sockaddr_in addr;
	int len = sizeof(addr);
	if (getpeername(sock, (sockaddr*)&addr, &len) == 0)
	{
		char ip[16];
		inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
		return string(ip);
	}
	return "";
}
//通过套接字获取端口号
int GetPort(SOCKET sock)
{
	sockaddr_in addr;
	int len = sizeof(addr);
	if (getpeername(sock, (sockaddr*)&addr, &len) == 0)
	{
		return ntohs(addr.sin_port);
	}
	return 0;
}
//对比两个套接字是否相同
bool IsSameSocket(SOCKET sock1, SOCKET sock2)
{
	sockaddr_in addr1, addr2;
	int len1 = sizeof(addr1), len2 = sizeof(addr2);
	if (getsockname(sock1, (sockaddr*)&addr1, &len1) == 0 && getsockname(sock2, (sockaddr*)&addr2, &len2) == 0)
	{
		return addr1.sin_addr.S_un.S_addr == addr2.sin_addr.S_un.S_addr && addr1.sin_port == addr2.sin_port;
	}
	return false;
}
bool IsSameIPAndPort(string ip1, int port1, SOCKET sock2)
{
	sockaddr_in addr2;
	int len2 = sizeof(addr2);
	if (getpeername(sock2, (sockaddr*)&addr2, &len2) == 0)
	{
		//判断IP是否相同
		if (inet_addr(ip1.c_str()) == addr2.sin_addr.S_un.S_addr)
		{
			//判断端口是否相同
			if (htons(port1) == addr2.sin_port)
			{
				return true;
			}
		}
	}
	return false;
}

void AskActionFunList_static(HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg)
{
	vector<Callbackfunc> funclist;
	m_pServerComm->AskActionFunList_Local(NULL, funclist);
	for (int i = 0; i < funclist.size(); ++i)
	{
		HValue value;
		value = funclist[i].funcname.c_str();
		outputPrams.m_Values.push_back(value);
		value = funclist[i].inputImagesnums;
		outputPrams.m_Values.push_back(value);
		value = funclist[i].inputPramsnums;
		outputPrams.m_Values.push_back(value);
		value = funclist[i].outputImagesnums;
		outputPrams.m_Values.push_back(value);
		value = funclist[i].outputPramsnums;
		outputPrams.m_Values.push_back(value);
	}
}

CMsvServerComm::CMsvServerComm(CommPorts inputs)
{
	//初始化日志
	SystemLog.Mz_GetInstance(L"CMsvServerComm");
	WSADATA wsaData;
	int result;

	// 请求的 Winsock 版本是 2.2
	WORD wVersionRequested = MAKEWORD(2, 2);

	result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0) {
		// 初始化失败
		// 可以调用 WSAGetLastError() 获取错误代码
		return;
	}
	m_localCommPorts = inputs;
	//初始化套接字锁
	InitializeCriticalSection(&m_lockSocketSource);
	InitializeCriticalSection(&m_lockReturnDataRecv);
	m_pServerComm = this;
	std::cout << "This line is in file: " << __FILE__ << ", line: " << __LINE__ << std::endl;
}

CMsvServerComm::~CMsvServerComm(void)
{
	StopWork();
	WSACleanup();
}

int CMsvServerComm::StartWork()
{
	if (FALSE == InterlockedExchangeAdd((long*)&(m_bThreadRun), 0))
	{
		InterlockedExchange((long*)&m_bThreadRun, TRUE);
		
		m_vecThreadParams.ThreadName = m_localCommPorts.PortName;
		m_vecThreadParams.Socketsvr = InitSocks(m_localCommPorts);
		m_vecThreadParams.pThisClass = this;
		m_vecThreadParams.isActasServer = m_localCommPorts.isActAsServer;
		m_vecThreadParams.CommPort = m_localCommPorts;
			
		m_vecThreadParams.hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, ParamThreadRecvProc, &m_vecThreadParams, 0, NULL);
		m_vecThreadParams.hThreadSend = (HANDLE)_beginthreadex(NULL, 0, ParamThreadSendProc, &m_vecThreadParams, 0, NULL);
			
	}
	Callbackfunc func;
	func.funcname = "ASK_FUNLIST";
	func.inputImagesnums = 0;
	func.inputPramsnums = 0;
	func.outputImagesnums = 0;
	func.outputPramsnums = 1;
	func.func = AskActionFunList_static;
	RegisterActionFun_Local(func);
	std::cout << "This line is in file: " << __FILE__ << ", line: " << __LINE__ << std::endl;
	return 0;
}

int CMsvServerComm::StopWork()
{
	if (InterlockedExchangeAdd((long*)&(m_bThreadRun), 0))
	{
		InterlockedExchange((long*)&m_bThreadRun, FALSE);
		ReleaseSocks();//释放套接字资源

		//关闭接收线程
		WaitForSingleObject(m_vecThreadParams.hThreadRecv, INFINITE);
		CloseHandle(m_vecThreadParams.hThreadRecv);
		m_vecThreadParams.hThreadRecv = NULL;
		//关闭发送线程
		WaitForSingleObject(m_vecThreadParams.hThreadSend, INFINITE);
		CloseHandle(m_vecThreadParams.hThreadSend);
		m_vecThreadParams.hThreadSend = NULL;
	}
	std::cout << __FILE__ << __LINE__ << "DelAllSocketSource start" << std::endl;
	DelAllSocketSource();
	
	return 0;
}
//执行本地函数
bool CMsvServerComm::DoActionLocalFun(string funcName, HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg)
{
	Callbackfunc func = FindActionFun_Local(funcName);
	if (func.funcname == "")
	{
		errcode = -1;
		errmsg = "No function found.";
		return false;
	}
	func.func(inputImages, inputPrams, outputImages, outputPrams, errcode, errmsg);
	return true;
}
//执行远程函数
int CMsvServerComm::DoActionRemoteFun(CommPorts portName, string funcName, HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg,int Timeout)
{
	std::cout << "This line is in file: " << __FILE__ << ", line: " << __LINE__ << std::endl;
	SOCKET sock = INVALID_SOCKET;
	if (portName.isActAsServer == 0)
		sock = GetSocketByIPAndPort(portName.localhost_IP.IP, portName.localhost_IP.Port);
	else
		sock = GetSocketByfuncName(funcName);
	if (sock == INVALID_SOCKET)
	{
		errcode = -1;
		errmsg = "No socket found.";
		return -1;
	}
	PushPacketDataSend(sock, SC_PacketData(funcName, inputImages,inputPrams));
	//等待接收函数返回的消息
	int starttime = ::GetTickCount64();
	while(true)
	{
		int endtime = ::GetTickCount64();
		int costtime = endtime - starttime;
		if (costtime > Timeout)
		{
			errcode = -1;
			errmsg = "No response data from server.";
			return -1;//Fail
		}
		SC_PacketDataEx aMsg;
		int ret;
		if (ret=GetReturnData(aMsg))
		{
			///TODO: 处理返回数据...
			string functionnamerev;
			AnasysPacket(aMsg.data, functionnamerev, outputPrams, outputImages, errcode, errmsg);
			return 0;//Success
		}
		Sleep(50);
	}
	return -1;
}

Callbackfunc CMsvServerComm::FindActionFun_Remote(string funcName)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		for (int j = 0; j < m_vecSocketRemote[i].m_vecfuncs.size(); ++j)
		{
			if (m_vecSocketRemote[i].m_vecfuncs[j].funcname == funcName)
			{
				LeaveCriticalSection(&m_lockSocketSource);
				return m_vecSocketRemote[i].m_vecfuncs[j];
			}
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);

	return Callbackfunc();
}
Callbackfunc CMsvServerComm::FindActionFun_Local(string funcName)
{
	for (int j = 0; j < m_socketLocal.m_vecfuncs.size(); ++j)
	{
		if (m_socketLocal.m_vecfuncs[j].funcname == funcName)
		{
			LeaveCriticalSection(&m_lockSocketSource);
			return m_socketLocal.m_vecfuncs[j];
		}
	}
	return Callbackfunc();
}
//通过通信协议获取远程端的函数列表
int CMsvServerComm::GetFunList_Remote(SOCKET sock, vector<Callbackfunc>& funclist,int Timeout)
{
	if (sock == INVALID_SOCKET)
	{
		return -1;
	}
	PushPacketDataSend(sock, SC_PacketData("ASK_FUNLIST", HImages(), HValues()));//发送请求函数列表的消息
	SC_PacketDataEx aMsg;
	//等待接收函数列表的消息
	Sleep(Timeout);
	GetRemoteFuncListBySocket(sock, funclist);
	return 0;
}
int CMsvServerComm::AskActionFunList_Remote(CommPorts portName, vector<Callbackfunc>& funclist)
{
	SOCKET sock = GetSocketByIPAndPort(portName.localhost_IP.IP, portName.localhost_IP.Port);
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		if(IsSameSocket(m_vecSocketRemote[i].m_pSocket, sock))
		{
			for (int j = 0; j < m_vecSocketRemote[i].m_vecfuncs.size(); ++j)
			{
				funclist.push_back(m_vecSocketRemote[i].m_vecfuncs[j]);
			}
			LeaveCriticalSection(&m_lockSocketSource);
			return 0;
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);
	return 0;
}


int CMsvServerComm::AskActionFunList_Local(SOCKET sock, vector<Callbackfunc>& funclist)
{
	for(int j = 0; j < m_socketLocal.m_vecfuncs.size(); ++j)
	{
		funclist.push_back(m_socketLocal.m_vecfuncs[j]);
	}
	return 0;
}
int CMsvServerComm::RegisterActionFun_Local(Callbackfunc func)
{
	if(IsFuncNameExist_Local(m_socketLocal.m_pSocket,func.funcname) == true)
		return -1;
	m_socketLocal.m_vecfuncs.push_back(func);
	return 0;
}
int CMsvServerComm::RegisterActionFun_Remote(SOCKET sock, Callbackfunc func)
{
	if(IsFuncNameExist_Remote(sock,func.funcname) == true)
		return -1;

	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		if (IsSameSocket(m_vecSocketRemote[i].m_pSocket, sock))
		{
			m_vecSocketRemote[i].m_vecfuncs.push_back(func);
			LeaveCriticalSection(&m_lockSocketSource);
			return 0;
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);
	return 0;
}
bool CMsvServerComm::IsFuncNameExist_Remote(SOCKET sock,string funcName)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		if (IsSameSocket(m_vecSocketRemote[i].m_pSocket, sock))
		{
			for (int j = 0; j < m_vecSocketRemote[i].m_vecfuncs.size(); ++j)
			{
				if (m_vecSocketRemote[i].m_vecfuncs[j].funcname == funcName)
				{
					LeaveCriticalSection(&m_lockSocketSource);
					return true;
				}
			}
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);
	return false;
}
bool CMsvServerComm::IsFuncNameExist_Local(SOCKET sock, string funcName)
{
	for (int j = 0; j < m_socketLocal.m_vecfuncs.size(); ++j)
	{
		if (m_socketLocal.m_vecfuncs[j].funcname == funcName)
		{
			return true;
		}
	}
	return false;
}
bool CMsvServerComm::GetRemoteFuncListBySocket(SOCKET sock, vector<Callbackfunc>& funclist)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		if (IsSameSocket(m_vecSocketRemote[i].m_pSocket, sock))
		{
			for (int j = 0; j < m_vecSocketRemote[i].m_vecfuncs.size(); ++j)
			{
				funclist.push_back(m_vecSocketRemote[i].m_vecfuncs[j]);
			}
			LeaveCriticalSection(&m_lockSocketSource);
			return true;
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);
	return false;
}
bool CMsvServerComm::UpdataRemoteFuncList(SOCKET sock, vector<Callbackfunc>& funclist)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		if (IsSameSocket(m_vecSocketRemote[i].m_pSocket, sock))
		{
			m_vecSocketRemote[i].m_vecfuncs = funclist;
			LeaveCriticalSection(&m_lockSocketSource);
			return true;
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);
	return false;
}
void CMsvServerComm::AskRemoteCommports(vector<CommPorts>& commports)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		string ip = GetIP_s(m_vecSocketRemote[i].m_pSocket);
		int port = GetPort(m_vecSocketRemote[i].m_pSocket);
		commports.push_back(CommPorts(ip, port));
	}
	LeaveCriticalSection(&m_lockSocketSource);
}
//初始化套接字资源
void* CMsvServerComm::InitSocks(CommPorts input)
{
	if (input.isActAsServer == 1)
	{
		CAsyncTCPServer* tempSer = new CAsyncTCPServer(input.localhost_IP.Port);
		tempSer->SetOnReceiveCallback(&OnRecvDataServer);
		tempSer->SetOnConnectCallback(&OnConnectServer);
		tempSer->SetOnDisconnectCallback(&OnDisconnectServer);
		tempSer->Start();
		return (void*)tempSer;
	}
	else
	{
		CAsyncTCPClient* tempCli = new CAsyncTCPClient(input.localhost_IP.IP.c_str(), input.localhost_IP.Port);
		tempCli->SetOnReceiveCallback(&OnRecvDataClient);
		tempCli->SetOnConnectCallback(&OnConnectClientState);
		tempCli->Connect();
		return (void*)tempCli;
	}
	return NULL;
}

//释放套接字资源
int CMsvServerComm::ReleaseSocks()
{
	
	if(m_vecThreadParams.isActasServer == 1)
	{
		if (m_vecThreadParams.Socketsvr != NULL)
		{
			CAsyncTCPServer* tempSer = (CAsyncTCPServer*)m_vecThreadParams.Socketsvr;
			tempSer->Stop();
			delete tempSer;
			tempSer = NULL;
			m_vecThreadParams.Socketsvr = NULL;//add by tjq
			//std::cout << "ReleaseSocks: m_socket=" << m_socket << std::endl;
		}

	}
	else
	{

		if (m_vecThreadParams.Socketsvr != NULL)
		{
			CAsyncTCPClient* tempSer = (CAsyncTCPClient*)m_vecThreadParams.Socketsvr;
			tempSer->Disconnect();
			delete tempSer;
			tempSer = NULL;
			m_vecThreadParams.Socketsvr = NULL;//add by tjq
			std::cout << "ReleaseSocks: Deleted CAsyncTCPClient" << std::endl;
			//std::cout << "ReleaseSocks: m_socket=" << m_socket << std::endl;
		}
	}

	return 0;
}

int CMsvServerComm::PushPacketDataSend(SOCKET  comportname, const SC_PacketData aPacket)
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
		
		m_vecPacetDataSend[nIndex].push_back(SC_PacketDataEx(comportname, aPacket));
		nRet = m_vecPacetDataSend[nIndex].size();
		LeaveCriticalSection(&m_lockPacetDataSend[nIndex]);
	}
	return nRet;
}

BOOL CMsvServerComm::GetPacketDataSend(SOCKET comportname, SC_PacketDataEx& aPacket)
{
	BOOL bRet = FALSE;
	int nIndex = -1;
	FindIndexByPortName(comportname, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockPacetDataSend[nIndex]);
		if(nIndex >= 0 && nIndex <= m_vecPacetDataSend.size()-1 && m_vecPacetDataSend.size() > 0)
		{
			if (m_vecPacetDataSend[nIndex].size() > 0)
			{
				aPacket = m_vecPacetDataSend[nIndex][0];
				m_vecPacetDataSend[nIndex].erase(m_vecPacetDataSend[nIndex].begin());
				bRet = TRUE;
			}
		}
		LeaveCriticalSection(&m_lockPacetDataSend[nIndex]);
	}
	return bRet;
}

int CMsvServerComm::PushPacketDataRecv(SOCKET comportname, const SC_PacketData aPacket)
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
		m_vecPacetDataRecv[nIndex].push_back(SC_PacketDataEx(comportname, aPacket));
		nRet = m_vecPacetDataRecv[nIndex].size();
		LeaveCriticalSection(&m_lockPacetDataRecv[nIndex]);
	}
	return nRet;
}

BOOL CMsvServerComm::GetPacketDataRecv(SOCKET comportname, SC_PacketDataEx& aPacket)
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

void CMsvServerComm::PushReturnData(SC_PacketDataEx aPacket)
{
	EnterCriticalSection(&m_lockReturnDataRecv);
	if ((int)m_vecReturnDataRecv.size() >= 100)
	{
		m_vecReturnDataRecv.erase(m_vecReturnDataRecv.begin());
	}
	m_vecReturnDataRecv.push_back(aPacket);
	LeaveCriticalSection(&m_lockReturnDataRecv);
}

BOOL CMsvServerComm::GetReturnData(SC_PacketDataEx& aPacket)
{
	BOOL bRet = FALSE;
	EnterCriticalSection(&m_lockReturnDataRecv);
	if (m_vecReturnDataRecv.size() > 0)
	{
		aPacket = m_vecReturnDataRecv[0];
		m_vecReturnDataRecv.erase(m_vecReturnDataRecv.begin());
		bRet = TRUE;
	}
	LeaveCriticalSection(&m_lockReturnDataRecv);
	return bRet;
}


int CMsvServerComm::DispatchParamData(SOCKET  comportname, const char* buf, int nLen)
{
	static char* s_AnlyBuff = new char[4096];
	static size_t s_AnlyBuffLen = 4096;
	static int s_offset = 0;

	//解包...
	if (s_offset + nLen > s_AnlyBuffLen)
	{
		s_AnlyBuffLen = s_offset + nLen + 1024;
		char* temp = new char[s_AnlyBuffLen];
		memcpy(temp, s_AnlyBuff, s_offset);
		delete[] s_AnlyBuff;
		s_AnlyBuff = new char[s_AnlyBuffLen];
		memcpy(s_AnlyBuff, temp, s_offset);
		delete[] temp;
	}
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
		int data_len = (int)(pHead->data_len);
		data_len = data_len + sizeof(SC_PackHead);
		if (s_offset < data_len)
		{
			return -1;
		}

		//解析参数
		PushPacketDataRecv(comportname, SC_PacketData(pHead->GetData()));

		//移动剩下的包数据
		s_offset -= data_len;
		memcpy(s_AnlyBuff, s_AnlyBuff + data_len, s_offset);
	}

	return 0;
}

void CMsvServerComm::AnasysPacket(SC_PacketData inputpacket,string& outputfuncname, HValues	&outputParams, HImages& outputImages, int& errcode, string& errmsg)
{
//解析数据包
	SC_PackHead tempHead = inputpacket.head;
	outputfuncname = tempHead.user_define.toStr();
	int inputparams = tempHead.inputParamsnums;
	int inputimages = tempHead.inputImagesnums;
	int offsetPrt = 0;
	for (int i = 0; i < inputparams; ++i)
	{
		HValue tempValue;
		//tempValue = *(HValue*)(inputpacket.data.buffer + i * sizeof(HValue));
		memcpy(&tempValue, inputpacket.data.buffer + i * sizeof(HValue), sizeof(HValue));
		outputParams.m_Values.push_back(tempValue);
		offsetPrt += sizeof(HValue);
	}
	for (int i = 0; i < inputimages; ++i)
	{
		HImage tempImage;
		ImageHeader  imageHead;
		memcpy(&imageHead, inputpacket.data.buffer + offsetPrt, sizeof(ImageHeader));

		tempImage = HImage(inputpacket.data.buffer + offsetPrt, sizeof(ImageHeader));
		offsetPrt = offsetPrt + sizeof(ImageHeader) + imageHead.length;
		outputImages.m_Images.push_back(tempImage);
	}
}
void CMsvServerComm::FindIndexByPortName(SOCKET portname, int& index)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		if (IsSameSocket(portname, m_vecSocketRemote[i].m_pSocket))
		{
			index = i;
			LeaveCriticalSection(&m_lockSocketSource);
			return;
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);
}
//添加套接字资源
void CMsvServerComm::AddSocketSource(SOCKET socket)
{
	EnterCriticalSection(&m_lockSocketSource);
	m_vecSocketRemote.push_back(socket);
	LeaveCriticalSection(&m_lockSocketSource);
	//开辟接收缓存
	m_lockPacetDataRecv.push_back(CRITICAL_SECTION());
	InitializeCriticalSection(&m_lockPacetDataRecv[m_lockPacetDataRecv.size() - 1]);

	m_lockPacetDataSend.push_back(CRITICAL_SECTION());
	InitializeCriticalSection(&m_lockPacetDataSend[m_lockPacetDataSend.size() - 1]);

	EnterCriticalSection(&m_lockPacetDataRecv[m_lockPacetDataRecv.size() - 1]);
	m_vecPacetDataRecv.push_back(vector<SC_PacketDataEx>());
	LeaveCriticalSection(&m_lockPacetDataRecv[m_lockPacetDataRecv.size() - 1]);

	EnterCriticalSection(&m_lockPacetDataSend[m_lockPacetDataSend.size() - 1]);
	m_vecPacetDataSend.push_back(vector<SC_PacketDataEx>());
	LeaveCriticalSection(&m_lockPacetDataSend[m_lockPacetDataSend.size() - 1]);

	//获取函数列表
	EnterCriticalSection(&m_lockSocketSource);
	GetFunList_Remote(m_vecSocketRemote[m_vecSocketRemote.size() - 1].m_pSocket, m_vecSocketRemote[m_vecSocketRemote.size() - 1].m_vecfuncs);
	LeaveCriticalSection(&m_lockSocketSource);
}
//删除套接字资源
void CMsvServerComm::DelSocketSource(SOCKET socket)
{
	int nIndex = -1;
	FindIndexByPortName(socket, nIndex);
	if (nIndex != -1)
	{
		EnterCriticalSection(&m_lockSocketSource);
		m_vecSocketRemote.erase(m_vecSocketRemote.begin() + nIndex);
		LeaveCriticalSection(&m_lockSocketSource);

		EnterCriticalSection(&m_lockPacetDataRecv[nIndex]);
		m_vecPacetDataRecv[nIndex].clear();
		m_vecPacetDataRecv.erase(m_vecPacetDataRecv.begin() + nIndex);
		LeaveCriticalSection(&m_lockPacetDataRecv[nIndex]);

		EnterCriticalSection(&m_lockPacetDataSend[nIndex]);
		m_vecPacetDataSend[nIndex].clear();
		m_vecPacetDataSend.erase(m_vecPacetDataSend.begin() + nIndex);
		LeaveCriticalSection(&m_lockPacetDataSend[nIndex]);

		DeleteCriticalSection(&m_lockPacetDataRecv[nIndex]);
		DeleteCriticalSection(&m_lockPacetDataSend[nIndex]);

		m_lockPacetDataRecv.erase(m_lockPacetDataRecv.begin() + nIndex);
		m_lockPacetDataSend.erase(m_lockPacetDataSend.begin() + nIndex);
		
		
	}
}
int CMsvServerComm::GetSocketCount()
{
	EnterCriticalSection(&m_lockSocketSource);
	int nCount = m_vecSocketRemote.size();
	LeaveCriticalSection(&m_lockSocketSource);
	return nCount;
}
SOCKET CMsvServerComm::GetSocketByIndex(int index)
{
	EnterCriticalSection(&m_lockSocketSource);
	if (index >= 0 && index < m_vecSocketRemote.size())
	{
		SOCKET socket = m_vecSocketRemote[index].m_pSocket;
		LeaveCriticalSection(&m_lockSocketSource);
		return socket;
	}
	LeaveCriticalSection(&m_lockSocketSource);
	return -1;
}
//删除所有套接字资源
void CMsvServerComm::DelAllSocketSource()
{
	
	EnterCriticalSection(&m_lockSocketSource);
	m_vecSocketRemote.clear();
	LeaveCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_lockPacetDataRecv.size(); ++i)
	{
		EnterCriticalSection(&m_lockPacetDataRecv[i]);
		m_vecPacetDataRecv[i].clear();
		LeaveCriticalSection(&m_lockPacetDataRecv[i]);

		EnterCriticalSection(&m_lockPacetDataSend[i]);
		m_vecPacetDataSend[i].clear();
		LeaveCriticalSection(&m_lockPacetDataSend[i]);
		DeleteCriticalSection(&m_lockPacetDataRecv[i]);
		DeleteCriticalSection(&m_lockPacetDataSend[i]);
	}
	m_lockPacetDataRecv.clear();
	m_lockPacetDataSend.clear();
}

SOCKET CMsvServerComm::GetSocketByIPAndPort(string ip, int port)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		if (IsSameIPAndPort(ip, port, m_vecSocketRemote[i].m_pSocket))
		{
			LeaveCriticalSection(&m_lockSocketSource);
			return m_vecSocketRemote[i].m_pSocket;
		}
	}
	LeaveCriticalSection(&m_lockSocketSource);
	return -1;
}

SOCKET CMsvServerComm::GetSocketByfuncName(string funcName)
{
	EnterCriticalSection(&m_lockSocketSource);
	for (int i = 0; i < m_vecSocketRemote.size(); ++i)
	{
		for (int j = 0; j < m_vecSocketRemote[i].m_vecfuncs.size(); ++j)
		{
			if (m_vecSocketRemote[i].m_vecfuncs[j].funcname == funcName)
			{
				LeaveCriticalSection(&m_lockSocketSource);
				return m_vecSocketRemote[i].m_pSocket;
			}
		}
	}	
	LeaveCriticalSection(&m_lockSocketSource);
	return -1;
}
//异步发送数据
unsigned int __stdcall ParamThreadRecvProc(void* pParam)
{
	ThreadParam* pThis = (ThreadParam*)pParam;
	CMsvServerComm *m_pServerComm = (CMsvServerComm*)pThis->pThisClass;
	char recvBuf[4096] = { 0 };
	
	while (TRUE)
	{
		if (FALSE == InterlockedExchangeAdd((long*)&(m_pServerComm->m_bThreadRun), 0))
		{
			TCHAR errmsg[1024] = { 0 };
			_stprintf_s(errmsg, 1024, _T("%s线程退出"), AnsiToUnicode(pThis->ThreadName.c_str()).c_str());
			m_pServerComm->SystemLog.Mz_AddLog(errmsg);
			break;
		}
		//获取数据包
		//轮询连接 
		//解析数据包
		string funcname;
		HValues paramsIN, paramsOUT;
		HImages imagesIN, imagesOUT;
		int errcode = 0;
		string errmsg;
		for(int i = 0; i < m_pServerComm->GetSocketCount(); ++i)
		{
			SC_PacketDataEx packetdata;
			SOCKET socket = m_pServerComm->GetSocketByIndex(i);
			if (m_pServerComm->GetPacketDataRecv(socket, packetdata))
			{

				m_pServerComm->AnasysPacket(packetdata.data, funcname, paramsIN, imagesIN, errcode, errmsg);
				//这里要分析是执行函数命令还是回复函数命令
				if (funcname.find("OnReturn_") == 0)
				{
					//这里判断是不是返回的查询函数列表的命令
					if (funcname == "OnReturn_ASK_FUNLIST")
					{
						//这里要更新函数列表
						//解析函数列表
						vector<Callbackfunc> tempfuncs;
						for (int j = 0; j < paramsIN.m_Values.size();)
						{
							Callbackfunc tempfunc;
							tempfunc.funcname = paramsIN.m_Values[j].S();
							tempfunc.inputImagesnums = paramsIN.m_Values[j + 1].I();
							tempfunc.inputPramsnums = paramsIN.m_Values[j + 2].I();
							tempfunc.outputImagesnums = paramsIN.m_Values[j + 3].I();
							tempfunc.outputPramsnums = paramsIN.m_Values[j + 4].I();
							tempfuncs.push_back(tempfunc);
							j += 5;
						}
						//更新函数列表
						m_pServerComm->UpdataRemoteFuncList(socket, tempfuncs);
					}
					else
					{
						//这里是返回的其他函数的执行结果
						m_pServerComm->PushReturnData(packetdata);
					}
				}
				else
				{
					//执行函数
					if (m_pServerComm->IsFuncNameExist_Local(socket, funcname))
					{
						m_pServerComm->DoActionLocalFun(funcname, imagesIN, paramsIN, imagesOUT, paramsOUT, errcode, errmsg);
						//写入返回数据
						funcname = "OnReturn_" + funcname;
						SC_PacketData tempdata = SC_PacketData(funcname, imagesOUT, paramsOUT);
						m_pServerComm->PushPacketDataSend(socket, tempdata);
					}
					else
					{
						errcode = -1;
						errmsg = "函数不存在";
					}
					//写入日志
					TCHAR logmsg[1024] = { 0 };
					_stprintf_s(logmsg, 1024, _T("接收到来自%s的请求，函数名：%s，参数个数：%d，图像个数：%d，错误代码：%d，错误信息：%s"), GetIP(socket).c_str(), AnsiToUnicode(funcname.c_str()).c_str(), paramsIN.m_Values.size(), imagesIN.m_Images.size(), errcode, AnsiToUnicode(errmsg.c_str()).c_str());
					m_pServerComm->SystemLog.Mz_AddLog(logmsg);
				}
			}
			Sleep(50);
		}
		Sleep(0);
	}
	return 0;
}

unsigned int __stdcall ParamThreadSendProc(void* pParam)
{
	ThreadParam* pThis = (ThreadParam*)pParam;
	CMsvServerComm* m_pServerComm = (CMsvServerComm*)pThis->pThisClass;
	SC_PacketDataEx alarmdata;
	while (TRUE)
	{
		if (FALSE == InterlockedExchangeAdd((long*)&(m_pServerComm->m_bThreadRun), 0))
		{
			TCHAR errmsg[1024] = { 0 };
			_stprintf_s(errmsg, 1024, _T("%s线程退出"), AnsiToUnicode(pThis->ThreadName.c_str()).c_str());
			m_pServerComm->SystemLog.Mz_AddLog(errmsg);
			break;
		}
		alarmdata.data.Clear();
		//发送数据包
		for (int i = 0; i < m_pServerComm->GetSocketCount(); ++i)
		{
			SOCKET socket = m_pServerComm->GetSocketByIndex(i);
			if (m_pServerComm->GetPacketDataSend(socket, alarmdata))
			{
				//发送数据包
				int nLen = alarmdata.data.getdatelength();
				char* pBuf = new char[nLen];

				memcpy(pBuf, alarmdata.data.getHeaddata(), sizeof(SC_PackHead));
				memcpy(pBuf + sizeof(SC_PackHead), alarmdata.data.GetDatadata(), nLen - sizeof(SC_PackHead));
				bool bRet = false;
				if (pThis->CommPort.isActAsServer == true)
				{
					CAsyncTCPServer *pServer = (CAsyncTCPServer*)pThis->Socketsvr;
					bRet = pServer->SendToClient(socket, pBuf, nLen);
				}
				else
				{
					CAsyncTCPClient *pClient = (CAsyncTCPClient*)pThis->Socketsvr;
					bRet = pClient->Send(pBuf, nLen);
				}
				
				delete[] pBuf;
				pBuf = NULL;
				if (bRet == false)
				{
					TCHAR logmsg[1024] = { 0 };
					_stprintf_s(logmsg, 1024, _T("发送数据包失败，IP地址：%s，端口号：%d，发送长度：%d"), GetIP(socket).c_str(), GetPort(socket), nLen);
					m_pServerComm->SystemLog.Mz_AddLog(logmsg);
				}
				else
				{
					TCHAR logmsg[1024] = { 0 };
					_stprintf_s(logmsg, 1024, _T("发送数据包成功，IP地址：%s，端口号：%d，发送长度：%d"), GetIP(socket).c_str(), GetPort(socket), nLen);
					m_pServerComm->SystemLog.Mz_AddLog(logmsg);
				}
			}
			Sleep(50);
		}
		Sleep(0);
	}

	return 0;
}

void OnRecvDataServer(SOCKET pSocket, const char* buf, int nLen)
{
	if (nLen > 0)
	{
		m_pServerComm->DispatchParamData(pSocket, buf, nLen);
	}
}

void OnRecvDataClient(SOCKET pSocket, const char* buf, int nLen)
{
	if (nLen > 0)
	{
		m_pServerComm->DispatchParamData(pSocket, buf, nLen);
	}
}

void OnDisconnectServer(SOCKET pSocket)
{
	//有客户端断开连接，关闭接收缓存
	m_pServerComm->DelSocketSource(pSocket);
	//有客户端断开连接，写入日志
	TCHAR logmsg[1024] = { 0 };
	_stprintf_s(logmsg, 1024, _T("有客户端断开连接，IP地址：%s，端口号：%d"), GetIP(pSocket).c_str(),GetPort(pSocket));
	m_pServerComm->SystemLog.Mz_AddLog(logmsg);
}

void OnConnectServer(SOCKET pSocket)
{
	//有客户端连接成功，开辟接收缓存
	m_pServerComm->AddSocketSource(pSocket);
	//有客户端连接成功，写入日志
	TCHAR logmsg[1024] = { 0 };
	_stprintf_s(logmsg, 1024, _T("有客户端连接成功，IP地址：%s，端口号：%d"), GetIP(pSocket).c_str(), GetPort(pSocket));
	m_pServerComm->SystemLog.Mz_AddLog(logmsg);
}

void OnConnectClientState(SOCKET pSocket, const bool bConnected)
{
	if (bConnected)
	{
		//开辟接收缓存
		m_pServerComm->AddSocketSource(pSocket);
		//有客户端连接成功，写入日志
		TCHAR logmsg[1024] = { 0 };
		_stprintf_s(logmsg, 1024, _T("连接服务端成功"));
		m_pServerComm->SystemLog.Mz_AddLog(logmsg);
	}
	else
	{
		//关闭缓存
		m_pServerComm->DelSocketSource(pSocket);
		//有客户端断开连接，写入日志
		TCHAR logmsg[1024] = { 0 };
		_stprintf_s(logmsg, 1024, _T("断开服务端连接"));
		m_pServerComm->SystemLog.Mz_AddLog(logmsg);
	}
}


