#pragma once
#define YY_MMD_API __declspec(dllexport)

#include <Windows.h>

typedef __int32								MMDInt32;
typedef unsigned __int32                    MMDUInt32;

typedef struct _MMD_Texture
{
	MMDUInt32 textureID;                     /* OpenGL texture id.*/
	MMDUInt32 textureID_a;                   /* OpenGL texture id.*/
	MMDUInt32 target;                        /* OpenGL texture target, e.g. GL_TEXTURE_2D, GL_TEXTURE_EXTERNAL_OES.*/
	MMDUInt32 format;                        /* OpenGL texture format, e.g. GL_RGBA.*/
	MMDUInt32 pixelComponentFormat;          /* OpenGL texture pixel component size. e.g. rgba4444 texture format is GL_RGBA, pixelComponentFormat is _GL_UNSIGNED_SHORT_4_4_4_4 */
	MMDInt32 width;                          /* OpenGL texture width.*/
	MMDInt32 height;                         /* OpenGL texture height.*/
} MMD_Texture;

EXTERN_C
{
class IYYMMD
{
	// 初始化dll，输入调用者版本号
	virtual bool init(const char* version) = 0;
	// 写入模型文件,方便测试，先将vmd、音频文件路径也传过来
	virtual void setModelPath(const char* modePath, const char* motionPath, const char* musicPath) = 0;
	// 写入骨骼数据
	virtual void setBoneData(const float* data) = 0;
	// 处理完成输出的texture
	virtual void copyOfTexture(MMD_Texture* dst) = 0;
	// 设置日志打印路径
	virtual void setLogPath(const char* logPath) = 0;
	// 反初始化dll
	virtual bool unInit() = 0;
};

//获取IYYMMD指针
YY_MMD_API IYYMMD* GetInstance();
};