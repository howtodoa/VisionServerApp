#include "pch.h"
#include "CVC3000.h"

//����ģ��SDK��ؽӿ�

//������� && ���ɴ���32�����ھ��
int CControlInterface::CreateHandle(void** handle)
{
    return MV_IO_CreateHandle(handle);
}

//���پ��
int CControlInterface::DestroyHandle(void* handle)
{
    return MV_IO_DestroyHandle(handle);
}

//�򿪴���
int CControlInterface::Open(void* handle, MV_IO_SERIAL* pstSerial)
{
    return MV_IO_Open(handle, pstSerial);
}

//�رմ���
int CControlInterface::Close(void* handle)
{
    return MV_IO_Close(handle);
}

//��ȡSDK�汾��
int CControlInterface::GetSDKVersion(MV_IO_VERSION* pstVersion)
{
    return MV_IO_GetSDKVersion(pstVersion);
}

//��ȡ�̼��汾��
int CControlInterface::GetFirmwareVersion(void* handle, MV_IO_VERSION* pstVersion)
{
    return MV_IO_GetFirmwareVersion(handle, pstVersion);
}

//�̼�����
int CControlInterface::LocalUpgrade(void* handle, unsigned char* pData, unsigned int nLen, MV_IO_LOCAL_UPGRADE* pstLocalUpgrade)
{
    return MV_IO_LocalUpgrade(handle, pData, nLen, pstLocalUpgrade);
}

//���ò���
int CControlInterface::ResetParam(void* handle)
{
    return MV_IO_ResetParam(handle);
}

//�����豸
int CControlInterface::Reboot(void* handle)
{
    return MV_IO_Reboot(handle);
}

//�������
int CControlInterface::SaveParam(void* handle)
{
    return MV_IO_SaveParam(handle);
}

//ע���ؼ��ص�
int CControlInterface::RegisterEdgeDetectionCallBack(void* handle, DetectFunType cbOutput, void* pUser)
{
    return MV_IO_RegisterEdgeDetectionCallBack(handle, cbOutput, pUser);
}

//��Dugview���ߵ��Խӿ�
void CControlInterface::SetDebugView(unsigned int nFlag)
{
    return MV_IO_SetDebugView(nFlag);
}

//����RS232/485/422
int CControlInterface::SetRSConfig(MV_COM_CONFIG* pstComConfig, MV_COM_DEVICE_TYPE oType)
{
    return MV_IO_SetRSConfig(pstComConfig, oType);
}


//IOģ��
//�����������
int CIOInterface::SetOutput(void* handle, MV_IO_SET_OUTPUT* pstOutput)
{
    return MV_IO_SetOutput(handle, pstOutput);
}

//�������ʹ��
int CIOInterface::SetOutputEnable(void* handle, MV_IO_OUTPUT_ENABLE* pstEnable)
{
    return MV_IO_SetOutputEnable(handle, pstEnable);
}

//��ȡPNPʹ��
int CIOInterface::GetPNPEnable(void* handle, MV_IO_PNP_TYPE* pnType)
{
    return MV_IO_GetPNPEnable(handle, pnType);
}


//����PNPʹ��
int CIOInterface::ExcutePNPEnable(void* handle, MV_IO_PNP_TYPE nType)
{
    return MV_IO_ExcutePNPEnable(handle, nType);
}

//�����������
int CIOInterface::SetInput(void* handle, MV_IO_SET_INPUT* pstInput)
{
    return MV_IO_SetInput(handle, pstInput);
}

//���������������
int CIOInterface::SetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam)
{
    return MV_IO_SetOutPortTriggerSource(handle, pstSourceParam);
}


//�����������˿�
int CIOInterface::AssociatedOutPort(void* handle, MV_IO_ASSOCIATEPORT_PARAM* pstAssocParam)
{
    return MV_IO_AssociatedOutPort(handle, pstAssocParam);
}

//��ȡ�����������
int CIOInterface::GetPortInputParam(void* handle, MV_IO_SET_INPUT* pstInput)
{
    return MV_IO_GetPortInputParam(handle, pstInput);
}

//��ȡ�����������
int CIOInterface::GetPortOutputParam(void* handle, MV_IO_SET_OUTPUT* pstInput)
{
    return MV_IO_GetPortOutputParam(handle, pstInput);
}

//��ȡ�����ƽ��Ϣ,���ؼ�⿪��ʱ������ʹ�øýӿڻ�ȡ��ƽ
int CIOInterface::GetInputLevel(void* handle, MV_IO_INPUT_LEVEL* pstInputLevel)
{
    return MV_IO_GetInputLevel(handle, pstInputLevel);
}

//��ȡ�����������
int CIOInterface::GetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam)
{
    return MV_IO_GetOutPortTriggerSource(handle, pstSourceParam);
}

int CIOInterface::SetMainIODeviceType(MV_MAINIO_DEVICE_TYPE enDeviceType)
{
    return MV_IO_SetMainIODeviceType(enDeviceType);
}

//��ʼ��WIN IO�ӿ�
int CIOInterface::WinIOInit()
{
    return MV_IO_WinIO_Init();
}

//ע��WIN IO�ӿ�
int CIOInterface::WinIODeInit()
{
    return MV_IO_WinIO_DeInit();
}

//��ʼ������IO
int CIOInterface::MainIOInit()
{
    return MV_IO_MainIOInit();
}

//��ȡ����IO�����ƽ,����ǰ��Ҫ�ȳ�ʼ��MV_IO_WinIO_Init
int CIOInterface::GetMainInputLevel(unsigned char* pStatus)
{
    return MV_IO_GetMainInputLevel(pStatus);
}

//��������IO�����ƽ������ǰ���ʼ��MV_IO_WinIO_Init
int CIOInterface::SetMainOutputLevel(MV_GIO_PORT_NUMBER nPort, MV_GIO_LEVEL nStatus)
{
    return MV_IO_SetMainOutputLevel(nPort, nStatus);
}

//��������IO PNPģʽ������ǰ���ʼ��MV_IO_WinIO_Init 
int CIOInterface::SetMainGPO_NPN(unsigned int nEnable)
{
    return MV_IO_SetMainGPO_NPN(nEnable);
}

//��Դģ��
//LIGHT(����)��������
int CLightInterface::SetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam)
{
    return MV_IO_SetLightParam(handle, pstLightParam);
}

//LIGHT(����)������ȡ
int CLightInterface::GetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam)
{
    return MV_IO_GetLightParam(handle, pstLightParam);
}

//���ù�Դ��������
int CLightInterface::SetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam)
{
    return MV_IO_SetLightTriggerSource(handle, pstSourceParam);
}

//��ȡ��Դ��������
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

//���������������
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

//��ȡ�����������
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