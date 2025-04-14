#include "pch.h"
#include "CPCIe1230.h"


CPCIe1230::CPCIe1230()
:m_CardID(-1)
{
}

CPCIe1230::~CPCIe1230()
{
}

int CPCIe1230::OpenDevice(int CardID)
{
	return  Pci1230Open(CardID);
}

int CPCIe1230::CloseDevice()
{
	return Pci1230Close(m_CardID);
}

void CPCIe1230::SetCardID(int inputID)
{
	m_CardID = inputID;
}
int CPCIe1230::WriteDeviceIO(UINT writeDoDat)
{
	return Pci1230Write(m_CardID, writeDoDat);
}

int CPCIe1230::ReadDeviceIO(UINT& readDiDat)
{
	return Pci1230Read(m_CardID, &readDiDat);
}

int CPCIe1230::WriteDeviceIODobit(int bit, UINT writeDoDat)
{
	return Pci1230ReadDiBit(m_CardID, bit, &writeDoDat);
}

int CPCIe1230::ReadDeviceIODobit(int bit, UINT& readDiDat)
{
	return Pci1230ReadDoBit(m_CardID, bit, &readDiDat);
}

int CPCIe1230::ReadDeviceIODibit(int bit, UINT& readDiDat)
{
	return Pci1230ReadDiBit(m_CardID, bit, &readDiDat);
}

