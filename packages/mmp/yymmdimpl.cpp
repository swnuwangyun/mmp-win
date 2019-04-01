#include "yymmdimpl.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <SOIL.h>

#include "texthandle.h"
#include "pmx.h"
#include "vmd.h"
#include "shader.h"
#include "pmxvLogger.h"

#include "motioncontroller.h"
#include "bulletphysics.h"
#include "mmdphysics.h"

#include "sound.h"

#include "glfw_func_callbacks.h"

#include "libtext.h"
#include "libpath.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//#define MODELDUMP true

using namespace std;
using namespace ClosedMMDFormat;

static string __DATA_PATH()
{
	wstring wpath = libpath::getThisDllPath();
	string path = libtext::wstring2string(wpath);
	return path;
}
#define DATA_PATH	__DATA_PATH()

YYMMDImpl* YYMMDImpl::m_instance = NULL;

YYMMDImpl::YYMMDImpl()
{
	GLVersionHintMajor = 4;
	GLVersionHintMinor = 2;
	m_vertShaderPath = "";
	m_fragShaderPath = "";
	m_shaderProgram = -1;
	m_bulletVertPath = "";
	m_bulletFragPath = "";
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
	hackShaderFiles();
	createOffscreenRendering();

	return true;
}

void YYMMDImpl::setModelPath(const char* modePath, const char* motionPath, const char* musicPath)
{
	string modePathTemp(modePath);
	int index = modePathTemp.rfind("/");
	string modelFilePath, modelFolderPath;

	if (index == -1)
	{
		modelFolderPath = "";
		modelFilePath = modePathTemp;
	}
	else
	{
		modelFilePath = modePathTemp.substr(index);
		modelFolderPath = modePathTemp.substr(0, index);
	}

	{
		QcCriticalLock lock(m_pmxvmdlock);
		resetPmxVmd();
		m_pmxInfo = &readPMX(modelFolderPath, modelFilePath);
		m_vmdInfo = &readVMD(motionPath);
	}

	loadTextures();
	initBuffers();
	linkShaders(m_shaderProgram);
	glUseProgram(m_shaderProgram);

	initUniformVarLocations();
	MVP_loc = glGetUniformLocation(m_shaderProgram, "MVP");

	//Set OpenGL render settings
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(5.0);
	glClearColor(1, 1, 1, 1);
	

	//Setup MotionController, Physics
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);
	m_motionController = new VMDMotionController(*m_pmxInfo, *m_vmdInfo, m_shaderProgram);

	initSound(musicPath);

	m_bulletPhysics = new BulletPhysics(m_bulletVertPath, m_bulletFragPath);
	glUseProgram(m_shaderProgram); //restore GL shader program binding to Viewer's shader program after initializing BulletPhysic's debugDrawer
	m_mmdPhysics = new MMDPhysics(*m_pmxInfo, m_motionController, m_bulletPhysics);

	m_motionController->updateVertexMorphs();
	m_motionController->updateBoneAnimation();

	m_modelTranslate = glm::vec3(0.0f, -10.0f, 0.0f);
}

