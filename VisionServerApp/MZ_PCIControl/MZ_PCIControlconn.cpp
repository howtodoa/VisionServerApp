#include "pch.h"
#include "MZ_PCIControlconn.h"

#define VC2000_VB2000 1
#define VT3000 2

#define PNP_TYPE 1
#define NPN_TYPE 2
IOCommom::IOCommom()
{
    m_Cvc3000 = new CIOControlSDK();
    m_Cpcie1230 = new CPCIe1230();

    m_Cvc3000->Init();
}

IOCommom::~IOCommom()
{
    if (m_Cvc3000)
    {
        delete m_Cvc3000;
        m_Cvc3000 = NULL;
    }

    if (m_Cpcie1230)
    {
        delete m_Cpcie1230;
        m_Cpcie1230 = NULL;
    }
}

int IOCommom::OpenGPIO(int GPIO_Type)
{
    int nRet = -1;
    m_GPIO_Type = GPIO_Type;
    if (VC2000_VB2000 == GPIO_Type)//VC2000_VB2000
    {
        return MV_OK;
    }
    else if (VT3000 == GPIO_Type)//VT3000
    {
        m_Cvc3000->WinIODeInit();
        m_Cvc3000->SetMainIODeviceType(MV_DEVICE_VT3000);
        m_Cvc3000->WinIOInit();

        //VT3000设备，只支持NPN模式
        nRet = SetPnpEnable(NPN_TYPE);
    }
    else
    {
        m_Cvc3000->WinIODeInit();
        m_Cvc3000->SetMainIODeviceType(MV_DEVICE_VC3000);
        nRet = m_Cvc3000->WinIOInit();
    }
    return nRet;
}

int IOCommom::SetGPO_Hi(int port)
{
    int nRet = MV_OK;
    MV_GIO_LEVEL nStatus = MV_GIO_HIGHT;
    if (VT3000 == m_GPIO_Type)//VT3000
    {
        //VT3000的电平相反
        nStatus = MV_GIO_LOW;
    }

    MV_GIO_PORT_NUMBER__ nport = static_cast<MV_GIO_PORT_NUMBER__>(port);
    nRet = m_Cvc3000->SetMainOutputLevel(nport, nStatus);
    return nRet;
}

int IOCommom::SetGPO_Lo(int port)
{
    int nRet = MV_OK;
    bool bSelect = false;

    MV_GIO_LEVEL nStatus = MV_GIO_LOW;
    if (VT3000 == m_GPIO_Type)//VT3000
    {
        //VT3000的电平相反
        nStatus = MV_GIO_HIGHT;
    }
    MV_GIO_PORT_NUMBER__ nport = static_cast<MV_GIO_PORT_NUMBER__>(port);
    nRet = m_Cvc3000->SetMainOutputLevel(nport, nStatus);
    return nRet;
}

int IOCommom::GetGPI(BYTE& level)
{
    int nRet = MV_OK;
    memset(&level, 0, sizeof(unsigned char));
    nRet = m_Cvc3000->GetMainInputLevel(&level);
    if (MV_OK != nRet)
    {
        //LOG("Getting the electrical level status failed.");
        return false;
    }
    //LOG("The status of the input electrical level is obtained.");
    return nRet;
}

int IOCommom::SetPnpEnable(int pnptpye)
{
    int nRet = MV_OK;
    int nType = pnptpye;//PNP
    if (PNP_TYPE == pnptpye)//选择PNP
    {
        nType = 0x01;
    }
    else if (NPN_TYPE == pnptpye)//选择NPN
    {
        nType = 0x02;
    }

    nRet = m_Cvc3000->SetMainGPO_NPN(nType);
    return nRet;
}

int IOCommom::SetLightParam(string ComName,int LightPort, int LightValue, int nLightState, int LightEdge, int DurTime,int &SourcePort)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }

    int nRet = MV_OK;
    MV_IO_LIGHT_PARAM stParam = { 0 };
    stParam.nPortNumber = LightPort;
    int nDurationTime = DurTime;
    stParam.nLightValue = LightValue;
    stParam.nLightState = nLightState;

    stParam.nLightEdge = LightEdge;// (TRUE == m_rdbRisingEdge.GetCheck() ? MV_IO_EDGE_RISING : MV_IO_EDGE_FALLING);
    if (nDurationTime > 30000U)
    {
        stParam.nDurationTime = 30000U;
    }
    else
    {
        stParam.nDurationTime = nDurationTime;
    }

    nRet = m_Cvc3000->SetLightParam(m_ComDeviceListVec[index].m_hDeviceHandle, &stParam);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    MV_IO_LIGHTSOURCE_PARAM stParam2 = { 0 };
    stParam2.nLightPortNum = LightPort;

    nRet = m_Cvc3000->GetLightTriggerSource(m_ComDeviceListVec[index].m_hDeviceHandle, &stParam2);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    SourcePort = stParam2.nInputPortNum;
    return nRet;
}

