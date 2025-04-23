#include "pch.h"
#include "MZ_Device.h"

// 数据帧回调函数
// Data frame callback function
static void onGetFrame(IMV_Frame* pFrame, void* pUser)
{
	class_net_engine* pThis = (class_net_engine*)pUser;
	if (pThis == NULL)
		return;
	if (pFrame == NULL)
	{
		printf("pFrame is NULL\n");
		return;
	}
	cv::Mat srcMat = pThis->Convert2Mat(&pFrame->frameInfo, pFrame->pData);
	pThis->m_localcallbackfun(srcMat, NULL);
	printf("Get frame blockId = %llu\n", pFrame->frameInfo.blockId);
	return;
}

void __stdcall ImageCallBackEx(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	class_net_engine* pThis = (class_net_engine*)pUser;
	if (pThis == NULL)
		return;
	if (pFrameInfo)
	{
		cv::Mat srcMat = pThis->Convert2Mat(pFrameInfo, pData);
		pThis->m_localcallbackfun(srcMat, NULL);
	}
	return;
}
///////////////////////////////////////////////////////
class_net_engine::class_net_engine(MsvDeviceId inputType, void* DeviceInfo)
{
	m_Devtype = inputType;
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		DH_Camera = new IMvCamera((IMV_DeviceInfo*)DeviceInfo);
	}
	else
	{
		HK_Camera = new CMvCamera((MV_CC_DEVICE_INFO*)DeviceInfo);
	}
}

class_net_engine::~class_net_engine()
{
	
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		delete DH_Camera;
		DH_Camera = NULL;
	}
	else
	{
		delete HK_Camera;
		HK_Camera = NULL;
	}
}
int class_net_engine::OpenDevice()
{

	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		int nRet =  DH_Camera->Open();
		m_isOpen = true;
		return nRet;
	}
	else
	{
		int nRet = HK_Camera->Open();
		m_isOpen = true;
		return nRet;
	}
	return 0;
}

int class_net_engine::CloseDevice()
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		int nRet = DH_Camera->Close();
		m_isOpen = false;
		return nRet;
	}
	else
	{
		int nRet = HK_Camera->Close();
		m_isOpen = false;
		return nRet;
	}
	return 0;
}

int class_net_engine::IsDevConnect()
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->IsDeviceConnected();
	}
	else
	{
		return HK_Camera->IsDeviceConnected();
	}
	return 0;
}

int class_net_engine::SetParam(char* key, bool Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->SetBoolValue(key, Value);
	}
	else
	{
		return HK_Camera->SetBoolValue(key, Value);
	}
	return 0;
}

int class_net_engine::GetParam(char* key, bool* Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->GetBoolValue(key, Value);
	}
	else
	{
		return HK_Camera->GetBoolValue(key, Value);
	}
	return 0;
}

int class_net_engine::SetParam(char* key, float Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->SetFloatValue(key, Value);
	}
	else
	{
		return HK_Camera->SetFloatValue(key, Value);
	}
	return 0;
}

int class_net_engine::GetParam(char* key, float* Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		double tempValue;
		int nRet =  DH_Camera->GetFloatValue(key, &tempValue);
		if (nRet == IMV_OK)
		{
			*Value = tempValue;
		}
		return nRet;
	}
	else
	{
		MVCC_FLOATVALUE tempValue;
		int nRet = HK_Camera->GetFloatValue(key, &tempValue);
		if (nRet == MV_OK)
		{
			*Value = tempValue.fCurValue;
		}
		return nRet;

	}
	return 0;
}

int class_net_engine::SetParam(char* key, int Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->SetIntValue(key, Value);
	}
	else
	{
		return HK_Camera->SetIntValue(key, Value);
	}
	return 0;
}

int class_net_engine::GetParam(char* key, int* Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		_int64 tempValue;
		int nRet = DH_Camera->GetIntValue(key, &tempValue);
		if (nRet == IMV_OK)
		{
			*Value = tempValue;
		}
		return nRet;
	}
	else
	{
		MVCC_INTVALUE_EX tempValue;
		int nRet = HK_Camera->GetIntValue(key, &tempValue);
		if (nRet == MV_OK)
		{
			*Value = tempValue.nCurValue;
		}
		return nRet;

	}
	return 0;
}

