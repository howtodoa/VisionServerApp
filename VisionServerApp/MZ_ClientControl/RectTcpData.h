#pragma once
#include <Windows.h>
#include <vector>
#include <list>

//////////////////////////////////////////////////////////////////////////

template<int Length = 1024, int MAX_CNT = 50, int Index = 1>
class RECEIVE_TCP_DATA
{

public:
	char m_pData[Length];			//数据缓冲区
	int m_nLen;									//数据的长度

	RECEIVE_TCP_DATA() {}
	RECEIVE_TCP_DATA(RECEIVE_TCP_DATA* pTcpData)
	{
		memcpy(m_pData, pTcpData->m_pData, Length * sizeof(char));
		m_nLen = pTcpData->m_nLen;
	}

	void* operator new(size_t nSize)
	{
		void* pContext = NULL;


		try
		{
			EnterCriticalSection(&s_IDLQueLock);
			if (NULL == s_Heap)
			{
				throw ((long)(__LINE__));
			}

			//std:: vector<RECEIVE_TCP_DATA* >::iterator iter = s_IDLQue.begin();
			std::list<RECEIVE_TCP_DATA* >::iterator iter = s_IDLQue.begin();
			if (s_IDLQue.end() != iter)
			{
				pContext = *iter;
				s_IDLQue.erase(iter);
			}
			else
			{
				pContext = HeapAlloc(s_Heap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, nSize);
				if (NULL != pContext)
				{
					++s_hisNewCount;
					++s_newRealCount;
				}
			}

			if (NULL == pContext)
			{
				throw ((long)(__LINE__));

			}
			LeaveCriticalSection(&s_IDLQueLock);
		}
		catch (const long& /*iErrCode*/)
		{
			pContext = NULL;
			LeaveCriticalSection(&s_IDLQueLock);
		}

		return pContext;
	}

	void operator delete(void* p)
	{
		if (p)
		{
			EnterCriticalSection(&s_IDLQueLock);
			const DWORD QUE_SIZE = (DWORD)(s_IDLQue.size());
			RECEIVE_TCP_DATA* const pContext = (RECEIVE_TCP_DATA*)p;

			if (QUE_SIZE <= MAX_IDL_DATA)
			{
				s_IDLQue.push_back(pContext);
			}
			else
			{
				++s_hisDeleteCount;
				--s_newRealCount;
				HeapFree(s_Heap, HEAP_NO_SERIALIZE, p);
			}
			LeaveCriticalSection(&s_IDLQueLock);
		}

		return;
	}

	static void InitReource()
	{
		if (NULL == s_Heap)
		{
			s_Heap = HeapCreate(0, S_PAGE_SIZE, 0);

			InitializeCriticalSection(&(s_IDLQueLock));

			//s_IDLQue.reserve(MAX_IDL_DATA );
		}
	}

	static void ReleaseReource()
	{
		if (NULL != s_Heap)
		{
			HeapDestroy(s_Heap);
			s_Heap = NULL;

			EnterCriticalSection(&(s_IDLQueLock));
			s_IDLQue.clear();
			LeaveCriticalSection(&(s_IDLQueLock));

			DeleteCriticalSection(&(s_IDLQueLock));
		}
	}

	enum
	{
		S_PAGE_SIZE = (Length + sizeof(int)),
		MAX_IDL_DATA = MAX_CNT,									//队列大小
		RCV_HEAP_SIZE = S_PAGE_SIZE * MAX_IDL_DATA,			//s_Heap堆的大小	
	};

public:
	static unsigned int s_newRealCount;
	static unsigned int s_hisNewCount;
	static unsigned int s_hisDeleteCount;
private:
	//static std::vector<RECEIVE_TCP_DATA* > s_IDLQue;
	static std::list<RECEIVE_TCP_DATA* > s_IDLQue;
	static CRITICAL_SECTION s_IDLQueLock;
	static HANDLE s_Heap;			//RCV_DATA的堆
};

template<int Length, int MAX_CNT, int Index>
HANDLE														RECEIVE_TCP_DATA<Length, MAX_CNT, Index>::s_Heap = NULL;

template<int Length, int MAX_CNT, int Index>
//std::vector<RECEIVE_TCP_DATA<Length, MAX_CNT,Index>* >		RECEIVE_TCP_DATA<Length, MAX_CNT,Index>::s_IDLQue;
std::list<RECEIVE_TCP_DATA<Length, MAX_CNT, Index>* >		RECEIVE_TCP_DATA<Length, MAX_CNT, Index>::s_IDLQue;

template<int Length, int MAX_CNT, int Index>
CRITICAL_SECTION											RECEIVE_TCP_DATA<Length, MAX_CNT, Index>::s_IDLQueLock;

template<int Length, int MAX_CNT, int Index>
unsigned int												RECEIVE_TCP_DATA<Length, MAX_CNT, Index>::s_newRealCount = 0;

template<int Length, int MAX_CNT, int Index>
unsigned int												RECEIVE_TCP_DATA<Length, MAX_CNT, Index>::s_hisNewCount = 0;

template<int Length, int MAX_CNT, int Index>
unsigned int												RECEIVE_TCP_DATA<Length, MAX_CNT, Index>::s_hisDeleteCount = 0;