int IOCommom::SetLightTrigger(string ComName, int LightPort, int SourcePort)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }

    MV_IO_LIGHTSOURCE_PARAM stParam = { 0 };
    stParam.nLightPortNum = LightPort;
    stParam.nInputPortNum = SourcePort;

    int nRet = m_Cvc3000->SetLightTriggerSource(m_ComDeviceListVec[index].m_hDeviceHandle, &stParam);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    return nRet;
}

int IOCommom::IOBoard_SetInput(string ComName, int Port, int Edge, int Enable, int DelayTime, int GlitchTime)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }
    MV_IO_SET_INPUT stInput = { 0 };
    stInput.nPort = Port;
    stInput.nEdge = Edge;
    stInput.nEnable = Enable;
    stInput.nDelayTime = DelayTime;
    stInput.nGlitch = GlitchTime;
    int nRet = m_Cvc3000->SetInput(m_ComDeviceListVec[index].m_hDeviceHandle, &stInput);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    return nRet;
}


int IOCommom::CheckOutputParam(int type,int m_tbDurationTime,int m_tbPulsePeriod,int m_tbPulseWidth)
{
    int nRet = MV_OK;
    unsigned int nGOPTuration = m_tbDurationTime;
    unsigned int nGPeriod = m_tbPulsePeriod;
    unsigned int nGPOPulseWidth = m_tbPulseWidth;

    if (MV_IO_PATTERN_PWM == type)  //Multi-Pulse  Dutime > period >pulse width
    {
        if (nGPeriod > 30000U ||
            nGPOPulseWidth > 30000U)
        {
            //ShowErrorMsg(_T("The parameter exceeds the max. value of 30000 ms."), MV_E_PARAMETER);
            nRet = MV_E_PARAMETER;
        }

        if (nGPeriod == 0 || nGPOPulseWidth == 0)
        {
            //ShowErrorMsg(_T("The pulse period or the pulse width of the multi-pulse cannot be 0."), MV_E_PARAMETER);
            nRet = MV_E_PARAMETER;
        }


        if (nGOPTuration < nGPeriod ||
            nGOPTuration < nGPOPulseWidth ||
            nGPeriod < nGPOPulseWidth)
        {
            //ShowErrorMsg(_T("The parameter exceeds the max. value of 30000 ms."), MV_E_PARAMETER);
            nRet = MV_E_PARAMETER;
        }

        // ch:Duration time 必须是周期的整数倍 | en:Duration time must be an integral multiple of the period 
        if (nGPeriod != 0)
        {
            if ((0 != nGOPTuration % nGPeriod) &&
                30000U != nGOPTuration)
            {
                //ShowErrorMsg(_T("The pulse duration should be an integral multiple of the pulse period."), MV_E_PARAMETER);
                nRet = MV_E_PARAMETER;
            }
        }

    }
    else if (MV_IO_PATTERN_SINGLE == type)
    {
        if (nGOPTuration > 30000U)
        {
            //ShowErrorMsg(_T("The parameter exceeds the max. value of 30000 ms."), MV_E_PARAMETER);
            nRet = MV_E_PARAMETER;
        }
    }
    return nRet;
}

int IOCommom::IOBoard_SetOutput(string ComName, int Port, int Type, int PluseWidth, int PlusePeriod, int DurTime, int ValidLevel)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }
    int nRet = MV_OK;
    nRet = CheckOutputParam(Type, DurTime, PlusePeriod, PluseWidth);
    if (MV_E_PARAMETER == nRet)
    {
        return nRet;
    }

    MV_IO_SET_OUTPUT stOutput = { 0 };
    stOutput.nPort = Port;
    stOutput.nType = Type;
    stOutput.nPulseWidth = PluseWidth;
    stOutput.nPulsePeriod = PlusePeriod;
    stOutput.nDurationTime = DurTime;
    stOutput.nValidLevel = ValidLevel;
    nRet = m_Cvc3000->SetOutput(m_ComDeviceListVec[index].m_hDeviceHandle, &stOutput);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    return nRet;
}

