#include "pch.h"
#include "IMvCamera.h"



// 断线通知处理
static void onDeviceLinkNotify(const IMV_SConnectArg* pConnectArg, void* pUser)
{
    IMvCamera* pCMvCamera = (IMvCamera*)pUser;
    if (!pCMvCamera)
    {
        //TRACE("pCMvCamera is NULL!\n");
        return;
    }

    if (!pConnectArg)
    {
        //TRACE("pConnectArg is NULL!\n");
        return;
    }

    int ret = IMV_OK;
    
    if (NULL == pCMvCamera)
    {
        //printf("devHandle is NULL!");
        return;
    }

    memset(&pCMvCamera->m_devInfo, 0, sizeof(pCMvCamera->m_devInfo));
    ret = pCMvCamera->GetDeviceInfo(pCMvCamera->m_devInfo);
    if (IMV_OK != ret)
    {
        //printf("Get device info failed! ErrorCode[%d]\n", ret);
        return;
    }

    // 断线通知
    // offLine notify 
    if (offLine == pConnectArg->event)
    {
        //printf("------cameraKey[%s] : OffLine------\n", devInfo.cameraKey);
        pCMvCamera->deviceOffLine();
    }
    // 上线通知
    // onLine notify 
    else if (onLine == pConnectArg->event)
    {
       // printf("------cameraKey[%s] : OnLine------\n", devInfo.cameraKey);
        pCMvCamera->deviceOnLine();
    }
}


IMvCamera::IMvCamera(IMV_DeviceInfo* pstDeviceInfo)
{
	m_hDevHandle = NULL;
    m_devInfo = pstDeviceInfo;
}

IMvCamera::~IMvCamera()
{
    if (m_hDevHandle)
    {
        IMV_DestroyHandle(m_hDevHandle);
        m_hDevHandle = NULL;
    }
    g_isMainThreadExit = true;
}
#if 0
int IMvCamera::InitSDK()
{
    return 1;
}

int IMvCamera::FinalizeSDK()
{
    return 1;
}

const char* IMvCamera::GetSDKVersion()
{
    /*
    *@~chinese
        * @brief  获取SDK版本号
        * @return 返回4字节版本号
        | 主 | 次 | 修正 | 测试 |
        8bits  8bits  8bits  8bits
        * @remarks 比如返回值为0x01000001，即SDK版本号为V1.0.0.1。
        * 
        */
    return IMV_GetVersion();
}



bool IMvCamera::IsDeviceAccessible(IMV_DeviceInfo* pstDevInfo, unsigned int nAccessMode)
{
    return 1;
}
#endif

int IMvCamera::EnumDevices(unsigned int nTLayerType, IMV_DeviceList* pstDevList)
{
    return IMV_EnumDevices(pstDevList, nTLayerType);
}

int IMvCamera::Open()
{
    if (NULL == m_devInfo)
    {
        return IMV_ERROR;
    }

    if (m_hDevHandle)
    {
        return IMV_ERROR;
    }

    int nRet = IMV_CreateHandle(&m_hDevHandle, modeByCameraKey, m_devInfo->cameraKey);
    if (IMV_OK != nRet)
    {
        return nRet;
    }

    nRet = IMV_Open(m_hDevHandle);
    if (IMV_OK != nRet)
    {
        IMV_DestroyHandle(m_hDevHandle);
        m_hDevHandle = NULL;
    }
    else
    {
        // 设备连接状态事件回调函数
        // Device connection status event callback function
        nRet = IMV_SubscribeConnectArg(m_hDevHandle, onDeviceLinkNotify, this);
        if (IMV_OK == nRet)
        {
            //printf("SubscribeConnectArg failed! ErrorCode[%d]\n", ret);
            //break;
            GetDeviceInfo(m_devInfo);
        }
       
    }
    return nRet;
}

int IMvCamera::Close()
{
    if (NULL == m_hDevHandle)
    {
        return IMV_ERROR;
    }

    IMV_Close(m_hDevHandle);

    int nRet = IMV_DestroyHandle(m_hDevHandle);
    m_hDevHandle = NULL;

    return nRet;
}

bool IMvCamera::IsDeviceConnected()
{
    return IMV_IsOpen(m_hDevHandle);
}

int IMvCamera::RegisterImageCallBack(IMV_FrameCallBack pFrameInfo, void* pUser)
{
    m_procallbackfun = pFrameInfo;
    return IMV_AttachGrabbing(m_hDevHandle, pFrameInfo, pUser);
}

