#include "pch.h"
#include "GlxCamera.h"

GlxCamera::GlxCamera()
{
    m_hDevHandle = NULL;
}

GlxCamera::~GlxCamera()
{
    if (m_hDevHandle)
    {
		GXCloseDevice(m_hDevHandle);
        m_hDevHandle = NULL;
    }
}
/**
\brief   相机初始化

\return  无
*/
int GlxCamera::InitSDK()
{
	return GXInitLib();
}
/**
\Chinese：
\brief		关闭设备库，释放资源
\attention	释放库资源，当用户不再使用库的时候调用此接口。
			如果用户之前没有调用GXInitLib，直接调用GXCloseLib，接口返回成功。
\return		GX_STATUS_SUCCESS             操作成功，没有发生错误
			其它错误情况请参见GX_STATUS_LIST

\English:
\brief		Close the device library to release resources.
\attention	Frees library resources and invokes this interface when the user is no longer using the library.
			If the user did not call GXInitLib before, call GXCloseLib directly and the interface returns success.
\return		GX_STATUS_SUCCESS				The operation is successful, no error occurs.
			The errors that are not covered above please reference GX_STATUS_LIST.
*/
int GlxCamera::FinalizeSDK()
{
	return GXCloseLib();
}

int GlxCamera::GetSDKVersion()
{
	return 0;
}

int GlxCamera::EnumDevices(unsigned int nTLayerType, DEVICE_BASE_INFO_LIST* pstDevList)
{
	GX_STATUS  emStatus = GX_STATUS_SUCCESS;
	uint32_t   nDeviceNum = 0;
	bool       bRet = true;
	size_t     nSize = 0;

	emStatus = GXUpdateAllDeviceListEx(nTLayerType, &nDeviceNum, 1000);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		return 0;
	}

	//设备个数大于0,分配资源获取设备信息
	if (nDeviceNum > 0)
	{
		// 为获取设备信息准备资源
		pstDevList->m_pBaseInfo = new GX_DEVICE_BASE_INFO[nDeviceNum];
		if (pstDevList->m_pBaseInfo == NULL)
		{
			// 将主窗口的设备个数置为0
			nDeviceNum = 0;
			return 0;
		}

		//获取所有枚举相机的信息
		nSize = nDeviceNum * sizeof(GX_DEVICE_BASE_INFO);  //计算结构体大小
		emStatus = GXGetAllDeviceBaseInfo(pstDevList->m_pBaseInfo, &nSize);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			delete[]pstDevList->m_pBaseInfo;
			pstDevList->m_pBaseInfo = NULL;

			// 将设备个数置为0
			nDeviceNum = 0;

			return;
		}
	}

	pstDevList->m_nDeviceNum = nDeviceNum;
	return 1;
}

bool GlxCamera::IsDeviceAccessible(GX_DEVICE_BASE_INFO* pstDevInfo, unsigned int nAccessMode)
{
	return false;
}

int GlxCamera::Open(GX_DEVICE_BASE_INFO* pstDeviceInfo)
{
	GX_STATUS      emStatus = GX_STATUS_SUCCESS;
	// 打开设备
	if (NULL == pstDeviceInfo)
	{
		return GX_STATUS_INVALID_PARAMETER;
	}

	if (m_hDevHandle)
	{
		return GX_STATUS_INVALID_HANDLE;
	}

	// 打开枚举列表中的第一台设备
	GX_OPEN_PARAM  stOpenParam;
	stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
	stOpenParam.openMode = GX_OPEN_SN;
	stOpenParam.pszContent = pstDeviceInfo->szSN;
	emStatus = GXOpenDevice(&stOpenParam, &m_hDevHandle);

	if (emStatus != GX_STATUS_SUCCESS)
	{
		m_hDevHandle = NULL;
	}

	return emStatus;
}

int GlxCamera::Close()
{
	GX_STATUS      emStatus = GX_STATUS_SUCCESS;
	if (m_hDevHandle != NULL)
	{
		emStatus = GXCloseDevice(m_hDevHandle);
		m_hDevHandle = NULL;
	}
	return emStatus;
}

bool GlxCamera::IsDeviceConnected()
{
	
}