int IOCommom::IOBoard_SetOutputTrigger(string ComName,int OutputPort,int TriggerSource)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }

    MV_IO_OUTPUTSOURCE_PARAM stParam = { 0 };
    stParam.nOutPortNum = OutputPort;
    stParam.nInPortNum = TriggerSource;

    int nRet = m_Cvc3000->SetOutPortTriggerSource(m_ComDeviceListVec[index].m_hDeviceHandle, &stParam);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    return nRet;
}

int IOCommom::IOBoard_GetIOInputLevel(string ComName,int &PortNum,vector<int> &level)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }
    int nRet = MV_OK;
    MV_IO_INPUT_LEVEL m_stInputLevel;
    memset(&m_stInputLevel, 0, sizeof(MV_IO_INPUT_LEVEL));
    nRet = m_Cvc3000->GetInputLevel(m_ComDeviceListVec[index].m_hDeviceHandle, &m_stInputLevel);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    PortNum = m_stInputLevel.nPortNumber;
    level.push_back(m_stInputLevel.nLevel0);
    level.push_back(m_stInputLevel.nLevel1);
    level.push_back(m_stInputLevel.nLevel2);
    level.push_back(m_stInputLevel.nLevel3);
    level.push_back(m_stInputLevel.nLevel4);
    level.push_back(m_stInputLevel.nLevel5);
    level.push_back(m_stInputLevel.nLevel6);
    level.push_back(m_stInputLevel.nLevel7);
    return nRet;
}

int IOCommom::IOBoard_SetOutputEnable(string ComName, int Port)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }
    int nRet = MV_OK;

    MV_IO_OUTPUT_ENABLE stParam = { 0 };
    stParam.nPortNumber = Port;
    stParam.nType = MV_IO_ENABLE_START;//((ENABLE_STATE.关闭使能 == (ENABLE_STATE)cbOutputEnable.SelectedItem) ? (CIOControllerSDK.MV_IO_ENABLE_TYPE.MV_IO_ENABLE_END) : (CIOControllerSDK.MV_IO_ENABLE_TYPE.MV_IO_ENABLE_START));

    nRet = m_Cvc3000->SetOutputEnable(m_ComDeviceListVec[index].m_hDeviceHandle, &stParam);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    return nRet;
}

int IOCommom::IOBoard_SetOutputDisable(string ComName, int Port)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is open.");
        return -1;
    }
    int nRet = MV_OK;

    MV_IO_OUTPUT_ENABLE stParam = { 0 };
    stParam.nPortNumber = Port;
    stParam.nType = MV_IO_ENABLE_END;//((ENABLE_STATE.关闭使能 == (ENABLE_STATE)cbOutputEnable.SelectedItem) ? (CIOControllerSDK.MV_IO_ENABLE_TYPE.MV_IO_ENABLE_END) : (CIOControllerSDK.MV_IO_ENABLE_TYPE.MV_IO_ENABLE_START));

    nRet = m_Cvc3000->SetOutputEnable(m_ComDeviceListVec[index].m_hDeviceHandle, &stParam);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    return nRet;
}

int IOCommom::PCIe_OpenDevice(string BoardName, int CardID)
{

    int nRet =  m_Cpcie1230->OpenDevice(CardID);
    if(nRet == PCI1230Success)
    {
        PCIeHand tempHand;
        tempHand.BoardID = CardID;
        tempHand.BoardName = BoardName;
        m_PCIeDeviceListVec.push_back(tempHand);
    }
    return nRet;
}

int IOCommom::PCIe_GetDeviceIDbyName(string BoardName)
{
    int nRet = -1;
    for (int i = 0; i < m_PCIeDeviceListVec.size(); i++)
    {
        if (m_PCIeDeviceListVec[i].BoardName == BoardName && m_PCIeDeviceListVec[i].BoardID != -1)
            return i;
    }
    return nRet;
}

int IOCommom::PCIe_CloseDevice(string BoardName)
{
    int index = PCIe_GetDeviceIDbyName(BoardName);
    if (index == -1)
        return -1;
    m_Cpcie1230->SetCardID(index);
    return m_Cpcie1230->CloseDevice();
}

int IOCommom::PCIe_WriteDeviceIO(string BoardName, UINT writeDoDat)
{
    int index = PCIe_GetDeviceIDbyName(BoardName);
    if (index == -1)
        return -1;
    return m_Cpcie1230->WriteDeviceIO(writeDoDat);
}

