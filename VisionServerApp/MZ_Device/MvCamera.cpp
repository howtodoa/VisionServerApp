#include "pch.h"
#include "MvCamera.h"

void __stdcall ReconnectDevice(unsigned int nMsgType, void* pUser)
{
    if (nMsgType == MV_EXCEPTION_DEV_DISCONNECT)
    {
        CMvCamera* pThis = (CMvCamera*)pUser;

        if (pThis->m_bOpenDevice)
        {
            pThis->Close();

            BOOL bConnected = FALSE;
            while (1)
            {
                int nRet = pThis->Open();
                if (MV_OK == nRet)
                {
                    pThis->RegisterExceptionCallBack(ReconnectDevice, pUser);
                    bConnected = TRUE;
                    break;
                }
                else
                {
                    Sleep(100);
                }
                if (pThis->g_isMainThreadExit)
                    return;
            }

            if (bConnected && pThis->m_bStartGrabbing)
            {
                pThis->StartGrabbing();
            }
        }
    }
}


CMvCamera::CMvCamera(MV_CC_DEVICE_INFO* DevInfo)
{
    m_hDevHandle = MV_NULL;
    m_stDevInfo = DevInfo;
}

CMvCamera::~CMvCamera()
{
    if (m_hDevHandle)
    {
        MV_CC_DestroyHandle(m_hDevHandle);
        m_hDevHandle    = MV_NULL;
    }
    g_isMainThreadExit = true;
}

// ch:初始化SDK | en:Initialize SDK
int CMvCamera::InitSDK()
{
	return MV_CC_Initialize();
}

// ch:反初始化SDK | en:Finalize SDK
int CMvCamera::FinalizeSDK()
{
	return MV_CC_Finalize();
}

// ch:获取SDK版本号 | en:Get SDK Version
int CMvCamera::GetSDKVersion()
{
    return MV_CC_GetSDKVersion();
}

// ch:枚举设备 | en:Enumerate Device
int CMvCamera::EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList)
{
    return MV_CC_EnumDevices(nTLayerType, pstDevList);
}

// ch:判断设备是否可达 | en:Is the device accessible
bool CMvCamera::IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode)
{
    return MV_CC_IsDeviceAccessible(pstDevInfo, nAccessMode);
}

// ch:打开设备 | en:Open Device
int CMvCamera::Open()
{
    if (MV_NULL == m_stDevInfo)
    {
        return MV_E_PARAMETER;
    }

    if (m_hDevHandle)
    {
        return MV_E_CALLORDER;
    }

    int nRet  = MV_CC_CreateHandle(&m_hDevHandle, m_stDevInfo);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    nRet = MV_CC_OpenDevice(m_hDevHandle);
    if (MV_OK != nRet)
    {
        MV_CC_DestroyHandle(m_hDevHandle);
        m_hDevHandle = MV_NULL;
    }
    else
    {
        int nRet = RegisterExceptionCallBack(ReconnectDevice, this);
        m_bOpenDevice = true;
        GetDeviceInfo(m_stDevInfo);
    }
        
   
    return nRet;
}

// ch:关闭设备 | en:Close Device
int CMvCamera::Close()
{
    if (MV_NULL == m_hDevHandle)
    {
        return MV_E_HANDLE;
    }

    int nRet = MV_CC_CloseDevice(m_hDevHandle);
    if(nRet == MV_OK)
        m_bOpenDevice = true;

    nRet = MV_CC_DestroyHandle(m_hDevHandle);
    m_hDevHandle = MV_NULL;

    return nRet;
}

// ch:判断相机是否处于连接状态 | en:Is The Device Connected
bool CMvCamera::IsDeviceConnected()
{
    return MV_CC_IsDeviceConnected(m_hDevHandle);
}

// ch:注册图像数据回调 | en:Register Image Data CallBack
int CMvCamera::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
    m_procallbackfun = cbOutput;
    return MV_CC_RegisterImageCallBackEx(m_hDevHandle, m_procallbackfun, pUser);
}

// ch:开启抓图 | en:Start Grabbing
int CMvCamera::StartGrabbing()
{
    if (m_bStartGrabbing == true)
        return MV_OK;
    int ret =  MV_CC_StartGrabbing(m_hDevHandle);
    if(ret == MV_OK)
        m_bStartGrabbing = true;
    return ret;
}

// ch:停止抓图 | en:Stop Grabbing
int CMvCamera::StopGrabbing()
{
    if (m_bStartGrabbing == false)
        return false;
    int ret = MV_CC_StopGrabbing(m_hDevHandle);
    if (ret == MV_OK)
        m_bStartGrabbing = false;
    return ret;
}

