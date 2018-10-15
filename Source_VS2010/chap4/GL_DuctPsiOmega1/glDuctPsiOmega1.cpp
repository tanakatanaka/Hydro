/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
��Q�������Ԃɂ��镽�s���_�N�g
����֐�-�Q�x�@
�Q�x��Poisson�������ƉQ�x�̗A�������������݂Ɍv�Z
*/
#include <windows.h>
#include <GL/glut.h>
#include <GL/glui.h>//GLUI���g�p����
#include <math.h>
#include "../../support2D.h"
#include "../../myMath6.h"
#include "../../particle2D.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾
void idle();
void display();
void drawRegion();
void drawObstacle();
void resize(int w, int h);
void calculate(float dt);
void methodCIP(float deltaT);
void calcVelocity();
void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Psi[NUM_MAX_X][NUM_MAX_Y];//����֐�
float Omega[NUM_MAX_X][NUM_MAX_Y];//�Q�x
float gx[NUM_MAX_X][NUM_MAX_Y];//x��������
float gy[NUM_MAX_X][NUM_MAX_Y];//y��������

Vector2D Vel[NUM_MAX_X][NUM_MAX_Y];//�i�q�_�̑��x
Vector2D scale;//Window������(-1.0, -1.0)�Ƃ����f�J���g���W�ɕ\������Ƃ��̃X�P�[������

float maxPsi0, minPsi0, maxOmg0, minOmg0;

enum Type{INSIDE, INLET, OUTLET, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM, OBS_RIGHT, OBSTACLE, CORNER_UL, CORNER_UR, CORNER_LL, CORNER_LR};
Type type[NUM_MAX_X][NUM_MAX_Y]; //�i�q�_�̃^�C�v
int NX, NY;

