#pragma once
#include "PacketHead.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "../MZ_Log/Mz_LogConn.h"

#define MAX_ALARMMSG_NUM 128
#define MAX_PACKETDATA_NUM 128
//�����ݺ��׽��ֵĽṹ��

struct ThreadParam
{
	HANDLE hThreadRecv = NULL;//�����߳̾��
	HANDLE hThreadSend = NULL;//�����߳̾��
	int isActasServer = -1;//�Ƿ���Ϊ������
	CommPorts CommPort; //ͨ�Ŷ˿�
	string ThreadName = ""; //�߳�����
	void* Socketsvr = NULL; //ͨ�ŷ�����
	void* pThisClass = NULL; //�̲߳���
};

typedef struct _MySocket
{
	SOCKET m_pSocket; //�׽���
	//��������
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
static unsigned int __stdcall ParamThreadRecvProc(void* pParam);//���������߳�
static unsigned int __stdcall ParamThreadSendProc(void* pParam);//���������߳�


//�������ݻص�����
void OnRecvDataServer(SOCKET pSocket, const char* buf, int nLen);
void OnRecvDataClient(SOCKET pSocket, const char* buf, int nLen);
//�����ص�����
void OnDisconnectServer(SOCKET pSocket);
//�������ӻص�����
void OnConnectServer(SOCKET pSocket);
//�ͻ�������״̬�ص�����
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

	int PushPacketDataSend(SOCKET comportname,const SC_PacketData aPacket);//���ݰ����Ͷ���
	BOOL GetPacketDataSend(SOCKET comportname, SC_PacketDataEx& aPacket);//���ݰ����Ͷ���

	int PushPacketDataRecv(SOCKET comportname, const SC_PacketData aPacket);//���ݰ����Ͷ���
	BOOL GetPacketDataRecv(SOCKET comportname, SC_PacketDataEx& aPacket);

	//���շ��غ������ݰ�
	void PushReturnData(const SC_PacketDataEx aPacket);
	BOOL GetReturnData(SC_PacketDataEx& aPacket);
	
	//���ݰ����Ͷ���
	int DispatchParamData(SOCKET comportname, const char* buf, int nLen);
	void AnasysPacket(SC_PacketData inputpacket, string& outputfuncname, HValues& outputParams, HImages& outputImages, int& errcode, string& errmsg);
	void FindIndexByPortName(SOCKET portname, int& index);
	//��Դ
	void AddSocketSource(SOCKET socket);
	void DelSocketSource(SOCKET socket);
	int GetSocketCount();
	SOCKET GetSocketByIndex(int index);
	void DelAllSocketSource();
	//ͨ��IP�Ͷ˿ںŻ�ȡ�׽���
	SOCKET GetSocketByIPAndPort(string ip, int port);
	SOCKET GetSocketByfuncName(string funcName);
	//ͨ��IP��ȡ�׽���


	
private:
	CommPorts m_localCommPorts;
	//�׽��ֲ�����
	CRITICAL_SECTION m_lockSocketSource;
	//�׽�����Դ
	vector<MySocket> m_vecSocketRemote;
	MySocket m_socketLocal;
	//���ݰ����ն���
	vector < CRITICAL_SECTION >  m_lockPacetDataSend;
	vector<std::vector<SC_PacketDataEx>> m_vecPacetDataSend;
	//���ݰ����Ͷ���
	vector < CRITICAL_SECTION >  m_lockPacetDataRecv;
	vector<std::vector<SC_PacketDataEx>> m_vecPacetDataRecv;
	//�������ݶ���
	CRITICAL_SECTION   m_lockReturnDataRecv;
	std::vector<SC_PacketDataEx> m_vecReturnDataRecv;
	
	//�߳�
	ThreadParam m_vecThreadParams;
	volatile BOOL m_bThreadRun;
	Mz_Log::COperation SystemLog;
};

