#include "glHelper.h"
#include "gl/glew.h"
#include <gl/wglew.h>
#include <iostream>

#pragma comment(lib, "Opengl32.lib")

#define TAG "glHelper"

LRESULT WINAPI ESWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool WinCreate(HWND& _hWindow);

/*GRAPHIC_EXPORT*/int graphic_glInit_SEH(GLInitContext* initCtx)
{
	int result = -1;
	__try{
		result = graphic_glInit(initCtx);
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		return -100;
	}
	return result;
}

/*GRAPHIC_EXPORT*/ int graphic_glInit( GLInitContext* initCtx )
{
	if(NULL==initCtx)
		return -1;
	HWND _hWindow = NULL;
	HGLRC _hRC = NULL;
	HDC _hDC = NULL;

	// 创建一个隐藏的窗口
	if (!WinCreate(_hWindow))
	{
		return -2;
	}

	// 获取窗口DC
	_hDC = ::GetDC(_hWindow);
	if (_hDC == NULL)
	{
		::DestroyWindow(_hWindow);
		::UnregisterClassW(L"glHelper-class", GetModuleHandle(NULL));
		return -3;
	}

	int nPixelFormat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA,					// RGBA Color mode
		32,								// Want 32 bit color 
		0, 0, 0, 0, 0, 0,			    // Not used to select mode
		0, 0,							// Not used to select mode
		0, 0, 0, 0, 0,                  // Not used to select mode
		16,								// Size of depth buffer
		0,								// Not used 
		0,								// Not used 
		0,	            				// Not used 
		0,								// Not used 
		0, 0, 0 };						// Not used 

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(_hDC, &pfd);

	if (nPixelFormat == 0)
	{
		DWORD errorCode = GetLastError();

		::ReleaseDC(_hWindow, _hDC);
		::DestroyWindow(_hWindow);
		::UnregisterClassW(L"glHelper-class", GetModuleHandle(NULL));
		return -4;
	}


	// Set the pixel format for the device context
	BOOL bSet = ::SetPixelFormat(_hDC, nPixelFormat, &pfd);
	if (bSet == FALSE)
	{
		::ReleaseDC(_hWindow, _hDC);
		::DestroyWindow(_hWindow);
		::UnregisterClassW(L"glHelper-class", GetModuleHandle(NULL));
		return -5;
	}

	// Create OGL context and make it current
	_hRC = ::wglCreateContext(_hDC);

	if (_hRC == NULL)
	{
		DWORD errorCode = GetLastError();

		::ReleaseDC(_hWindow, _hDC);
		::DestroyWindow(_hWindow);
		::UnregisterClassW(L"glHelper-class", GetModuleHandle(NULL));
		return -6;
	}

	BOOL bMake = ::wglMakeCurrent(_hDC, _hRC);
	if (bMake == FALSE)
	{
		DWORD errorCode = ::GetLastError();

		::wglDeleteContext(_hRC);
		::ReleaseDC(_hWindow, _hDC);
		::DestroyWindow(_hWindow);
		::UnregisterClassW(L"glHelper-class", GetModuleHandle(NULL));
		return -7;
	}

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		return -8;
	}

	initCtx->hWindow = _hWindow;
	initCtx->hDC = _hDC;
	initCtx->hRC = _hRC;
	initCtx->threadID = GetCurrentThreadId();
	return 0;
}

/*GRAPHIC_EXPORT*/ void graphic_glDeinit( GLInitContext* initCtx )
{
	if(NULL==initCtx)
		return;
	::wglMakeCurrent(NULL, NULL);
	::wglDeleteContext(initCtx->hRC);
	initCtx->hRC = NULL;
	::ReleaseDC(initCtx->hWindow, initCtx->hDC);
	::DestroyWindow(initCtx->hWindow);
	::UnregisterClassW(L"glHelper-class", GetModuleHandle(NULL));
	initCtx->hDC = NULL;
	initCtx->hWindow = NULL;
}

/*GRAPHIC_EXPORT*/ int graphic_getGLVersion( char* strGLVer, int len )
{
	const char* ver = (const char*)glGetString(GL_VERSION);
	if(NULL==ver)
		return -1;
	int verlen = strlen(ver);
	if(verlen>=len)
		return -2;
	strcpy_s(strGLVer, len, ver);
	len = verlen;
	return 0;
}