Particle2D p[MAX_PARTICLE];
bool flagResetting = false;

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
	main_window = glutCreateWindow("GL_DuctPsiOmega1");
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
	glEnable(GL_POINT_SMOOTH);
	//�\���E�B���h�E�ɕ����������_�����O���鏀��
	setFontBitmaps();
	
	//���Ԋ֌W
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(�������j
	
	NX = rect.nMeshX;
	NY = rect.nMeshY;
	rect.delta.x = rect.size.x / (float)NX;
	rect.delta.y = rect.size.y / (float)NY;
	
	//float obsTop, obsBottom;
	//obsTop = rect.size.y/2.0 + rect.obs_width/2.0;
	//obsBottom = rect.size.y/2.0 - rect.obs_width/2.0;
	//��Q�����E�̕����_�ԍ�
	float eps = 0.5;
	int nMeshX1 = floor((rect.obs_left - rect.obs_thick/2.0) / rect.delta.x + eps);//��Q�����[�ʒu
	int nMeshX2 = floor((rect.obs_left + rect.obs_thick/2.0) / rect.delta.x + eps);//��Q���E�[�ʒu
	int nMeshY2 = floor((rect.size.y + rect.obs_width) / (2.0 * rect.delta.y) + eps);//��Q����[�ʒu
	int nMeshY1 = floor((rect.size.y - rect.obs_width) / (2.0 * rect.delta.y) + eps);//��Q�����[�ʒu

	int nMeshY_ObsW = floor(rect.obs_width / rect.delta.y + eps);
	//�i�q�Ԋu�̐����{��
	float left1, left2, bottom1, bottom2;
	left1 = rect.delta.x * (float)(nMeshX1);//���[�����Q�����[�܂�
	left2 = rect.obs_left - rect.obs_thick/2.0;
	bottom1 = rect.delta.y * (float)nMeshY1;
	bottom2 = (rect.size.y - rect.obs_width)/2.0;
	printf("left1=%f, left2=%f, bottom1=%f, bottom2=%f \n", left1, left2, bottom1, bottom2);
	eps = 0.00001;
	if( fabs(left1 - left2) > eps || fabs(bottom1 - bottom2) > eps )
	{
		printf("��Q���̈ʒu�C�T�C�Y��ݒ肵�Ȃ���! \n");
		flagResetting = true;
	}
	else flagResetting = false;

	//�i�q�_�̃^�C�v
	int i, j;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			type[i][j] = INSIDE;//���_
			if(j == 0) type[i][j] = BOTTOM;//�����ǖ�
			if(j == rect.nMeshY) type[i][j] = TOP;//�㑤�ǖ�
			if(i == 0) type[i][j] = INLET;//�����[
			if(i == rect.nMeshX) type[i][j] = OUTLET;//���o�[
			if(i == nMeshX1 && j > nMeshY1 && j < nMeshY2) type[i][j] = OBS_LEFT;//��Q�����[
			if(i == nMeshX2 && j > nMeshY1 && j < nMeshY2) type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i >= nMeshX1 && i <= nMeshX2 && j == nMeshY2) type[i][j] = OBS_TOP;//��Q����[
			if(i >= nMeshX1 && i <= nMeshX2 && j == nMeshY1) type[i][j] = OBS_BOTTOM;//��Q�����[
			if(i > nMeshX1 && i < nMeshX2 && j > nMeshY1 && j < nMeshY2) type[i][j] = OBSTACLE;//��Q������
			if(i == nMeshX1 && j == nMeshY1) type[i][j] = CORNER_LL;
			if(i == nMeshX1 && j == nMeshY2) type[i][j] = CORNER_UL;
			if(i == nMeshX2 && j == nMeshY1) type[i][j] = CORNER_LR;
			if(i == nMeshX2 && j == nMeshY2)  type[i][j] = CORNER_UR;
		}

	//�����l
	for (i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			//����֐�
			Psi[i][j] = (float)j * rect.delta.y;;
			//�Q�x
			Omega[i][j] = 0.0;
			//���x
			Vel[i][j].x = 1.0;
			if(type[i][j] == BOTTOM || type[i][j] == TOP || type[i][j] >= OBS_LEFT) Vel[i][j].x = 0.0;
			Vel[i][j].y = 0.0;
			gx[i][j] = 0.0;
			gy[i][j] = 0.0;
		}
	//���E�C��������
	//���[����/�o���͗���1(psi = y)
	//���̕ǂ� psi = 0�C��̕ǂ� psi = rect.size.y
	for (i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			if(type[i][j] == BOTTOM) Psi[i][j] = 0.0;
			else if(type[i][j] == TOP) Psi[i][j] = rect.size.y;
			else if(type[i][j] == INLET || type[i][j] == OUTLET) Psi[i][j] = (float)j * rect.delta.y;
			else if(type[i][j] >= OBS_LEFT) Psi[i][j] = rect.size.y / 2.0;//��Q���\��
		}

	calcVelocity();

	maxPsi0 = -1000.0; minPsi0 = 1000.0;
	maxOmg0 = -1000.0; minOmg0 = 1000.0;

	countP = 0;
	for(i = 0; i < MAX_PARTICLE; i++) p[i] = Particle2D();

}

void display(void)
{
	//���Ԍv��
	QueryPerformanceFrequency( &freq );
	QueryPerformanceCounter( &currentTime);
	pTime = (double)((currentTime.QuadPart - lastTime.QuadPart) * 1000000 / freq.QuadPart);
	float dt = pTime / 1000000.0;//[sec]
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
	
	if(flagStart && !flagFreeze) 
	{ 
		elapseTime2 += dt;
		elapseTime3 += deltaT ;//������
		calculate(deltaT); 
		
		if(flagStep) flagFreeze = true;
	}

	if( flagStream ) drawContour(Psi, minPsi, maxPsi, BLACK);
	if( flagVorticity ) drawContour(Omega, minOmg, maxOmg, RED);

	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();

	if(flagP_Start) drawParticle(deltaT);//���q�A�j���[�V����

	drawObstacle();
	//���U���Ȃ����߂̖ڈ���m�邽��
	float Courant, diff_num;
	if(rect.delta.x < rect.delta.y)
	{
		Courant = deltaT / rect.delta.x;	
		diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.x);//�g�U��
	}
	else
	{
		Courant = deltaT / rect.delta.y;	
		diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.y);//�g�U��
	}

	if(flagParameter)
	{
		drawParam("f_rate=%d[fps]", fps0, -0.99, 0.9);
		drawParam("t_step=%2.5f[s]", dt, -0.6, 0.9);
		drawParam("e_time=%3.3f[s]", elapseTime2, -0.2, 0.9);
		drawParam("deltaT=%1.3f", deltaT, 0.2, 0.9);
		drawParam("n_time=%3.3f", elapseTime3, 0.5, 0.9);
		drawParam("Courant=%1.4f", Courant, -0.99, 0.8);
		drawParam("diff_num=%1.4f", diff_num, -0.6, 0.8);
		drawParam("Re=%5.1f", Re, -0.1, 0.8);
	}
	if(flagResetting) drawParam("Reset obstacle size",  -0.5, 0.0);
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

