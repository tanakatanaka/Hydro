/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

�P�����ڗ�������
*/
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include "../../support2D.h"
#include "../../myMath6.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾

void idle();
void display();
void drawRegion();
void resize(int w, int h);
void calculate(float deltaT, float c);
void methodCIP(float deltaT);
void strictSolution(float dt);
void drawProfile();
void drawStrict();
void drawTimeSeries();

//������
int nMesh;
//�i�q�Ԋu
float deltaX;//��x

#define NUM_MAX  501
float f0[NUM_MAX];//�����ʁi���x�C�Z�x etc.)�v�Z�O
float f1[NUM_MAX];//�����ʁi���x�C�Z�x etc.)�v�Z��
float g0[NUM_MAX];//�����i�v�Z�O�j
float g1[NUM_MAX];//�����i�v�Z��j

float f_t[4][NUM_MAX][10];//���n��f�[�^
float time[] = {0.0, 2.0, 4.0, 6.0, 8.0};//���̎���[s]
int mark[4][10];
int nTime = 5;

Vector2D scale;
int count = 0;//��������`�悷��ۂɁC���x�i�q�Ԋu��ʉ߂����Ƃ������f�[�^���X�V���邽�߂̃J�E���^
float tt = 0;//��������`�悷��Ƃ��̌o�ߎ���
float hh = 0.25;//1�̃v���t�@�C���̕\���g�̍���

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
	main_window = glutCreateWindow("GL_Advection");//GLUI���ΏۂƂ���E�B���h�E
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

	int i, j, k;
	int nWidth, nCenter;
	nMesh = rect.nMesh;
	deltaX = rect.size.x / (float)nMesh;//��͗̈��[0, 1]�ɌŒ�(rect.size.x�̒P�ʂ�����[m]�Ƃ���j
//printf("deltaX=%f nMesh=%d \n", deltaX, nMesh);
	nWidth = widthInit / deltaX;
	nCenter = centerInit / deltaX;

	if(flagReset == 1)
	{
		for(k = 0; k < nTime; k++)
		{
			for(int i = 0; i <= nMesh ; i++)
			{
				for(j = 0; j < 4; j++) 
				{
					f_t[j][i][k] = 0.0;
					mark[j][k] = 0;//�m��ς݂̂Ƃ�1
				}
			}
		}
	}

	//�����l
	f0[0] = 0.0;
	for(i = 1; i <= nMesh; i++)
	{
		if(i < nCenter - nWidth / 2)  f0[i] = 0.0;
		else if(i < nCenter + nWidth / 2) f0[i] = 1.0;
		else  f0[i] = 0.0;
		for(j = 0; j < 4; j++) f_t[j][i][0] = f1[i] = f0[i];//�����l�i���n��f�[�^�j
	}

	//�����̏����l(CIP�@�j
	for (i = 0; i <= nMesh; i++)
	{
		g0[i] = g1[i] = 0.0;//(f0[i+1] - f0[i-1]) / (2.0 * deltaX);//���ׂ�0�ł��قƂ�Ǔ���
	}

	for(k = 0; k < nTime; k++)
	{
		for(int i = 0; i <= nMesh ; i++)
			for(j = 0; j < 4; j++) mark[j][k] = 0;//�m��ς݂̂Ƃ�1
		
		time[k] = (float)k / fabs(speed) / (float)nTime;//speed=0.1�̂Ƃ�2s�Ԋu
	}
	
	count = 0;
	flagReset = 0;//Riset�{�^�����������܂Ń��Z�b�g���Ȃ�
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

	//�v�Z�̈�`��
	drawRegion();

	//float deltaT = dt / (float)thinningN;
	float deltaT = deltaT0 / (float)thinningN;
	float c = speed * deltaT / deltaX;//�N�[������(speed<0�̂Ƃ�c�����ɂȂ�j

	if(flagStart && !flagFreeze) 
	{ 
		elapseTime2 += deltaT;
		if(method <= 2)//1�����x�C2�����x, CIP
		{
			for(int i = 0; i < thinningN; i++) calculate( deltaT, c ); 
		}

		else strictSolution(deltaT);
	}

	if(method < 3) drawProfile();
	else drawStrict();

	drawTimeSeries();

	if(flagParameter)
	{
		drawParam("framerate=%d[fps]", fps0, -0.9, 0.92);
		drawParam("timestep=%1.5f[s]", dt, -0.2, 0.92);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 0.5, 0.92);
		drawParam("deltaX=%1.5f", deltaX, -0.9, 0.85);
		drawParam("deltaT=%1.5f[s]", deltaT, -0.2, 0.85);
		drawParam("Courant=%1.5f", c, 0.5, 0.85);
	}
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