void YYMMDImpl::render()
{
	QcAutoCriticalLock lock(m_pmxvmdlock);

	glBindFramebuffer(GL_FRAMEBUFFER, m_imageFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
	glEnable(GL_DEPTH_TEST);

	if (m_motionController && !m_motionController->advanceTime())
	{
		m_motionController->updateVertexMorphs();
		m_motionController->updateBoneAnimation(); // 必须
	}

	if (m_mmdPhysics)
		m_mmdPhysics->updateBones(true);

	if (m_motionController)
		m_motionController->updateBoneMatrix(); // 必须

	if (m_bulletPhysics)
	{
		glUseProgram(m_bulletPhysics->debugDrawer->shaderProgram);
		setCamera(m_bulletPhysics->debugDrawer->MVPLoc);
		glUseProgram(m_shaderProgram);
		setCamera(MVP_loc);
	}

	renderModel();
	glfwSwapBuffers();
}

void YYMMDImpl::setBoneData(const float* data)
{
	if (m_motionController)
		m_motionController->setBoneData(data);
}

void YYMMDImpl::copyOfTextureData(unsigned char* dst)
{
	unsigned char* dstData = new unsigned char[TEXTURE_WIDTH * TEXTURE_HEIGHT * 4];
	memset(dstData, 0, TEXTURE_WIDTH * TEXTURE_HEIGHT * 4);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, dstData);
	memcpy_s((void*)dst, TEXTURE_WIDTH *TEXTURE_HEIGHT * 4, (void*)dstData, TEXTURE_WIDTH *TEXTURE_HEIGHT * 4);
	/*int save_result = SOIL_save_image
	(
	"opengltest1.png",
	SOIL_SAVE_TYPE_BMP,
	TEXTURE_WIDTH, TEXTURE_HEIGHT, 4,
	dstData
	);*/
	SOIL_free_image_data(dstData);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void YYMMDImpl::setLogPath(const char* logPath)
{
}

bool YYMMDImpl::unInit()
{
	if (m_pmxInfo)
	{
		delete m_pmxInfo;
		m_pmxInfo = NULL;
	}
	if (m_vmdInfo)
	{
		delete m_vmdInfo;
		m_vmdInfo = NULL;
	}
	if (m_motionController)
	{
		delete m_motionController;
		m_motionController = NULL;
	}
	if (m_bulletPhysics)
	{
		delete m_bulletPhysics;
		m_bulletPhysics = NULL;
	}
	if (m_mmdPhysics)
	{
		delete m_mmdPhysics;
		m_mmdPhysics = NULL;
	}
	glDeleteFramebuffers(1, &m_imageFBO);
	return true;
}

void YYMMDImpl::resetPmxVmd()
{
	delete m_pmxInfo;
	m_pmxInfo = NULL;
	delete m_vmdInfo;
	m_vmdInfo = NULL;
	delete m_motionController;
	m_motionController = NULL;
	delete m_mmdPhysics;
	m_mmdPhysics = NULL;
}

void YYMMDImpl::hackShaderFiles()
{
	//If using OpenGL 3.0, produce temporary shader files that use #version 130 (to prevent issues with OpenGL 3.0/GLSL 1.30 users
	//Otherwise, use #version 150 to avoid issues in Mac OSX (which does not support #version 130)
	if (GLVersionMajor == 3 && (GLVersionMinor == 0 || GLVersionMinor == 1))
	{
		cout << "Going to hack shader files for OpenGL " << GLVersionMajor << "." << GLVersionMinor << " (assuming GLSL version 130 supported)" << endl;

		string vertPath, fragPath; //vertex/fragment shader file paths
		ifstream test("shaders/model.vert");
		if (!test.is_open())
		{
			vertPath = DATA_PATH + "/shaders/model.vert";
			fragPath = DATA_PATH + "/shaders/model.frag";
		}
		else
		{
			vertPath = "shaders/model.vert";
			fragPath = "shaders/model.frag";
		}
		vertPath = hackShaderFile(vertPath);
		fragPath = hackShaderFile(fragPath);

		m_shaderProgram = compileShaders(vertPath, fragPath);

		remove(vertPath.c_str());
		remove(fragPath.c_str());

		ifstream test2("shaders/bulletDebug.vert");
		if (!test2.is_open())
		{
			vertPath = DATA_PATH + "/shaders/bulletDebug.vert";
			fragPath = DATA_PATH + "/shaders/bulletDebug.frag";
		}
		else
		{
			vertPath = "shaders/bulletDebug.vert";
			fragPath = "shaders/bulletDebug.frag";
		}
		vertPath = hackShaderFile(vertPath);
		fragPath = hackShaderFile(fragPath);

		m_bulletVertPath = vertPath;
		m_bulletFragPath = fragPath;

	}
	else
	{
		//Load included shader files as-is
		ifstream test("shaders/model.vert");
		if (!test.is_open())
		{
			m_shaderProgram = compileShaders(DATA_PATH + "/shaders/model.vert", DATA_PATH + "/shaders/model.frag");
		}
		else
		{
			m_shaderProgram = compileShaders("shaders/model.vert", "shaders/model.frag");
		}
		test.close();

		ifstream test2("shaders/bulletDebug.vert");
		if (!test2.is_open())
		{
			m_bulletVertPath = DATA_PATH + "/shaders/bulletDebug.vert";
			m_bulletFragPath = DATA_PATH + "/shaders/bulletDebug.frag";
		}
		else
		{
			m_bulletVertPath = "shaders/bulletDebug.vert";
			m_bulletFragPath = "shaders/bulletDebug.frag";
		}
	}
}

void YYMMDImpl::createOffscreenRendering()
{
	// 创建FBO对象
	glGenFramebuffersEXT(1, &m_imageFBO);
	// 启用FBO
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_imageFBO);

	// 创建纹理
	glGenTextures(1, &m_offscreenTexture);
	// 启用纹理
	glBindTexture(GL_TEXTURE_2D, m_offscreenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_offscreenTexture, 0);

	glGenRenderbuffersEXT(1, &m_depthTextureID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthTextureID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthTextureID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(5.0);
	glClearColor(1, 1, 1, 1);

	glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "Framebuffer complete." << std::endl;
		return;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
		return;


	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
		return;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
		return;


	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
		return;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
		return;

	default:
		std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
		return;
	}
}