void calculate(float deltaT)
{
	int i, j;
	float error = 0.0;
	float maxError = 0.0;
	float pp;
	float DX = rect.delta.x;
	float DY = rect.delta.y;
	float DX2 = DX * DX;
	float DY2 = DY * DY;
	float DD2 = DX2 + DY2;//corner

	//�Q�x���E����
	float psi_obs = rect.size.y/2.0;
	for (i = 0; i <= NX; i++)
		for (j = 0; j <= NY; j++) 
		{
			if(type[i][j] == INSIDE) continue;
			else if(type[i][j] == INLET) Omega[i][j] = 0.0;//Dirichlet(�������j
			else if(type[i][j] == OUTLET) Omega[i][j] = Omega[i-1][j];//Neumann�i���o���j
			else if(type[i][j] == TOP) Omega[i][j] = -2.0 *( Psi[i][NY-1] - rect.size.y) / DY2;
			else if(type[i][j] == BOTTOM) Omega[i][j] = -2.0 * Psi[i][1] / DY2;
			else if(type[i][j] == OBS_LEFT) Omega[i][j] = -2.0 * (Psi[i-1][j] - psi_obs) / DX2;
			else if(type[i][j] == OBS_RIGHT) Omega[i][j] = -2.0 *( Psi[i+1][j] - psi_obs) / DX2;
			else if(type[i][j] == OBS_TOP) Omega[i][j] = -2.0 *( Psi[i][j+1] - psi_obs) / DY2;
			else if(type[i][j] == OBS_BOTTOM) Omega[i][j] = -2.0 * (Psi[i][j-1] - psi_obs) / DY2;
			else if(type[i][j] == CORNER_UL) Omega[i][j] = -2.0 * (Psi[i-1][j+1] - psi_obs) / DD2;
			else if(type[i][j] == CORNER_UR) Omega[i][j] = -2.0 * (Psi[i+1][j+1] - psi_obs) / DD2;
			else if(type[i][j] == CORNER_LL) Omega[i][j] = -2.0 * (Psi[i-1][j-1] - psi_obs) / DD2;
			else if(type[i][j] == CORNER_LR) Omega[i][j] = -2.0 * (Psi[i+1][j-1] - psi_obs) / DD2;
		}
	
	//Poisson�̕�����������
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		for (i = 1; i < NX; i++)
			for (j = 1; j < NY; j++)
			{
				if(type[i][j] >= OBS_LEFT) continue;
				pp = ( DY2 * (Psi[i - 1][j] + Psi[i + 1][j]) + DX2 *(Psi[i][j - 1] + Psi[i][j + 1])
					 + Omega[i][j] * DX2 * DY2 ) / (2.0 * (DX2 + DY2));
				error = fabs(pp - Psi[i][j]);
				if (error > maxError) maxError = error;
				Psi[i][j] = pp;//�X�V
			}
		if (maxError < tolerance) break;
		cnt++;
	}

	if(flagParameter)
	{
		drawParam("cnt=%d", cnt, 0.2, 0.8);
		drawParam("maxError=%2.7f", maxError, 0.5, 0.8);
	}

	calcVelocity();//���x�̍X�V(//�Ձ��֕ϊ�)

	//�Q�x�A��������������
	methodCIP(deltaT);

	//����֐��C�Q�x�̍ŏ��l�C�ő�l
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			if(Psi[i][j] > maxPsi0) maxPsi0 = Psi[i][j];
			if(Psi[i][j] < minPsi0) minPsi0 = Psi[i][j];
			if(Omega[i][j] > maxOmg0) maxOmg0 = Omega[i][j];
			if(Omega[i][j] < minOmg0) minOmg0 = Omega[i][j];
		}