int IMvCamera::StartGrabbing()
{
    return IMV_StartGrabbing(m_hDevHandle);
}

int IMvCamera::StopGrabbing()
{
    return IMV_StopGrabbing(m_hDevHandle);
}

int IMvCamera::GetImageBuffer(IMV_Frame * pFrame, int nMsec)
{
    return IMV_GetFrame(m_hDevHandle, pFrame, nMsec);
}

int IMvCamera::FreeImageBuffer(IMV_Frame* pFrame)
{
    return IMV_ReleaseFrame(m_hDevHandle, pFrame);
}

int IMvCamera::SetImageNodeNum(unsigned int nNum)
{
    return IMV_SetBufferCount(m_hDevHandle, nNum);
}

int IMvCamera::GetDeviceInfo(IMV_DeviceInfo* pstDevInfo)
{
    return IMV_GetDeviceInfo(m_hDevHandle, pstDevInfo);
}

int IMvCamera::GetIntValue(IN const char* strKey, OUT int64_t* nValue)
{
    return IMV_GetIntFeatureValue(m_hDevHandle, strKey, nValue);
}

int IMvCamera::SetIntValue(IN const char* strKey, IN int64_t nValue)
{
    return IMV_SetIntFeatureValue(m_hDevHandle, strKey, nValue);
}


int IMvCamera::GetEnumValue(IN const char* strKey, OUT uint64_t* pEnumValue)
{
    return IMV_GetEnumFeatureValue(m_hDevHandle, strKey, pEnumValue);
}

int IMvCamera::SetEnumValue(IN const char* strKey, IN unsigned int nValue)
{
    return IMV_SetEnumFeatureValue(m_hDevHandle, strKey, nValue);
}

int IMvCamera::SetEnumValueByString(IN const char* strKey, IN const char* sValue)
{
    return IMV_SetEnumFeatureSymbol(m_hDevHandle, strKey, sValue);
}

int IMvCamera::GetEnumEntrySymbolic(IN const char* strKey, IN IMV_String* pstEnumEntry)
{
    return IMV_GetEnumFeatureSymbol(m_hDevHandle, strKey, pstEnumEntry);
}

int IMvCamera::GetFloatValue(IN const char* strKey, OUT double* pFloatValue)
{
    return IMV_GetDoubleFeatureValue(m_hDevHandle, strKey, pFloatValue);
}

int IMvCamera::SetFloatValue(IN const char* strKey, IN float fValue)
{
    return IMV_SetDoubleFeatureValue(m_hDevHandle, strKey, fValue);
}

int IMvCamera::GetBoolValue(IN const char* strKey, OUT bool* pbValue)
{
    return IMV_GetBoolFeatureValue(m_hDevHandle, strKey, pbValue);
}

int IMvCamera::SetBoolValue(IN const char* strKey, IN bool bValue)
{
    return IMV_SetBoolFeatureValue(m_hDevHandle, strKey, bValue);
}

int IMvCamera::GetStringValue(IN const char* strKey, OUT IMV_String* pStringValue)
{
    return IMV_GetStringFeatureValue(m_hDevHandle, strKey, pStringValue);
}

int IMvCamera::SetStringValue(IN const char* strKey, IN const char* strValue)
{
    return IMV_SetStringFeatureValue(m_hDevHandle, strKey, strValue);
}

int IMvCamera::CommandExecute(IN const char* strKey)
{
    return IMV_ExecuteCommandFeature(m_hDevHandle, strKey);
}

int IMvCamera::ForceIp(const char* nIP, const char* nSubNetMask, const char* nDefaultGateWay)
{
    return IMV_GIGE_ForceIpAddress(m_hDevHandle, nIP, nSubNetMask, nDefaultGateWay);;
}

int IMvCamera::SaveImageToFile(IMV_SaveImageToFileParam* pSaveImageParam, const char* pcImagePath)
{
    return IMV_SaveImageToFile(m_hDevHandle, pSaveImageParam);
}

