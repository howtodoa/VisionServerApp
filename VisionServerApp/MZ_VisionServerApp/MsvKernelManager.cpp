#include "StdAfx.h"
#include "MsvKernelManager.h"

MsvKernelManager * g_msvKernelMng = NULL;

MsvKernelManager::MsvKernelManager(void)
{
	//m_nCurCameraNum = 1;
	m_bDetectStartFlag = FALSE;
}


MsvKernelManager::~MsvKernelManager(void)
{
	StopWork();
}


void MsvKernelManager::MsvInitResource(void)
{

}

void MsvKernelManager::MsvReleaseResource(void)
{
}


int MsvKernelManager::StartWork()
{
	return 0;
}


int MsvKernelManager::StopWork()
{
	m_bDetectStartFlag = FALSE;

	return 0;
}