int class_net_engine::SetParam(char* key, std::string Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		int nRet = DH_Camera->SetStringValue(key, Value.c_str());
		return nRet;
	}
	else
	{
		int nRet = HK_Camera->SetStringValue(key, Value.c_str());
		return nRet;

	}
	return 0;
}

int class_net_engine::GetParam(char* key, std::string* Value)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		IMV_String tempValue;
		int nRet = DH_Camera->GetStringValue(key, &tempValue);
		*Value = tempValue.str;
		return nRet;
	}
	else
	{
		MVCC_STRINGVALUE tempValue;
		int nRet = HK_Camera->GetStringValue(key, &tempValue);
		*Value = tempValue.chCurValue;
		return nRet;

	}
	return 0;
}

int class_net_engine::SetTrrigerModel(int model)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->SetTriggerMode(model);
	}
	else
	{
		return HK_Camera->SetTriggerMode(model);
	}
	return 0;
}

int class_net_engine::SetTrrigerSource(int source)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->SetTriggerSource(source);
	}
	else
	{
		return HK_Camera->SetTriggerSource(source);
	}
	return 0;
}

int class_net_engine::SoftTrigger()
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->SoftTriggerOnce();
	}
	else
	{
		return HK_Camera->SoftTriggerOnce();
	}
	return 0;
}

int class_net_engine::StartFrame()
{
	if (m_bStartGrabbing == true)
		return 1;
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		int Ret =  DH_Camera->StartGrabbing();
		if (Ret == IMV_OK)
		{
			m_bStartGrabbing = true;
		}
		return Ret;
	}
	else
	{
		int Ret = HK_Camera->StartGrabbing();
		if (Ret == MV_OK)
		{
			m_bStartGrabbing = true;
		}
		return Ret;
	}
	return 0;
}

int class_net_engine::StopFrame()
{
	if (m_bStartGrabbing == false)
		return 1;
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		int Ret = DH_Camera->StopGrabbing();
		if (Ret == MV_OK)
		{
			m_bStartGrabbing = false;
		}
		return Ret;
	}
	else
	{
		int Ret = HK_Camera->StopGrabbing();
		if (Ret == MV_OK)
		{
			m_bStartGrabbing = false;
		}
		return Ret;
	}
	return 0;
}

std::string class_net_engine::GetDeviceSN()
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->GetDeviceSN();
	}
	else
	{
		return HK_Camera->GetDeviceSN();
	}
	return nullptr;
}

std::string class_net_engine::GetDeviceIP()
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->GetDeviceIp();
	}
	else
	{
		return HK_Camera->GetDeviceIp();
	}
	return nullptr;
}

int class_net_engine::SaveFeature(char* fileName)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->SaveFeature(fileName);
	}
	else
	{
		return HK_Camera->SaveFeature(fileName);
	}
	return 0;
}

int class_net_engine::LoadFeature(char* fileName)
{
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->LoadFeature(fileName);
	}
	else
	{
		return HK_Camera->LoadFeature(fileName);
	}
	return 0;
}

void class_net_engine::SetDeviceOffline()
{
	if (m_bStartGrabbing)
		StopFrame();
	if (m_isOpen)
		CloseDevice();
	m_isRealse = true;
}

bool class_net_engine::IsOfflline()
{
	return m_isRealse;
}
bool class_net_engine::IsOpen()
{
	return m_isOpen;
}
bool class_net_engine::IsConnect()
{
	return m_isConnect;
}

int class_net_engine::RegeditImageCallback(DeviceImageCallback callbackfun, void* pUser)
{
	m_localcallbackfun = callbackfun;
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		return DH_Camera->RegisterImageCallBack(onGetFrame,this);
	}
	else
	{
		return HK_Camera->RegisterImageCallBack(ImageCallBackEx, this);
	}
	return 0;
}