/*
*     MV_TRIGGER_MODE_OFF                 = 0,                        ///< \~chinese 关闭                         \~english Off
    MV_TRIGGER_MODE_ON                  = 1,                        ///< \~chinese 打开                         \~english ON
*/
int IMvCamera::SetTriggerMode(int m_nTriggerMode)
{
    
    if(m_nTriggerMode == 0)
    {
        SetEnumValueByString("TriggerSelector", "FrameStop");
        return SetEnumValueByString("TriggerMode", "Off");
    }
    else
    {
        SetEnumValueByString("TriggerSelector", "FrameStart");
        return SetEnumValueByString("TriggerMode", "On");
    }
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
int IMvCamera::SetTriggerSource(int m_nTriggerSource)
{
    if(m_nTriggerSource == 0)
        return SetEnumValueByString("TriggerSource","Line0");
    if (m_nTriggerSource == 1)
        return SetEnumValueByString("TriggerSource", "Line1");
    if (m_nTriggerSource == 2)
        return SetEnumValueByString("TriggerSource", "Line2");
    if (m_nTriggerSource == 3)
        return SetEnumValueByString("TriggerSource", "Line3");
    if (m_nTriggerSource == 7)
        return SetEnumValueByString("TriggerSource", "Software");
    return -1;
}

int IMvCamera::SoftTriggerOnce()
{
    return CommandExecute("TriggerSoftware");
}

// offLine notify processing
void IMvCamera::deviceOffLine()
{
    // 停止拉流 
    // Stop grabbing 
    StopGrabbing();

    return;
}

// 上线通知处理
// onLine notify processing
void IMvCamera::deviceOnLine()
{
    int ret = IMV_OK;

    // 关闭相机
    // Close camera 
    Close();

    do
    {
        if (g_isMainThreadExit)
        {
            return;
        }

        ret = IMV_Open(m_hDevHandle);
        if (IMV_OK != ret)
        {
            //printf("Retry open camera failed! ErrorCode[%d]\n", ret);
        }
        else
        {
            //printf("Retry open camera successfully!\n");
            break;
        }

        Sleep(500);

    } while (true);

    // 重新设备连接状态事件回调函数
    // Device connection status event callback function again
    ret = IMV_SubscribeConnectArg(m_hDevHandle, onDeviceLinkNotify, this);
    if (IMV_OK != ret)
    {
        //printf("SubscribeConnectArg failed! ErrorCode[%d]\n", ret);
    }

    // 重新注册数据帧回调函数
    // Register data frame callback function again
    ret = IMV_AttachGrabbing(m_hDevHandle, m_procallbackfun, NULL);
    if (IMV_OK != ret)
    {
        //printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
    }

    // 开始拉流 
    // Start grabbing 
    ret = IMV_StartGrabbing(m_hDevHandle);
    if (IMV_OK != ret)
    {
        //printf("Start grabbing failed! ErrorCode[%d]\n", ret);
    }
    else
    {
        //printf("Start grabbing successfully\n");
    }
}

int IMvCamera::SaveFeature(char* fileName)
{
    return IMV_SaveDeviceCfg(m_hDevHandle, fileName);
}

int IMvCamera::LoadFeature(char* fileName)
{
    _IMV_ErrorList errorList;
    memset(&errorList, 0, sizeof(IMV_ErrorList));
    return IMV_LoadDeviceCfg(m_hDevHandle, fileName, &errorList);
}

std::string IMvCamera::GetDeviceSN()
{
    //std::string tempValue = m_stDevInfo.SpecialInfo.stGigEInfo.chSerialNumber;
    std::string str(m_devInfo->serialNumber, m_devInfo->serialNumber + sizeof(m_devInfo->serialNumber));
    return str;
}
#if 0
std::string IMvCamera::GetDeviceSN(IMV_DeviceInfo Input)
{
    std::string str(Input.serialNumber, Input.serialNumber + sizeof(Input.serialNumber));
    return str;

}
std::string IMvCamera::GetDeviceIp(IMV_DeviceInfo Input)
{
    std::string str(Input.InterfaceInfo.gigeInterfaceInfo.ipAddress, Input.InterfaceInfo.gigeInterfaceInfo.ipAddress + sizeof(Input.InterfaceInfo.gigeInterfaceInfo.ipAddress));
    return str;

}
#endif
std::string IMvCamera::GetDeviceIp()
{
    std::string str(m_devInfo->InterfaceInfo.gigeInterfaceInfo.ipAddress, m_devInfo->InterfaceInfo.gigeInterfaceInfo.ipAddress + sizeof(m_devInfo->InterfaceInfo.gigeInterfaceInfo.ipAddress));
    return str;
}

std::string IMvCamera::GetDeviceIp(IMV_DeviceInfo* Input)
{
    return std::string(Input->DeviceSpecificInfo.gigeDeviceInfo.ipAddress);
}

void IMvCamera::SetMainThreadExit(bool input)
{
    g_isMainThreadExit = input;
}
