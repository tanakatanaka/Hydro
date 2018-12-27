#pragma once
#include <GL/glew.h>
#include <GL/glui.h>

//myGLSLのラッパークラス
class SubGLSL
{
public:
	void InitGlsl(GLuint *program, std::string vertexFile, std::string fragmentFile = "");
	void InitWindow();

};