// ch:主动获取一帧图像数据 | en:Get one frame initiatively
int CMvCamera::GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec)
{
    return MV_CC_GetImageBuffer(m_hDevHandle, pFrame, nMsec);
}

// ch:释放图像缓存 | en:Free image buffer
int CMvCamera::FreeImageBuffer(MV_FRAME_OUT* pFrame)
{
    return MV_CC_FreeImageBuffer(m_hDevHandle, pFrame);
}

// ch:设置SDK内部图像缓存节点个数 | en:Set the number of the internal image cache nodes in SDK
int CMvCamera::SetImageNodeNum(unsigned int nNum)
{
    return MV_CC_SetImageNodeNum(m_hDevHandle, nNum);
}

// ch:获取设备信息 | en:Get device information
int CMvCamera::GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo)
{
    return MV_CC_GetDeviceInfo(m_hDevHandle, pstDevInfo);
}

// ch:获取和设置Int型参数，如 Width和Height
// en:Get Int type parameters, such as Width and Height
int CMvCamera::GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX *pIntValue)
{
    return MV_CC_GetIntValueEx(m_hDevHandle, strKey, pIntValue);
}

int CMvCamera::SetIntValue(IN const char* strKey, IN int64_t nValue)
{
    return MV_CC_SetIntValueEx(m_hDevHandle, strKey, nValue);
}

// ch:获取和设置Enum型参数，如 PixelFormat
// en:Get Enum type parameters, such as PixelFormat
int CMvCamera::GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE *pEnumValue)
{
    return MV_CC_GetEnumValue(m_hDevHandle, strKey, pEnumValue);
}

int CMvCamera::SetEnumValue(IN const char* strKey, IN unsigned int nValue)
{
    return MV_CC_SetEnumValue(m_hDevHandle, strKey, nValue);
}

int CMvCamera::SetEnumValueByString(IN const char* strKey, IN const char* sValue)
{
    return MV_CC_SetEnumValueByString(m_hDevHandle, strKey, sValue);
}

int CMvCamera::GetEnumEntrySymbolic(IN const char* strKey, IN MVCC_ENUMENTRY* pstEnumEntry)
{
    return MV_CC_GetEnumEntrySymbolic(m_hDevHandle, strKey, pstEnumEntry);
}

// ch:获取和设置Float型参数，如 ExposureTime和Gain
// en:Get Float type parameters, such as ExposureTime and Gain
int CMvCamera::GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE *pFloatValue)
{
    return MV_CC_GetFloatValue(m_hDevHandle, strKey, pFloatValue);
}

int CMvCamera::SetFloatValue(IN const char* strKey, IN float fValue)
{
    return MV_CC_SetFloatValue(m_hDevHandle, strKey, fValue);
}

// ch:获取和设置Bool型参数，如 ReverseX
// en:Get Bool type parameters, such as ReverseX
int CMvCamera::GetBoolValue(IN const char* strKey, OUT bool *pbValue)
{
    return MV_CC_GetBoolValue(m_hDevHandle, strKey, pbValue);
}

int CMvCamera::SetBoolValue(IN const char* strKey, IN bool bValue)
{
    return MV_CC_SetBoolValue(m_hDevHandle, strKey, bValue);
}

// ch:获取和设置String型参数，如 DeviceUserID
// en:Get String type parameters, such as DeviceUserID
int CMvCamera::GetStringValue(IN const char* strKey, MVCC_STRINGVALUE *pStringValue)
{
    return MV_CC_GetStringValue(m_hDevHandle, strKey, pStringValue);
}

int CMvCamera::SetStringValue(IN const char* strKey, IN const char* strValue)
{
    return MV_CC_SetStringValue(m_hDevHandle, strKey, strValue);
}

// ch:执行一次Command型命令，如 UserSetSave
// en:Execute Command once, such as UserSetSave
int CMvCamera::CommandExecute(IN const char* strKey)
{
    return MV_CC_SetCommandValue(m_hDevHandle, strKey);
}

// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
int CMvCamera::GetOptimalPacketSize(unsigned int* pOptimalPacketSize)
{
    if (MV_NULL == pOptimalPacketSize)
    {
        return MV_E_PARAMETER;
    }

    int nRet = MV_CC_GetOptimalPacketSize(m_hDevHandle);
    if (nRet < MV_OK)
    {
        return nRet;
    }

    *pOptimalPacketSize = (unsigned int)nRet;

    return MV_OK;
}