int graphic_getGLAdapter( char* strAdptName, int len )
{
	const char* ver = (const char*)glGetString(GL_RENDERER);
	if(NULL==ver)
		return -1;
	int verlen = strlen(ver);
	if(verlen>=len)
		return -2;
	strcpy_s(strAdptName, len, ver);
	len = verlen;
	return 0;
}

bool graphic_copyTextureToMemory( uint32_t textureId, uint32_t texTarget, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo )
{
	if (dstData != NULL && fbo!=0)
	{
		GLint oldFBO;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
		GLenum err = glGetError();
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		err = glGetError();
		//if (m_outputTexture.format == GL_DEPTH_COMPONENT)
		//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_outputTexture.target, m_outputTexture.textureID, 0);
		//else
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, /*m_outputTexture.target*/texTarget, /*m_outputTexture.textureID*/textureId, 0);
			err = glGetError();
		if(!graphic_checkFramebufferStatus())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
			return false;
		}
		glReadPixels(0, 0, /*m_outputTexture.width*/dstWidth, /*m_outputTexture.height*/dstHeight, dstGLFormat, GL_UNSIGNED_BYTE, dstData);
		err = glGetError();
		glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
		if (err != GL_NO_ERROR)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool graphic_copyTextureToMemory( uint32_t textureId, uint32_t texTarget, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo, uint32_t pboUp, uint32_t pboDown )
{
	if (dstData != NULL)
	{
		GLint oldFBO;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
		GLenum err = glGetError();
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		err = glGetError();
		//if (m_outputTexture.format == GL_DEPTH_COMPONENT)
		//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_outputTexture.target, m_outputTexture.textureID, 0);
		//else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, /*m_outputTexture.target*/texTarget, /*m_outputTexture.textureID*/textureId, 0);
		err = glGetError();
		if(!graphic_checkFramebufferStatus())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
			return false;
		}

		if(pboUp && pboDown)
		{
			// 先将Framebuffer绑定的Texture复制到PBO中
			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboUp);
			glReadPixels(0, 0, dstWidth, dstHeight/2, dstGLFormat, GL_UNSIGNED_BYTE, 0);

			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboDown);
			glReadPixels(0, dstHeight/2, dstWidth, dstHeight/2, dstGLFormat, GL_UNSIGNED_BYTE, 0);

			uint8_t* pDstData = dstData;
			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboUp);
			GLubyte* srcPBO = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY);
			if(srcPBO)
			{
				memcpy(dstData, srcPBO, dstWidth * (dstHeight/2) * 4);
				//glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
			}
			else
			{
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
				return false;
			}
			pDstData = dstData + dstWidth * 4 * (dstHeight/2);
			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboDown);
			GLubyte* srcPBODown = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY);
			if(srcPBODown)
			{
				memcpy(pDstData, srcPBODown, dstWidth * (dstHeight/2) * 4);
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboUp);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboDown);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
			}
			else
			{
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboUp);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
				return false;
			}
			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
		}
		else
		{
			glReadPixels(0, 0, dstWidth, dstHeight, dstGLFormat, GL_UNSIGNED_BYTE, dstData);
		}
		err = glGetError();
		glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
		if (err != GL_NO_ERROR)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool graphic_copyTextureToMemoryI420( uint32_t textureId, uint32_t texTarget, uint32_t texHeight, uint8_t* dstData, int dstWidth, int dstHeight, uint32_t dstGLFormat, uint32_t fbo, uint32_t pboUp )
{
	if(dstData==NULL)
		return false;
	if(0==pboUp)
		return false;
	GLint oldFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
	GLenum err = glGetError();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	err = glGetError();
	//if (m_outputTexture.format == GL_DEPTH_COMPONENT)
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_outputTexture.target, m_outputTexture.textureID, 0);
	//else
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, /*m_outputTexture.target*/texTarget, /*m_outputTexture.textureID*/textureId, 0);
	err = glGetError();
	if(!graphic_checkFramebufferStatus())
	{
		glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
		return false;
	}

	// 先将Framebuffer绑定的Texture复制到PBO中
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboUp);
	glReadPixels(0, 0, dstWidth, texHeight, dstGLFormat, GL_UNSIGNED_BYTE, 0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboUp);
	GLubyte* srcPBO = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY);
	if(srcPBO)
	{
		int length = dstWidth * dstHeight; 
		int srcpos = 0;
		int dstpos = 0;
		// copy Y
		memcpy(dstData + dstpos, srcPBO+srcpos, length);
		// copy UV
		srcpos += length;
		dstpos += length;
		length = dstWidth * dstHeight / 2;
		memcpy(dstData + dstpos, srcPBO+srcpos, length);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	}
	else
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
		return false;
	}
	return true;
}