int IOCommom::PCIe_ReadDeviceIO(string BoardName, UINT& readDiDat)
{
    int index = PCIe_GetDeviceIDbyName(BoardName);
    if (index == -1)
        return -1;
    return m_Cpcie1230->ReadDeviceIO(readDiDat);
}

int IOCommom::PCIe_WriteDeviceIODobit(string BoardName, int bit, UINT writeDoDat)
{
    int index = PCIe_GetDeviceIDbyName(BoardName);
    if (index == -1)
        return -1;
    return m_Cpcie1230->WriteDeviceIODobit(bit, writeDoDat);
}

int IOCommom::PCIe_ReadDeviceIODobit(string BoardName, int bit, UINT& readDiDat)
{
    int index = PCIe_GetDeviceIDbyName(BoardName);
    if (index == -1)
        return -1;
    return m_Cpcie1230->ReadDeviceIODobit(bit, readDiDat);
}

int IOCommom::PCIe_ReadDeviceIODibit(string BoardName, int bit, UINT& readDiDat)
{
    int index = PCIe_GetDeviceIDbyName(BoardName);
    if (index == -1)
        return -1;
    return m_Cpcie1230->ReadDeviceIODibit(bit, readDiDat);
}


int IOCommom::GetSerialComIndexByName(std::string ComName)
{
    int nRet = -1;
    for (int i = 0; i < m_ComDeviceListVec.size(); i++)
    {
        if (m_ComDeviceListVec[i].ComName == ComName && m_ComDeviceListVec[i].m_hDeviceHandle != NULL)
            return i;
    }
    return nRet;
}
int IOCommom::SaveParam(string ComName)
{
    int index = GetSerialComIndexByName(ComName);
    if (index != -1)
    {
        //LOG("Error. The serial port is open.");
        return true;
    }
    int nRet = m_Cvc3000->SaveParam(m_ComDeviceListVec[index].m_hDeviceHandle);
    return nRet;
}
int IOCommom::Reboot(string ComName)
{
    int index = GetSerialComIndexByName(ComName);
    if (index != -1)
    {
        //LOG("Error. The serial port is open.");
        return true;
    }
    int nRet = m_Cvc3000->Reboot(m_ComDeviceListVec[index].m_hDeviceHandle);
    if (MV_OK == nRet)
    {
       // LOG("The command of rebooting the hardware is sent.");
        m_Cvc3000->Close(m_ComDeviceListVec[index].m_hDeviceHandle);
        m_Cvc3000->DestroyHandle(m_ComDeviceListVec[index].m_hDeviceHandle);
        //DisableIOParams();
        m_ComDeviceListVec[index].m_hDeviceHandle = NULL;
    }
    return nRet;
}
bool IOCommom::OpenSerial(std::string ComName)
{
    int index = GetSerialComIndexByName(ComName);
    if (index != -1)
    {
        //LOG("Error. The serial port is open.");
        return true;
    }
    CommHand tempHand;
    tempHand.ComName = ComName;
    int nRet = m_Cvc3000->CreateHandle(&tempHand.m_hDeviceHandle);
    if (MV_OK != nRet || NULL == tempHand.m_hDeviceHandle)
    {
        return false;
    }

    // 2.ch:打开串口 | en:Connect Serial 
    MV_IO_SERIAL m_stSerial;
    memcpy(m_stSerial.strComName, tempHand.ComName.c_str(), tempHand.ComName.length());
    nRet = m_Cvc3000->Open(tempHand.m_hDeviceHandle, &m_stSerial);
    if (MV_OK != nRet)
    {
        m_Cvc3000->Close(tempHand.m_hDeviceHandle);
        m_Cvc3000->DestroyHandle(tempHand.m_hDeviceHandle);
        return false;
    }
    else
    {
        m_ComDeviceListVec.push_back(tempHand);
    }
    return true;
}

bool IOCommom::CloseSerial(std::string ComName)
{
    int index = GetSerialComIndexByName(ComName);
    if (index == -1)
    {
        //LOG("Error. The serial port is not open.");
        return true;
    }

    m_Cvc3000->Close(m_ComDeviceListVec[index].m_hDeviceHandle);
    m_Cvc3000->DestroyHandle(m_ComDeviceListVec[index].m_hDeviceHandle);
    m_ComDeviceListVec[index].m_hDeviceHandle = NULL;
    m_ComDeviceListVec.erase(m_ComDeviceListVec.begin() + index);
    return true;
}