void YYMMDImpl::loadTextures()
{
	for (int i = 0; i<m_pmxInfo->texture_continuing_datasets; ++i)
	{
		cout << "Loading " << m_pmxInfo->texturePaths[i] << "...";
		if (m_pmxInfo->texturePaths[i].substr(m_pmxInfo->texturePaths[i].size() - 3) == "png" || m_pmxInfo->texturePaths[i].substr(m_pmxInfo->texturePaths[i].size() - 3) == "spa")
		{
			GLuint texture;
			int width, height;
			unsigned char* image;
			string loc = m_pmxInfo->texturePaths[i];

			ifstream test(loc);
			if (!test.is_open())
			{
				cerr << "Texture file could not be found: " << loc << endl;
				//exit(EXIT_FAILURE);
			}
			test.close();

			glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			image = SOIL_load_image(loc.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (texture == 0)
			{
				cerr << "Texture failed to load: " << m_pmxInfo->texturePaths[i] << endl;
				printf("SOIL loading error: '%s'\n", SOIL_last_result());
				exit(EXIT_FAILURE);
			}

			cout << "done" << endl;

			m_textures.push_back(texture);
		}
		else if (m_pmxInfo->texturePaths[i].substr(m_pmxInfo->texturePaths[i].size() - 3) == "tga")
		{
			//cerr<<"WARNING: TGA files only mildly tested"<<endl;

			/*static GLuint texture = 0;

			FIBITMAP *bitmap = FreeImage_Load( FreeImage_GetFileType(m_pmxInfo->texturePaths[i].c_str(), 0),m_pmxInfo->texturePaths[i].c_str() );
			FIBITMAP *pImage = FreeImage_ConvertTo24Bits(bitmap);

			int nWidth = FreeImage_GetWidth(pImage);
			int nHeight = FreeImage_GetHeight(pImage);

			glActiveTexture( GL_TEXTURE0 );
			glGenTextures(1, &texture);

			glBindTexture(GL_TEXTURE_2D, texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight,
			0, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glGet
			if(texture==0)
			{
			cerr<<"Texture failed to load: "<<m_pmxInfo->texturePaths[i]<<endl;
			exit(EXIT_FAILURE);
			}

			FreeImage_Unload(bitmap);

			m_textures.push_back(texture);

			cout<<"done"<<endl;*/

			GLuint texture;
			int width, height, channels;
			unsigned char* image;
			string loc = m_pmxInfo->texturePaths[i];

			ifstream test(loc);
			if (!test.is_open())
			{
				cerr << "Texture file could not be found: " << loc << endl;
				//exit(EXIT_FAILURE);
			}
			test.close();

			glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			image = SOIL_load_image(loc.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (texture == 0)
			{
				cerr << "Texture failed to load: " << m_pmxInfo->texturePaths[i] << endl;
				printf("SOIL loading error: '%s'\n", SOIL_last_result());
				exit(EXIT_FAILURE);
			}

			cout << "done" << endl;

			m_textures.push_back(texture);
		}
		else
		{
			GLuint texture;
			int width, height;
			unsigned char* image;
			string loc = m_pmxInfo->texturePaths[i];

			ifstream test(loc);
			if (!test.is_open())
			{
				cerr << "Texture file could not be found: " << loc << endl;
				//exit(EXIT_FAILURE);
			}
			test.close();

			glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			image = SOIL_load_image(loc.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			if (texture == 0)
			{
				cerr << "Texture failed to load: " << m_pmxInfo->texturePaths[i] << endl;
				printf("SOIL loading error: '%s'\n", SOIL_last_result());
				exit(EXIT_FAILURE);
			}

			m_textures.push_back(texture);

			cout << "done" << endl;
		}
	}

	for (int i = 1; i <= 10; ++i)
	{
		//cout<<"Loading toon"<<i<<".bmp...";

		GLuint texture;
		int width, height, channels;
		unsigned char* image;
		stringstream loc;
		if (i != 10) loc << DATA_PATH << "\\data\\share\\toon0" << i << ".bmp";
		else loc << DATA_PATH << "\\data\\share\\toon10.bmp";

		string locstr = loc.str();
		ifstream test(locstr);
		if (!test.is_open())
		{
			//cerr<<"Texture file could not be found: "<<loc.str()<<endl;
			//exit(EXIT_FAILURE);

			loc.str(std::string()); //clear ss
			if (i != 10) loc << DATA_PATH << "/m_textures/toon0" << i << ".bmp";
			else loc << DATA_PATH << "/m_textures/toon10.bmp";

			ifstream test2(loc.str());
			if (!test2.is_open())
			{
				cerr << "Texture file could not be found: " << loc.str() << endl;
			}

		}
		test.close();

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		image = SOIL_load_image(loc.str().c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (texture == 0)
		{
			cerr << "Toon Texture failed to load: " << i << endl;
			printf("SOIL loading error: '%s'\n", SOIL_last_result());
			exit(EXIT_FAILURE);
		}

		//cout<<"done"<<endl;

		m_textures.push_back(texture);
	}
}

void YYMMDImpl::initBuffers()
{
#ifdef MODELDUMP
	ofstream modeldump("modeldump.txt");
	modeldump << "indices:" << endl;
#endif

	//Note: vertex indices are loaded statically, since they do not change.
	//The actual vertex data is loaded dynamically each frame, so its memory is managed by the MotionController.
	GLuint *vertexIndices = (GLuint*)calloc(m_pmxInfo->face_continuing_datasets, sizeof(GLuint) * 3);
	for (int i = 0; i<m_pmxInfo->faces.size(); ++i) //faces.size()
	{
		int j = i * 3;
		vertexIndices[j] = m_pmxInfo->faces[i]->points[0];
		vertexIndices[j + 1] = m_pmxInfo->faces[i]->points[1];
		vertexIndices[j + 2] = m_pmxInfo->faces[i]->points[2];

#ifdef MODELDUMP
		modeldump << vertexIndices[j] << " " << vertexIndices[j + 1] << " " << vertexIndices[j + 2] << endl;
#endif
	}


#ifdef MODELDUMP
	modeldump << "vertices:" << endl;
#endif

#ifdef MODELDUMP
	modeldump.close();
#endif

	//Generate all Viewer Buffers
	glGenBuffers(NumBuffers, Buffers);

	//init Element Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[VertexIndexBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pmxInfo->face_continuing_datasets * sizeof(GLuint) * 3, vertexIndices, GL_STATIC_DRAW);

	free(vertexIndices);

	//Init Vertex Array Buffer Object
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Vertices]);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);

	//Intialize Vertex Attribute Pointers
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(0)); //4=number of components updated per vertex
	glBindAttribLocation(m_shaderProgram, vPosition, "vPosition"); //Explicit vertex attribute index specification for older OpenGL version support. (Newer method is layout qualifier in vertex shader)
	glEnableVertexAttribArray(vPosition);

	glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4)));
	glBindAttribLocation(m_shaderProgram, vUV, "vUV");
	glEnableVertexAttribArray(vUV);

	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4) + sizeof(glm::vec2)));
	glBindAttribLocation(m_shaderProgram, vNormal, "vNormal");
	glEnableVertexAttribArray(vNormal);

	glVertexAttribPointer(vBoneIndices, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4) + sizeof(glm::vec2) + sizeof(glm::vec3) + sizeof(GLfloat)));
	glBindAttribLocation(m_shaderProgram, vBoneIndices, "vBoneIndices");
	glEnableVertexAttribArray(vBoneIndices);

	glVertexAttribPointer(vBoneWeights, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4) + sizeof(glm::vec2) + sizeof(glm::vec3) + sizeof(GLfloat) * 5));
	glBindAttribLocation(m_shaderProgram, vBoneWeights, "vBoneWeights");
	glEnableVertexAttribArray(vBoneWeights);

	glVertexAttribPointer(vWeightFormula, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(glm::vec4) + sizeof(glm::vec2) + sizeof(glm::vec3)));
	glBindAttribLocation(m_shaderProgram, vWeightFormula, "vWeightFormula");
	glEnableVertexAttribArray(vWeightFormula);
}

