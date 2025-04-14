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
\brief   �����ʼ��

\return  ��
*/
int GlxCamera::InitSDK()
{
	return GXInitLib();
}
/**
\Chinese��
\brief		�ر��豸�⣬�ͷ���Դ
\attention	�ͷſ���Դ�����û�����ʹ�ÿ��ʱ����ô˽ӿڡ�
			����û�֮ǰû�е���GXInitLib��ֱ�ӵ���GXCloseLib���ӿڷ��سɹ���
\return		GX_STATUS_SUCCESS             �����ɹ���û�з�������
			�������������μ�GX_STATUS_LIST

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

	//�豸��������0,������Դ��ȡ�豸��Ϣ
	if (nDeviceNum > 0)
	{
		// Ϊ��ȡ�豸��Ϣ׼����Դ
		pstDevList->m_pBaseInfo = new GX_DEVICE_BASE_INFO[nDeviceNum];
		if (pstDevList->m_pBaseInfo == NULL)
		{
			// �������ڵ��豸������Ϊ0
			nDeviceNum = 0;
			return 0;
		}

		//��ȡ����ö���������Ϣ
		nSize = nDeviceNum * sizeof(GX_DEVICE_BASE_INFO);  //����ṹ���С
		emStatus = GXGetAllDeviceBaseInfo(pstDevList->m_pBaseInfo, &nSize);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			delete[]pstDevList->m_pBaseInfo;
			pstDevList->m_pBaseInfo = NULL;

			// ���豸������Ϊ0
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
	// ���豸
	if (NULL == pstDeviceInfo)
	{
		return GX_STATUS_INVALID_PARAMETER;
	}

	if (m_hDevHandle)
	{
		return GX_STATUS_INVALID_HANDLE;
	}

	// ��ö���б��еĵ�һ̨�豸
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

