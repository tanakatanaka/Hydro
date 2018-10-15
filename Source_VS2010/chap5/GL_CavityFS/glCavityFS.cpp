/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
�����`�̈��Cavity
�X�^�K�[�h�i�q��FS�@�iFractional Step Method)
���͂�Poisson�������Ƒ��x�̗A�������������݂Ɍv�Z
���x�̗A����������CIP���g�p
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
void resize(int w, int h);
void calculate(float dt);
void methodCIP(float f[][NUM_MAX],  float gx[][NUM_MAX], float gy[][NUM_MAX], float vx[][NUM_MAX], float vy[][NUM_MAX]);
void drawContour(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawPressureC();
void drawPressureI(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawPsi(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Prs[NUM_MAX][NUM_MAX];//����
float velX[NUM_MAX][NUM_MAX];//staggered�i�q�_��x���x
float velY[NUM_MAX][NUM_MAX];//staggered�i�q�_��y���x
float velXgx[NUM_MAX][NUM_MAX];//���x����
float velXgy[NUM_MAX][NUM_MAX];//���x����
float velYgx[NUM_MAX][NUM_MAX];//���x����
float velYgy[NUM_MAX][NUM_MAX];//���x����
Vector2D Vel[NUM_MAX][NUM_MAX];//�i�q�_�̑��x�i�\���i�q�_�͈��͂Ɠ����i�q�_�j
float Psi[NUM_MAX][NUM_MAX];//����֐��iy���x�Ōv�Z�j
float Omg[NUM_MAX][NUM_MAX];//�Q�x�ix,y���x�Ōv�Z�j
Vector2D scale;//Window������(-1.0, -1.0)�Ƃ����f�J���g���W�ɕ\������Ƃ��̃X�P�[������

int NX, NY;//��͂���Ƃ��̊i�q��
float DX, DY, A1, A2, A3;
int count;
float Courant, diff_num;
float maxPsi0, minPsi0, maxPrs0, minPrs0, maxOmg0, minOmg0;

Particle2D p[MAX_PARTICLE];

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
	main_window = glutCreateWindow("GL_CavityFS");
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(resize);
	//�\��
	glutDisplayFunc(display);
	//�}�E�X����
//	glutMouseFunc(mouse);

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
	elapseTime3 = 0.0;//����������
	
	DX = rect.delta.x = rect.size.x / (float)rect.mesh;
	DY = rect.delta.y = rect.size.y / (float)rect.mesh;

	int i, j;
	NX = rect.mesh + 2;//��͂̊i�q�_�͕ǂ̓�����1����
	NY = NX;//(�����`�j
	//step1(��������)
	for(j = 0; j <= NY; j++)
		for (i = 0; i <= NX; i++)
		{
			
			Prs[i][j] = 0.0;//����
			velX[i][j] = 0.0;//��͗p���x
			velY[i][j] = 0.0;//��͗p���x
			velXgx[i][j] = 0.0;//CIP�ŗ��p���鑬�x�̔���
			velXgy[i][j] = 0.0;//
			velYgx[i][j] = 0.0;//
			velYgy[i][j] = 0.0;//
			Vel[i][j] = Vector2D(0.0, 0.0);//�\���p���x
			Psi[i][j] = 0.0;//����֐�
			Omg[i][j] = 0.0;//�Q�x

		}
		
	float DX2, DY2;
	DX2 = DX * DX;
	DY2 = DY * DY;
	A1 = 0.5 * DY2 / (DX2 + DY2);
	A2 = 0.5 * DX2 / (DX2 + DY2);
	A3 = 0.5 * DY2 / (1.0 + DY2 / DX2);

	count = 0;//�^�C���X�e�b�v��

	countP = 0;//���q���̃J�E���g
	for(i = 0; i < MAX_PARTICLE; i++) p[i] = Particle2D();
	//���q�����z�u
	for(j = 1; j < rect.mesh; j++)
		for (i = 1; i < rect.mesh; i++)
		{
			p[countP].pos = Vector2D((float)i * rect.delta.x, (float)j * rect.delta.y);
			p[countP].size = sizeParticle;
			p[countP].speedC = 1.0;
			if(i <= rect.mesh/3) p[countP].color = RED;
			else if(i <= 2 * rect.mesh/3)  p[countP].color = GREEN;			
			else  p[countP].color = BLUE;
			countP ++;
			if(countP > MAX_PARTICLE) printf("MAX_PARTICLE ��傫�����Ă������� \n");
		}

	maxPrs0 = -1000.0; minPrs0 = 1000.0;
	maxPsi0 = -1000.0; minPsi0 = 1000.0;
	maxOmg0 = -1000.0; minOmg0 = 1000.0;

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
		calculate(deltaT); 
		elapseTime2 += dt;
		elapseTime3 += deltaT;
		count++;
		if(flagStep) flagFreeze = true;
	}

	if( flagPressureColor) drawPressureC(); 
	if( flagPressureIsobar) drawPressureI(Prs, minPrs, maxPrs, BLACK); 
	if( flagStream ) drawContour(Psi, minPsi, maxPsi, BLACK);
	if( flagVorticity ) drawContour(Omg, minOmg, maxOmg, RED);

	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();
	
	if(flagP_Start) drawParticle(deltaT);//���q�A�j���[�V����

	//���U���Ȃ����߂̖ڈ���m�邽��
	float Courant = 1.0 * deltaT / rect.delta.x;	
	float diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.x);//�g�U��

	if(flagParameter)
	{
		drawParam("f_rate=%d[fps]", fps0, -0.99, 0.9);
		drawParam("t_step=%2.5f[s]", 1.0/(float)fps0, -0.5, 0.9);
		drawParam("e_time=%3.2f[s]", elapseTime2, 0.0, 0.9);
		drawParam("n_time=%3.3f", elapseTime3, 0.55, 0.9);
		drawParam("Courant=%1.4f", Courant, -0.99, 0.8);
		drawParam("diff_num=%1.4f", diff_num, -0.45, 0.8);
		drawParam("Re=%5.1f", Re, 0.1, 0.8);
	}
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

