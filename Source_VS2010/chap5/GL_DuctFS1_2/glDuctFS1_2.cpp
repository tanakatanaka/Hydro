/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
2�̏�Q�������S���ɑ΂��Ώ̂ɒu���ꂽ���s���_�N�g
���M�����[�i�q
���x-���͖@�i�t���N�V���i���E�X�e�b�v�@�j
���͂�Poisson�������Ƒ��x�̗A�������������݂Ɍv�Z
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
void methodCIP(float f[][NUM_MAX_Y],  float gx[][NUM_MAX_Y], float gy[][NUM_MAX_Y], float vx[][NUM_MAX_Y], float vy[][NUM_MAX_Y]);
void calcVelocity();
void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color);
void drawPressureC();
void drawOmegaC();
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Prs[NUM_MAX_X][NUM_MAX_Y];//����
float velX[NUM_MAX_X][NUM_MAX_Y];//�i�q�_�̑��x(��͗p�j
float velY[NUM_MAX_X][NUM_MAX_Y];//���x����
float velXgx[NUM_MAX_X][NUM_MAX_Y];//���x����
float velXgy[NUM_MAX_X][NUM_MAX_Y];//���x����
float velYgx[NUM_MAX_X][NUM_MAX_Y];//���x����
float velYgy[NUM_MAX_X][NUM_MAX_Y];//���x����
float Omg[NUM_MAX_X][NUM_MAX_Y];//�Q�x

Vector2D Vel[NUM_MAX_X][NUM_MAX_Y];//�i�q�_�̑��x�i�\���p�jWindow������(-1.0, -1.0)�Ƃ����f�J���g���W�ɕ\������Ƃ��̃X�P�[������
Vector2D scale;//Window������(-1.0, -1.0)�Ƃ����f�J���g���W�ɕ\������Ƃ��̃X�P�[������

float minPrs0, maxPrs0, minOmg0, maxOmg0;

enum Type{INSIDE, INLET, OUTLET, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM, OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type type[NUM_MAX_X][NUM_MAX_Y]; //�i�q�_�̃^�C�v
int NX, NY;
float DX, DY, A1, A2, A3;
//int meshX1, meshY1, meshX2, meshY2;

Particle2D p[MAX_PARTICLE];
bool flagResetting = false;
int count;

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
	main_window = glutCreateWindow("GL_DuctFS1_2");
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
	
	NX = rect.meshX;
	NY = rect.meshY;
	DX = rect.delta.x = rect.size.x / (float)NX;
	DY = rect.delta.y = rect.size.y / (float)NY;
	A1 = 0.5 * DY*DY / (DX*DX + DY*DY);
	A2 = 0.5 * DX*DX / (DX*DX + DY*DY);
	A3 = 0.25 * DY*DY / (1.0 + DY*DY / (DX*DX));

	float obsTop, obsBottom;
	float yy0 = (rect.size.y - rect.obs_dist) / 2.0;//���̏�Q���̒��S
	obsTop = yy0 + rect.obs_width / 2.0;
	obsBottom = yy0 - rect.obs_width / 2.0;
	float eps = 0.5;
	//���̏�Q�����E�̕����_�ԍ�
	int NX1_1 = floor((rect.obs_left - rect.obs_thick/2.0) / rect.delta.x + eps);//��Q�����[�ʒu
	int NX1_2 = floor((rect.obs_left + rect.obs_thick/2.0) / rect.delta.x + eps);//��Q���E�[�ʒu
	int NY1_2 = floor((yy0 + rect.obs_width/2.0) / (rect.delta.y) + eps);//��Q����[�ʒu
	int NY1_1 = floor((yy0 - rect.obs_width/2.0) / (rect.delta.y) + eps);//��Q�����[�ʒu

