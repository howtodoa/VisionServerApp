#pragma once
#include "WinSock2.h"
#include <vector>
#include "PacketHead.h"
#include "TcpServer.h"
#include "..//MZ_Log/Mz_LogConn.h"

typedef struct _AlarmMsg
{
	BOOL bAlarmFlag;
	std::string strAlarmMsg;
}AlarmMsg;

class MsvClientComm
{
public:
	MsvClientComm(void);
	~MsvClientComm(void);

	int StartWork();
	int StopWork();

protected:

	int InitSocks(CommPorts SysParamPort);
	int ReleaseSocks();
	int DispatchParamData(const char * buf, int nLen);

	unsigned int __stdcall ParamThreadProc(void* pParam);
	//�������
	int HandleCameraParam(const char * buf, int nLen);

	//IO������
	int HandleSigMnrParam(const char * buf, int nLen);

	//���ϵͳ����
	int HandleSystemParam(const char * buf, int nLen);

	//�㷨����
	int HandleAgorithmParam(const char* buf, int nLen);
	/////////////////////////////////////////////////////////////
	volatile BOOL m_bThreadRun;
public:
	CTcpServer* SysParamPort;//ϵͳ�����˿�
	CTcpServer* DataPackSendPort;//���ݷ��Ͷ˿�
	CTcpServer* CameraStaSendPort;//���״̬���Ͷ˿�
	CTcpServer* AlarmMsgPort;//������Ϣ���Ͷ˿�

private:
	Mz_Log::COperation SystemLog;
};

