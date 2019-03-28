#pragma once
#include <windows.h>

class QcCriticalLock 
{
public:
	QcCriticalLock()
	{	
		InitializeCriticalSection(&m_csLock);
	}

	~QcCriticalLock()
	{
		DeleteCriticalSection(&m_csLock);
	}

	void lock()
	{
		EnterCriticalSection(&m_csLock);
	}
	void unlock() 
	{
		LeaveCriticalSection(&m_csLock);
	}
	bool tryLock()
	{
		BOOL bRet = TryEnterCriticalSection(&m_csLock);
		if (bRet)
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
private:
	CRITICAL_SECTION	m_csLock;
};

class QcAutoCriticalLock
{
public:
	QcAutoCriticalLock(QcCriticalLock& csLock) : m_csLock(csLock)
	{
		m_csLock.lock();
	}

	~QcAutoCriticalLock()
	{
		m_csLock.unlock();
	}
private:
	QcCriticalLock& m_csLock;
};

class QcAutoSafeLock
{
public:
	QcAutoSafeLock(QcCriticalLock& csLock) : m_csLock(csLock), m_bLock(false)
	{
	}

	~QcAutoSafeLock()
	{
		unLock();
	}
	bool tryLock(void)
	{
		if (!m_bLock)
		{
			m_bLock = m_csLock.tryLock();
			return m_bLock;
		}
		else
			return false;
	}

	bool unLock(void)
	{
		if (m_bLock)
		{
			m_bLock = false;
			m_csLock.unlock();
		}
		else
			return false;
		return true;
	}

private:
	QcCriticalLock& m_csLock;
	bool m_bLock;
};

#define QmCsLocker(cs) QcAutoCriticalLock QmUniqueVarName(cs)