void YYMMDImpl::initUniformVarLocations()
{
	uniformVars[uAmbient] = glGetUniformLocation(m_shaderProgram, "ambient");
	uniformVars[uDiffuse] = glGetUniformLocation(m_shaderProgram, "diffuse");
	uniformVars[uSpecular] = glGetUniformLocation(m_shaderProgram, "specular");

	uniformVars[uShininess] = glGetUniformLocation(m_shaderProgram, "shininess");

	uniformVars[uIsEdge] = glGetUniformLocation(m_shaderProgram, "isEdge");
	uniformVars[uEdgeColor] = glGetUniformLocation(m_shaderProgram, "edgeColor");
	uniformVars[uEdgeSize] = glGetUniformLocation(m_shaderProgram, "edgeSize");

	uniformVars[uHalfVector] = glGetUniformLocation(m_shaderProgram, "halfVector");

	uniformVars[uLightDirection] = glGetUniformLocation(m_shaderProgram, "lightDirection");

	uniformVars[uSphereMode] = glGetUniformLocation(m_shaderProgram, "fSphereMode");

	uniformVars[uTextureSampler] = glGetUniformLocationARB(m_shaderProgram, "textureSampler");
	uniformVars[uSphereSampler] = glGetUniformLocationARB(m_shaderProgram, "sphereSampler");
	uniformVars[uToonSampler] = glGetUniformLocationARB(m_shaderProgram, "toonSampler");
}

