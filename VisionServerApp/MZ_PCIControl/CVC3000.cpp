#include "pch.h"
#include "CVC3000.h"

//控制模块SDK相关接口

//创建句柄 && 最多可创建32个串口句柄
int CControlInterface::CreateHandle(void** handle)
{
    return MV_IO_CreateHandle(handle);
}

//销毁句柄
int CControlInterface::DestroyHandle(void* handle)
{
    return MV_IO_DestroyHandle(handle);
}

//打开串口
int CControlInterface::Open(void* handle, MV_IO_SERIAL* pstSerial)
{
    return MV_IO_Open(handle, pstSerial);
}

//关闭串口
int CControlInterface::Close(void* handle)
{
    return MV_IO_Close(handle);
}

//获取SDK版本号
int CControlInterface::GetSDKVersion(MV_IO_VERSION* pstVersion)
{
    return MV_IO_GetSDKVersion(pstVersion);
}

//获取固件版本号
int CControlInterface::GetFirmwareVersion(void* handle, MV_IO_VERSION* pstVersion)
{
    return MV_IO_GetFirmwareVersion(handle, pstVersion);
}

//固件升级
int CControlInterface::LocalUpgrade(void* handle, unsigned char* pData, unsigned int nLen, MV_IO_LOCAL_UPGRADE* pstLocalUpgrade)
{
    return MV_IO_LocalUpgrade(handle, pData, nLen, pstLocalUpgrade);
}

//重置参数
int CControlInterface::ResetParam(void* handle)
{
    return MV_IO_ResetParam(handle);
}

//重启设备
int CControlInterface::Reboot(void* handle)
{
    return MV_IO_Reboot(handle);
}

//保存参数
int CControlInterface::SaveParam(void* handle)
{
    return MV_IO_SaveParam(handle);
}

//注册沿检测回调
int CControlInterface::RegisterEdgeDetectionCallBack(void* handle, DetectFunType cbOutput, void* pUser)
{
    return MV_IO_RegisterEdgeDetectionCallBack(handle, cbOutput, pUser);
}

//打开Dugview工具调试接口
void CControlInterface::SetDebugView(unsigned int nFlag)
{
    return MV_IO_SetDebugView(nFlag);
}

//设置RS232/485/422
int CControlInterface::SetRSConfig(MV_COM_CONFIG* pstComConfig, MV_COM_DEVICE_TYPE oType)
{
    return MV_IO_SetRSConfig(pstComConfig, oType);
}


//IO模块
//输出参数设置
int CIOInterface::SetOutput(void* handle, MV_IO_SET_OUTPUT* pstOutput)
{
    return MV_IO_SetOutput(handle, pstOutput);
}

//开启输出使能
int CIOInterface::SetOutputEnable(void* handle, MV_IO_OUTPUT_ENABLE* pstEnable)
{
    return MV_IO_SetOutputEnable(handle, pstEnable);
}

//获取PNP使能
int CIOInterface::GetPNPEnable(void* handle, MV_IO_PNP_TYPE* pnType)
{
    return MV_IO_GetPNPEnable(handle, pnType);
}


//开启PNP使能
int CIOInterface::ExcutePNPEnable(void* handle, MV_IO_PNP_TYPE nType)
{
    return MV_IO_ExcutePNPEnable(handle, nType);
}

//输入参数设置
int CIOInterface::SetInput(void* handle, MV_IO_SET_INPUT* pstInput)
{
    return MV_IO_SetInput(handle, pstInput);
}

//设置输出触发输入
int CIOInterface::SetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam)
{
    return MV_IO_SetOutPortTriggerSource(handle, pstSourceParam);
}


//输入关联输出端口
int CIOInterface::AssociatedOutPort(void* handle, MV_IO_ASSOCIATEPORT_PARAM* pstAssocParam)
{
    return MV_IO_AssociatedOutPort(handle, pstAssocParam);
}

//获取输入参数设置
int CIOInterface::GetPortInputParam(void* handle, MV_IO_SET_INPUT* pstInput)
{
    return MV_IO_GetPortInputParam(handle, pstInput);
}

//获取输出参数设置
int CIOInterface::GetPortOutputParam(void* handle, MV_IO_SET_OUTPUT* pstInput)
{
    return MV_IO_GetPortOutputParam(handle, pstInput);
}

//获取输入电平信息,在沿检测开启时，不能使用该接口获取电平
int CIOInterface::GetInputLevel(void* handle, MV_IO_INPUT_LEVEL* pstInputLevel)
{
    return MV_IO_GetInputLevel(handle, pstInputLevel);
}

