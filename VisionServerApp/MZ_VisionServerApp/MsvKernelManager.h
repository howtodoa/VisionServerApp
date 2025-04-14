#pragma once

class MsvKernelManager
{
public:
	MsvKernelManager(void);
	~MsvKernelManager(void);

public:
	static void MsvInitResource(void);
	static void MsvReleaseResource(void);

public:
	int StartWork();
	int StopWork();

public:

public:
	//int m_nCurCameraNum;
	volatile BOOL m_bDetectStartFlag;
};

extern MsvKernelManager * g_msvKernelMng;