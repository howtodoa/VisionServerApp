#include "pch.h"
#include "MZ_IOControlLib.h"
#include "MZ_PCIControlconn.h"
namespace Mz_IOControl
{
    static IOCommom* m_iocomm;
void COperation::InitSDK()
{
    m_iocomm = new IOCommom();
}

bool COperation::OpenSerial(std::string ComName)
{
    return m_iocomm->OpenSerial(ComName);
}

bool COperation::CloseSerial(std::string ComName)
{
    return m_iocomm->CloseSerial(ComName);
}


int COperation::SaveParam(string ComName)
{
    return m_iocomm->SaveParam(ComName);
}

int COperation::Reboot(string ComName)
{
    return m_iocomm->Reboot(ComName);
}

int COperation::OpenGPIO(int GPIO_Type)
{
    return m_iocomm->OpenGPIO(GPIO_Type);
}

int COperation::SetGPO_Hi(int port)
{
    return m_iocomm->SetGPO_Hi(port);
}

int COperation::SetGPO_Lo(int port)
{
    return m_iocomm->SetGPO_Lo(port);
}

int COperation::GetGPI(BYTE& level)
{
    return m_iocomm->GetGPI(level);
}

int COperation::SetPnpEnable(int pnptpye)
{
    return m_iocomm->SetPnpEnable(pnptpye);
}

int COperation::SetLightParam(string ComName, int LightPort, int LightValue, int nLightState, int LightEdge, int DurTime, int& SourcePort)
{
    return m_iocomm->SetLightParam( ComName,  LightPort,  LightValue,  nLightState,  LightEdge,  DurTime,  SourcePort);
}

int COperation::SetLightTrigger(string ComName, int LightPort, int SourcePort)
{
    return m_iocomm->SetLightTrigger(ComName, LightPort, SourcePort);
}

int COperation::IOBoard_SetInput(string ComName, int Port, int Edge, int Enable, int DelayTime, int GlitchTime)
{
    return m_iocomm->IOBoard_SetInput(ComName, Port, Edge, Enable, DelayTime, GlitchTime);
}


int COperation::IOBoard_SetOutput(string ComName, int Port, int Type, int PluseWidth, int PlusePeriod, int DurTime, int ValidLevel)
{
    return m_iocomm->IOBoard_SetOutput(ComName, Port, Type, PluseWidth, PlusePeriod, DurTime, ValidLevel);
}

int COperation::IOBoard_SetOutputTrigger(string ComName, int OutputPort, int TriggerSource)
{
    return m_iocomm->IOBoard_SetOutputTrigger(ComName, OutputPort, TriggerSource);
}

int COperation::IOBoard_GetIOInputLevel(string ComName, int& PortNum, vector<int>& level)
{
    return m_iocomm->IOBoard_GetIOInputLevel(ComName, PortNum, level);
}

int COperation::IOBoard_SetOutputEnable(string ComName, int Port)
{
    return m_iocomm->IOBoard_SetOutputEnable(ComName, Port);
}

int COperation::IOBoard_SetOutputDisable(string ComName, int Port)
{
    return m_iocomm->IOBoard_SetOutputDisable(ComName, Port);
}

int COperation::PCIe_OpenDevice(string BoardName, int CardID)
{
    return m_iocomm->PCIe_OpenDevice(BoardName, CardID);
}

int COperation::PCIe_CloseDevice(string BoardName)
{
    return m_iocomm->PCIe_CloseDevice(BoardName);
}

int COperation::PCIe_WriteDeviceIO(string BoardName, UINT writeDoDat)
{
    return m_iocomm->PCIe_WriteDeviceIO(BoardName, writeDoDat);
}

int COperation::PCIe_ReadDeviceIO(string BoardName, UINT& readDiDat)
{
    return m_iocomm->PCIe_ReadDeviceIO(BoardName, readDiDat);
}

int COperation::PCIe_WriteDeviceIODobit(string BoardName, int bit, UINT writeDoDat)
{
    return m_iocomm->PCIe_WriteDeviceIODobit(BoardName, bit, writeDoDat);
}

int COperation::PCIe_ReadDeviceIODobit(string BoardName, int bit, UINT& readDiDat)
{
    return m_iocomm->PCIe_ReadDeviceIODobit(BoardName, bit, readDiDat);
}

int COperation::PCIe_ReadDeviceIODibit(string BoardName, int bit, UINT& readDiDat)
{
    return m_iocomm->PCIe_ReadDeviceIODibit(BoardName, bit, readDiDat);
}
 }