cv::Mat class_net_engine::GetFrame(int TimeOut)
{
	cv::Mat SrcMat;
	if (m_Devtype.m_type == DeviceType::DEVICE_TYPE_DHUA)
	{
		IMV_Frame pFrame;
		DH_Camera->GetImageBuffer(&pFrame, TimeOut);
		SrcMat = Convert2Mat(&pFrame.frameInfo, pFrame.pData);
		DH_Camera->FreeImageBuffer(&pFrame);
	}
	else
	{
		MV_FRAME_OUT pFrame;
		HK_Camera->GetImageBuffer(&pFrame, TimeOut);
		SrcMat = Convert2Mat(&pFrame.stFrameInfo, pFrame.pBufAddr);
		HK_Camera->FreeImageBuffer(&pFrame);
	}
	return SrcMat;
}

// ch:像素排列由RGB转为BGR | en:Convert pixel arrangement from RGB to BGR
void class_net_engine::RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight)
{
	if (NULL == pRgbData)
	{
		return;
	}

	// red和blue数据互换
	for (unsigned int j = 0; j < nHeight; j++)
	{
		for (unsigned int i = 0; i < nWidth; i++)
		{
			unsigned char red = pRgbData[j * (nWidth * 3) + i * 3];
			pRgbData[j * (nWidth * 3) + i * 3] = pRgbData[j * (nWidth * 3) + i * 3 + 2];
			pRgbData[j * (nWidth * 3) + i * 3 + 2] = red;
		}
	}
}
// ch:帧数据转换为Mat格式图片并保存 | en:Convert data stream to Mat format then save image
cv::Mat class_net_engine::Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char* pData)
{
	cv::Mat srcImage;

	if (NULL == pstImageInfo || NULL == pData)
	{
		printf("NULL info or data.\n");
		return srcImage;
	}

	if (PixelType_Gvsp_Mono8 == pstImageInfo->enPixelType)                // Mono8类型
	{
		srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC1, pData);
	}
	else if (PixelType_Gvsp_RGB8_Packed == pstImageInfo->enPixelType)     // RGB8类型
	{
		// Mat像素排列格式为BGR，需要转换
		RGB2BGR(pData, pstImageInfo->nWidth, pstImageInfo->nHeight);
		srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, pData);
	}
	else
	{
		/* Bayer 格式转换mat格式的方法:
		1. 使用相机句柄销毁前 调用 MV_CC_ConvertPixelType 将PixelType_Gvsp_BayerRG8 等Bayer格式转换成 PixelType_Gvsp_BGR8_Packed
		2. 参考上面 将BGR转换为 mat格式
		*/

		printf("Unsupported pixel format\n");
		return srcImage;
	}

	if (NULL == srcImage.data)
	{
		printf("Creat Mat failed.\n");
		return srcImage;
	}
	return srcImage;
}

cv::Mat class_net_engine::Convert2Mat(IMV_FrameInfo* pstImageInfo, unsigned char* pData)
{
	cv::Mat srcImage;

	if (NULL == pstImageInfo || NULL == pData)
	{
		printf("NULL info or data.\n");
		return srcImage;
	}
	
	if (gvspPixelMono8 == pstImageInfo->pixelFormat)                // Mono8类型
	{
		srcImage = cv::Mat(pstImageInfo->height, pstImageInfo->width, CV_8UC1, pData);
	}
	else if (gvspPixelRGB8 == pstImageInfo->pixelFormat)     // RGB8类型
	{
		// Mat像素排列格式为BGR，需要转换
		RGB2BGR(pData, pstImageInfo->width, pstImageInfo->height);
		srcImage = cv::Mat(pstImageInfo->height, pstImageInfo->width, CV_8UC3, pData);
	}
	else
	{
		/* Bayer 格式转换mat格式的方法:
		1. 使用相机句柄销毁前 调用 MV_CC_ConvertPixelType 将PixelType_Gvsp_BayerRG8 等Bayer格式转换成 PixelType_Gvsp_BGR8_Packed
		2. 参考上面 将BGR转换为 mat格式
		*/

		printf("Unsupported pixel format\n");
		return srcImage;
	}

	if (NULL == srcImage.data)
	{
		printf("Creat Mat failed.\n");
		return srcImage;
	}
	return srcImage;
}