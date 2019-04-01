#pragma once
#ifndef _GL_HELPER_H_
#define _GL_HELPER_H_
#include <Windows.h>
#include <stdint.h>

/**
 *	@name		GLInitContext
 *	@brief		graphic_glInit成功时创建的context等资源
 **/
struct GLInitContext
{
	HGLRC	hRC;
	HWND	hWindow;
	HDC		hDC;
	unsigned int	threadID;

	GLInitContext()
		: hWindow(NULL), hDC(NULL), hRC(NULL)
		, threadID(-1)
	{

	}
};

/**
 *	@name		graphic_glInit
 *	@brief		初始化opengl的context
 *	@param[out]	GLInitContext * initCtx 获取成功初始化之后的context、windows都相关信息句柄
 *	@return		int 0--成功 其他--失败
 **/
int graphic_glInit(GLInitContext* initCtx);

/**
 *	@name		graphic_glInit_SEH
 *	@brief		初始化opengl的context
 *				该函数调用graphic_glInit并尝试捕获可能的崩溃
 *	@param[out]	GLInitContext * initCtx 获取成功初始化之后的context、windows都相关信息句柄
 *	@return		int 0--成功 其他--失败
 **/
int graphic_glInit_SEH(GLInitContext* initCtx);

/**
 *	@name		graphic_glDeinit
 *	@brief		释放graphic_glInit时创建的context等资源
 *	@param[in]	GLInitContext * initCtx graphic_glInit成功时创建的context等资源
 *	@return		void
 **/
void graphic_glDeinit(GLInitContext* initCtx);

/**
 *	@name		graphic_getGLVersion
 *	@brief		获取opengl的版本号
 *	@param[out]	char * strGLVer	opengle版本号字符串
 *	@param[in]	int len 传入的strGLVer长度
 *	@return		int 0--成功 其他--失败
 **/
int graphic_getGLVersion(char* strGLVer, int len);

/**
 *	@name		graphic_getGLAdapter
 *	@brief		获取当前opengl的context使用的显卡名称
 *	@param[in]	char * strAdptName 当前opengl的context使用的显卡名称
 *	@param[in]	int len 传入的strGLVer长度
 *	@return		int 0--成功 其他--失败
 **/
int graphic_getGLAdapter(char* strAdptName, int len);

/**
 *	@name		graphic_copyTextureToMemory
 *	@brief		将OpenGL Texture的内容拷贝到本地内存中
 *	@param[in]	uint32_t textureId Texture名称
 *	@param[in]	uint32_t texTarget Texture target类型，现在只支持GL_TEXTURE_2D
 *	@param[in]	uint8_t * dstData 保存拷贝数据的目的地址
 *	@param[in]	int dstWidth 拷贝内容的宽
 *	@param[in]	int dstHeight 拷贝内容的高
 *	@param[in]	uint32_t dstGLFormat 拷贝内容的像素格式，GL_BGRA等
 *	@param[in]	uint32_t fbo 拷贝时使用的FrameBuffer
 *	@return		bool true--成功 false--失败
 **/
bool graphic_copyTextureToMemory( uint32_t textureId, uint32_t texTarget, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo );
bool graphic_copyTextureToMemory( uint32_t textureId, uint32_t texTarget, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo, uint32_t pboUp, uint32_t pboDown );
bool graphic_copyTextureToMemoryI420( uint32_t textureId, uint32_t texTarget, uint32_t texHeight, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo, uint32_t pboUp );

/**
 *	@name		graphic_checkFramebufferStatus
 *	@brief		调用glCheckFramebufferStatus检查当前绑定的Framebuffer状态十分正常
 *	@return		bool true--正常 false--不正常
 **/
bool graphic_checkFramebufferStatus();

/**
 *	@name		graphic_createAndSetupTextureFramebuffer
 *	@brief		创建Texture、Framebuffer、Renderbuffer对象
 *				将Texture绑定到Framebuffer的ATTACHMENT0，将Renderbuffer绑定到Framebuffer的DEPTH
 *	@param[in]	int width 宽
 *	@param[in]	int height 高
 *	@param[in]	uint32_t * texture 保存创建的Texture
 *	@param[in]	uint32_t * fbo 保存创建的Framebuffer
 *	@param[in]	uint32_t * rbo 保存创建的Renderbuffer
 *	@return		bool true--成功 false--失败
 **/
bool graphic_createAndSetupTextureFramebuffer(int width, int height, uint32_t* texture, uint32_t* fbo, uint32_t* rbo);

/**
 *	@name		createPBO
 *	@brief		创建PBO，并初始化申请内存
 *	@param[in]	uint32_t * pbo 保存成功创建的PBO
 *	@param[in]	int width 申请内存的宽
 *	@param[in]	int height 申请内存的高
 *	@return		bool true--成功  false--失败
 **/
bool createPBO(uint32_t* pbo, int width, int height);

/**
 *	@name		releasePBO
 *	@brief		安全释放PBO
 *	@param[in]	uint32_t * pbo 释放*pbo，并重置成0
 **/
void releasePBO(uint32_t* pbo);
#endif //_GL_HELPER_H_