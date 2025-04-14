#pragma once
#include "GxIAPI.h"
#include <iostream>

using namespace std;
struct  DEVICE_BASE_INFO_LIST
{
    GX_DEVICE_BASE_INFO* m_pBaseInfo;
    int m_nDeviceNum;
};


class GlxCamera
{
public:
    GlxCamera();
    ~GlxCamera();

    // ch:��ʼ��SDK | en:Initialize SDK
    static int InitSDK();

    // ch:����ʼ��SDK | en:Finalize SDK
    static int FinalizeSDK();

    // ch:��ȡSDK�汾�� | en:Get SDK Version
    static int GetSDKVersion();

    // ch:ö���豸 | en:Enumerate Device
    static int EnumDevices(unsigned int nTLayerType, DEVICE_BASE_INFO_LIST* pstDevList);

    // ch:�ж��豸�Ƿ�ɴ� | en:Is the device accessible
    static bool IsDeviceAccessible(GX_DEVICE_BASE_INFO* pstDevInfo, unsigned int nAccessMode);

    // ch:���豸 | en:Open Device
    int Open(GX_DEVICE_BASE_INFO* pstDeviceInfo);

    // ch:�ر��豸 | en:Close Device
    int Close();

    // ch:�ж�����Ƿ�������״̬ | en:Is The Device Connected
    bool IsDeviceConnected();

    // ch:ע��ͼ�����ݻص� | en:Register Image Data CallBack
    int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);

    // ch:����ץͼ | en:Start Grabbing
    int StartGrabbing();

    // ch:ֹͣץͼ | en:Stop Grabbing
    int StopGrabbing();

    // ch:������ȡһ֡ͼ������ | en:Get one frame initiatively
    int GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec);

    // ch:�ͷ�ͼ�񻺴� | en:Free image buffer
    int FreeImageBuffer(MV_FRAME_OUT* pFrame);

    // ch:��ʾһ֡ͼ�� | en:Display one frame image
    int DisplayOneFrame(void* hDisplay, MV_CC_IMAGE* pImageInfo);

    // ch:����SDK�ڲ�ͼ�񻺴�ڵ���� | en:Set the number of the internal image cache nodes in SDK
    int SetImageNodeNum(unsigned int nNum);

    // ch:��ȡ�豸��Ϣ | en:Get device information
    int GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo);

    // ch:��ȡGEV�����ͳ����Ϣ | en:Get detect info of GEV camera
    int GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect);

    // ch:��ȡU3V�����ͳ����Ϣ | en:Get detect info of U3V camera
    int GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect);

    // ch:��ȡ������Int�Ͳ������� Width��Height
    // en:Get Int type parameters, such as Width and Height
    int GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX* pIntValue);
    int SetIntValue(IN const char* strKey, IN int64_t nValue);

    // ch:��ȡ������Enum�Ͳ������� PixelFormat
    // en:Get Enum type parameters, such as PixelFormat
    int GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE* pEnumValue);
    int SetEnumValue(IN const char* strKey, IN unsigned int nValue);
    int SetEnumValueByString(IN const char* strKey, IN const char* sValue);
    int GetEnumEntrySymbolic(IN const char* strKey, IN MVCC_ENUMENTRY* pstEnumEntry);

    // ch:��ȡ������Float�Ͳ������� ExposureTime��Gain
    // en:Get Float type parameters, such as ExposureTime and Gain
    int GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE* pFloatValue);
    int SetFloatValue(IN const char* strKey, IN float fValue);

    // ch:��ȡ������Bool�Ͳ������� ReverseX
    // en:Get Bool type parameters, such as ReverseX
    int GetBoolValue(IN const char* strKey, OUT bool* pbValue);
    int SetBoolValue(IN const char* strKey, IN bool bValue);

    // ch:��ȡ������String�Ͳ������� DeviceUserID
    // en:Get String type parameters, such as DeviceUserID
    int GetStringValue(IN const char* strKey, MVCC_STRINGVALUE* pStringValue);
    int SetStringValue(IN const char* strKey, IN const char* strValue);

    // ch:ִ��һ��Command������� UserSetSave
    // en:Execute Command once, such as UserSetSave
    int CommandExecute(IN const char* strKey);

    // ch:̽��������Ѱ���С(ֻ��GigE�����Ч) | en:Detection network optimal package size(It only works for the GigE camera)
    int GetOptimalPacketSize(unsigned int* pOptimalPacketSize);

    // ch:ע����Ϣ�쳣�ص� | en:Register Message Exception CallBack
    int RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser);

    // ch:ע�ᵥ���¼��ص� | en:Register Event CallBack
    int RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO* pEventInfo, void* pUser), void* pUser);

    // ch:ǿ��IP | en:Force IP
    int ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay);

    // ch:����IP��ʽ | en:IP configuration method
    int SetIpConfig(unsigned int nType);

    // ch:�������紫��ģʽ | en:Set Net Transfer Mode
    int SetNetTransMode(unsigned int nType);

    // ch:���ظ�ʽת�� | en:Pixel format conversion
    int ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM_EX* pstCvtParam);

    // ch:����ͼƬ | en:save image
    int SaveImage(MV_SAVE_IMAGE_PARAM_EX3* pstParam);

    // ch:����ͼƬΪ�ļ� | en:Save the image as a file
    int SaveImageToFile(MV_CC_IMAGE* pstImage, MV_CC_SAVE_IMAGE_PARAM* pSaveImageParam, const char* pcImagePath);

    // ch:����Բ�θ����� | en:Draw circle auxiliary line
    int DrawCircle(MVCC_CIRCLE_INFO* pCircleInfo);

    // ch:�������θ����� | en:Draw lines auxiliary line
    int DrawLines(MVCC_LINES_INFO* pLinesInfo);

private:

    void* m_hDevHandle;


};

