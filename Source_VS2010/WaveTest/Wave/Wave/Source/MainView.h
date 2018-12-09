#pragma once
#include <memory>
#include "Scene/scene.h"
#include "myGlsl.h"
#include "myGLUI.h"

enum Type 
{
	INSIDE, LEFT, RIGHT, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
	OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR
};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ

int NX, NY, NX1, NY1, NX2, NY2;
float DX, DY;
float direction = 1.0;//障害物直線移動方向

//解析用データのサイズ
int texWidth;
int texHeight;//全格子数

//解析用データ配列
float *velX;//速度VelXと圧力
float *velY;//速度VelYと渦度
float *wave;//波の高さの速度，位置
GLubyte *caus;
float *particle;//粒子位置
GLuint texID[6];//texture object
GLuint fbo[5];//frame buffer object
GLuint rbo;   //render buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;
GLenum type = GL_FLOAT;
//CubeMap用
#define CUBE_WIDTH  512
#define CUBE_HEIGHT 512

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
	MyGLUI myGLUI;
};

