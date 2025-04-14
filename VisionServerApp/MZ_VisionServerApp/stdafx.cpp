// stdafx.cpp : source file that includes just the standard includes
// MsvGSISys.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

//MsvKernelManager * g_msvKernelMng = NULL;



#if defined(_WIN32) && defined(_MSC_VER)
#include <windows.h>
double abtic() {
	__int64 freq;
	__int64 clock;
	QueryPerformanceFrequency( (LARGE_INTEGER *)&freq );
	QueryPerformanceCounter( (LARGE_INTEGER *)&clock );
	return (double)clock/freq*1000*1000;
}
#else
#include <time.h>
#include <sys/time.h>
double abtic() {
	double result = 0.0;
	struct timeval tv;
	gettimeofday( &tv, NULL );
	result = tv.tv_sec*1000*1000 + tv.tv_usec;
	return result;
}
#endif /* _WIN32 */