void calculate(float deltaT, float cou)
{
	int im1 = 0, ip1 = 0, im2 = 0, ip2 = 0;
	float fm1 = 0.0, fp1 = 0.0, fm2 = 0.0, fp2 = 0.0;

	if(method <= 1)//1�����x�C2�����x
	{	
		for(int i = 0; i <= nMesh; i++)
		{
			im1 = i-1; ip1 = i+1;
			if(im1 < 0) fm1 = 0.0; else fm1 = f0[im1];
			if(ip1 > nMesh) fp1 = 0.0; else fp1 = f0[ip1];

			if(method == 0)
			{
				//1�����x
				f1[i] = f0[i] + 0.5 * (cou * (fm1 - fp1) + fabs(cou) * (fp1 + fm1 - 2.0 * f0[i]));
			}
			else if(method == 1) 
			{
				im2 = i - 2; ip2 = i + 2;
				if(im2 < 0) fm2 = 0.0; else fm2 = f0[im2];
				if(ip2 > nMesh) fp2 = 0.0; else fp2 = f0[ip2];
				//2�����x
				f1[i] = f0[i] - 0.25 * ( cou * (fm2 - 4.0 * (fm1 - fp1) - fp2)
					    + fabs(cou) * (fm2 - 4.0 * (fm1 + fp1) + 6.0 * f0[i] + fp2));
			}
		}
		for(int i = 0; i <= nMesh; i++) f0[i] = f1[i];//�v�Z��̃f�[�^������̌v�Z�̂��߂ɕۑ�

	}
	else if(method == 2) methodCIP(deltaT);//CIP

	//���n��f�[�^�̕ۑ�
	for(int k = 1; k < nTime; k++)
	{
		if(mark[method][k] == 1) continue;
		if(elapseTime2 >= time[k] / (float)thinningN)
		{
			for(int i = 0; i <= nMesh; i++) f_t[method][i][k] = f0[i];
			mark[method][k] = 1;
		}
	}
}

void methodCIP(float deltaT)
{
	int ip;
	float Fp, Gp;

	float c3, c2, x;

	float s;

	if( speed > 0.0) s = 1.0;
	else s = -1.0;
	for(int i = 0; i <= nMesh; i++)
	{
		ip = i-(int)s;//�㗬�_
		if(ip < 0.0 || ip > nMesh){
			Fp = 0.0;
			Gp = 0.0;
		}
		else{ 
			Fp = f0[ip];
			Gp = g0[ip];
		}

		float dx = -s * deltaX;
		float dx2 = dx * dx;
		float dx3 = dx2 * dx;

		c3 = (g0[i] + Gp) / dx2 + 2.0 * (f0[i] - Fp) / dx3;
		c2 = 3.0 * (Fp - f0[i]) / dx2 - (2.0 * g0[i] + Gp) / dx;
		x = - speed * deltaT;

		f1[i] += ( (c3 * x + c2) * x + g0[i] ) * x ;
		g1[i] += ( 3.0 * c3 * x + 2.0 * c2 ) * x ;	
	}
	for(int i = 0; i <= nMesh; i++) { f0[i] = f1[i]; g0[i] = g1[i]; }

}

