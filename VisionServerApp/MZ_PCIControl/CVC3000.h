#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "MvIOInterfaceBox.h"
#include "MvIOInterfaceBoxDefine.h"
#include "MvErrorDefine.h"

using namespace std;

//控制模块
class CControlInterface
{
public:
    CControlInterface()
    {
    }
    ~CControlInterface()
    {
    }

    int CreateHandle(void** handle); //创建句柄 && 最多可创建32个串口句柄
    int DestroyHandle(void* handle); //销毁句柄

    int Open(void* handle, MV_IO_SERIAL* pstSerial); //打开串口
    int Close(void* handle); //关闭串口

    int GetSDKVersion(MV_IO_VERSION* pstVersion); //获取SDK版本号
    int GetFirmwareVersion(void* handle, MV_IO_VERSION* pstVersion); //获取固件版本号

    int LocalUpgrade(void* handle, unsigned char* pData, unsigned int nLen, MV_IO_LOCAL_UPGRADE* pstLocalUpgrade); //固件升级
    int ResetParam(void* handle); //重置参数
    int Reboot(void* handle); //重启设备
    int SaveParam(void* handle); //保存参数
    int RegisterEdgeDetectionCallBack(void* handle, DetectFunType cbOutput, void* pUser); //注册沿检测回调

    void SetDebugView(unsigned int nFlag); //打开Dugview工具调试接口
    int SetRSConfig(MV_COM_CONFIG* pstComConfig, MV_COM_DEVICE_TYPE oType); //设置RS232/485/422
};

//IO模块
class CIOInterface
{
public:
    CIOInterface()
    {
    }
    ~CIOInterface()
    {
    }

    //设置IO参数
    int SetOutput(void* handle, MV_IO_SET_OUTPUT* pstOutput); //输出参数设置
    int SetOutputEnable(void* handle, MV_IO_OUTPUT_ENABLE* pstEnable); //开启输出使能
    int GetPNPEnable(void* handle, MV_IO_PNP_TYPE* pnType); //获取PNP使能
    int ExcutePNPEnable(void* handle, MV_IO_PNP_TYPE nType); //开启PNP使能
    int SetInput(void* handle, MV_IO_SET_INPUT* pstInput); //输入参数设置
    int AssociatedOutPort(void* handle, MV_IO_ASSOCIATEPORT_PARAM* pstAssocParam); //输入关联输出端口
    int SetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam); //设置输出触发输入

    //获取IO参数
    int GetPortInputParam(void* handle, MV_IO_SET_INPUT* pstInput); //获取输入参数设置
    int GetPortOutputParam(void* handle, MV_IO_SET_OUTPUT* pstInput); //获取输出参数设置
    int GetInputLevel(void* handle, MV_IO_INPUT_LEVEL* pstInputLevel); //获取输入电平信息,在沿检测开启时，不能使用该接口获取电平
    int GetOutPortTriggerSource(void* handle, MV_IO_OUTPUTSOURCE_PARAM* pstSourceParam);//获取输出触发输入

    //获取主板GPIO
    int SetMainIODeviceType(MV_MAINIO_DEVICE_TYPE enDeviceType); //设置主板设备类型，默认为VC3000
    int WinIOInit(); //初始化WIN IO接口
    int WinIODeInit(); //注销WIN IO接口
    int MainIOInit(); //初始化主板IO

    int GetMainInputLevel(unsigned char* pStatus); //获取主板IO输入电平,调用前需要先初始化MV_IO_WinIO_Init

    int SetMainOutputLevel(MV_GIO_PORT_NUMBER nPort, MV_GIO_LEVEL nStatus); //设置主板IO输出电平，调用前需初始化MV_IO_WinIO_Init
    int SetMainGPO_NPN(unsigned int nEnable); //设置主板IO PNP模式，调用前需初始化MV_IO_WinIO_Init 
};

//光源模块
class CLightInterface
{
public:
    CLightInterface()
    {
    }
    ~CLightInterface()
    {
    }

    int SetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam); //LIGHT(亮度)参数设置
    int GetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam); //LIGHT(亮度)参数获取

    int SetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);//设置光源触发输入
    int GetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);//获取光源触发输入
};


class CIOControlSDK
{
public:
    CIOControlSDK();
    ~CIOControlSDK();
    int Init();

    //句柄操作
    int CreateHandle(void** handle);
    int DestroyHandle(void* handle);

    //获取版本号
    int GetSDKVersion(MV_IO_VERSION* pstVersion);
    int GetFirmwareVersion(void* handle, MV_IO_VERSION* pstVersion);

    //打开/关闭串口
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

    //设置输入输出接口
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

    //VC3000 VT3000 GPIO接口
    int SetMainIODeviceType(MV_MAINIO_DEVICE_TYPE enDeviceType);
    int WinIOInit();
    int WinIODeInit();
    int MainIOInit();

    int GetMainInputLevel(unsigned char* pStatus);
    int SetMainOutputLevel(MV_GIO_PORT_NUMBER nPort, MV_GIO_LEVEL nStatus);
    int SetMainGPO_NPN(unsigned int nEnable);

    //设置光源接口
    int SetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam);
    int GetLightParam(void* handle, MV_IO_LIGHT_PARAM* pstLightParam);
    int SetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);
    int GetLightTriggerSource(void* handle, MV_IO_LIGHTSOURCE_PARAM* pstSourceParam);
private:
    CIOInterface* m_pcIOModule;           //IO模块实例
    CLightInterface* m_pcLightModule;     //光源模块实例
    CControlInterface* m_pcControlModule; //控制模块实例
};