//printf("maxPsi=%f minPsi=%f \n", maxPsi0, minPsi0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);
}

void calcVelocity()
{
	float DX = rect.delta.x;
	float DY = rect.delta.y;
	int i, j;
	//���x�x�N�g���̌v�Z
	//�i�q�_�̑��x�x�N�g��(�㉺���E�̗���֐��̍��ŋ��߂�)
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			//�|�e���V�����̒Ⴂ�����獂������
			Vel[i][j].x = (Psi[i][j+1] - Psi[i][j-1]) / (DY * 2.0);
			Vel[i][j].y = (Psi[i-1][j] - Psi[i+1][j]) / (DX * 2.0);
		}
}
void methodCIP(float deltaT)
{
	float newOmega[NUM_MAX_X][NUM_MAX_Y];//�Q�x
	float newGx[NUM_MAX_X][NUM_MAX_Y];//x��������
	float newGy[NUM_MAX_X][NUM_MAX_Y];//y��������

	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dx2, dx3, dy, dy2, dy3; 

	int i, j, ip, jp;
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			if(type[i][j] >= OBS_LEFT) continue;

			if(Vel[i][j].x >= 0.0) sx = 1.0; else sx = -1.0;
			if(Vel[i][j].y >= 0.0) sy = 1.0; else sy = -1.0;

			x = - Vel[i][j].x * deltaT;
			y = - Vel[i][j].y * deltaT;
			ip = i - (int)sx;//�㗬�_
			jp = j - (int)sy;
			dx = sx * rect.delta.x;
			dy = sy * rect.delta.y;
			dx2 = dx * dx;
			dx3 = dx2 * dx;
			dy2 = dy * dy;
			dy3 = dy2 * dy;

			c30 = ((gx[ip][j] + gx[i][j]) * dx - 2.0 * (Omega[i][j] - Omega[ip][j])) / dx3;
			c20 = (3.0 * (Omega[ip][j] - Omega[i][j]) + (gx[ip][j] + 2.0 * gx[i][j]) * dx) / dx2;
			c03 = ((gy[i][jp] + gy[i][j]) * dy - 2.0 * (Omega[i][j] - Omega[i][jp])) / dy3;
			c02 = (3.0 * (Omega[i][jp] - Omega[i][j]) + (gy[i][jp] + 2.0 * gy[i][j]) * dy) / dy2;
			a = Omega[i][j] - Omega[i][jp] - Omega[ip][j] + Omega[ip][jp];
			b = gy[ip][j] - gy[i][j];
			c12 = (-a - b * dy) / (dx * dy2);
			c21 = (-a - (gx[i][jp] - gx[i][j]) * dx) / (dx2*dy);
			c11 = - b / dx + c21 * dx;

			newOmega[i][j] = Omega[i][j] + ((c30 * x + c21 * y + c20) * x + c11 * y + gx[i][j]) * x
				        + ((c03 * y + c12 * x + c02) * y + gy[i][j]) * y;

			newGx[i][j] = gx[i][j] + (3.0 * c30 * x + 2.0 * (c21 * y + c20)) * x + (c12 * y + c11) * y;
			newGy[i][j] = gy[i][j] + (3.0 * c03 * y + 2.0 * (c12 * x + c02)) * y + (c21 * x + c11) * x;

			//�S�����ɒ�������
			newOmega[i][j] += deltaT * ( (Omega[i-1][j] - 2.0 * Omega[i][j] + Omega[i+1][j]) / dx2
							 + (Omega[i][j-1] - 2.0 * Omega[i][j] + Omega[i][j+1]) / dy2 ) / Re;

		}
	//�X�V
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			Omega[i][j] = newOmega[i][j];
			gx[i][j] = newGx[i][j];
			gy[i][j] = newGy[i][j];
		}
			
}

