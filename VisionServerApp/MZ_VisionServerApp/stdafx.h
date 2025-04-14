// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "winsock2.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>

#define FLOAT_ZERO 0.000001f


#ifdef UNDER_CE
# pragma comment(lib,"winsock.lib")
#else
# pragma comment(lib,"ws2_32.lib")
#endif

#include "MZ_Device.h"
#pragma comment(lib, "MZ_CameraDevice.lib")

//#ifdef _DEBUG
//#pragma comment(lib, "opencv_core2410d.lib")
//#pragma comment(lib, "opencv_highgui2410d.lib")
//#pragma comment(lib, "opencv_imgproc2410d.lib")
//#else
//#pragma comment(lib, "opencv_core2410.lib")
//#pragma comment(lib, "opencv_highgui2410.lib")
//#pragma comment(lib, "opencv_imgproc2410.lib")
//#endif


#include "./../MZ_Log/Mz_LogConn.h"
#define SystemLog (Mz_Log::COperation::Mz_GetInstance())


// #include "MsvKernelManager.h"
// extern MsvKernelManager * g_msvKernelMng;

#define C_FIELD_INDEX 1

#define FEATURE_NUM	60


#if defined(_WIN32) && defined(_MSC_VER)
#include <windows.h>
extern double abtic();
#else
#include <time.h>
#include <sys/time.h>
extern double abtic();
#endif /* _WIN32 */
