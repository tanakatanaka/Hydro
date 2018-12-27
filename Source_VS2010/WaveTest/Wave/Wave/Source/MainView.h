#pragma once
#include <memory>
#include "Scene/scene.h"
#include "myGlsl.h"
#include "myGLUI.h"


//CubeMap用
#define CUBE_WIDTH  512
#define CUBE_HEIGHT 512

//シェーダプログラム名
GLuint shader1, shader2, shader3, shader4, shader5, shader6, shader7, shader8;

//テクスチャー構造体(cube map)
struct Target 
{
	GLenum name;//ターゲット名
	GLdouble cx, cy, cz;//注視点
	GLdouble ux, uy, uz;//アップベクトル
};

Target c_target[] = 
{
	{//右
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//左
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		-1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//奥
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
	},
	{//手前
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		0.0, -1.0, 0.0,
		0.0, 0.0, -1.0,
	},
	{//上方
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		0.0, 0.0, 1.0,
		0.0, -1.0, 0.0,
	},
	{//床面
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		0.0, 0.0, -1.0,
		0.0, -1.0, 0.0,
	}
};

//myGLUI
extern std::unique_ptr<MyGLUI> myGLUI;

class MainView
{
public:
    void InitializeGL(int argc, char** argv);
    void paintGL();
    void resizeGL(int w, int h);
    void toggleAnimation();
    void takeScreenShot();

private:
	std::unique_ptr<Scene> scene;
};