void drawRegion()
{
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

	scale.x = 1.5 * rect.scale * s1 ;
	scale.y = 1.5 * rect.scale * s2 ;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//������_
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;

	//�S�̘g
	glLineWidth(1.0);
	//�F
	glColor3f(0.0, 0.0, 0.0);
	//���[�h
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0);

	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glEnd();

	glLineWidth(3.0);
	glBegin(GL_LINES);
	//����
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	//�㑤
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	//��Q�����[
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	//��Q���E�[
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	//��Q���㕔
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width) / 2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width) / 2.0);
	//��Q������
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width) / 2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width) / 2.0);
	glEnd();

}

void drawObstacle()
{
	//��Q��
	glColor3f(0.7, 0.7, 0.7);
	//���[�h
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	glEnd();
}

void resize(int w, int h)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport(tx, ty, tw, th);

	//�r���[�|�[�g�ϊ�
	//glViewport(0, 0, w, h);
	//�\���E�B���h�E�̃T�C�Y
	win_width = tw;//w;
	win_height = th;//h;
	win_ratio = (float)win_height / (float)win_width;

	glutPostRedisplay();
}

void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color)
{
	glLineWidth(2.0);
	if(color == BLACK) glColor3f(0.0, 0.0, 0.0);
	else if(color == RED) glColor3f(1.0, 0.0, 0.0);
	else if(color == GREEN) glColor3f(0.0, 1.0, 0.0);
	else if(color == BLUE) glColor3f(0.0, 0.0, 1.0);
	else if(color == DARK_RED) glColor3f(0.5, 0.0, 0.0);
	else if(color == DARK_GREEN) glColor3f(0.0, 0.5, 0.0);
	else if(color == DARK_BLUE) glColor3f(0.0, 0.0, 0.5);
	else if(color == YELLOW) glColor3f(1.0, 1.0, 0.0);
	else if(color == CYAN) glColor3f(0.0, 1.0, 1.0);
	else if(color == MAGENTA) glColor3f(1.0, 0.0, 1.0);
	
	float dp0 = (maxP - minP) / (float)(nLine +1);//�|�e���V�����Ԋu

	float pp;

	float x, y, x1, y1, x2, y2;
	int i, j, k;

    for (k = 0; k < nLine; k++)
    {
        pp = (float)(k+1) * dp0 + minP;
        for (i = 0; i < rect.nMeshX; i++)
        {
            for (j = 0; j < rect.nMeshY; j++)
            {
 				//1�ł����_�Ȃ�`��
				if( type[i][j] != INSIDE && type[i+1][j] != INSIDE 
					&& type[i+1][j+1] != INSIDE && type[i][j+1] != INSIDE ) continue;
               
				x1 = -1.0f; x2 = -1.0f; y1 = -1.0f; y2 = -1.0f;

                //�i�q�̍��ӂ𒲍�
                if ((pp >= PP[i][j] && pp < PP[i][j + 1]) || (pp <= PP[i][j] && pp > PP[i][j + 1]))
                {
                    x1 = (float)i;
                    y1 = (float)j + (pp - PP[i][j]) / (PP[i][j + 1] - PP[i][j]);
                }

                //��ӂ𒲍�
                if ((pp >= PP[i][j + 1] && pp < PP[i + 1][j + 1]) || (pp <= PP[i][j + 1] && pp > PP[i + 1][j + 1]))
                {
                    x = (float)i + (pp - PP[i][j + 1]) / (PP[i + 1][j + 1] - PP[i][j + 1]);
                    y = (float)(j + 1);
                    if (x1 < 0.0f || y1 < 0.0f)
                    {
                        x1 = x; y1 = y;
                    }
                    else
                    {
                        x2 = x; y2 = y; goto drawLine;
                    }
                }
                //�E�ӂ𒲍�
                if ((pp >= PP[i + 1][j] && pp < PP[i + 1][j + 1]) || (pp <= PP[i + 1][j] && pp > PP[i + 1][j + 1]))
                {
                    x = (float)(i + 1) ;
                    y = (float)j  + (pp - PP[i + 1][j]) / (PP[i + 1][j + 1] - PP[i + 1][j]);

                    if (x1 < 0.0f || y1 < 0.0f)
                    {
                        x1 = x; y1 = y;
                    }
                    else
                    {
                        x2 = x; y2 = y; goto drawLine;
                    }
                }
                //���ӂ𒲍�

                if ((pp >= PP[i][j] && pp < PP[i + 1][j]) || (pp <= PP[i][j] && pp > PP[i + 1][j]))
                {
                    x2 = (float)i + (pp - PP[i][j]) / (PP[i + 1][j] - PP[i][j]);
                    y2 = (float)j;
                }
            drawLine: ;
                if (x1 < 0.0 || y1 < 0.0 || x2 < 0.0 || y2 < 0.0) continue;
				x1 *= rect.delta.x; x2 *= rect.delta.x;
				y1 *= rect.delta.y; y2 *= rect.delta.y;
				glBegin(GL_LINES);
				glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
				glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
				glEnd();
            }
        }
    }
}