	int NY_ObsW = floor(rect.obs_width / DY + eps);
	//�i�q�Ԋu�̐����{��
	float left1, left2, bottom1;
	left1 = DX * (float)(NX1_1);//���[�����Q�����[�܂�
	left2 = rect.obs_left - rect.obs_thick/2.0;
	bottom1 = DY * (float)NY1_1;
	//bottom2 = (rect.size.y - rect.obs_width)/2.0;
	printf("left1=%f, left2=%f, bottom1=%f, obsBottom=%f \n", left1, left2, bottom1, obsBottom);
	eps = 0.00001;
	if( fabs(left1 - left2) > eps || fabs(bottom1 - obsBottom) > eps )
	{
		printf("��Q���̈ʒu�C�T�C�Y��ݒ肵�Ȃ���! \n");
		flagResetting = true;
	}
	else flagResetting = false;

	//��̏�Q�����E�̕����_�ԍ�
	yy0 = (rect.size.y + rect.obs_dist) / 2.0;
	int NX2_1 = floor((rect.obs_left - rect.obs_thick/2.0) / rect.delta.x + eps);//��Q�����[�ʒu
	int NX2_2 = floor((rect.obs_left + rect.obs_thick/2.0) / rect.delta.x + eps);//��Q���E�[�ʒu
	int NY2_2 = floor((yy0 + rect.obs_width/2.0) / (rect.delta.y) + eps);//��Q����[�ʒu
	int NY2_1 = floor((yy0 - rect.obs_width/2.0) / (rect.delta.y) + eps);//��Q�����[�ʒu

	//�i�q�_�̃^�C�v
	int i, j;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			type[i][j] = INSIDE;//���_
			if(j == 0) type[i][j] = BOTTOM;//�����ǖ�
			if(j == rect.meshY) type[i][j] = TOP;//�㑤�ǖ�
			if(i == 0) type[i][j] = INLET;//�����[
			if(i == NX) type[i][j] = OUTLET;//���o�[
			//���̏�Q��
			if(i == NX1_1 && j > NY1_1 && j < NY1_2) type[i][j] = OBS_LEFT;//��Q�����[
			if(i == NX1_2 && j > NY1_1 && j < NY1_2) type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i >= NX1_1 && i <= NX1_2 && j == NY1_2) type[i][j] = OBS_TOP;//��Q����[
			if(i >= NX1_1 && i <= NX1_2 && j == NY1_1) type[i][j] = OBS_BOTTOM;//��Q�����[
			if(i > NX1_1 && i < NX1_2 && j > NY1_1 && j < NY1_2) type[i][j] = OBSTACLE;//��Q������
			if(i == NX1_1 && j == NY1_1) type[i][j] = OBS_LL;//����
			if(i == NX1_1 && j == NY1_2) type[i][j] = OBS_UL;//����
			if(i == NX1_2 && j == NY1_1) type[i][j] = OBS_LR;//�E��
			if(i == NX1_2 && j == NY1_2)  type[i][j] = OBS_UR;//�E��
			//��̏�Q��
			if(i == NX2_1 && j > NY2_1 && j < NY2_2) type[i][j] = OBS_LEFT;//��Q�����[
			if(i == NX2_2 && j > NY2_1 && j < NY2_2) type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i >= NX2_1 && i <= NX2_2 && j == NY2_2) type[i][j] = OBS_TOP;//��Q����[
			if(i >= NX2_1 && i <= NX2_2 && j == NY2_1) type[i][j] = OBS_BOTTOM;//��Q�����[
			if(i > NX2_1 && i < NX2_2 && j > NY2_1 && j < NY2_2) type[i][j] = OBSTACLE;//��Q������
			if(i == NX2_1 && j == NY2_1) type[i][j] = OBS_LL;//����
			if(i == NX2_1 && j == NY2_2) type[i][j] = OBS_UL;//����
			if(i == NX2_2 && j == NY2_1) type[i][j] = OBS_LR;//�E��
			if(i == NX2_2 && j == NY2_2)  type[i][j] = OBS_UR;//�E��
		}

	//�����l
	for (i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			//����
			Prs[i][j] = 0.0;
			//���x
			velX[i][j] = 1.0;
			if(type[i][j] == BOTTOM || type[i][j] == TOP || type[i][j] >= OBS_LEFT) velX[i][j] = 0.0;
			velY[i][j] = 0.0;
			velYgx[i][j] = 0.0;
			velYgy[i][j] = 0.0;
			Vel[i][j].x = velX[i][j];
			Vel[i][j].y = velY[i][j];
			Omg[i][j] = 0.0;//�Q�x
		}

	maxPrs0 = -1000.0; minPrs0 = 1000.0;
	maxOmg0 = -1000.0; minOmg0 = 1000.0;

	count = 0;//�^�C���X�e�b�v��
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
		count++;
		if(count >= 10000) flagFreeze = true;
	}
