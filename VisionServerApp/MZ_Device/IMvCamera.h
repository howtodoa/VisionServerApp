#pragma once
#include "IMVApi.h"
#include <string>
//ȱ�ݰ��ص�����
class IMvCamera
{
    friend static void onDeviceLinkNotify(const IMV_SConnectArg* pConnectArg, void* pUser);
public:
    IMvCamera(IMV_DeviceInfo* pstDeviceInfo);
    ~IMvCamera();
#if 0
    // ch:��ʼ��SDK | en:Initialize SDK
    static int InitSDK();

    // ch:����ʼ��SDK | en:Finalize SDK
    static int FinalizeSDK();

    // ch:��ȡSDK�汾�� | en:Get SDK Version
    static const char* GetSDKVersion();



    // ch:�ж��豸�Ƿ�ɴ� | en:Is the device accessible
    static bool IsDeviceAccessible(IMV_DeviceInfo* pstDevInfo, unsigned int nAccessMode);
#endif
    // ch:ö���豸 | en:Enumerate Device
    static int EnumDevices(unsigned int nTLayerType, IMV_DeviceList* pstDevList);

    // ch:���豸 | en:Open Device
    int Open();

    // ch:�ر��豸 | en:Close Device
    int Close();

    // ch:�ж�����Ƿ�������״̬ | en:Is The Device Connected
    bool IsDeviceConnected();

    // ch:ע��ͼ�����ݻص� | en:Register Image Data CallBack
    int RegisterImageCallBack(IMV_FrameCallBack pFrameInfo, void* pUser);

    // ch:����ץͼ | en:Start Grabbing
    int StartGrabbing();

    // ch:ֹͣץͼ | en:Stop Grabbing
    int StopGrabbing();

    // ch:������ȡһ֡ͼ������ | en:Get one frame initiatively
    int GetImageBuffer(IMV_Frame* pFrame, int nMsec);

    // ch:�ͷ�ͼ�񻺴� | en:Free image buffer
    int FreeImageBuffer(IMV_Frame* pFrame);

    // ch:����SDK�ڲ�ͼ�񻺴�ڵ���� | en:Set the number of the internal image cache nodes in SDK
    int SetImageNodeNum(unsigned int nNum);

    // ch:��ȡ�豸��Ϣ | en:Get device information
    int GetDeviceInfo(IMV_DeviceInfo* pstDevInfo);

    // ch:��ȡ������Int�Ͳ������� Width��Height
    // en:Get Int type parameters, such as Width and Height
    int GetIntValue(IN const char* strKey, OUT int64_t* nValue);
    int SetIntValue(IN const char* strKey, IN int64_t nValue);

    // ch:��ȡ������Enum�Ͳ������� PixelFormat
    // en:Get Enum type parameters, such as PixelFormat
    int GetEnumValue(IN const char* strKey, OUT uint64_t* pEnumValue);
    int SetEnumValue(IN const char* strKey, IN unsigned int nValue);
    int SetEnumValueByString(IN const char* strKey, IN const char* sValue);
    int GetEnumEntrySymbolic(IN const char* strKey, IN IMV_String* pstEnumEntry);

    // ch:��ȡ������Float�Ͳ������� ExposureTime��Gain
    // en:Get Float type parameters, such as ExposureTime and Gain
    int GetFloatValue(IN const char* strKey, OUT double* pFloatValue);
    int SetFloatValue(IN const char* strKey, IN float fValue);

    // ch:��ȡ������Bool�Ͳ������� ReverseX
    // en:Get Bool type parameters, such as ReverseX
    int GetBoolValue(IN const char* strKey, OUT bool* pbValue);
    int SetBoolValue(IN const char* strKey, IN bool bValue);

    // ch:��ȡ������String�Ͳ������� DeviceUserID
    // en:Get String type parameters, such as DeviceUserID
    int GetStringValue(IN const char* strKey, OUT IMV_String* pStringValue);
    int SetStringValue(IN const char* strKey, IN const char* strValue);

    // ch:ִ��һ��Command������� UserSetSave
    // en:Execute Command once, such as UserSetSave
    int CommandExecute(IN const char* strKey);

    // ch:ǿ��IP | en:Force IP
    int ForceIp(const char* nIP, const char* nSubNetMask, const char* nDefaultGateWay);
    // ch:����ͼƬΪ�ļ� | en:Save the image as a file
    int SaveImageToFile(IMV_SaveImageToFileParam* pSaveImageParam, const char* pcImagePath);

    int SetTriggerMode(int m_nTriggerMode);

    int SetTriggerSource(int m_nTriggerSource);

    int SoftTriggerOnce();

    void deviceOffLine();
    void deviceOnLine();
    int SaveFeature(char* fileName);
    int LoadFeature(char* fileName);

    std::string GetDeviceSN();
    std::string GetDeviceIp();
   static std::string GetDeviceIp(IMV_DeviceInfo* Input);
    void SetMainThreadExit(bool input);
private:
    void* m_hDevHandle;
    bool  g_isMainThreadExit = false;
    IMV_FrameCallBack m_procallbackfun = NULL;
    IMV_DeviceInfo* m_devInfo;
};