void calculate(float deltaT)
{
	int i, j;
	float error, maxError=0.0;
	float a, b, pp;

	//step2(���x�̋��E�����j
	//�㉺
	for (i = 0; i <= NX; i++) {
		velY[i][0] = velY[i][2];
		velY[i][1] = 0.0;
		velX[i][0] = - velX[i][1];

		velX[i][NY-1]= 2.0 - velX[i][NY-2];//�㋫�E�̑��x��1�Ƃ���(���ϒl��1�ƂȂ�)
		velY[i][NY] =  velY[i][NY-2];
		velY[i][NY-1] = 0.0;
	}
	//���E
	for (j = 0; j <= NY; j++) {
		velX[0][j] = velX[2][j];
		velX[1][j] = 0.0;
		velY[0][j] = - velY[1][j];

		velX[NX][j] =   velX[NX-2][j];
		velX[NX-1][j] = 0.0;
		velY[NX-1][j] = -velY[NX-2][j];
	}

	//step3(CIP�ɂ�鑬�x�A��������)
	float vel[NUM_MAX][NUM_MAX];
	//x�������x��`�_�ɂ����鑬�x
	for(i = 1; i < NX; i++)
	for(j = 1; j < NY; j++) 
		vel[i][j] = (velY[i-1][j] + velY[i][j] + velY[i-1][j+1] + velY[i][j+1]) / 4.0;

	methodCIP(velX, velXgx, velXgy, velX, vel);
	//y����
	//y�������x��`�_�ɂ����鑬�x
	for(i = 1; i < NX; i++)
	for(j = 1; j < NY; j++)
		vel[i][j] = (velX[i][j] + velX[i][j-1] + velX[i+1][j-1] + velX[i+1][j]) / 4.0;
	
	methodCIP(velY, velYgx, velYgy, vel, velY);

	//step4(Poisson�������̉�)
	//Poisson�������̉E��
	float D[NUM_MAX][NUM_MAX];
	for (j = 1; j < NY-1; j++)
		for (i = 1; i < NX-1; i++)
		{
			a = (velX[i+1][j] - velX[i][j]) / DX;
			b = (velY[i][j+1] - velY[i][j]) / DY;
			D[i][j] = A3 * (a + b) / deltaT;
		}

	//�����@
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		//���͋��E�l
		for (j = 1; j < NY; j++) {
			Prs[0][j] = Prs[1][j] - 2.0 * velX[0][j] / (DX * Re);//���[
			Prs[NX-1][j] = Prs[NX-2][j] + 2.0 * velX[NX][j] / (DX * Re);//�E�[
		}
		for (i = 1; i < NX; i++){
			Prs[i][0] = Prs[i][1] - 2.0 * velY[i][0] / (DY * Re) ;//���[
			Prs[i][NY-1] = Prs[i][NY-2] + 2.0 * velY[i][NY] / (DY * Re);//��[
		}				
	
		for (j = 1; j < NY-1; j++)
			for (i = 1; i < NX-1; i++)
			{
				pp = A1 * (Prs[i+1][j] + Prs[i-1][j]) + A2 * (Prs[i][j+1] + Prs[i][j-1]) - D[i][j];
				error = fabs(pp -  Prs[i][j]);
				if (error > maxError) maxError = error;
				Prs[i][j] = pp;//�X�V 
			}
		if (maxError < tolerance) break;

		cnt++;
	}
	if(flagParameter) drawParam("cnt=%d", cnt, 0.5, 0.8);

	//step5(�X�^�K�[�h�i�q�_�̑��x�x�N�g���̍X�V�j
	for (j = 1; j < NY-1; j++)
		for(i = 2; i < NX-1; i++)
		{	        
			velX[i][j] += - deltaT * (Prs[i][j] - Prs[i-1][j]) / DX;
		}
	for (j = 2; j < NY-1; j++)
		for(i = 1; i < NX-1; i++)
		{
			velY[i][j] += - deltaT * (Prs[i][j] - Prs[i][j-1]) / DY;
		}

	//�\���̂��߂̑��x�͈��͂Ɠ����ʒu��
	for(j = 1; j <= NY-2; j++)
		for(i = 1; i <= NX-2; i++)
		{
			Vel[i][j].x = (velX[i][j] + velX[i+1][j]) / 2.0;
			Vel[i][j].y = (velY[i][j] + velY[i][j+1]) / 2.0;
		}

	//Psi
	for(j = 0; j < NY-1; j++)
	{
		Psi[0][j] = 0.0;
		for (i = 1; i <= NX-1; i++)
			Psi[i][j] = Psi[i-1][j] - DX * (velY[i-1][j] + velY[i][j]) / 2.0;
	}
	//Omega
	for(i = 1; i <= NX-1; i++)
		for (j = 1; j <= NY-1; j++) 
		{
			Omg[i][j] = 0.5 * ((Vel[i+1][j].y - Vel[i-1][j].y) / DX - (Vel[i][j+1].x - Vel[i][j-1].x) / DY);
		}

	//����֐��C���͂̍ő�l�E�ŏ��l
	for(i = 1; i < NX-1; i++)
		for (j = 1; j < NY-1; j++)
		{
			if(Psi[i][j] > maxPsi0) maxPsi0 = Psi[i][j];
			if(Psi[i][j] < minPsi0) minPsi0 = Psi[i][j];
			if(Prs[i][j] > maxPrs0) maxPrs0 = Prs[i][j];
			if(Prs[i][j] < minPrs0) minPrs0 = Prs[i][j];
			if(Omg[i][j] > maxOmg0) maxOmg0 = Omg[i][j];
			if(Omg[i][j] < minOmg0) minOmg0 = Omg[i][j];
		}

