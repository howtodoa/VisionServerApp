
#include "pch.h"
#include "MZ_Device.h"
#include "MsvDeviceLib.h"
// MathLibrary.cpp : Defines the exported functions for the DLL.

static void *m_stDevList = NULL;
class_net_engine* eng = NULL;
namespace Mz_CameraConn
{
	MZ_API Mz_CameraConn::MsvInitLib()
	{
		return MV_CC_Initialize();
	}

	MZ_API Mz_CameraConn::MsvCloseLib()
	{
		return MV_CC_Finalize();
	}

	MZ_API MsvEnumerateDevics(std::vector<MsvDeviceId>& deviceList)
	{
		if (deviceList[0].m_type == DEVICE_TYPE_HIKVISION)
		{
			if (m_stDevList == NULL) {
				m_stDevList = malloc(sizeof(IMV_DeviceList));
			}
			memset(m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
			int nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE, (MV_CC_DEVICE_INFO_LIST*)m_stDevList);
			return nRet;
		}
		else
		{
			if (m_stDevList == NULL) {
				m_stDevList = malloc(sizeof(IMV_DeviceList));
			}
			memset(m_stDevList, 0, sizeof(IMV_DeviceList));
			int nRet = IMvCamera::EnumDevices(interfaceTypeGige, (IMV_DeviceList*)m_stDevList);
			return nRet;
		}
	}

	COperation::COperation(MsvDeviceId inputID)
	{
		if (inputID.m_type == DEVICE_TYPE_HIKVISION)
		{
			auto* devList = (MV_CC_DEVICE_INFO_LIST*)m_stDevList;
			for (int i = 0; i < devList->nDeviceNum; i++)
			{
				if (CMvCamera::GetDeviceIp(devList->pDeviceInfo[i]) == inputID.m_ip)
				{
					eng = new class_net_engine(inputID, devList->pDeviceInfo[i]);
					return;
				}
			}
		}
		else
		{
		
			auto* devList = (IMV_DeviceList*)m_stDevList;
			for (int i = 0; i < devList->nDevNum; i++)
			{
				if (IMvCamera::GetDeviceIp(&devList->pDevInfo[i]) == inputID.m_ip)
				{
					eng = new class_net_engine(inputID, &devList->pDevInfo[i]);
					return;
				}
			}
		}
	}
	COperation::~COperation()
	{
		if (eng)
		{
			delete eng;
			eng = NULL;
		}
	}
	int COperation::MsvOpenDevice()
	{
		if (eng != NULL)
			return eng->OpenDevice();
		return -1;
	}
	int COperation::MsvCloseDevice()
	{
		if (eng != NULL)
			return eng->CloseDevice();
		return -1;
	}

	int COperation::MsvSaveParams(char* filename)
	{
		if (eng != NULL)
			return eng->SaveFeature(filename);
		return -1;
	}

	int COperation::MsvLoadParams(char* filename)
	{
		if (eng != NULL)
			return eng->LoadFeature(filename);
		return -1;
	}

	int COperation::MsvStartImageCapture()
	{
		if (eng != NULL)
			return eng->StartFrame();
		return -1;
	}

	int COperation::MsvStopImageCapture()
	{
		if (eng != NULL)
			return eng->StopFrame();
		return -1;
	}

	int COperation::IsDevConnect()
	{
		if (eng != NULL)
			return eng->IsDevConnect();
		return -1;
	}

	int COperation::SetParam(char* key, bool Value)
	{
		if (eng != NULL)
			return eng->SetParam(key, Value);
		return -1;
	}
	int COperation::SetParam(char* key, float Value)
	{
		if (eng != NULL)
			return eng->SetParam(key, Value);
		return -1;
	}
	int COperation::SetParam(char* key, int Value)
	{
		if (eng != NULL)
			return eng->SetParam(key, Value);
		return -1;
	}
	int COperation::SetParam(char* key, std::string Value)
	{
		if (eng != NULL)
			return eng->SetParam(key, Value);
		return -1;
	}

	int COperation::GetParam(char* key, bool* Value)
	{
		if (eng != NULL)
			return eng->GetParam(key, Value);
		return -1;
	}
	int COperation::GetParam(char* key, float* Value)
	{
		if (eng != NULL)
			return eng->GetParam(key, Value);
		return -1;
	}
	int COperation::GetParam(char* key, int* Value)
	{
		if (eng != NULL)
			return eng->GetParam(key, Value);
		return -1;
	}
	int COperation::GetParam(char* key, std::string* Value)
	{
		if (eng != NULL)
			return eng->GetParam(key, Value);
		return -1;
	}

	int COperation::SetTrrigerModel(int model)
	{
		if (eng != NULL)
			return eng->SetTrrigerModel(model);
		return -1;
	}

	int COperation::SetTrrigerSource(int source)
	{
		if (eng != NULL)
			return eng->SetTrrigerSource(source);
		return -1;
	}

	int COperation::SoftTrigger()
	{
		if (eng != NULL)
			return eng->SoftTrigger();
		return -1;
	}

	cv::Mat COperation::GetFrame(int Timeout)
	{
		if (eng != NULL)
			return eng->GetFrame(Timeout);
		return cv::Mat();
	}

	int COperation::RegisterImageCallBack(DeviceImageCallback inputCallbackfun)
	{
		if (eng != NULL)
			return eng->RegeditImageCallback(inputCallbackfun,NULL);
		return -1;
	}
}
