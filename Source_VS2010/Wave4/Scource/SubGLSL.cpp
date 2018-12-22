#include"SubGLSL.h"


void SubGLSL::InitGlsl(GLuint *program, std::string vertexFile, std::string fragmentFile)
{
	std::string vertexPass = "shader/" + vertexFile;

	if (fragmentFile != "")
	{
		std::string fragmentPass = "shader/" + fragmentFile;
		initGlsl(program, vertexPass.c_str(), fragmentPass.c_str());
	}
	else
	{
		initGlsl(program, vertexPass.c_str());
	}
}

