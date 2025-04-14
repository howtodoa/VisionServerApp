#pragma once
#include <string>
#include <vector>
using namespace std;
namespace Mz_IOControl
{
	class __declspec(dllexport) COperation
	{
		//初始化库
	public:
        void InitSDK();
        bool OpenSerial(std::string ComName);
        bool CloseSerial(std::string ComName);
        int SaveParam(string ComName);
        int Reboot(string ComName);
        //GPIO控制
        int OpenGPIO(int GPIO_Type);
        int SetGPO_Hi(int port);
        int SetGPO_Lo(int port);
        int GetGPI(BYTE& level);
        int SetPnpEnable(int pnptpye);
        //光源控制
        int SetLightParam(string ComName, int LightPort, int LightValue, int nLightState, int LightEdge, int DurTime, int& SourcePort);
        int SetLightTrigger(string ComName, int LightPort, int SourcePort);
        //IO板卡控制
        int IOBoard_SetInput(string ComName, int Port, int Edge, int Enable, int DelayTime, int GlitchTime);
        int IOBoard_SetOutput(string ComName, int Port, int Type, int PluseWidth, int PlusePeriod, int DurTime, int ValidLevel);
        int IOBoard_SetOutputTrigger(string ComName, int OutputPort, int TriggerSource);
        int IOBoard_GetIOInputLevel(string ComName, int& PortNum, std::vector<int>& level);
        int IOBoard_SetOutputEnable(string ComName, int Port);
        int IOBoard_SetOutputDisable(string ComName, int Port);
        //PCIE板卡控制
        int PCIe_OpenDevice(string BoardName, int CardID);
        int PCIe_CloseDevice(string BoardName);
        int PCIe_WriteDeviceIO(string BoardName, UINT writeDoDat);
        int PCIe_ReadDeviceIO(string BoardName, UINT& readDiDat);
        int PCIe_WriteDeviceIODobit(string BoardName, int bit, UINT writeDoDat);
        int PCIe_ReadDeviceIODobit(string BoardName, int bit, UINT& readDiDat);
        int PCIe_ReadDeviceIODibit(string BoardName, int bit, UINT& readDiDat);
	};
}
