#pragma once
#include <vector>
#include "PCI1230.h"

class CPCIe1230
{
public:
	CPCIe1230();
	~CPCIe1230();
	
	void SetCardID(int inputID);
	int OpenDevice(int CardID);
	int CloseDevice();
	int WriteDeviceIO(UINT writeDoDat);
	int ReadDeviceIO(UINT& readDiDat);

	int WriteDeviceIODobit(int bit,UINT writeDoDat);
	int ReadDeviceIODobit(int bit, UINT& readDiDat);
	int ReadDeviceIODibit(int bit, UINT& readDiDat);

private:
	int m_CardID;
};