// ch:注册消息异常回调 | en:Register Message Exception CallBack
int CMvCamera::RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),void* pUser)
{
    return MV_CC_RegisterExceptionCallBack(m_hDevHandle, cbException, pUser);
}

// ch:注册单个事件回调 | en:Register Event CallBack
int CMvCamera::RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO * pEventInfo, void* pUser), void* pUser)
{
    return MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);
}

// ch:强制IP | en:Force IP
int CMvCamera::ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay)
{
    return MV_GIGE_ForceIpEx(m_hDevHandle, nIP, nSubNetMask, nDefaultGateWay);
}

// ch:保存图片为文件 | en:Save the image as a file
int CMvCamera::SaveImageToFile(MV_CC_IMAGE* pstImage, MV_CC_SAVE_IMAGE_PARAM* pSaveImageParam, const char* pcImagePath)
{
	return MV_CC_SaveImageToFileEx2(m_hDevHandle, pstImage, pSaveImageParam, pcImagePath);
}

/*
*     MV_TRIGGER_MODE_OFF                 = 0,                        ///< \~chinese 关闭                         \~english Off
    MV_TRIGGER_MODE_ON                  = 1,                        ///< \~chinese 打开                         \~english ON
*/
int CMvCamera::SetTriggerMode(int m_nTriggerMode)
{
    return SetEnumValue("TriggerMode", m_nTriggerMode);
}
/*
    MV_TRIGGER_SOURCE_LINE0             = 0,                        ///< \~chinese Line0                        \~english Line0
    MV_TRIGGER_SOURCE_LINE1             = 1,                        ///< \~chinese Line1                        \~english Line1
    MV_TRIGGER_SOURCE_LINE2             = 2,                        ///< \~chinese Line2                        \~english Line2
    MV_TRIGGER_SOURCE_LINE3             = 3,                        ///< \~chinese Line3                        \~english Line3
    MV_TRIGGER_SOURCE_COUNTER0          = 4,                        ///< \~chinese Conuter0                     \~english Conuter0

    MV_TRIGGER_SOURCE_SOFTWARE          = 7,                        ///< \~chinese 软触发                       \~english Software
    MV_TRIGGER_SOURCE_FrequencyConverter= 8,                        ///< \~chinese 变频器                       \~english Frequency Converter
*/
int CMvCamera::SetTriggerSource(int m_nTriggerSource)
{
    return SetEnumValue("TriggerSource", m_nTriggerSource);
}

int CMvCamera::SoftTriggerOnce()
{
    return CommandExecute("TriggerSoftware");
}

int CMvCamera::SaveFeature(char *fileName)
{
    return MV_CC_FeatureSave(m_hDevHandle, fileName);
}

int CMvCamera::LoadFeature(char* fileName)
{
    return MV_CC_FeatureLoad(m_hDevHandle, fileName);
}

std::string CMvCamera::GetDeviceSN()
{
    //std::string tempValue = m_stDevInfo.SpecialInfo.stGigEInfo.chSerialNumber;
    std::string str(m_stDevInfo->SpecialInfo.stGigEInfo.chSerialNumber, m_stDevInfo->SpecialInfo.stGigEInfo.chSerialNumber + sizeof(m_stDevInfo->SpecialInfo.stGigEInfo.chSerialNumber));
    return str;
}
#if 0
std::string CMvCamera::GetDeviceSN(MV_CC_DEVICE_INFO* Input)
{
    std::string str(Input->SpecialInfo.stGigEInfo.chSerialNumber, Input->SpecialInfo.stGigEInfo.chSerialNumber + sizeof(Input->SpecialInfo.stGigEInfo.chSerialNumber));
    return str;

}
#endif
std::string CMvCamera::GetDeviceIp()
{
    int nIp1 = ((m_stDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
    int nIp2 = ((m_stDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
    int nIp3 = ((m_stDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
    int nIp4 = (m_stDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

    std::ostringstream oss;
    oss << nIp1 << "." << nIp2 << "." << nIp3 << "." << nIp4;

    std::string result = oss.str();
    return result;
}

std::string CMvCamera::GetDeviceIp(MV_CC_DEVICE_INFO* Input)
{
    int nIp1 = ((Input->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
    int nIp2 = ((Input->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
    int nIp3 = ((Input->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
    int nIp4 = (Input->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

    std::ostringstream oss;
    oss << nIp1 << "." << nIp2 << "." << nIp3 << "." << nIp4;

    std::string result = oss.str();
    return result;
}

void CMvCamera::SetMainThreadExit(bool input)
{
    g_isMainThreadExit = input;
}