//获取输出触发输入
int CIOInterface::GetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam)
{
    return MV_IO_GetOutPortTriggerSource(handle, pstSourceParam);
}

int CIOInterface::SetMainIODeviceType(MV_MAINIO_DEVICE_TYPE enDeviceType)
{
    return MV_IO_SetMainIODeviceType(enDeviceType);
}

//初始化WIN IO接口
int CIOInterface::WinIOInit()
{
    return MV_IO_WinIO_Init();
}

//注销WIN IO接口
int CIOInterface::WinIODeInit()
{
    return MV_IO_WinIO_DeInit();
}

//初始化主板IO
int CIOInterface::MainIOInit()
{
    return MV_IO_MainIOInit();
}

//获取主板IO输入电平,调用前需要先初始化MV_IO_WinIO_Init
int CIOInterface::GetMainInputLevel(unsigned char* pStatus)
{
    return MV_IO_GetMainInputLevel(pStatus);
}

//设置主板IO输出电平，调用前需初始化MV_IO_WinIO_Init
int CIOInterface::SetMainOutputLevel(MV_GIO_PORT_NUMBER nPort, MV_GIO_LEVEL nStatus)
{
    return MV_IO_SetMainOutputLevel(nPort, nStatus);
}

//设置主板IO PNP模式，调用前需初始化MV_IO_WinIO_Init 
int CIOInterface::SetMainGPO_NPN(unsigned int nEnable)
{
    return MV_IO_SetMainGPO_NPN(nEnable);
}

//光源模块
//LIGHT(亮度)参数设置
int CLightInterface::SetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam)
{
    return MV_IO_SetLightParam(handle, pstLightParam);
}

//LIGHT(亮度)参数获取
int CLightInterface::GetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam)
{
    return MV_IO_GetLightParam(handle, pstLightParam);
}

//设置光源触发输入
int CLightInterface::SetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam)
{
    return MV_IO_SetLightTriggerSource(handle, pstSourceParam);
}

//获取光源触发输入
int CLightInterface::GetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam)
{
    return MV_IO_GetLightTriggerSource(handle, pstSourceParam);
}

CIOControlSDK::CIOControlSDK()
{
    m_pcIOModule = NULL;
    m_pcLightModule = NULL;
    m_pcControlModule = NULL;
}

CIOControlSDK::~CIOControlSDK()
{
    if (m_pcIOModule)
    {
        delete m_pcIOModule;
        m_pcIOModule = NULL;
    }

    if (m_pcLightModule)
    {
        delete m_pcLightModule;
        m_pcLightModule = NULL;
    }

    if (m_pcControlModule)
    {
        delete m_pcControlModule;
        m_pcControlModule = NULL;
    }
}

int CIOControlSDK::Init()
{
    if (NULL == m_pcIOModule)
    {
        m_pcIOModule = new(std::nothrow) CIOInterface;
        if (NULL == m_pcIOModule)
        {
            return MV_E_RESOURCE;
        }
    }

    if (NULL == m_pcLightModule)
    {
        m_pcLightModule = new(std::nothrow) CLightInterface;
        if (NULL == m_pcLightModule)
        {
            return MV_E_RESOURCE;
        }
    }

    if (NULL == m_pcControlModule)
    {
        m_pcControlModule = new(std::nothrow) CControlInterface;
        if (NULL == m_pcControlModule)
        {
            return MV_E_RESOURCE;
        }
    }

    return MV_OK;
}

int CIOControlSDK::CreateHandle(void** handle)
{
    return m_pcControlModule->CreateHandle(handle);
}

int CIOControlSDK::DestroyHandle(void* handle)
{
    return m_pcControlModule->DestroyHandle(handle);
}

int CIOControlSDK::GetSDKVersion(MV_IO_VERSION* pstVersion)
{
    return m_pcControlModule->GetSDKVersion(pstVersion);
}

int CIOControlSDK::Open(void* handle, MV_IO_SERIAL* pstSerial)
{
    return m_pcControlModule->Open(handle, pstSerial);
}

int CIOControlSDK::Close(void* handle)
{
    return m_pcControlModule->Close(handle);
}

int CIOControlSDK::GetFirmwareVersion(void* handle, MV_IO_VERSION* pstVersion)
{
    return m_pcControlModule->GetFirmwareVersion(handle, pstVersion);
}

int CIOControlSDK::LocalUpgrade(void* handle, unsigned char* pData, unsigned int nLen, MV_IO_LOCAL_UPGRADE* pstLocalUpgrade)
{
    return m_pcControlModule->LocalUpgrade(handle, pData, nLen, pstLocalUpgrade);
}

int CIOControlSDK::ResetParam(void* handle)
{
    return m_pcControlModule->ResetParam(handle);
}

