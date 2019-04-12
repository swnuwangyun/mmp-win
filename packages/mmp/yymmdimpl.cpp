#include "yymmdimpl.h"
#include "pmxvLogger.h"
#include "libpath.h"
#include "libtext.h"

YYMMDImpl* YYMMDImpl::m_instance = NULL;

YYMMDImpl::YYMMDImpl()
: m_viewer(NULL)
, m_hModOF_glew(NULL)
, m_threadId(0)
{
}

YYMMDImpl::~YYMMDImpl()
{
}

YYMMDImpl *YYMMDImpl::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new YYMMDImpl();
	}
	return m_instance;
}

bool YYMMDImpl::init(const char* version)
{	
	//std::wstring path = libpath::combine(libpath::getThisDllPath(), L"of_glew.dll");
	//m_hModOF_glew = LoadLibraryEx(path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	//if (NULL == m_hModOF_glew)
	//{
	//	log(libtext::format("Failed to load of_glew.dll, lastError=", GetLastError()));
	//	return false;
	//}
	return true;
}

void YYMMDImpl::setModelPath(const char* modePath, const char* motionPath, const char* musicPath)
{
	if (m_viewer)
	{
		QcAutoCriticalLock lock(m_pmxvmdlock);
		setBoneAnimationFlag(false);
	}

	std::string model_file, motion_file;
	pmxVmdPathTranster(modePath, motionPath, model_file, motion_file);
	m_viewer = new Viewer(model_file, motion_file, musicPath, m_threadId != GetCurrentThreadId());
	m_viewer->init();
}

void YYMMDImpl::setBoneAnimationFlag(bool flag)
{
	if (m_viewer)
	{
		m_viewer->setBoneAnimationFlag(flag);
		if (!flag)
		{
			m_viewer = NULL;
		}
	}
}

void YYMMDImpl::render()
{
	QcAutoCriticalLock lock(m_pmxvmdlock);

	if (m_viewer)
		m_viewer->run();
}

void YYMMDImpl::updateBoneData(const BoneData* item, const int len)
{
	if (m_viewer)
		m_viewer->updateBoneData(item, len);
}

void YYMMDImpl::copyOfTextureData(unsigned char* dst)
{
	if (m_viewer)
		m_viewer->copyOfTextureData(dst);
}

void YYMMDImpl::setLogPath(const char* logPath)
{
	pmxvLogger::get()->setLogPath(logPath);
}

bool YYMMDImpl::unInit()
{
	if (m_viewer)
		m_viewer->unint();

	delete pmxvLogger::get();
	return true;
}

void YYMMDImpl::setMainThread(DWORD threadId)
{
	m_threadId = threadId;
}

void YYMMDImpl::resetViewer()
{
	if(m_viewer)
	{
		// 后续如果要更改模型，这里需要重置，暂时未实现
	}
}

void YYMMDImpl::pmxVmdPathTranster(const char* modePath, const char* motionPath, 
	std::string& dstModePath, std::string& dstmotionPath)
{
	std::string modestr = modePath;
	std::string vmdstr = motionPath;

	if (!modestr.empty() && modestr.substr(modestr.size() - 4) == ".pmx")
	{
		dstModePath = modestr;
		std::wstring wmodel_file = libtext::string2wstring(dstModePath);
		wmodel_file = libtext::replace(wmodel_file, L"\\", L"/");
		dstModePath = libtext::wstring2string(wmodel_file);
	}
	if (!vmdstr.empty() && vmdstr.substr(vmdstr.size() - 4) == ".vmd")
	{
		dstmotionPath = vmdstr;
		std::wstring wmotion_file = libtext::string2wstring(dstmotionPath);
		wmotion_file = libtext::replace(wmotion_file, L"\\", L"/");
		dstmotionPath = libtext::wstring2string(wmotion_file);
	}
}