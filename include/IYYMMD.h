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
	// ��ʼ��dll����������߰汾��
	virtual bool init(const char* version) = 0;
	// д��ģ���ļ�,������ԣ��Ƚ�vmd����Ƶ�ļ�·��Ҳ������
	virtual void setModelPath(const char* modePath, const char* motionPath, const char* musicPath) = 0;
	// д���������
	virtual void setBoneData(const float* data) = 0;
	// ������������texture
	virtual void copyOfTexture(MMD_Texture* dst) = 0;
	// ������־��ӡ·��
	virtual void setLogPath(const char* logPath) = 0;
	// ����ʼ��dll
	virtual bool unInit() = 0;
};

//��ȡIYYMMDָ��
YY_MMD_API IYYMMD* GetInstance();
};