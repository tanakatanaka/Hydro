/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

1�����g�U������
*/
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include "../../support2D.h"
#include "../../myMath6.h"
#include "../../simultaneous.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾
void idle();
void display();
void drawRegion();
void resize(int w, int h);
void intWave();
void calculate(float dt, float d);
void drawProfile();
void drawTimeSeries();
void setFontBitmaps();
void drawParam(const char *character, int param, float xx, float yy);
void drawParam(const char *character, float param, float xx, float yy);
void drawParam(const char *character, double param, float xx, float yy);

//�i�q�Ԋu
float deltaX;//��x

#define NUM_MAX  501
float f0[NUM_MAX];//�����ʁi���x�C�Z�x etc.�j
float f1[NUM_MAX];//�����ʁi���x�C�Z�x etc.�j
float f_t[NUM_MAX][10];//���n��f�[�^
float time[] = {0.0, 2.0, 4.0, 6.0, 8.0};//���̎���
int mark[10];
int nTime = 5;
Vector2D scale;

int count = 0;
Sle sle = Sle();//Sle�͘A��1���������̃N���X(simultaneous.h�Ɏ���)

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//�\�����[�h
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	//�\���E�B���h�E�̃T�C�Y
	glutInitWindowSize(win_width, win_height);
	//����̈ʒu
	glutInitWindowPosition(win_x0, win_y0);
	//�E�B���h�E�쐬
	main_window = glutCreateWindow("GL_Diffusion1D");//GLUI���ΏۂƂ���E�B���h�E
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(resize);
	//�\��
	glutDisplayFunc(display);

	//�����ݒ�
	init();
	setupGLUI();//myGLUI.h�Ɏ���

	GLUI_Master.set_glutIdleFunc( idle );
	//�C�x���g�������[�v�ɓ���
	glutMainLoop();
	return 0;
}
void idle(void)
{
	//Redisplay���b�Z�[�W��mai_window�ɂȂ�悤�ɃZ�b�g
	if(glutGetWindow() != main_window ) glutSetWindow(main_window);  
	//�ĕ`��
	glutPostRedisplay();
}

void init(void)
{
	//�w�i�F
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	//�\���E�B���h�E�ɕ����������_�����O���鏀��
	setFontBitmaps();

	//���Ԋ֌W
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start��̑��o�ߎ���

	deltaX = rect.size.x / (float)rect.nMesh;//��͗̈��[0, 1]�ɌŒ�(rect.size.x�̒P�ʂ�����[m]�Ƃ���j
	int nWidth = widthInit / deltaX;

	//�����l
	for (int i = 0; i <= rect.nMesh; i++)
	{
		if(i < (rect.nMesh - nWidth) / 2) f0[i] = 0.0;
		else if( i > (rect.nMesh + nWidth) / 2) f0[i] = 0.0;
		else  f0[i] = 1.0;
		
		f_t[i][0] = f1[i] = f0[i];//�����l�i���n��f�[�^�j
	}
	for(int k = 0; k < nTime; k++) {
		mark[k] = 0;//�m��ς݂̂Ƃ�1
		time[k] = (float)k * interval;
	}

	count = 0;
}

void display(void)
{
	//���Ԍv��
	QueryPerformanceFrequency( &freq );
	QueryPerformanceCounter( &currentTime);
	pTime = (double)((currentTime.QuadPart - lastTime.QuadPart) * 1000000 / freq.QuadPart);
	double dt = pTime / 1000000.0;//[sec]
	elapseTime1 += dt;
	static int fps = 0;
	static int fps0 = 0;
	fps ++;

	if(elapseTime1 >= 1.0)
	{
		elapseTime1 = 0.0;
		fps0 = fps;
		fps = 0;
	}
	QueryPerformanceCounter( &lastTime);

	//�J���[�o�b�t�@�̃N���A
	glClear(GL_COLOR_BUFFER_BIT);
  
	glLoadIdentity();

	//�v�Z�̈�`��
	drawRegion();

	//float deltaT = dt / (float)thinningN;	
	float deltaT = deltaT0 / (float)thinningN;
	float d = diff_coef * deltaT / (deltaX * deltaX);//�g�U��

	if(flagStart && !flagFreeze) 
	{ 
		for(int i = 0; i < thinningN; i++) calculate( deltaT, d ); 
		elapseTime2 += dt;
	}

	drawProfile();
	if(flagTime) drawTimeSeries();

	if(flagParameter)
	{
		drawParam("framerate=%d[fps]", fps0, -0.9, 0.85);
		drawParam("timestep=%1.5f[s]", dt, -0.2, 0.85);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 0.5, 0.85);

		drawParam("deltaX=%f", deltaX, -0.9, 0.7);
		drawParam("deltaT=%1.4f[s]", deltaT, -0.2, 0.7);
		drawParam("diffusion=%1.4f", d, 0.5, 0.7);
	}
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

