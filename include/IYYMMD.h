#pragma once
#define YY_MMD_API __declspec(dllexport)

#include <Windows.h>

#define TEXTURE_WIDTH	 1280
#define TEXTURE_HEIGHT   720

EXTERN_C
{
class IYYMMD
{
public:
	// ��ʼ��dll����������߰汾��
	virtual bool init(const char* version) = 0;
	// д��ģ���ļ�,������ԣ��Ƚ�vmd����Ƶ�ļ�·��Ҳ������,Ŀǰ���Դ���
	virtual void setModelPath(const char* modePath, const char* motionPath, const char* musicPath) = 0;
	// ��Ⱦ
	virtual void render() = 0;
	// д���������
	virtual void setBoneData(const float* data) = 0;
	// ������������texture
	virtual void copyOfTextureData(unsigned char* dst) = 0;
	// ������־��ӡ·��
	virtual void setLogPath(const char* logPath) = 0;
	// ����ʼ��dll
	virtual bool unInit() = 0;
};

//��ȡIYYMMDָ��
YY_MMD_API IYYMMD* GetInstance();
};