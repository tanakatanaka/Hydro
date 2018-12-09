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
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //�i�q�_�̃^�C�v

int NX, NY, NX1, NY1, NX2, NY2;
float DX, DY;
float direction = 1.0;//��Q�������ړ�����

//��͗p�f�[�^�̃T�C�Y
int texWidth;
int texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *velX;//���xVelX�ƈ���
float *velY;//���xVelY�ƉQ�x
float *wave;//�g�̍����̑��x�C�ʒu
GLubyte *caus;
float *particle;//���q�ʒu
GLuint texID[6];//texture object
GLuint fbo[5];//frame buffer object
GLuint rbo;   //render buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;
GLenum type = GL_FLOAT;
//CubeMap�p
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