void drawVelocity()
{
	int i, j;

	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
	//�`��
	float theta, mag;
	for(i = 1; i < rect.nMeshX; i++)
		for (j = 1; j < rect.nMeshY; j++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			if((i/thinningV) * thinningV != i) continue;
			if((j/thinningV) * thinningV != j) continue;

			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;
			mag = Vel[i][j].magnitude();
			glPushMatrix();
			glTranslatef(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y *  (float)j * rect.delta.y, 0.0);
			glRotatef(theta, 0.0, 0.0, 1.0);
			glScalef(scaleArrow * mag, scaleArrow * mag, 0.0);
			drawArrow();
			glPopMatrix();
		}
}
void drawArrow()
{
	glLineWidth(widthArrow);
	glColor3f(0.5, 0.0, 0.5);

	glBegin(GL_LINES);
	glVertex2f(-0.5, 0.0);
	glVertex2f( 0.5, 0.0);

	glVertex2f( 0.0, 0.2);
	glVertex2f( 0.5, 0.0);

	glVertex2f( 0.0,-0.2);
	glVertex2f( 0.5, 0.0);
	glEnd();
}

void drawGrid()
{
	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 1.0);

	int i, j;
	for(i = 1; i < rect.nMeshX; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
		glEnd();
	}

	for(j = 1; j < rect.nMeshY; j++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glEnd();
	}
}

void drawParticle(float dt)
{
	int k, kk;
	if(!flagFreeze && countInterval == 0.0)
	{
		for(k = 0; k < numP0; k++)
		{
			//�������ɗ��q����
			kk = countP + k;
			p[kk].size = sizeParticle;
			p[kk].speedC = speedCoef;
			p[kk].pos.x = 0.01;	
			p[kk].pos.y = rect.size.y * getRandom(0.0, 1.0);
			if(p[kk].pos.y < rect.size.y / 2.0) p[kk].color = RED;// DARK_YELLOW;
			else p[kk].color = BLUE;//DARK_CYAN;

		}
		countP += numP0;
//printf("size.x=%f , p[0].x = %f, p[1].y = %f \n", rect.size.x,  p[0].pos.x, p[0].pos.y);
	}

	for(k = 0; k < MAX_PARTICLE; k++)
	{
		p[k].vel = getVelocityParticle(p[k].pos);

		if(!flagFreeze) p[k].update(dt);

		if(p[k].pos.x >= 0.0 && p[k].pos.x < rect.size.x 
			&& p[k].pos.y >= 0.0 && p[k].pos.y < rect.size.y) p[k].show(rect.left0, scale);

	}

	if(countP > MAX_PARTICLE - numP0) countP = 0;
	countInterval += dt;
	if(countInterval > intervalP) {
		countInterval = 0.0;
	}
}

Vector2D getVelocityParticle(Vector2D pos)
{
	
	if( pos.x < 0.0 ) return Vector2D(-10.0, -10.0);
	if( pos.x > rect.size.x ) return Vector2D( 10.0, 10.0);
	if( pos.y <  0.0 ) return Vector2D(-10.0, -10.0);
	if( pos.y > rect.size.y ) return Vector2D( 10.0, 10.0);
	//�i�q�ԍ����擾
	int I = 0, J = 0;
	for(int i = 0; i < rect.nMeshX; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.nMeshY; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//�i�q�_�̑��x����`���
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
