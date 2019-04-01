#pragma once
#ifndef _GL_HELPER_H_
#define _GL_HELPER_H_
#include <Windows.h>
#include <stdint.h>

/**
 *	@name		GLInitContext
 *	@brief		graphic_glInit�ɹ�ʱ������context����Դ
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
 *	@brief		��ʼ��opengl��context
 *	@param[out]	GLInitContext * initCtx ��ȡ�ɹ���ʼ��֮���context��windows�������Ϣ���
 *	@return		int 0--�ɹ� ����--ʧ��
 **/
int graphic_glInit(GLInitContext* initCtx);

/**
 *	@name		graphic_glInit_SEH
 *	@brief		��ʼ��opengl��context
 *				�ú�������graphic_glInit�����Բ�����ܵı���
 *	@param[out]	GLInitContext * initCtx ��ȡ�ɹ���ʼ��֮���context��windows�������Ϣ���
 *	@return		int 0--�ɹ� ����--ʧ��
 **/
int graphic_glInit_SEH(GLInitContext* initCtx);

/**
 *	@name		graphic_glDeinit
 *	@brief		�ͷ�graphic_glInitʱ������context����Դ
 *	@param[in]	GLInitContext * initCtx graphic_glInit�ɹ�ʱ������context����Դ
 *	@return		void
 **/
void graphic_glDeinit(GLInitContext* initCtx);

/**
 *	@name		graphic_getGLVersion
 *	@brief		��ȡopengl�İ汾��
 *	@param[out]	char * strGLVer	opengle�汾���ַ���
 *	@param[in]	int len �����strGLVer����
 *	@return		int 0--�ɹ� ����--ʧ��
 **/
int graphic_getGLVersion(char* strGLVer, int len);

/**
 *	@name		graphic_getGLAdapter
 *	@brief		��ȡ��ǰopengl��contextʹ�õ��Կ�����
 *	@param[in]	char * strAdptName ��ǰopengl��contextʹ�õ��Կ�����
 *	@param[in]	int len �����strGLVer����
 *	@return		int 0--�ɹ� ����--ʧ��
 **/
int graphic_getGLAdapter(char* strAdptName, int len);

/**
 *	@name		graphic_copyTextureToMemory
 *	@brief		��OpenGL Texture�����ݿ����������ڴ���
 *	@param[in]	uint32_t textureId Texture����
 *	@param[in]	uint32_t texTarget Texture target���ͣ�����ֻ֧��GL_TEXTURE_2D
 *	@param[in]	uint8_t * dstData ���濽�����ݵ�Ŀ�ĵ�ַ
 *	@param[in]	int dstWidth �������ݵĿ�
 *	@param[in]	int dstHeight �������ݵĸ�
 *	@param[in]	uint32_t dstGLFormat �������ݵ����ظ�ʽ��GL_BGRA��
 *	@param[in]	uint32_t fbo ����ʱʹ�õ�FrameBuffer
 *	@return		bool true--�ɹ� false--ʧ��
 **/
bool graphic_copyTextureToMemory( uint32_t textureId, uint32_t texTarget, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo );
bool graphic_copyTextureToMemory( uint32_t textureId, uint32_t texTarget, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo, uint32_t pboUp, uint32_t pboDown );
bool graphic_copyTextureToMemoryI420( uint32_t textureId, uint32_t texTarget, uint32_t texHeight, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo, uint32_t pboUp );

/**
 *	@name		graphic_checkFramebufferStatus
 *	@brief		����glCheckFramebufferStatus��鵱ǰ�󶨵�Framebuffer״̬ʮ������
 *	@return		bool true--���� false--������
 **/
bool graphic_checkFramebufferStatus();

/**
 *	@name		graphic_createAndSetupTextureFramebuffer
 *	@brief		����Texture��Framebuffer��Renderbuffer����
 *				��Texture�󶨵�Framebuffer��ATTACHMENT0����Renderbuffer�󶨵�Framebuffer��DEPTH
 *	@param[in]	int width ��
 *	@param[in]	int height ��
 *	@param[in]	uint32_t * texture ���洴����Texture
 *	@param[in]	uint32_t * fbo ���洴����Framebuffer
 *	@param[in]	uint32_t * rbo ���洴����Renderbuffer
 *	@return		bool true--�ɹ� false--ʧ��
 **/
bool graphic_createAndSetupTextureFramebuffer(int width, int height, uint32_t* texture, uint32_t* fbo, uint32_t* rbo);

/**
 *	@name		createPBO
 *	@brief		����PBO������ʼ�������ڴ�
 *	@param[in]	uint32_t * pbo ����ɹ�������PBO
 *	@param[in]	int width �����ڴ�Ŀ�
 *	@param[in]	int height �����ڴ�ĸ�
 *	@return		bool true--�ɹ�  false--ʧ��
 **/
bool createPBO(uint32_t* pbo, int width, int height);

/**
 *	@name		releasePBO
 *	@brief		��ȫ�ͷ�PBO
 *	@param[in]	uint32_t * pbo �ͷ�*pbo�������ó�0
 **/
void releasePBO(uint32_t* pbo);
#endif //_GL_HELPER_H_