//printf("maxPrs=%f minPrs=%f \n", maxPrs0, minPrs0);
//printf("maxPsi=%f minPsi=%f \n", maxPsi0, minPsi0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);
}

void methodCIP(float f[][NUM_MAX],  float gx[][NUM_MAX], float gy[][NUM_MAX], float vx[][NUM_MAX], float vy[][NUM_MAX])
{
	float newF[NUM_MAX][NUM_MAX];//�֐�
	float newGx[NUM_MAX][NUM_MAX];//x��������
	float newGy[NUM_MAX][NUM_MAX];//y��������
	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dy, dx2, dy2, dx3, dy3; 

	int i, j, ip, jp;
	for(i = 1; i < NX; i++)
		for(j = 1; j < NY; j++)
		{
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
				               + (f[i][j-1] + f[i][j+1] - 2.0 * f[i][j]) / dy2 ) / Re;
		}

	//�X�V
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			f[i][j] = newF[i][j];
			gx[i][j] = newGx[i][j];
			gy[i][j] = newGy[i][j];
		}
}


void drawRegion()
{
	//Cavity
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

	scale.x = 2.0 * rect.scale * s1 * 0.8;
	scale.y = 2.0 * rect.scale * s2 * 0.8;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//�ǋ��E
	glLineWidth(2.0);//����
	//�F
	glColor3f(0.0, 0.0, 0.0);
	//���[�h
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y + sy);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y + sy);
	glEnd();
	//���͋��E
	glLineWidth(1.0);//����
	float sxp = scale.x * (rect.size.x + DX) / 2.0;
	float syp = scale.y * (rect.size.y + DY) / 2.0;
	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sxp, rect.pos0.y - syp);
	glVertex2f(rect.pos0.x + sxp, rect.pos0.y - syp);
	glVertex2f(rect.pos0.x + sxp, rect.pos0.y + syp);
	glVertex2f(rect.pos0.x - sxp, rect.pos0.y + syp);
	glEnd();

	//������_
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;
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