//printf("dt = %f \n", dt);

	if( flagPressureColor ) drawPressureC(); 
	if( flagOmegaColor ) drawOmegaC();
	if( flagPressureIsobar ) drawContour(Prs, minPrs, maxPrs, BLACK);
	if( flagOmega ) drawContour(Omg, minOmg, maxOmg, RED);

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
	float a, b, pp;

	//���x���E����
	for (j = 0; j <= NY; j++) 
	{
		velX[0][j] = 1.0;//velX[1][j];
		velY[0][j] = 0.0;//velY[1][j];
		velX[NX][j] = velX[NX-1][j]; 
		velY[NX][j] = velY[NX-1][j]; 
	}

	//NS�������ɂ�鑬�x�X�V
	methodCIP(velX, velXgx, velXgy, velX, velY);
	methodCIP(velY, velYgx, velYgy, velX, velY);

	//Poisson�������̉E��
	float D[NUM_MAX_X][NUM_MAX_Y];
	for (j = 1; j < NY; j++)
		for (i = 1; i < NX; i++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			a = (velX[i+1][j] - velX[i-1][j]) / DX;
			b = (velY[i][j+1] - velY[i][j-1]) / DY;
			D[i][j] = A3 * (a + b) / deltaT;
		}

	//Poisson�̕�����������
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		for (i = 0; i <= NX; i++)
			for (j = 0; j <= NY; j++) 
			{
				if(type[i][j] == INSIDE) continue;
				else if(type[i][j] == INLET)  Prs[i][j] = Prs[i+1][j];//Dirichlet(�������j
				else if(type[i][j] == OUTLET) Prs[i][j] = 0.0;//Prs[i-1][j];//Neumann�i���o���j
				else if(type[i][j] == TOP) Prs[i][j] = Prs[i][NY-1];
				else if(type[i][j] == BOTTOM) Prs[i][j] = Prs[i][1];
				else if(type[i][j] == OBS_LEFT) Prs[i][j] = Prs[i-1][j];
				else if(type[i][j] == OBS_RIGHT) Prs[i][j] = Prs[i+1][j];
				else if(type[i][j] == OBS_TOP) Prs[i][j] = Prs[i][j+1];
				else if(type[i][j] == OBS_BOTTOM) Prs[i][j] = Prs[i][j-1];
				else if(type[i][j] == OBS_UL) Prs[i][j] = Prs[i-1][j+1];
				else if(type[i][j] == OBS_UR) Prs[i][j] = Prs[i+1][j+1];
				else if(type[i][j] == OBS_LL) Prs[i][j] = Prs[i-1][j-1];
				else if(type[i][j] == OBS_LR) Prs[i][j] = Prs[i+1][j-1];
			}

		//�����v�Z
		for (j = 1; j < NY; j++)
			for (i = 1; i < NX; i++)
			{
				if(type[i][j] >= OBS_LEFT) continue;
				pp = A1 * (Prs[i+1][j] + Prs[i-1][j]) + A2 * (Prs[i][j+1] + Prs[i][j-1]) - D[i][j];
				error = fabs(pp -  Prs[i][j]);
				if (error > maxError) maxError = error;
				Prs[i][j] = pp;//�X�V 
			}
			
		if (maxError < tolerance) break;
		cnt++;
	}

	if(flagParameter)
	{
		drawParam("cnt=%d", cnt, 0.2, 0.8);
		drawParam("maxError=%2.7f", maxError, 0.5, 0.8);
	}

	//���x�x�N�g���̍X�V
	for (j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{	        
			if(type[i][j] >= OBS_LEFT) continue;
			velX[i][j] += - 0.5 * deltaT * (Prs[i+1][j] - Prs[i-1][j]) / DX;
			velY[i][j] += - 0.5 * deltaT * (Prs[i][j+1] - Prs[i][j-1]) / DY;

			//�\���p
			Vel[i][j].x = velX[i][j];
			Vel[i][j].y = velY[i][j];
		}

	//Omega
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++) 
		{
			Omg[i][j] = 0.5 * ((Vel[i+1][j].y - Vel[i-1][j].y) / DX - (Vel[i][j+1].x - Vel[i][j-1].x) / DY);
		}
	
	//����/�Q�x�̍ő�l�E�ŏ��l
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			if(Prs[i][j] > maxPrs0) maxPrs0 = Prs[i][j];
			if(Prs[i][j] < minPrs0) minPrs0 = Prs[i][j];
			if(Omg[i][j] > maxOmg0) maxOmg0 = Omg[i][j];
			if(Omg[i][j] < minOmg0) minOmg0 = Omg[i][j];
		}

