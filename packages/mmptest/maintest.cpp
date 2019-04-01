#include <Windows.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GL/glfw.h>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>
#include "IYYMMD.h"
#include "glHelper.h"
#include <SOIL.h>
#include "libtext.h"

HMODULE m_dllHD = NULL;
typedef IYYMMD*(__cdecl*DllInstance)();
DllInstance m_dllInstance;
IYYMMD* yymmd = NULL;

void initOpenGL()
{
	// 检查opengl运行环境，版本、使用的显卡等
	GLInitContext initCtx;
	// 获取opengl的版本号和运行OF SDK的显卡名称
	int result = graphic_glInit_SEH(&initCtx);
}

bool loadYYMMD()
{
	m_dllHD = LoadLibrary(L"yymmd.dll");
	if (m_dllHD)
	{
		m_dllInstance = (DllInstance)GetProcAddress(m_dllHD, "GetInstance");
		yymmd = m_dllInstance();
		if (yymmd)
		{
			return true;
		}
	}

	return false;
}

void main()
{
	initOpenGL();
	if (loadYYMMD())
	{
		yymmd->init("1.12.0.6");

		std::string model_file = "";
		std::string motion_file = "";
		std::string pmstr = "E:\\mmp-win\\data\\model_pmx\\TDA Electric Love Miku\\TDA Electric Love Miku.pmx";
		std::string vmdtr = "E:\\mmp-win\\data\\motion\\arm.vmd";
		if (pmstr.substr(pmstr.size() - 4) == ".pmx")
		{
			model_file = pmstr;
			std::wstring wmodel_file = libtext::string2wstring(model_file);
			wmodel_file = libtext::replace(wmodel_file, L"\\", L"/");
			model_file = libtext::wstring2string(wmodel_file);
		}
		else if (vmdtr.substr(vmdtr.size() - 4) == ".vmd")
		{
			motion_file = pmstr;
			std::wstring wmotion_file = libtext::string2wstring(motion_file);
			wmotion_file = libtext::replace(wmotion_file, L"\\", L"/");
			motion_file = libtext::wstring2string(wmotion_file);
		}
		yymmd->setModelPath(model_file.c_str(), motion_file.c_str(), "");
		
		yymmd->render();
		unsigned char* dstData = new unsigned char[TEXTURE_WIDTH * TEXTURE_HEIGHT * 4];
		memset(dstData, 0, TEXTURE_WIDTH * TEXTURE_HEIGHT * 4);
		yymmd->copyOfTextureData(dstData);

		int save_result = SOIL_save_image
		(
			"opengltest1.png",
			SOIL_SAVE_TYPE_BMP,
			TEXTURE_WIDTH, TEXTURE_HEIGHT, 4,
			dstData
		);
		SOIL_free_image_data(dstData);
	}
	system("pause");
}