void drawContour(float PP[][NUM_MAX],  float minP, float maxP, Color color)
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

	float dp0 = (maxP - minP) / (float)(nLine +1);//�����Ԋu
	float pp;

	float x1, y1, x2, y2;
	float p[6], x[6], y[6];
	int i, j, k, m, j0 = 0;

	//�O�p�`�Z���ɕ���
	for (k = 0; k < nLine; k++)
	{
		pp = minP + (double)(k + 1) * dp0;

		for(i = 1; i <= NX-2; i++)
		{//(�X�^�K�[�h�i�q�ł�i=1,j=1���ǖʁj
			for(j = 1; j <= NY-2; j++)
			{//�O�p�`�Z���ɕ���
				p[0] = PP[i][j];     x[0] = (float)i * rect.delta.x;     y[0] = (float)j * rect.delta.y;
				p[1] = PP[i][j+1];   x[1] = (float)i * rect.delta.x;     y[1] = (float)(j+1) * rect.delta.y;
				p[2] = PP[i+1][j+1]; x[2] = (float)(i+1) * rect.delta.x; y[2] = (float)(j+1) * rect.delta.y;
				p[3] = PP[i+1][j];   x[3] = (float)(i+1) * rect.delta.x; y[3] = (float)j * rect.delta.y;
				p[4] = p[0]; x[4] = x[0]; y[4] = y[0];//0�Ԗڂɓ���
				//���S
				p[5] = (p[0] + p[1] + p[2] + p[3]) / 4.0;
				x[5] = (x[0] + x[1] + x[2] + x[3]) / 4.0;
				y[5] = (y[0] + y[1] + y[2] + y[3]) / 4.0;

				for(m = 0; m < 4; m++)//�e�O�p�`�ɂ���
				{
					x1 = -1.0; y1 = -1.0; 
					
					if((p[m] <= pp && pp < p[m+1]) || (p[m] >= pp && pp > p[m+1]))
					{
						x1 = x[m] + (x[m+1] - x[m]) * (pp - p[m]) / (p[m+1] - p[m]);
						y1 = y[m] + (y[m+1] - y[m]) * (pp - p[m]) / (p[m+1] - p[m]);
					}
					if((p[m] <= pp && pp < p[5]) || (p[m] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//�܂���_�Ȃ�
						{
							x1 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y1 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
						}
						else//x1�͌�������
						{
							x2 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y2 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
							goto draw_line;
						}
						
					}
					if((p[m+1] <= pp && pp < p[5]) || (p[m+1] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//�܂���_�Ȃ�
						{
							x1 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y1 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
						else//x1�͌�������
						{
							x2 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y2 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
					}

					if(x1 < 0.0) continue;
draw_line:;
					glBegin(GL_LINES);
					glVertex2f(rect.left0.x + scale.x * (x1 - rect.delta.x), rect.left0.y + scale.y * (y1 - rect.delta.y));
					glVertex2f(rect.left0.x + scale.x * (x2 - rect.delta.x), rect.left0.y + scale.y * (y2 - rect.delta.y));
					glEnd();
				}//m
			}//j
		}//i
	}//k
}

void drawPressureI(float PP[][NUM_MAX], float minP, float maxP, Color color)
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

	float dp0 = (maxP - minP) / (float)(nLine +1);//�����Ԋu
	float pp;

	float x1, y1, x2, y2;
	float p[6], x[6], y[6];
	int i, j, k, m, j0 = 0;

	//�O�p�`�Z���ɕ���
	for (k = 0; k < nLine; k++)
	{
		pp = minP + (double)(k + 1) * dp0;

		for(i = 0; i <= NX-2; i++)
		{
			for(j = 0; j <= NY-2; j++)
			{//�O�p�`�Z���ɕ���
				p[0] = PP[i][j];     x[0] = (float)i * rect.delta.x;     y[0] = (float)j * rect.delta.y;
				p[1] = PP[i][j+1];   x[1] = (float)i * rect.delta.x;     y[1] = (float)(j+1) * rect.delta.y;
				p[2] = PP[i+1][j+1]; x[2] = (float)(i+1) * rect.delta.x; y[2] = (float)(j+1) * rect.delta.y;
				p[3] = PP[i+1][j];   x[3] = (float)(i+1) * rect.delta.x; y[3] = (float)j * rect.delta.y;
				p[4] = p[0]; x[4] = x[0]; y[4] = y[0];//0�Ԗڂɓ���
				//���S
				p[5] = (p[0] + p[1] + p[2] + p[3]) / 4.0;
				x[5] = (x[0] + x[1] + x[2] + x[3]) / 4.0;
				y[5] = (y[0] + y[1] + y[2] + y[3]) / 4.0;

				for(m = 0; m < 4; m++)//�e�O�p�`�ɂ���
				{
					x1 = -1.0; y1 = -1.0; 
					
					if((p[m] <= pp && pp < p[m+1]) || (p[m] >= pp && pp > p[m+1]))
					{
						x1 = x[m] + (x[m+1] - x[m]) * (pp - p[m]) / (p[m+1] - p[m]);
						y1 = y[m] + (y[m+1] - y[m]) * (pp - p[m]) / (p[m+1] - p[m]);
					}
					if((p[m] <= pp && pp < p[5]) || (p[m] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//�܂���_�Ȃ�
						{
							x1 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y1 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
						}
						else//x1�͌�������
						{
							x2 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y2 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
							goto draw_line;
						}
						
					}
					if((p[m+1] <= pp && pp < p[5]) || (p[m+1] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//�܂���_�Ȃ�
						{
							x1 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y1 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
						else//x1�͌�������
						{
							x2 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y2 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
					}

					if(x1 < 0.0) continue;
draw_line:;
					glBegin(GL_LINES);
					glVertex2f(rect.left0.x + scale.x * (x1 - rect.delta.x/2.0), rect.left0.y + scale.y * (y1 - rect.delta.y/2.0));
					glVertex2f(rect.left0.x + scale.x * (x2 - rect.delta.x/2.0), rect.left0.y + scale.y * (y2 - rect.delta.y/2.0));
					glEnd();
				}//m
			}//j
		}//i
	}//k
}

void drawPressureC()
{
	float pressW = maxPrs -minPrs;//�\�����͍ő啝
	glPolygonMode(GL_FRONT, GL_FILL);

	float x1, y1, x2, y2, x3, y3, x4, y4;
	float pp[4], rr[4], gg[4], bb[4];
	int i, j, k;

    for (i = 0; i <= NX-2; i++)
    {
        for (j = 0; j <= NY-2; j++)
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
			glVertex2f(rect.left0.x + scale.x * (x1 - rect.delta.x/2.0), rect.left0.y + scale.y * (y1 - rect.delta.y/2.0));
			glColor3f(rr[1], gg[1], bb[1]);
			glVertex2f(rect.left0.x + scale.x * (x2 - rect.delta.x/2.0), rect.left0.y + scale.y * (y2 - rect.delta.y/2.0));
			glColor3f(rr[2], gg[2], bb[2]);
			glVertex2f(rect.left0.x + scale.x * (x3 - rect.delta.x/2.0), rect.left0.y + scale.y * (y3 - rect.delta.y/2.0));
			glColor3f(rr[3], gg[3], bb[3]);
			glVertex2f(rect.left0.x + scale.x * (x4 - rect.delta.x/2.0), rect.left0.y + scale.y * (y4 - rect.delta.y/2.0));
			glEnd();
        }
    }
    
}


void drawVelocity()
{
	int i, j;

	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
	//�`��(�X�^�K�[�h�i�q�ł�i=1,j=1�͑��x�i�q���̕ǖʁj
	float theta, mag;
	for(i = 1; i <= NX-2; i++)
		for (j = 1; j <= NY-2; j++)
		{
			//�Ԉ����\��
			if((i/thinningV) * thinningV != i) continue;
			if((j/thinningV) * thinningV != j) continue;

			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;// 180.0 / M_PI;
			mag = Vel[i][j].magnitude();
			glPushMatrix();
			glTranslatef(rect.left0.x + scale.x * ((float)i-0.5) * rect.delta.x, rect.left0.y + scale.y *  ((float)j-0.5) * rect.delta.y, 0.0);
			//glTranslatef(rect.left0.x + scale.x * ((float)i) * rect.delta.x, rect.left0.y + scale.y *  ((float)j) * rect.delta.y, 0.0);
			glRotatef(theta, 0.0, 0.0, 1.0);
			glScalef(scaleArrow * mag, scaleArrow * mag, 0.0);
			drawArrow();
			glPopMatrix();
		}
}
void drawArrow()
{
	glLineWidth(widthArrow);
	glColor3f(0.0, 0.0, 0.0);

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
	
	for(i = 1; i < rect.mesh; i++)//����i=0�͑��x���i�q����I=1�ɑ���
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * (float)rect.mesh * rect.delta.y);
		glEnd();
	}
	for(j = 1; j < rect.mesh; j++)//j=0�͑��x���i�q����J=1�ɑ���
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * (float)rect.mesh * rect.delta.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glEnd();
	}
}

void drawParticle(float dt)
{
	int k, kk;

	for(k = 0; k < countP; k++)
	{
		p[k].vel = getVelocityParticle(p[k].pos);

		if(!flagFreeze) p[k].update(dt);

		if(p[k].pos.x >= 0.0 && p[k].pos.x < rect.size.x 
			&& p[k].pos.y >= 0.0 && p[k].pos.y < rect.size.y) p[k].show(rect.left0, scale);

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
	for(int i = 0; i < rect.mesh; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.mesh; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//�i�q�_�̑��x����`���
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