void calculate(float deltaT, float d)
{
	int nWidth = widthInit / deltaX;
	if(flagConstant)//��ɒ��S���x��1�ƂȂ�悤�ɐݒ�
	{
		for (int i = 0; i <= rect.nMesh; i++)
		{
			if(i >= (rect.nMesh - nWidth) / 2 && i <= (rect.nMesh + nWidth) / 2) f0[i] = 1.0;
		}
	}

	if(method == 0)//�z��@
	{
		if(boundary == 0) { f0[0] = 0.0; f0[rect.nMesh] = 0.0;}//Dirichlet
		else if(boundary == 1) { f0[0] = f0[1]; f0[rect.nMesh] = f0[rect.nMesh-1];}//Neumann

		for(int i = 0; i <= rect.nMesh; i++)
		{
			f1[i] = f0[i] + d * (f0[i-1] - 2.0 * f0[i] + f0[i+1]);
		}
		for(int i = 0; i < rect.nMesh; i++) f0[i] = f1[i];
	}

	else//Thomas�@
	{
		float a[3];
		a[0] = - d;
		a[1] = 1.0 + 2.0 * d;
		a[2] = - d;
			
		if(boundary==0) { f0[0] = 0.0; f0[rect.nMesh] = 0.0;}
		else if(boundary == 1) { f0[0] = f0[1]; f0[rect.nMesh] = f0[rect.nMesh-1];}
		sle.Thomas(a, f0, rect.nMesh, boundary);
//if(boundary==0) { f0[0] = 0.0; f0[rect.nMesh] = 0.0;}
//else if(boundary == 1) { f0[0] = f0[1]; f0[rect.nMesh] = f0[rect.nMesh-1];}
		

	}
	
	for(int k = 1; k < nTime; k++)
	{
		if(mark[k] == 1) continue;
		if(elapseTime2 >= time[k])
		{
			for(int i = 0; i <= rect.nMesh; i++) f_t[i][k] = f0[i];
			mark[k] = 1;
		}
	}
}

void drawRegion()
{
	//�S�̘g
	//����
	glLineWidth(1.0);
	//�F
	glColor3f(0.0, 0.0, 0.0);
	//���[�h
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//GL_FILL�ɂ���Ɠh��Ԃ�

	float s1, s2;
	if(win_width >= win_height) 
	{
	  s1 = win_ratio;
	  s2 = 1.0;
	}
	else
	{
	  s1 = 1.0;
	  s2 = 1.0 / win_ratio;
	}

	scale.x = 2.5 * rect.scale * s1;
	scale.y = 3.0 * rect.scale * s2;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;
	//������_
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;

	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glEnd();

	//�ڐ��\��
	//x��
	for(int i = 0; i <= 5; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + (float)i * 0.2 * rect.size.x * scale.x, rect.left0.y);
		glVertex2f(rect.left0.x + (float)i * 0.2 * rect.size.x * scale.x, rect.left0.y + 0.05);
		glEnd();
	}
	drawParam("0", 0 , rect.left0.x - 0.01, rect.left0.y - 0.15);
	drawParam("1", 0 , rect.left0.x - 0.01 + rect.size.x * scale.x, rect.left0.y - 0.15);
	drawParam("x", 0 , rect.left0.x + rect.size.x * scale.x + 0.02, rect.left0.y - 0.05);
	//�c��
	for(int i = 0; i <= 5; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x,        rect.left0.y + (float)i * 0.2 * rect.size.y * 0.8 * scale.y);
		glVertex2f(rect.left0.x + 0.04, rect.left0.y + (float)i * 0.2 * rect.size.y * 0.8 * scale.y);
		glEnd();
	}
	drawParam("0", 0 , rect.left0.x - 0.05, rect.left0.y - 0.05);
	drawParam("1", 0 , rect.left0.x - 0.05, rect.left0.y + rect.size.y * 0.8 * scale.y - 0.05);
	drawParam("f", 0 , rect.left0.x - 0.05, rect.left0.y + rect.size.y * 0.9 * scale.y);



}
void resize(int w, int h)
{
	//�r���[�|�[�g�ϊ�
	glViewport(0, 0, w, h);

	//�\���E�B���h�E�̃T�C�Y
	win_width = w;
	win_height = h;
	win_ratio = (float)win_height / (float)win_width;
}

void drawProfile()
{
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);

	for(int i = 0; i < rect.nMesh; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * deltaX, rect.left0.y + scale.y * f0[i] * rect.size.y * 0.8);
		glVertex2f(rect.left0.x + scale.x * (float)(i+1) * deltaX, rect.left0.y + scale.y * f0[i+1] * rect.size.y * 0.8);
		glEnd();
	}

}
void drawTimeSeries()
{
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);

	for(int k = 0; k < nTime; k++)
	{
		for(int i = 0; i < rect.nMesh; i++)
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x + scale.x * (float)i * deltaX, rect.left0.y + scale.y * f_t[i][k] * rect.size.y * 0.8);
			glVertex2f(rect.left0.x + scale.x * (float)(i+1) * deltaX, rect.left0.y + scale.y * f_t[i+1][k] * rect.size.y * 0.8);
			glEnd();
		}
	}

}

