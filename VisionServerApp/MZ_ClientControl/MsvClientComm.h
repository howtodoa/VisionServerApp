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
	//相机参数
	int HandleCameraParam(const char * buf, int nLen);

	//IO卡参数
	int HandleSigMnrParam(const char * buf, int nLen);

	//软件系统参数
	int HandleSystemParam(const char * buf, int nLen);

	//算法参数
	int HandleAgorithmParam(const char* buf, int nLen);
	/////////////////////////////////////////////////////////////
	volatile BOOL m_bThreadRun;
public:
	CTcpServer* SysParamPort;//系统参数端口
	CTcpServer* DataPackSendPort;//数据发送端口
	CTcpServer* CameraStaSendPort;//相机状态发送端口
	CTcpServer* AlarmMsgPort;//报警信息发送端口

private:
	Mz_Log::COperation SystemLog;
};

