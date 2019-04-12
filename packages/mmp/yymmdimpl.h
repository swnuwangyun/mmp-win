#pragma once
#include "IYYMMD.h"
#include "QcCriticalLock.h"
#include "viewer.h"

class YYMMDImpl : public YYMMD::IYYMMD
{
public:
	YYMMDImpl();
	virtual ~YYMMDImpl();
	static YYMMDImpl *GetInstance();

	virtual bool init(const char* version);
	virtual void setModelPath(const char* modePath, const char* motionPath, const char* musicPath);
	virtual void setBoneAnimationFlag(bool flag);
	virtual void render();
	virtual void updateBoneData(const BoneData* item, const int len);
	virtual void copyOfTextureData(unsigned char* dst);
	virtual void setLogPath(const char* logPath);
	virtual bool unInit();

public:
	// 用于区分是否本工程调用还是外界DLL调用
	void setMainThread(DWORD threadId);

private:
	void resetViewer();
	void pmxVmdPathTranster(const char* modePath, const char* motionPath, std::string& dstModePath, std::string& dstmotionPath);

private:
	QcCriticalLock   m_pmxvmdlock;
	HMODULE			 m_hModOF_glew;
	static YYMMDImpl *m_instance;
	Viewer* m_viewer;

	DWORD m_threadId;
};