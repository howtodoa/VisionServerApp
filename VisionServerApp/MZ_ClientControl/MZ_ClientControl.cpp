#include "pch.h"
#include "MZ_ClientControl.h"
#include "CMsvServerComm.h"

static CMsvServerComm* m_ServerComm = NULL;

void Mz_ClientControl::ClientOperation::InitSDK(CommPorts inputs)
{
	//logError("这是调试信息 from DLLMZ\n");
	m_ServerComm = new CMsvServerComm(inputs);
}

void Mz_ClientControl::ClientOperation::FreeSDK()
{
	if (m_ServerComm != NULL)
	{
		delete m_ServerComm;
		m_ServerComm = NULL;
	}
}

void Mz_ClientControl::ClientOperation::StartWork()
{
	m_ServerComm->StartWork();
}
void Mz_ClientControl::ClientOperation::StopWork()
{
	m_ServerComm->StopWork();
}

void Mz_ClientControl::ClientOperation::DoActionFun(CommPorts portname, string funname, HValues inputValues, HImages inputImages, HValues* outPutValues, HImages* outPutImages, int* retCode, string* retMsg,int timeout)
{

	m_ServerComm->DoActionRemoteFun(portname, funname, inputImages, inputValues, *outPutImages, *outPutValues, *retCode, *retMsg,timeout);
}

void Mz_ClientControl::ClientOperation::RegsiterFunitFun(Callbackfunc	func)
{
	m_ServerComm->RegisterActionFun_Local(func);
}

void Mz_ClientControl::ClientOperation::GetFunList_Remote(CommPorts portname, vector<Callbackfunc>& funList)
{
	m_ServerComm->AskActionFunList_Remote(portname, funList);
}
void Mz_ClientControl::ClientOperation::GetRemoteCommports(vector<CommPorts>& commports)
{
	m_ServerComm->AskRemoteCommports(commports);
}

