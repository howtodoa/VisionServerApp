#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include "WinSock2.h"
#include "PacketHead.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "..//..//VisionServerApp/MZ_Log/Mz_LogConn.h"

#define MAX_ALARMMSG_NUM 128
#define MAX_PACKETDATA_NUM 128

struct ThreadParam
{
	HANDLE hThreadRecv = NULL;//�����߳̾��
	HANDLE hThreadSend = NULL;//�����߳̾��
	unsigned int nCameraID = -1;//���ID
	string ThreadName = ""; //�߳�����
	int isActasServer = -1;//�Ƿ���Ϊ������
	CommPorts CommPort; //ͨ�Ŷ˿�
	void* Socketsvr = NULL; //ͨ�ŷ�����
	void* pParam = NULL; //�̲߳���
};
typedef struct R_CPacketData
{
	char* buffer = NULL;
	int bufferLength = 0;
	R_CPacketData()
	{
		buffer = NULL;
		bufferLength = 0;
	}
	R_CPacketData(const R_CPacketData& aPacket)
	{
		buffer = new char[aPacket.bufferLength];
		memcpy(buffer, aPacket.buffer, aPacket.bufferLength);
		bufferLength = aPacket.bufferLength;
	}
	R_CPacketData(char *inputbuffer, int inputbufferLength)
	{
		buffer = new char[inputbufferLength];
		memcpy(buffer, inputbuffer, inputbufferLength);
		bufferLength = inputbufferLength;
	}
	~R_CPacketData()
	{
		if (buffer)
		{
			delete[] buffer;
			buffer = NULL;
		}
	}
	R_CPacketData(string funcName,HValues inputValues,HImages inputImages)
	{

	}


};

static unsigned int __stdcall ParamThreadRecvProc(void* pParam);//���������߳�
static unsigned int __stdcall ParamThreadSendProc(void* pParam);//���������߳�

class CMsvServerComm
{
	friend unsigned int __stdcall ParamThreadRecvProc(void* pParam);
	friend unsigned int __stdcall ParamThreadSendProc(void* pParam);
public:
	CMsvServerComm(vector<CommPorts> inputs);
	~CMsvServerComm(void);

	int StartWork();
	int StopWork();
	int DoActionFun(string portName, string funcName, HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg);
	Callbackfunc FindActionFun(string portName, string funcName);
	int AskActionFunList(string portName, vector<_Callbackfunc>& funclist);
	int RegisterActionFun(string portName, string funcName, Callbackfunc func);
private:
	void* InitSocks(CommPorts inputs);
	int ReleaseSocks();

	int PushPacketDataSend(string comportname,const R_CPacketData& aPacket);//���ݰ����Ͷ���
	BOOL GetPacketDataSend(string comportname, R_CPacketData& aPacket);//���ݰ����Ͷ���

	int PushPacketDataRecv(string comportname, const R_CPacketData& aPacket);//���ݰ����Ͷ���
	BOOL GetPacketDataRecv(string comportname, R_CPacketData& aPacket);//���ݰ����Ͷ���

	int DispatchParamData(string comportname, const char* buf, int nLen);
	void AnasysPacket(R_CPacketData inputpacket, string& outputfuncname, HValues& outputParams, HImages& outputImages, int& errcode, string& errmsg);//�������ݰ�
	void FindIndexByPortName(string comportname, int& index);
	//����
	//����
	void Pushfuncs(string potrname,Callbackfunc func);
	bool Getfunc(string potrname,string funcname, Callbackfunc& func);
private:
	vector<CommPorts> m_vecCommPorts;
	//���ݰ�����
	vector < CRITICAL_SECTION >  m_lockPacetDataSend;
	vector<std::vector<R_CPacketData>> m_vecPacetDataSend;

	vector < CRITICAL_SECTION >  m_lockPacetDataRecv;
	vector<std::vector<R_CPacketData>> m_vecPacetDataRecv;

	vector<CRITICAL_SECTION> m_lockfuncs;
	vector<vector<Callbackfunc>> m_vecfuncs;
	
	//�߳�
	vector<ThreadParam> m_vecThreadParams;
	volatile BOOL m_bThreadRun;
	Mz_Log::COperation SystemLog;
};