void strictSolution(float dt)
{
	//float ff[NUM_MAX];

	int ip;

	//��͉�(�������j		
	if(elapseTime2 >= (float)count * deltaX / fabs(speed))
	{
		tt = 0.0;

		for(int i = 0; i <= nMesh; i++){
			if(speed > 0.0) ip = i-1;//�㗬�_
			else ip = i+1;//speed<0�̂Ƃ��̏㗬�_
			f1[i] = f0[ip];
		}
		for(int i = 0; i <= nMesh; i++) f0[i] = f1[i];

		count ++;
	}
	tt += dt;

	for(int k = 0; k < nTime; k++)
	{
		if(mark[3][k] == 1) continue;
		if(elapseTime2 >= time[k])
		{
			for(int i = 0; i <= nMesh; i++) {
				if(speed > 0.0) f_t[3][i][k] = f0[i+1];
				else  f_t[3][i][k] = f0[i-1];
			}
			mark[3][k] = 1;
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	scale.x = 2.0 * rect.scale * s1 ;
	scale.y = 1.7 * rect.scale * s2 ;
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

	hh = rect.size.y / 4.0;//1������̕\���g�̍���
	int i;
	//�e�i�̉���
	for(i = 0; i <= 3; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x - 0.04 , rect.left0.y + (float)i * hh * scale.y );
		glVertex2f(rect.left0.x + rect.size.x * scale.x, rect.left0.y + (float)i * hh * scale.y );
		glEnd();
	}
	//����5���������c���ɕ��s�Ɉ����i0.2�Ԋu�C2�b�Ԋu�j
	for(i = 1; i <= 4; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + (float)i * 0.2 * rect.size.x * scale.x, rect.left0.y);
		glVertex2f(rect.left0.x + (float)i * 0.2 * rect.size.x * scale.x, rect.left0.y + rect.size.y * scale.y);
		glEnd();
	}
	//�ڐ��\��
	drawParam("0", 0 , rect.left0.x - 0.01, rect.left0.y - 0.08);
	drawParam("1", 0 , rect.left0.x - 0.01 + rect.size.x * scale.x, rect.left0.y - 0.08);
	drawParam("x", 0 , rect.left0.x + rect.size.x * scale.x + 0.02, rect.left0.y - 0.03);
	//�c���ڐ�
	for(i = 0; i <= 4; i++)
	{
		//f = 1
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x,        rect.left0.y + ((float)i * hh + hh * 0.8) * scale.y);
		glVertex2f(rect.left0.x - 0.04, rect.left0.y + ((float)i * hh + hh * 0.8) * scale.y);
		glEnd();
		//f = 0.5
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x,        rect.left0.y + ((float)i * hh + hh * 0.4) * scale.y);
		glVertex2f(rect.left0.x - 0.02, rect.left0.y + ((float)i * hh + hh * 0.4) * scale.y);
		glEnd();
	}
	drawParam("f", 0 , rect.left0.x - 0.03, rect.left0.y  + 4.0 * hh * scale.y -0.05);
	drawParam("1st order", 0 , rect.left0.x + 0.05, rect.left0.y  + 4.0 * hh * scale.y -0.08);
	drawParam("2nd order", 0 , rect.left0.x + 0.05, rect.left0.y  + 3.0 * hh * scale.y -0.08);
	drawParam("CIP method", 0 , rect.left0.x + 0.05, rect.left0.y  + 2.0 * hh * scale.y -0.08);
	drawParam("strict", 0 , rect.left0.x + 0.05, rect.left0.y  + 1.0 * hh * scale.y -0.08);
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
	float HH;
	
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);

	for(int i = 0; i < nMesh; i++)
	{
		HH = (3.0 - (float)method) * hh;
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i* deltaX, rect.left0.y + scale.y * (HH + (float)f0[i] * hh * 0.8));
		glVertex2f(rect.left0.x + scale.x * (float)(i+1) * deltaX , rect.left0.y + scale.y * (HH + (float)f0[i+1] * hh * 0.8));
		glEnd();
	}
}

void drawStrict()
{
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);

	for(int i = 0; i < nMesh; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * ((float)i* deltaX + speed * tt), rect.left0.y + scale.y * (float)f0[i] * hh * 0.8);
		glVertex2f(rect.left0.x + scale.x * ((float)(i+1) * deltaX + speed * tt), rect.left0.y + scale.y * (float)f0[i+1] * hh * 0.8);
		glEnd();
	}
}

void drawTimeSeries()
{
	float HH;
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);

	for(int j = 0; j < 4; j++)
	for(int k = 0; k < nTime; k++)
	{
		HH = (3.0 - (float)j) * hh;
		for(int i = 0; i < nMesh; i++)
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x + scale.x * (float)i * deltaX, rect.left0.y + scale.y * (HH + (float)f_t[j][i][k] * hh * 0.8));
			glVertex2f(rect.left0.x + scale.x * (float)(i+1) * deltaX, rect.left0.y + scale.y * (HH + (float)f_t[j][i+1][k] * hh * 0.8));
			glEnd();
		}
	}

}



