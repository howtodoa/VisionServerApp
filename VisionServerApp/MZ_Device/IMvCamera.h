#pragma once
#include "IMVApi.h"
#include <string>
//缺陷包回调函数
class IMvCamera
{
    friend static void onDeviceLinkNotify(const IMV_SConnectArg* pConnectArg, void* pUser);
public:
    IMvCamera(IMV_DeviceInfo* pstDeviceInfo);
    ~IMvCamera();
#if 0
    // ch:初始化SDK | en:Initialize SDK
    static int InitSDK();

    // ch:反初始化SDK | en:Finalize SDK
    static int FinalizeSDK();

    // ch:获取SDK版本号 | en:Get SDK Version
    static const char* GetSDKVersion();



    // ch:判断设备是否可达 | en:Is the device accessible
    static bool IsDeviceAccessible(IMV_DeviceInfo* pstDevInfo, unsigned int nAccessMode);
#endif
    // ch:枚举设备 | en:Enumerate Device
    static int EnumDevices(unsigned int nTLayerType, IMV_DeviceList* pstDevList);

    // ch:打开设备 | en:Open Device
    int Open();

    // ch:关闭设备 | en:Close Device
    int Close();

    // ch:判断相机是否处于连接状态 | en:Is The Device Connected
    bool IsDeviceConnected();

    // ch:注册图像数据回调 | en:Register Image Data CallBack
    int RegisterImageCallBack(IMV_FrameCallBack pFrameInfo, void* pUser);

    // ch:开启抓图 | en:Start Grabbing
    int StartGrabbing();

    // ch:停止抓图 | en:Stop Grabbing
    int StopGrabbing();

    // ch:主动获取一帧图像数据 | en:Get one frame initiatively
    int GetImageBuffer(IMV_Frame* pFrame, int nMsec);

    // ch:释放图像缓存 | en:Free image buffer
    int FreeImageBuffer(IMV_Frame* pFrame);

    // ch:设置SDK内部图像缓存节点个数 | en:Set the number of the internal image cache nodes in SDK
    int SetImageNodeNum(unsigned int nNum);

    // ch:获取设备信息 | en:Get device information
    int GetDeviceInfo(IMV_DeviceInfo* pstDevInfo);

    // ch:获取和设置Int型参数，如 Width和Height
    // en:Get Int type parameters, such as Width and Height
    int GetIntValue(IN const char* strKey, OUT int64_t* nValue);
    int SetIntValue(IN const char* strKey, IN int64_t nValue);

    // ch:获取和设置Enum型参数，如 PixelFormat
    // en:Get Enum type parameters, such as PixelFormat
    int GetEnumValue(IN const char* strKey, OUT uint64_t* pEnumValue);
    int SetEnumValue(IN const char* strKey, IN unsigned int nValue);
    int SetEnumValueByString(IN const char* strKey, IN const char* sValue);
    int GetEnumEntrySymbolic(IN const char* strKey, IN IMV_String* pstEnumEntry);

    // ch:获取和设置Float型参数，如 ExposureTime和Gain
    // en:Get Float type parameters, such as ExposureTime and Gain
    int GetFloatValue(IN const char* strKey, OUT double* pFloatValue);
    int SetFloatValue(IN const char* strKey, IN float fValue);

    // ch:获取和设置Bool型参数，如 ReverseX
    // en:Get Bool type parameters, such as ReverseX
    int GetBoolValue(IN const char* strKey, OUT bool* pbValue);
    int SetBoolValue(IN const char* strKey, IN bool bValue);

    // ch:获取和设置String型参数，如 DeviceUserID
    // en:Get String type parameters, such as DeviceUserID
    int GetStringValue(IN const char* strKey, OUT IMV_String* pStringValue);
    int SetStringValue(IN const char* strKey, IN const char* strValue);

    // ch:执行一次Command型命令，如 UserSetSave
    // en:Execute Command once, such as UserSetSave
    int CommandExecute(IN const char* strKey);

    // ch:强制IP | en:Force IP
    int ForceIp(const char* nIP, const char* nSubNetMask, const char* nDefaultGateWay);
    // ch:保存图片为文件 | en:Save the image as a file
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

