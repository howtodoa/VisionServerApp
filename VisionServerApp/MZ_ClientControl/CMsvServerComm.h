#pragma once
#include "PacketHead.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "../MZ_Log/Mz_LogConn.h"

#define MAX_ALARMMSG_NUM 128
#define MAX_PACKETDATA_NUM 128
//绑定数据和套接字的结构体

struct ThreadParam
{
	HANDLE hThreadRecv = NULL;//接收线程句柄
	HANDLE hThreadSend = NULL;//发送线程句柄
	int isActasServer = -1;//是否作为服务器
	CommPorts CommPort; //通信端口
	string ThreadName = ""; //线程名称
	void* Socketsvr = NULL; //通信服务器
	void* pThisClass = NULL; //线程参数
};

typedef struct _MySocket
{
	SOCKET m_pSocket; //套接字
	//函数队列
	vector<Callbackfunc> m_vecfuncs;
	_MySocket(SOCKET pSocket)
	{
		m_pSocket = pSocket;
	}
	_MySocket()
	{
		m_pSocket = INVALID_SOCKET;
	}
}MySocket;
static unsigned int __stdcall ParamThreadRecvProc(void* pParam);//参数接收线程
static unsigned int __stdcall ParamThreadSendProc(void* pParam);//参数发送线程


//接收数据回调函数
void OnRecvDataServer(SOCKET pSocket, const char* buf, int nLen);
void OnRecvDataClient(SOCKET pSocket, const char* buf, int nLen);
//断连回调函数
void OnDisconnectServer(SOCKET pSocket);
//接收连接回调函数
void OnConnectServer(SOCKET pSocket);
//客户端连接状态回调函数
void OnConnectClientState(SOCKET pSocket,const bool bConnected);
class CMsvServerComm
{
	friend void OnRecvDataServer(SOCKET pSocket, const char* buf, int nLen);
	friend void OnRecvDataClient(SOCKET pSocket, const char* buf, int nLen);
	friend void OnDisconnectServer(SOCKET pSocket);
	friend void OnConnectServer(SOCKET pSocket);
	friend void OnConnectClientState(SOCKET pSocket, const bool bConnected);
	friend unsigned int __stdcall ParamThreadRecvProc(void* pParam);
	friend unsigned int __stdcall ParamThreadSendProc(void* pParam);
public:
	CMsvServerComm(CommPorts inputs);
	~CMsvServerComm(void);

	int StartWork();
	int StopWork();

	//int DoActionRemoteFun(CommPorts portName, string funcName, HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg);
	bool DoActionLocalFun(string funcName, HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg);

	int DoActionRemoteFun(CommPorts portName, string funcName, HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg, int Timeout=2000);

	Callbackfunc FindActionFun_Remote(string funcName);
	Callbackfunc FindActionFun_Local(string funcName);
	int GetFunList_Remote(SOCKET sock, vector<Callbackfunc>& funclist, int Timeout = 200);
	int AskActionFunList_Remote(CommPorts portName, vector<Callbackfunc>& funclist);
	int AskActionFunList_Local(SOCKET sock, vector<Callbackfunc>& funclist);
	int RegisterActionFun_Local(Callbackfunc func);
	int RegisterActionFun_Remote(SOCKET sock, Callbackfunc func);
	bool IsFuncNameExist_Remote(SOCKET sock,string funcName);
	bool IsFuncNameExist_Local(SOCKET sock, string funcName);
	bool GetRemoteFuncListBySocket(SOCKET sock, vector<Callbackfunc>& funclist);
	bool UpdataRemoteFuncList(SOCKET sock, vector<Callbackfunc>& funclist);
	void AskRemoteCommports(vector<CommPorts>& commports);
private:
	void* InitSocks(CommPorts inputs);
	int ReleaseSocks();

	int PushPacketDataSend(SOCKET comportname,const SC_PacketData aPacket);//数据包发送队列
	BOOL GetPacketDataSend(SOCKET comportname, SC_PacketDataEx& aPacket);//数据包发送队列

	int PushPacketDataRecv(SOCKET comportname, const SC_PacketData aPacket);//数据包发送队列
	BOOL GetPacketDataRecv(SOCKET comportname, SC_PacketDataEx& aPacket);

	//接收返回函数数据包
	void PushReturnData(const SC_PacketDataEx aPacket);
	BOOL GetReturnData(SC_PacketDataEx& aPacket);
	
	//数据包发送队列
	int DispatchParamData(SOCKET comportname, const char* buf, int nLen);
	void AnasysPacket(SC_PacketData inputpacket, string& outputfuncname, HValues& outputParams, HImages& outputImages, int& errcode, string& errmsg);
	void FindIndexByPortName(SOCKET portname, int& index);
	//资源
	void AddSocketSource(SOCKET socket);
	void DelSocketSource(SOCKET socket);
	int GetSocketCount();
	SOCKET GetSocketByIndex(int index);
	void DelAllSocketSource();
	//通过IP和端口号获取套接字
	SOCKET GetSocketByIPAndPort(string ip, int port);
	SOCKET GetSocketByfuncName(string funcName);
	//通过IP获取套接字


	
private:
	CommPorts m_localCommPorts;
	//套接字操作锁
	CRITICAL_SECTION m_lockSocketSource;
	//套接字资源
	vector<MySocket> m_vecSocketRemote;
	MySocket m_socketLocal;
	//数据包接收队列
	vector < CRITICAL_SECTION >  m_lockPacetDataSend;
	vector<std::vector<SC_PacketDataEx>> m_vecPacetDataSend;
	//数据包发送队列
	vector < CRITICAL_SECTION >  m_lockPacetDataRecv;
	vector<std::vector<SC_PacketDataEx>> m_vecPacetDataRecv;
	//返回数据队列
	CRITICAL_SECTION   m_lockReturnDataRecv;
	std::vector<SC_PacketDataEx> m_vecReturnDataRecv;
	
	//线程
	ThreadParam m_vecThreadParams;
	volatile BOOL m_bThreadRun;
	Mz_Log::COperation SystemLog;
};

