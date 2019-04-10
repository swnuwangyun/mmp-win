#pragma once
#define YY_MMD_API extern "C" __declspec(dllexport)

#include <Windows.h>
#include <vector>

#define TEXTURE_WIDTH	 1280
#define TEXTURE_HEIGHT   720

// 可能是kinect
struct BoneData
{
	wchar_t* name;
	float x;
	float y;
	float z;
};

namespace YYMMD
{
	class IYYMMD
	{
	public:
		// 初始化dll，输入调用者版本号
		virtual bool init(const char* version) = 0;
		// 设置日志打印路径
		virtual void setLogPath(const char* logPath) = 0;

		// 写入模型文件,若写入运动文件，则会启用运动文件的坐标驱动模型，若为空，则启用updateBoneData写入的骨骼数据
		virtual void setModelPath(const char* modePath, const char* motionPath, const char* musicPath) = 0;

		//这里插入代码加载或卸载mmd模型
		virtual void setBoneAnimationFlag(bool flag) = 0;

		// 渲染
		virtual void render() = 0;

		// 写入骨骼数据
		virtual void updateBoneData(const BoneData* item, const int len) = 0;

		// 处理完成输出的texture
		virtual void copyOfTextureData(unsigned char* dst) = 0;

		// 反初始化dll
		virtual bool unInit() = 0;
	};

	//获取IYYMMD指针
	YY_MMD_API IYYMMD* GetInstance();
}