#include "pch.h"
#include "CMainProcess.h"
//全局初始化
//采集线程将图片推入采集队列
//将采集队列图片推入计算线程并返回计算结果将计算结果推入结果队列
//将结果队列数据推入到发送线程


//收取图片线程
DWORD WINAPI ThreadImageDateProcess(LPVOID lpVoid)
{
	CMainProcess* pThis = (CMainProcess*)lpVoid;
	while (1)
	{
		break;
	}
	return 0;
}
//将图片推入算法库
DWORD WINAPI ThreadPushAlgorthmProcess(LPVOID lpVoid)
{
	CMainProcess* pThis = (CMainProcess*)lpVoid;
	while (1)
	{
		break;
	}
	return 0;
}
//从算法库获取结果并推入结果队列
DWORD WINAPI ThreadGetAlgorthmProcess(LPVOID lpVoid)
{
	CMainProcess* pThis = (CMainProcess*)lpVoid;
	while (1)
	{
		break;
	}
	return 0;
}
//将结果队列数据发送
DWORD WINAPI ThreadSendResultProcess(LPVOID lpVoid)
{
	CMainProcess* pThis = (CMainProcess*)lpVoid;
	while (1)
	{
		break;
	}
	return 0;
}

///////////////客户端回调处理函数
static void ClientCameraParam()
{
	return;
}
static void ClientSystemParam()
{
	return;
}
static void ClientIOBoardParam()
{
	return;
}
static void ClientAlgorithmParam()
{
	return;
}
///////////////////////////图片队列
void CMainProcess::Image_Fifo_Push(cv::Mat* temp, int Index)
{
	m_StlLock_Image[Index]->lock();
	if (List_TotalImage[Index].size() > MAX_VEC_IMAGE_NUM)
	{
		cv::Mat* iter = List_TotalImage[Index][0];
		if (iter != NULL)
		{
			delete iter;
			iter = NULL;
		}
		List_TotalImage[Index].erase(List_TotalImage[Index].begin());
	}

	List_TotalImage[Index].push_back(temp);
	m_StlLock_Image[Index]->unlock();
}
cv::Mat* CMainProcess::Image_Fifo_Get(int Index)
{
	cv::Mat* temp = NULL;
	m_StlLock_Image[Index]->lock();
	if (List_TotalImage[Index].size() > 0)
	{
		temp = new cv::Mat(List_TotalImage[Index][0]->clone());

		cv::Mat* iter = List_TotalImage[Index][0];
		if (iter != NULL)
		{
			iter->release();
			delete iter;
			iter = NULL;
		}
		List_TotalImage[Index].erase(List_TotalImage[Index].begin());
	}
	m_StlLock_Image[Index]->unlock();
	return temp;
}
int CMainProcess::Image_Fifo_Length(int Index)
{
	m_StlLock_Image[Index]->lock();
	int temp = (int)List_TotalImage[Index].size();
	m_StlLock_Image[Index]->unlock();
	return temp;
}
void CMainProcess::Image_Fifo_Init(int Index)
{
	m_StlLock_Image[Index]->lock();
	List_TotalImage[Index].clear();
	m_StlLock_Image[Index]->unlock();
	return;
}
//
void CMainProcess::Result_Fifo_Push(ClacResult* temp, int Index)
{
}
ClacResult* CMainProcess::Result_Fifo_Get(int Index)
{
	return nullptr;
}
int CMainProcess::Result_Fifo_Length(int Index)
{
	return 0;
}
void CMainProcess::Result_Fifo_Init(int Index)
{
}
/// <summary>
///结果队列
/// </summary>
/// <returns></returns>

int CMainProcess::InitSystem()
{
	return 0;
}

int CMainProcess::LoadParamFile()
{
	return 0;
}

int CMainProcess::StartWorkProcess()
{
	return 0;
}

int CMainProcess::StopWorkProcess()
{
	return 0;
}