//printf("maxPrs=%f minPrs=%f \n", maxPrs0, minPrs0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);

}

void methodCIP(float f[][NUM_MAX_Y],  float gx[][NUM_MAX_Y], float gy[][NUM_MAX_Y], float vx[][NUM_MAX_Y] , float vy[][NUM_MAX_Y])
{
	float newF[NUM_MAX_X][NUM_MAX_Y];//�֐�
	float newGx[NUM_MAX_X][NUM_MAX_Y];//x��������
	float newGy[NUM_MAX_X][NUM_MAX_Y];//y��������

	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dy, dx2, dy2, dx3, dy3; 

	int i, j, ip, jp;
	for(i = 1; i < NX; i++)
		for(j = 1; j < NY; j++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			if(vx[i][j] >= 0.0) sx = 1.0; else sx = -1.0;
			if(vy[i][j] >= 0.0) sy = 1.0; else sy = -1.0;

			x = - vx[i][j] * deltaT;
			y = - vy[i][j] * deltaT;
			ip = i - (int)sx;//�㗬�_
			jp = j - (int)sy;
			dx = sx * rect.delta.x;
			dy = sy * rect.delta.y;
			dx2 = dx * dx;
			dy2 = dy * dy;
			dx3 = dx2 * dx;
			dy3 = dy2 * dy;

			c30 = ((gx[ip][j] + gx[i][j]) * dx - 2.0 * (f[i][j] - f[ip][j])) / dx3;
			c20 = (3.0 * (f[ip][j] - f[i][j]) + (gx[ip][j] + 2.0 * gx[i][j]) * dx) / dx2;
			c03 = ((gy[i][jp] + gy[i][j]) * dy - 2.0 * (f[i][j] - f[i][jp])) / dy3;
			c02 = (3.0 * (f[i][jp] - f[i][j]) + (gy[i][jp] + 2.0 * gy[i][j]) * dy) / dy2;
			a = f[i][j] - f[i][jp] - f[ip][j] + f[ip][jp];
			b = gy[ip][j] - gy[i][j];
			c12 = (-a - b * dy) / (dx * dy2);
			c21 = (-a - (gx[i][jp] - gx[i][j]) * dx) / (dx2*dy);
			c11 = -b / dx + c21 * dx;

			newF[i][j] = f[i][j] + ((c30 * x + c21 * y + c20) * x + c11 * y + gx[i][j]) * x
				       + ((c03 * y + c12 * x + c02) * y + gy[i][j]) * y;

			newGx[i][j] = gx[i][j] + (3.0 * c30 * x + 2.0 * (c21 * y + c20)) * x + (c12 * y + c11) * y;
			newGy[i][j] = gy[i][j] + (3.0 * c03 * y + 2.0 * (c12 * x + c02)) * y + (c21 * x + c11) * x;
	
	
			//�S�����ɒ�������
			newF[i][j] += deltaT * ( (f[i-1][j] + f[i+1][j] - 2.0 * f[i][j]) / dx2 
				               +  (f[i][j-1] + f[i][j+1] - 2.0 * f[i][j]) / dy2 ) / Re;

		}

	//�X�V
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			f[i][j] = newF[i][j];
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
	//glLineWidth(3.0);

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

	//���̏�Q��
	float yy0 = rect.size.y / 2.0 - rect.obs_dist / 2.0;
	//��Q�����[
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	//��Q���E�[
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	//��Q���㕔
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	//��Q������
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	
	//��̏�Q��
	yy0 = rect.size.y / 2.0 + rect.obs_dist / 2.0;
	//��Q�����[
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	//��Q���E�[
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	//��Q���㕔
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	//��Q������
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glEnd();

}

