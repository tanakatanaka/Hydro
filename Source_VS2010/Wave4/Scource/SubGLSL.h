#pragma once
#include <GL/glew.h>
#include <GL/glui.h>

//myGLSL�̃��b�p�[�N���X
class SubGLSL
{
public:
	void InitGlsl(GLuint *program, std::string vertexFile, std::string fragmentFile = "");
	void InitWindow();

};