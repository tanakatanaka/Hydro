#pragma once
#include <memory>
#include "Scene/scene.h"
#include "myGlsl.h"
#include "myGLUI.h"


//CubeMap�p
#define CUBE_WIDTH  512
#define CUBE_HEIGHT 512

//�V�F�[�_�v���O������
GLuint shader1, shader2, shader3, shader4, shader5, shader6, shader7, shader8;

//�e�N�X�`���[�\����(cube map)
struct Target 
{
	GLenum name;//�^�[�Q�b�g��
	GLdouble cx, cy, cz;//�����_
	GLdouble ux, uy, uz;//�A�b�v�x�N�g��
};

Target c_target[] = 
{
	{//�E
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//��
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		-1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//��
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
	},
	{//��O
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		0.0, -1.0, 0.0,
		0.0, 0.0, -1.0,
	},
	{//���
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		0.0, 0.0, 1.0,
		0.0, -1.0, 0.0,
	},
	{//����
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