void drawObstacle()
{
	float yy0;
	//��Q��
	glColor3f(0.7, 0.7, 0.7);
	//���[�h
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//���̏�Q��
	yy0 = rect.size.y / 2.0 - rect.obs_dist / 2.0;
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	glEnd();
	//��̏�Q��
	yy0 = rect.size.y / 2.0 + rect.obs_dist / 2.0;
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (yy0 + rect.obs_width/2.0));
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
        for (i = 0; i < rect.meshX; i++)
        {
            for (j = 0; j < rect.meshY; j++)
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

void drawPressureC()
{
	float pressW = maxPrs -minPrs;//�\�����͍ő啝
	glPolygonMode(GL_FRONT, GL_FILL);

	float x1, y1, x2, y2, x3, y3, x4, y4;
	float pp[4], rr[4], gg[4], bb[4];
	int i, j, k;

    for (i = 0; i < NX; i++)
    {
        for (j = 0; j < NY; j++)
        {

			x1 = (float)i * rect.delta.x;     y1 = (float)j * rect.delta.y;
			x2 = (float)(i+1) * rect.delta.x; y2 = (float)j * rect.delta.y;
			x3 = (float)(i+1) * rect.delta.x; y3 = (float)(j+1) * rect.delta.y;
			x4 = (float)i * rect.delta.x;     y4 = (float)(j+1) * rect.delta.y;
			
			pp[0] = (Prs[i][j] - minPrs) / pressW; 
			pp[1] = (Prs[i+1][j] - minPrs) / pressW;
			pp[2] = (Prs[i+1][j+1] - minPrs) / pressW; 
			pp[3] = (Prs[i][j+1] - minPrs) / pressW;
/*
			for(int k = 0; k < 4; k++)
			{
				if(pp[k] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 2.0 * pp[k]; bb[k] = 1.0 - 2.0 * pp[k];
				}
				else
				{
					rr[k] = 2.0 * pp[k] - 1.0; gg[k] = 2.0 * (1.0 - pp[k]); bb[k] = 0.0;
				}
			}

*/
			for(k = 0; k < 4; k++)
			{

				if(pp[k] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * pp[k]; bb[k] = 1.0;
				}
				else if(pp[k] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - pp[k]);
				}
				else if(pp[k] < 0.75)
				{
					rr[k] = 4.0 * (pp[k] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - pp[k]) * 4.0; bb[k] = 0.0;
				}

			}

			glBegin(GL_QUADS);
			glColor3f(rr[0], gg[0], bb[0]);
			glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
			glColor3f(rr[1], gg[1], bb[1]);
			glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
			glColor3f(rr[2], gg[2], bb[2]);
			glVertex2f(rect.left0.x + scale.x * x3, rect.left0.y + scale.y * y3);
			glColor3f(rr[3], gg[3], bb[3]);
			glVertex2f(rect.left0.x + scale.x * x4, rect.left0.y + scale.y * y4);
			glEnd();
        }
    }
    
}

