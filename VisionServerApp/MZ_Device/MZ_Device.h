#pragma once
#include "MsvDeviceLib.h"
#include "IMvCamera.h"
#include "MvCamera.h"


//typedef void (*DeviceImageCallback)(cv::Mat& outputmat, void* pUser);
class class_net_engine
{
public:
	class_net_engine(MsvDeviceId inputType,void* DeviceInfo);
	~class_net_engine();

	int OpenDevice();
	int CloseDevice();
	int IsDevConnect();

	int SetParam(char *key,bool Value);
	int GetParam(char* key, bool *Value);

	int SetParam(char* key, float Value);
	int GetParam(char* key, float* Value);

	int SetParam(char* key, int Value);
	int GetParam(char* key, int* Value);

	int SetParam(char* key, std::string Value);
	int GetParam(char* key, std::string* Value);

	int SetTrrigerModel(int model);
	int SetTrrigerSource(int source);
	int SoftTrigger();

	int StartFrame();
	int StopFrame();

	std::string GetDeviceSN();
	std::string GetDeviceIP();

	int SaveFeature(char* fileName);
	int LoadFeature(char* fileName);

	void SetDeviceOffline();
	bool IsOfflline();
	bool IsOpen();
	bool IsConnect();

	int RegeditImageCallback(DeviceImageCallback callbackfun,void* pUser);
	cv::Mat GetFrame(int TimeOut);
	cv::Mat Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char* pData);
	cv::Mat Convert2Mat(IMV_FrameInfo* pstImageInfo, unsigned char* pData);
public:
	void* m_DeviceHead;
	DeviceImageCallback m_localcallbackfun;
private:
	CMvCamera* HK_Camera = NULL;
	IMvCamera* DH_Camera = NULL;
	MsvDeviceId m_Devtype;
	void* m_DeviceInfo = NULL;
	bool m_isOpen = false;
	bool m_bStartGrabbing = false;
	bool m_isRealse = false;
	bool m_isConnect = false;
	void RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight);
	

};

