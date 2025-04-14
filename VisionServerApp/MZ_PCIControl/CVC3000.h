#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "MvIOInterfaceBox.h"
#include "MvIOInterfaceBoxDefine.h"
#include "MvErrorDefine.h"

using namespace std;

//����ģ��
class CControlInterface
{
public:
    CControlInterface()
    {
    }
    ~CControlInterface()
    {
    }

    int CreateHandle(void** handle); //������� && ���ɴ���32�����ھ��
    int DestroyHandle(void* handle); //���پ��

    int Open(void* handle, MV_IO_SERIAL* pstSerial); //�򿪴���
    int Close(void* handle); //�رմ���

    int GetSDKVersion(MV_IO_VERSION* pstVersion); //��ȡSDK�汾��
    int GetFirmwareVersion(void* handle, MV_IO_VERSION* pstVersion); //��ȡ�̼��汾��

    int LocalUpgrade(void* handle, unsigned char* pData, unsigned int nLen, MV_IO_LOCAL_UPGRADE* pstLocalUpgrade); //�̼�����
    int ResetParam(void* handle); //���ò���
    int Reboot(void* handle); //�����豸
    int SaveParam(void* handle); //�������
    int RegisterEdgeDetectionCallBack(void* handle, DetectFunType cbOutput, void* pUser); //ע���ؼ��ص�

    void SetDebugView(unsigned int nFlag); //��Dugview���ߵ��Խӿ�
    int SetRSConfig(MV_COM_CONFIG* pstComConfig, MV_COM_DEVICE_TYPE oType); //����RS232/485/422
};

//IOģ��
class CIOInterface
{
public:
    CIOInterface()
    {
    }
    ~CIOInterface()
    {
    }

    //����IO����
    int SetOutput(void* handle, MV_IO_SET_OUTPUT* pstOutput); //�����������
    int SetOutputEnable(void* handle, MV_IO_OUTPUT_ENABLE* pstEnable); //�������ʹ��
    int GetPNPEnable(void* handle, MV_IO_PNP_TYPE* pnType); //��ȡPNPʹ��
    int ExcutePNPEnable(void* handle, MV_IO_PNP_TYPE nType); //����PNPʹ��
    int SetInput(void* handle, MV_IO_SET_INPUT* pstInput); //�����������
    int AssociatedOutPort(void* handle, MV_IO_ASSOCIATEPORT_PARAM* pstAssocParam); //�����������˿�
    int SetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam); //���������������

    //��ȡIO����
    int GetPortInputParam(void* handle, MV_IO_SET_INPUT* pstInput); //��ȡ�����������
    int GetPortOutputParam(void* handle, MV_IO_SET_OUTPUT* pstInput); //��ȡ�����������
    int GetInputLevel(void* handle, MV_IO_INPUT_LEVEL* pstInputLevel); //��ȡ�����ƽ��Ϣ,���ؼ�⿪��ʱ������ʹ�øýӿڻ�ȡ��ƽ
    int GetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam);//��ȡ�����������

    //��ȡ����GPIO
    int SetMainIODeviceType(MV_MAINIO_DEVICE_TYPE enDeviceType); //���������豸���ͣ�Ĭ��ΪVC3000
    int WinIOInit(); //��ʼ��WIN IO�ӿ�
    int WinIODeInit(); //ע��WIN IO�ӿ�
    int MainIOInit(); //��ʼ������IO

    int GetMainInputLevel(unsigned char* pStatus); //��ȡ����IO�����ƽ,����ǰ��Ҫ�ȳ�ʼ��MV_IO_WinIO_Init

    int SetMainOutputLevel(MV_GIO_PORT_NUMBER nPort, MV_GIO_LEVEL nStatus); //��������IO�����ƽ������ǰ���ʼ��MV_IO_WinIO_Init
    int SetMainGPO_NPN(unsigned int nEnable); //��������IO PNPģʽ������ǰ���ʼ��MV_IO_WinIO_Init 
};

//��Դģ��
class CLightInterface
{
public:
    CLightInterface()
    {
    }
    ~CLightInterface()
    {
    }

    int SetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam); //LIGHT(����)��������
    int GetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam); //LIGHT(����)������ȡ

    int SetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);//���ù�Դ��������
    int GetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);//��ȡ��Դ��������
};


class CIOControlSDK
{
public:
    CIOControlSDK();
    ~CIOControlSDK();
    int Init();

    //�������
    int CreateHandle(void** handle);
    int DestroyHandle(void* handle);

    //��ȡ�汾��
    int GetSDKVersion(MV_IO_VERSION* pstVersion);
    int GetFirmwareVersion(void* handle, MV_IO_VERSION* pstVersion);

    //��/�رմ���
    int Open(void* handle, MV_IO_SERIAL* pstSerial);
    int Close(void* handle);

    //
    int LocalUpgrade(void* handle, unsigned char* pData, unsigned int nLen, MV_IO_LOCAL_UPGRADE* pstLocalUpgrade);
    int ResetParam(void* handle);
    int Reboot(void* handle);
    int SaveParam(void* handle);
    int RegisterEdgeDetectionCallBack(void* handle, DetectFunType cbOutput, void* pUser);

    void SetDebugView(unsigned int nFlag);
    int SetRSConfig(MV_COM_CONFIG* pstComConfig, MV_COM_DEVICE_TYPE oType);

    //������������ӿ�
    int SetOutput(void* handle, MV_IO_SET_OUTPUT* pstOutput);
    int SetOutputEnable(void* handle, MV_IO_OUTPUT_ENABLE* pstEnable);
    int GetPNPEnable(void* handle, MV_IO_PNP_TYPE* pnType);
    int ExcutePNPEnable(void* handle, MV_IO_PNP_TYPE nType);
    int SetInput(void* handle, MV_IO_SET_INPUT* pstInput);
    int AssociatedOutPort(void* handle, MV_IO_ASSOCIATEPORT_PARAM* pstAssocParam);
    int SetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam);

    int GetPortInputParam(void* handle, MV_IO_SET_INPUT* pstInput);
    int GetPortOutputParam(void* handle, MV_IO_SET_OUTPUT* pstInput);
    int GetInputLevel(void* handle, MV_IO_INPUT_LEVEL* pstInputLevel);
    int GetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam);

    //VC3000 VT3000 GPIO�ӿ�
    int SetMainIODeviceType(MV_MAINIO_DEVICE_TYPE enDeviceType);
    int WinIOInit();
    int WinIODeInit();
    int MainIOInit();

    int GetMainInputLevel(unsigned char* pStatus);
    int SetMainOutputLevel(MV_GIO_PORT_NUMBER nPort, MV_GIO_LEVEL nStatus);
    int SetMainGPO_NPN(unsigned int nEnable);

    //���ù�Դ�ӿ�
    int SetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam);
    int GetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam);
    int SetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);
    int GetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);
private:
    CIOInterface* m_pcIOModule;           //IOģ��ʵ��
    CLightInterface* m_pcLightModule;     //��Դģ��ʵ��
    CControlInterface* m_pcControlModule; //����ģ��ʵ��
};