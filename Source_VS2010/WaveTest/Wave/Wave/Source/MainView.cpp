#pragma once
#include <iostream>
#include <windows.h>
#include "mainview.h"
#include "Assist3D.h"
#include "../../../../rigid.h"
#include"Scene/CausticsScene/CausticsScene.h"
using std::cout;
using std::endl;
#include <cstdio>

void  MainView::InitializeGL(int argc, char** argv)
{
	LARGE_INTEGER freq;
	myGLUI.Initialize();

	glutInit(&argc, argv);
	//�\�����[�h
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//�\���E�B���h�E�̃T�C�Y
	glutInitWindowSize(assist3d.win_width, assist3d.win_height);
	//����̈ʒu
	glutInitWindowPosition(assist3d.win_x0, assist3d.win_y0);
	//�E�B���h�E�쐬
	myGLUI.main_window = glutCreateWindow("GP_Caustics2");
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(assist3d.Resize);
	//�\��
	glutDisplayFunc(Display);
	



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

void Display(void)
{
	//���Ԍv��
	QueryPerformanceFrequency(&assist3d.freq);
	QueryPerformanceCounter(&assist3d.currentTime);
	assist3d.pTime = (double)((assist3d.currentTime.QuadPart - assist3d.lastTime.QuadPart) * 1000000 / assist3d.freq.QuadPart);
	double dt = assist3d.pTime / 1000000.0;//[sec]
	assist3d.elapseTime1 += dt;
	static int fps = 0;
	static int fps0 = 0;
	fps++;

	if (assist3d.elapseTime1 >= 1.0)
	{
		assist3d.elapseTime1 = 0.0;
		fps0 = fps;
		fps = 0;
	}
	QueryPerformanceCounter(&assist3d.lastTime);

	int i, j, k;

	if (assist3d.flagStart == true && assist3d.flagFreeze == false)
	{
		//�ړ���Q���\�ʂ̑��x���E����
		for (i = 1; i < NX; i++)
			for (j = 0; j <= NY; j++)
			{
				if (g_type[i][j] < OBS_LEFT) continue;
				k = 4 * (i + j * texWidth);
				velX[k] = rigid[0].vVel.x;
				velY[k] = rigid[0].vVel.y;
			}

		//���͋��E����
		for (i = 0; i <= NX; i++)
			for (j = 0; j <= NY; j++)
			{
				k = 4 * (i + j * texWidth) + 3;//����
				if (g_type[i][j] == INSIDE) continue;
				else if (g_type[i][j] == LEFT)  velX[k] = 0.0;//-x
				else if (g_type[i][j] == RIGHT) velX[k] = 0.0;//+x
				else if (g_type[i][j] == TOP) velX[k] = 0.0;//+y
				else if (g_type[i][j] == BOTTOM) velX[k] = 0.0;//-y
				else if (g_type[i][j] == OBS_LEFT) velX[k] = velX[4 * (i - 1 + j * texWidth) + 3];
				else if (g_type[i][j] == OBS_RIGHT) velX[k] = velX[4 * (i + 1 + j * texWidth) + 3];
				else if (g_type[i][j] == OBS_TOP) velX[k] = velX[4 * (i + (j + 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_BOTTOM) velX[k] = velX[4 * (i + (j - 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_UL) velX[k] = velX[4 * (i - 1 + (j + 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_UR) velX[k] = velX[4 * (i + 1 + (j + 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_LL) velX[k] = velX[4 * (i - 1 + (j - 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_LR) velX[k] = velX[4 * (i + 1 + (j - 1) * texWidth) + 3];
			}

		setTextureVelX();
		setFramebufferVelX();
		setTextureVelY();
		setFramebufferVelY();

		renewVelX(); //���xX�ƈ��͂̍X�V
		renewVelY(); //���xY�ƉQ�x�̍X�V
		renewWaveVel();//�g�̂����������x�̍X�V
		renewWavePos();//�g�̍����̍X�V
					   /*
					   for(i=50;i<150; i++)
					   {
					   int k3 = 3*(i + texWidth * texHeight/2);
					   float aa = (float)caus[k3];
					   if(aa > 200.0) printf("i=%d caus=%f \n", i, aa);
					   }*/
					   //particle
		for (j = 0; j < texHeight; j++)
			for (i = 0; i < texWidth; i++)
			{
				k = i + j * texWidth;
				if (k > numParticle) break;
				//���xX
				if (particle[4 * k + 0] > rect.size.x / 2.0 || particle[4 * k + 0] < -rect.size.x / 2.0 ||
					particle[4 * k + 1] > rect.size.y / 2.0 || particle[4 * k + 1] < -rect.size.y / 2.0)
				{
					particle[4 * k + 0] = getRandom(-rect.size.x / 2.0, rect.size.x / 2.0);
					particle[4 * k + 1] = getRandom(-rect.size.y / 2.0, rect.size.y / 2.0);
				}
			}
		setTextureParticle();
		setFramebufferParticle();

		renewParticle();//���q�ʒu�̍X�V

		if (flagStep) flagFreeze = true;
		elapseTime2 += dt;
		elapseTime3 += deltaT;

		calcObsPos();

	}

	//���}�b�v�̃e�N�X�`���쐬
	makeTextureOfCubeMapping();

	//�J���[�o�b�t�@,�f�v�X�o�b�t�@�̃N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	resize(win_width, win_height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//���_��ς���Ƃ��͂��̈ʒu�ɕK�v

	if (cos(M_PI * view.theta / 180.0) >= 0.0)//�J�����p90�x���ޭ������޸�ِؑ�
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, 1.0);
	else
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, -1.0);

	//�����ݒ�
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	if (flagWireframe == 1)//'w'��wireframe��solid model�؂�ւ�
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_POINT);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	rendering();//����

	if (flagShowParticle) renderingP();

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);//�w�ʂ͕`�悵�Ȃ�
	drawUpperObject();

	//���e�}�b�s���O
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[5]);//caus[]
									// �V�F�[�_�v���O�����̓K�p 
	glUseProgram(shader8);
	GLint texLoc = glGetUniformLocation(shader8, "samplerCaus");
	glUniform1i(texLoc, 0);//GL_TEXTURE0��K�p

	drawLowerObject();
	fish1.motion(elapseTime2);
	fish2.motion(elapseTime2);
	if (flagShadowShow) drawShadow();
	//glDisable(GL_CULL_FACE);

	// �V�F�[�_�v���O�����̓K�p������	
	glUseProgram(0);

	//���U���Ȃ����߂̖ڈ���m�邽��
	float Courant, diff_num;
	if (DX < DY)
	{
		Courant = deltaT / DX;
		diff_num = (1.0 / Re) * deltaT / (DX * DX);//�g�U��
	}
	else
	{
		Courant = deltaT / DY;
		diff_num = (1.0 / Re) * deltaT / (DY * DY);//�g�U��
	}

	if (flagParamShow)
	{
		drawParam("f_rate=%d[fps]", fps0, 10, 30);
		drawParam("t_step=%1.5f[s]", dt, 150, 30);
		drawParam("e_time=%3.3f[s]", elapseTime2, 300, 30);
		drawParam("n_time=%3.3f", elapseTime3, 460, 30);
		drawParam("Courant=%1.4f", Courant, 10, 50);
		drawParam("diff_num=%1.4f", diff_num, 150, 50);
		drawParam("Re=%3.1f", Re, 300, 50);
	}
	if (flagCoordShow) drawWorldCoordinates(5.0);

	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}


void setTextureVelX()
{
	glBindTexture(target, texID[0]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, velX);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureVelY()
{
	glBindTexture(target, texID[1]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, velY);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureWave()
{
	glBindTexture(target, texID[2]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, wave);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferVelX()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[0]);
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0);
	//framebuffer object �̖�����
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void setFramebufferVelY()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[1]);
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[1], 0);
	//framebuffer object �̖�����
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
void setFramebufferWave()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[2]);
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[2], 0);
	//framebuffer object �̖�����
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setTextureParticle()
{
	glBindTexture(target, texID[3]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, GL_FLOAT, particle);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferParticle()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[3]);
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[3], 0);
	//framebuffer object �̖�����
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setFramebufferCube()
{
	//renderbuffer
	glGenRenderbuffersEXT(1, &rbo);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbo);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, CUBE_WIDTH, CUBE_HEIGHT);
	//framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[4]);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbo);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}



