#pragma once
#include <iostream>
#include <windows.h>
#include "mainview.h"
#include "Assist3D.h"


#include"Scene/CausticsScene/CausticsScene.h"
using std::cout;
using std::endl;
#include <cstdio>

void  MainView::InitializeGL(int argc, char** argv)
{
	LARGE_INTEGER freq;

	glutInit(&argc, argv);
	//�\�����[�h
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//�\���E�B���h�E�̃T�C�Y
	glutInitWindowSize(Assist3d.win_width, Assist3d.win_height);
	//����̈ʒu
	glutInitWindowPosition(Assist3d.win_x0, Assist3d.win_y0);
	//�E�B���h�E�쐬
	Assist3d.main_window = glutCreateWindow("GP_Caustics2");
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(resize);
	//�\��
	glutDisplayFunc(display);
	



	scene = std::make_unique<CausticsScene>();
	scene->InitScene();
}






void MainView::paintGL() 
{

	//scene->update(0.0f);
    //GLUtils::checkForOpenGLError(__FILE__,__LINE__);
	//scene->render();
}

void MainView::resizeGL(int w, int h )
{
    //scene->resize(w,h);
}


