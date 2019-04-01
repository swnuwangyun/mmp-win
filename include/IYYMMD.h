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
	// 初始化dll，输入调用者版本号
	virtual bool init(const char* version) = 0;
	// 写入模型文件,方便测试，先将vmd、音频文件路径也传过来,目前可以传空
	virtual void setModelPath(const char* modePath, const char* motionPath, const char* musicPath) = 0;
	// 渲染
	virtual void render() = 0;
	// 写入骨骼数据
	virtual void setBoneData(const float* data) = 0;
	// 处理完成输出的texture
	virtual void copyOfTextureData(unsigned char* dst) = 0;
	// 设置日志打印路径
	virtual void setLogPath(const char* logPath) = 0;
	// 反初始化dll
	virtual bool unInit() = 0;
};

//获取IYYMMD指针
YY_MMD_API IYYMMD* GetInstance();
};