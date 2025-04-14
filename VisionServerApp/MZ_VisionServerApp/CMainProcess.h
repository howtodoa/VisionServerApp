#pragma once
#include "TC_ThreadCond.h"
#include <vector>

#define MAX_VEC_IMAGE_NUM 100
using namespace std;
struct ClacResult
{

};
class CMainProcess
{
public:
	CMainProcess();
	~CMainProcess();
	
	//
	int InitSystem();
	int LoadParamFile();
	int StartWorkProcess();
	int StopWorkProcess();
public:
	//图片队列操作
	void Image_Fifo_Push(cv::Mat* temp, int Index);
	cv::Mat* Image_Fifo_Get(int Index);
	int Image_Fifo_Length(int Index);
	void Image_Fifo_Init(int Index);
	//结果队列操作
	void Result_Fifo_Push(ClacResult* temp, int Index);
	ClacResult* Result_Fifo_Get(int Index);
	int Result_Fifo_Length(int Index);
	void Result_Fifo_Init(int Index);
private:
	vector<Synchronized*>m_StlLock_Image;
	vector<vector<cv::Mat*>>List_TotalImage;
};