int CIOControlSDK::Reboot(void* handle)
{
    return m_pcControlModule->Reboot(handle);
}

int CIOControlSDK::SaveParam(void* handle)
{
    return m_pcControlModule->SaveParam(handle);
}

int CIOControlSDK::RegisterEdgeDetectionCallBack(void* handle, DetectFunType cbOutput, void* pUser)
{
    return m_pcControlModule->RegisterEdgeDetectionCallBack(handle, cbOutput, pUser);
}

void CIOControlSDK::SetDebugView(unsigned int nFlag)
{
    return m_pcControlModule->SetDebugView(nFlag);
}

int CIOControlSDK::SetRSConfig(MV_COM_CONFIG* pstComConfig, MV_COM_DEVICE_TYPE oType)
{
    return m_pcControlModule->SetRSConfig(pstComConfig, oType);
}

int CIOControlSDK::SetOutput(void* handle, MV_IO_SET_OUTPUT* pstOutput)
{
    return m_pcIOModule->SetOutput(handle, pstOutput);
}

int CIOControlSDK::SetOutputEnable(void* handle, MV_IO_OUTPUT_ENABLE* pstEnable)
{
    return m_pcIOModule->SetOutputEnable(handle, pstEnable);
}

int CIOControlSDK::GetPNPEnable(void* handle, MV_IO_PNP_TYPE* pnType)
{
    return m_pcIOModule->GetPNPEnable(handle, pnType);
}


int CIOControlSDK::ExcutePNPEnable(void* handle, MV_IO_PNP_TYPE nType)
{
    return m_pcIOModule->ExcutePNPEnable(handle, nType);
}

int CIOControlSDK::SetInput(void* handle, MV_IO_SET_INPUT* pstInput)
{
    return m_pcIOModule->SetInput(handle, pstInput);
}

//设置输出触发输入
int CIOControlSDK::SetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam)
{
    return m_pcIOModule->SetOutPortTriggerSource(handle, pstSourceParam);
}

int CIOControlSDK::GetPortInputParam(void* handle, MV_IO_SET_INPUT* pstInput)
{
    return m_pcIOModule->GetPortInputParam(handle, pstInput);
}

int CIOControlSDK::GetPortOutputParam(void* handle, MV_IO_SET_OUTPUT* pstInput)
{
    return m_pcIOModule->GetPortOutputParam(handle, pstInput);
}

int CIOControlSDK::GetInputLevel(void* handle, MV_IO_INPUT_LEVEL* pstInputLevel)
{
    return m_pcIOModule->GetInputLevel(handle, pstInputLevel);
}

int CIOControlSDK::AssociatedOutPort(void* handle, MV_IO_ASSOCIATEPORT_PARAM* pstAssocParam)
{
    return m_pcIOModule->AssociatedOutPort(handle, pstAssocParam);
}

//获取输出触发输入
int CIOControlSDK::GetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam)
{
    return m_pcIOModule->GetOutPortTriggerSource(handle, pstSourceParam);
}

int CIOControlSDK::SetMainIODeviceType(MV_MAINIO_DEVICE_TYPE enDeviceType)
{
    return m_pcIOModule->SetMainIODeviceType(enDeviceType);
}

int CIOControlSDK::WinIOInit()
{
    return m_pcIOModule->WinIOInit();
}
int CIOControlSDK::WinIODeInit()
{
    return m_pcIOModule->WinIODeInit();
}
int CIOControlSDK::MainIOInit()
{
    return m_pcIOModule->MainIOInit();
}
int CIOControlSDK::GetMainInputLevel(unsigned char* pStatus)
{
    return m_pcIOModule->GetMainInputLevel(pStatus);
}
int CIOControlSDK::SetMainOutputLevel(MV_GIO_PORT_NUMBER nPort, MV_GIO_LEVEL nStatus)
{
    return m_pcIOModule->SetMainOutputLevel(nPort, nStatus);
}
int CIOControlSDK::SetMainGPO_NPN(unsigned int nEnable)
{
    return m_pcIOModule->SetMainGPO_NPN(nEnable);
}

int CIOControlSDK::SetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam)
{
    return m_pcLightModule->SetLightParam(handle, pstLightParam);
}

int CIOControlSDK::GetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam)
{
    return m_pcLightModule->GetLightParam(handle, pstLightParam);
}

int CIOControlSDK::SetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam)
{
    return m_pcLightModule->SetLightTriggerSource(handle, pstSourceParam);
}

int CIOControlSDK::GetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam)
{
    return m_pcLightModule->GetLightTriggerSource(handle, pstSourceParam);
}