void drawOmegaC()
{
	float omegaW = maxOmg -minOmg;//�\���Q�x�ő啝
	glPolygonMode(GL_FRONT, GL_FILL);

	float x1, y1, x2, y2, x3, y3, x4, y4;
	float pp[4], rr[4], gg[4], bb[4];
	int i, j, k;

    for (i = 0; i < NX; i++)
    {
        for (j = 0; j < NY; j++)
        {

			x1 = (float)i * rect.delta.x;     y1 = (float)j * rect.delta.y;
			x2 = (float)(i+1) * rect.delta.x; y2 = (float)j * rect.delta.y;
			x3 = (float)(i+1) * rect.delta.x; y3 = (float)(j+1) * rect.delta.y;
			x4 = (float)i * rect.delta.x;     y4 = (float)(j+1) * rect.delta.y;
			
			pp[0] = (Omg[i][j] - minOmg) / omegaW; 
			pp[1] = (Omg[i+1][j] - minOmg) / omegaW;
			pp[2] = (Omg[i+1][j+1] - minOmg) / omegaW; 
			pp[3] = (Omg[i][j+1] - minOmg) / omegaW;
/*
			for(int k = 0; k < 4; k++)
			{
				if(pp[k] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 2.0 * pp[k]; bb[k] = 1.0 - 2.0 * pp[k];
				}
				else
				{
					rr[k] = 2.0 * pp[k] - 1.0; gg[k] = 2.0 * (1.0 - pp[k]); bb[k] = 0.0;
				}
			}

*/
			for(k = 0; k < 4; k++)
			{

				if(pp[k] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * pp[k]; bb[k] = 1.0;
				}
				else if(pp[k] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - pp[k]);
				}
				else if(pp[k] < 0.75)
				{
					rr[k] = 4.0 * (pp[k] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - pp[k]) * 4.0; bb[k] = 0.0;
				}

			}

			glBegin(GL_QUADS);
			glColor3f(rr[0], gg[0], bb[0]);
			glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
			glColor3f(rr[1], gg[1], bb[1]);
			glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
			glColor3f(rr[2], gg[2], bb[2]);
			glVertex2f(rect.left0.x + scale.x * x3, rect.left0.y + scale.y * y3);
			glColor3f(rr[3], gg[3], bb[3]);
			glVertex2f(rect.left0.x + scale.x * x4, rect.left0.y + scale.y * y4);
			glEnd();
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
	for(i = 1; i < rect.meshX; i++)
		for (j = 1; j < rect.meshY; j++)
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
	for(i = 1; i < rect.meshX; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
		glEnd();
	}

	for(j = 1; j < rect.meshY; j++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glEnd();
	}

	glPointSize(5);
	for(i = 0; i <= rect.meshX; i++)
	for(j = 0; j <= rect.meshY; j++)
	{
		if(type[i][j] == INSIDE) glColor3f(0.0, 0.0, 1.0);
		else if(type[i][j] == OBS_TOP || type[i][j] == OBS_BOTTOM ) glColor3f(1.0, 0.0, 0.0); 
		else if(type[i][j] == OBS_LEFT || type[i][j] == OBS_RIGHT) glColor3f(0.0, 1.0, 1.0);
		else if(type[i][j] == OBS_UL || type[i][j] == OBS_UR || type[i][j] == OBS_LL || type[i][j] == OBS_LR) glColor3f(0.0, 1.0, 0.0); 
		else glColor3f(0.0, 0.0, 0.0);

		glBegin(GL_POINTS);
		glVertex2f(rect.left0.x + scale.x * (float)i * DX, rect.left0.y + scale.y * (float)j * DY);
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
	for(int i = 0; i < rect.meshX; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.meshY; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//�i�q�_�̑��x����`���
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
