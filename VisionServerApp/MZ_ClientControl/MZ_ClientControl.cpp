#include "pch.h"
#include "MZ_ClientControl.h"
#include "CMsvServerComm.h"

static CMsvServerComm *m_ServerComm;

void Mz_ClientControl::ClientOperation::InitSDK(vector<CommPorts>inputs)
{
	m_ServerComm = new CMsvServerComm(inputs);
}

void Mz_ClientControl::ClientOperation::FreeSDK()
{
	delete m_ServerComm;
	m_ServerComm = nullptr;
}

void Mz_ClientControl::ClientOperation::StartWork()
{
	m_ServerComm->StartWork();
}
void Mz_ClientControl::ClientOperation::StopWork()
{
	m_ServerComm->StopWork();
}

void Mz_ClientControl::ClientOperation::DoActionFun(string portname,string funname, HValues inputValues, HImages inputImages, HValues* outPutValues, HImages* outPutImages, int* retCode, string* retMsg)
{
	m_ServerComm->DoActionFun(portname, funname, inputImages, inputValues, *outPutImages,*outPutValues, *retCode, *retMsg);
}