void YYMMDImpl::setCamera(GLuint MVPLoc)
{
	glm::mat4 Projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(cameraPosition.x, cameraPosition.y, -cameraPosition.z), // Camera is at (4,3,3), in World Space
		cameraTarget, // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	glm::mat4 Model = glm::translate(m_modelTranslate.x, m_modelTranslate.y, m_modelTranslate.z);
	glm::mat4 MVP = Projection * View * Model;

	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVP[0][0]);
}

std::string YYMMDImpl::hackShaderFile(std::string filename)
{
	//Variables for temporary file name paths
	char tmpFilePathChar[1024];
	tmpnam_s(tmpFilePathChar, 1024); //Get path to a temporary file location

	string tmpFilePath = tmpFilePathChar;
	ofstream tmpShaderFile(tmpFilePath);

	ifstream shaderFile(filename);

	string line;
	getline(shaderFile, line); //discard first line
	while (shaderFile.good())
	{
		getline(shaderFile, line);
		tmpShaderFile << line << endl;
	}

	return tmpFilePath;
}

void YYMMDImpl::drawModel(bool drawEdges)
{
	//Bind VAO and related Buffers
	glBindVertexArray(VAOs[Vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[VertexIndexBuffer]);

	if (drawEdges)
	{
		glDisable(GL_BLEND);
		glCullFace(GL_FRONT);
		glUniform1i(uniformVars[uIsEdge], 1);

		glDisable(GL_DEPTH_TEST);
	}
	else
	{
		glEnable(GL_BLEND);
		glCullFace(GL_BACK);
		glUniform1i(uniformVars[uIsEdge], 0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
	}


	glm::vec3 halfVector = glm::normalize(cameraPosition - cameraTarget);
	halfVector.z = -halfVector.z;

	glm::vec3 lightDirection = glm::normalize(glm::vec3(0.3, 1.0, 2.0));

	int faceCount = 0;
	for (int m = 0; m<m_pmxInfo->material_continuing_datasets; ++m) //m_pmxInfo->material_continuing_datasets
	{
		if (m_pmxInfo->materials[m]->textureIndex == -1)
		{
			cout << "Error: " << "m_pmxInfo->materials[m]->textureIndex==-1" << endl;
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textures[m_pmxInfo->materials[m]->textureIndex]);
			glUniform1iARB(uniformVars[uTextureSampler], 0);
		}

		if ((int)m_pmxInfo->materials[m]->sphereMode>0)
		{
			if (m_pmxInfo->materials[m]->sphereIndex != -1)
			{
				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, m_textures[m_pmxInfo->materials[m]->sphereIndex]);
				glUniform1iARB(uniformVars[uSphereSampler], 1);
			}
			else
			{
				cout << "Error: " << "m_pmxInfo->materials[m]->sphereIndex==-1" << endl;
			}
		}

		if ((int)m_pmxInfo->materials[m]->shareToon == 0)
		{
			if (m_pmxInfo->materials[m]->toonTextureIndex != -1)
			{
				glActiveTexture(GL_TEXTURE2);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, m_textures[m_pmxInfo->materials[m]->toonTextureIndex]);
				glUniform1iARB(uniformVars[uToonSampler], 2);
			}
			else
			{
				cout << "Error: " << "m_pmxInfo->materials[m]->toonTextureIndex==-1" << endl;
			}
		}
		else if ((int)m_pmxInfo->materials[m]->shareToon == 1)
		{
			glActiveTexture(GL_TEXTURE2);
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, m_textures[m_textures.size() - 11 + m_pmxInfo->materials[m]->shareToonTexture]);
			glUniform1iARB(uniformVars[uToonSampler], 2);
		}


		glUniform3fv(uniformVars[uAmbient], 1, (GLfloat*)&m_pmxInfo->materials[m]->ambient);
		glUniform4fv(uniformVars[uDiffuse], 1, (GLfloat*)&m_pmxInfo->materials[m]->diffuse);
		glUniform3fv(uniformVars[uSpecular], 1, (GLfloat*)&m_pmxInfo->materials[m]->specular);

		glUniform1f(uniformVars[uShininess], glm::normalize(m_pmxInfo->materials[m]->shininess));
		glUniform3f(uniformVars[uHalfVector], halfVector.x, halfVector.y, halfVector.z);
		glUniform3f(uniformVars[uLightDirection], lightDirection.x, lightDirection.y, lightDirection.z);

		glUniform4fv(uniformVars[uEdgeColor], 1, (GLfloat*)&m_pmxInfo->materials[m]->edgeColor);
		glUniform1f(uniformVars[uEdgeSize], glm::normalize(m_pmxInfo->materials[m]->edgeSize));

		glUniform1f(uniformVars[uSphereMode], m_pmxInfo->materials[m]->sphereMode);

		glDrawElements(GL_TRIANGLES, (m_pmxInfo->materials[m]->hasFaceNum), GL_UNSIGNED_INT, BUFFER_OFFSET(sizeof(GLuint)*faceCount));
		faceCount += m_pmxInfo->materials[m]->hasFaceNum;
	}
}

void YYMMDImpl::renderModel()
{
	drawModel(true); 
	drawModel(false); 

	glUseProgram(m_shaderProgram); //Restore shader program and buffer's to Viewer's after drawing Bullet debug
	glBindVertexArray(VAOs[Vertices]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VertexArrayBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[VertexIndexBuffer]);
}