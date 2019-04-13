#include "stdafx.h"
#include "utils.h"

namespace utils
{
	wstring appDir(void)
	{
		static wstring g_strDir = L"";
		if (!g_strDir.empty())
		{
			return g_strDir;
		}
		wchar_t buffer[512] = { 0 };
		GetModuleFileNameW(NULL, buffer, sizeof(buffer));
		wstring strPath = buffer;
		int nPos = strPath.find_last_of(L"\\");
		if (wstring::npos == nPos)
		{
			return L"";
		}
		g_strDir = strPath.substr(0, nPos);
		return g_strDir;
	}

	string appDirA(void)
	{
		static string g_strDir = "";
		if (!g_strDir.empty())
		{
			return g_strDir;
		}
		char buffer[512] = { 0 };
		GetModuleFileNameA(NULL, buffer, sizeof(buffer));
		string strPath = buffer;
		int nPos = strPath.find_last_of("\\");
		if (string::npos == nPos)
		{
			return "";
		}
		g_strDir = strPath.substr(0, nPos);
		return g_strDir;
	}

	unsigned char * LoadFileContent(const char *path, int &filesize) {
		unsigned char*fileContent = nullptr;
		filesize = 0;
		FILE* pFile = NULL;
		fopen_s(&pFile, path, "rb");
		if (pFile) {
			fseek(pFile, 0, SEEK_END);
			int nLen = ftell(pFile);
			if (nLen > 0) {
				rewind(pFile);
				fileContent = new unsigned char[nLen + 1];
				fread(fileContent, sizeof(unsigned char), nLen, pFile);
				fileContent[nLen] = '\0';
				filesize = nLen;
			}
			fclose(pFile);
		}
		return fileContent;
	}

	GLuint CompileShader(GLenum shaderType, const char*shaderCode) {
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderCode, nullptr);
		glCompileShader(shader);
		GLint compileResult = GL_TRUE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {
			char szLog[1024] = { 0 };
			GLsizei logLen = 0;
			glGetShaderInfoLog(shader, 1024, &logLen, szLog);
			printf("Compile Shader fail error log : %s \nshader code :\n%s\n", szLog, shaderCode);
			glDeleteShader(shader);
			shader = 0;
		}
		return shader;
	}

	GLuint CreateProgram(GLuint vsShader, GLuint fsShader) {
		GLuint program = glCreateProgram();
		glAttachShader(program, vsShader);
		glAttachShader(program, fsShader);
		glLinkProgram(program);
		glDetachShader(program, vsShader);
		glDetachShader(program, fsShader);
		GLint nResult;
		glGetProgramiv(program, GL_LINK_STATUS, &nResult);
		if (nResult == GL_FALSE) {
			char log[1024] = { 0 };
			GLsizei writed = 0;
			glGetProgramInfoLog(program, 1024, &writed, log);
			printf("create gpu program fail,link error : %s\n", log);
			glDeleteProgram(program);
			program = 0;
		}
		return program;
	}

	unsigned char* DecodeBMP(unsigned char*bmpFileData, int&width, int&height) {
		if (0x4D42 == *((unsigned short*)bmpFileData)) {
			int pixelDataOffset = *((int*)(bmpFileData + 10));
			width = *((int*)(bmpFileData + 18));
			height = *((int*)(bmpFileData + 22));
			unsigned char*pixelData = bmpFileData + pixelDataOffset;
			for (int i = 0; i < width*height * 3; i += 3) {
				unsigned char temp = pixelData[i];
				pixelData[i] = pixelData[i + 2];
				pixelData[i + 2] = temp;
			}
			return pixelData;
		}
		return nullptr;
	}

	GLuint CreateTexture2D(unsigned char*pixelData, int width, int height, GLenum type) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixelData);
		glBindTexture(GL_TEXTURE_2D, 0);
		return texture;
	}

	GLuint CreateTexture2DFromBMP(const char*bmpPath) {
		int nFileSize = 0;
		unsigned char *bmpFileContent = LoadFileContent(bmpPath, nFileSize);
		if (bmpFileContent == nullptr) {
			return 0;
		}
		int bmpWidth = 0, bmpHeight = 0;
		unsigned char*pixelData = DecodeBMP(bmpFileContent, bmpWidth, bmpHeight);
		if (bmpWidth == 0) {
			delete bmpFileContent;
			return 0;
		}
		GLuint texture = CreateTexture2D(pixelData, bmpWidth, bmpHeight, GL_RGB);
		delete bmpFileContent;
		return texture;
	}

	GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data /* = nullptr */) {
		GLuint object;
		glGenBuffers(1, &object);
		glBindBuffer(bufferType, object);
		glBufferData(bufferType, size, data, usage);
		glBindBuffer(bufferType, 0);
		return object;
	}

}