bool graphic_checkFramebufferStatus()
{
	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "Framebuffer complete." << std::endl;
		return true;


	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
		return false;


	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
		return false;
		/*
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
		return false;


		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
		std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
		return false;
		*/
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
		return false;


	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
		return false;


	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
		return false;


	default:
		std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
		return false;
	}
}

bool graphic_createAndSetupTextureFramebuffer(int width, int height, uint32_t* texture, uint32_t* fbo, uint32_t* rbo)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);       // Enable Texture Mapping
	glShadeModel(GL_SMOOTH);       // Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);    // Black Background
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);

	GLenum err;

	glGenTextures(1, texture);
	if(0==*texture)
	{
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	err = glGetError();
	glGenFramebuffers(1, fbo);
	if(0==*fbo)
	{
		glDeleteTextures(1, texture);
		*texture = 0;
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
	err = glGetError();

	// create a renderbuffer object to store depth info
	// NOTE: A depth renderable image should be attached the FBO for depth test.
	// If we don't attach a depth renderable image to the FBO, then
	// the rendering output will be corrupted because of missing depth test.
	// If you also need stencil test for your rendering, then you must
	// attach additional image to the stencil attachement point, too.
	glGenRenderbuffers(1, rbo);
	if(0==*rbo)
	{
		glDeleteTextures(1, texture);
		*texture = 0;
		glDeleteFramebuffers(1, fbo);
		*fbo = 0;
		return false;
	}
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach a texture to FBO color attachement point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);
	err = glGetError();

	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *rbo);
	err = glGetError();

	if(!graphic_checkFramebufferStatus())
	{
		glDeleteTextures(1, texture);
		*texture = 0;
		glDeleteFramebuffers(1, fbo);
		*fbo = 0;
		glDeleteRenderbuffers(1, rbo);
		*rbo = 0;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

/////////////////////////////// 以下为局部函数 ///////////////////////////////////
LRESULT WINAPI ESWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT  lRet = 1;
	return lRet;
}

bool WinCreate(HWND& _hWindow)
{
	WNDCLASSW wndclass = { 0 };
	DWORD    wStyle = 0;
	RECT     windowRect;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	wndclass.style = CS_OWNDC;
	wndclass.lpfnWndProc = (WNDPROC)ESWindowProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;//(HBRUSH)GetStockObject(BLACK_BRUSH);

	wchar_t* w_className = L"glHelper-class";
	wndclass.lpszClassName = w_className;

	if (!RegisterClassW(&wndclass))
	{
		int err = GetLastError();
		if(err!=ERROR_CLASS_ALREADY_EXISTS)
		{
			return FALSE;
		}
	}

	wStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	// Adjust the window rectangle so that the client area has
	// the correct number of pixels
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = CW_USEDEFAULT;
	windowRect.bottom = CW_USEDEFAULT;

	AdjustWindowRect(&windowRect, wStyle, FALSE);

	_hWindow = CreateWindowW(
		w_className,
		L"AREffect",
		wStyle,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	//ShowWindow(_hWindow, SW_HIDE);

	if (_hWindow == NULL)
	{
		return false;
	}
	return true;
}

bool createPBO( uint32_t* pbo, int width, int height )
{
	if(NULL==pbo || width<=0 || height<=0)
		return false;
	GLuint pboTmp = 0;
	glGenBuffers(1, &pboTmp);
	if(0==pboTmp)
		return false;
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pboTmp);
	int dataLen = width * height * 4;
	glBufferData(GL_PIXEL_PACK_BUFFER_ARB, dataLen, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
	*pbo = pboTmp;
	return true;
}

void releasePBO( uint32_t* pbo )
{
	if(pbo && *pbo!=0)
	{
		glDeleteBuffers(1, pbo);
		*pbo = 0;
	}
}
