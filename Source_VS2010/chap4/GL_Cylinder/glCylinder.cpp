/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
�~���܂��̗���(�S�~�C�~���̔��a��1�ɌŒ�j
�ɍ��W�ϊ��C����ɓ��a�����ɕs���Ԋu�i�q
����֐�-�Q�x�@
�Q�x��Poisson�������ƉQ�x�̗A�������������݂Ɍv�Z
CIP���g�p
*/
#include <windows.h>
#include <GL/glut.h>
#include <GL/glui.h>//GLUI���g�p����
#include <math.h>
#include "../../support2D.h"
#include "../../myMath6.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾
void idle();
void display();
void drawRegion();
void resize(int w, int h);
void calculate(float dt);
void methodCIP();
void drawContour(float minP, float maxP, float PP[][NUM_MAX], Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
void getVelocityParticle(int k);
void drawCylinder();

float Psi[NUM_MAX][NUM_MAX];//����֐�
float Omg[NUM_MAX][NUM_MAX];//�Q�x
float gx[NUM_MAX][NUM_MAX];//dOmg/dx
float gy[NUM_MAX][NUM_MAX];//dOmg/dy
Vector2D Vel[NUM_MAX][NUM_MAX];//�i�q�_�̑��x
Vector2D Pos[NUM_MAX][NUM_MAX];//�i�q�_�̍��W
Vector2D scale;

float DX, DY,DX2, DY2, DXi2, DYi2, fct;
int NX, NY, count;
float minOmega0, maxOmega0;
float minPsi0 , maxPsi0;
float maxVel;

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
	main_window = glutCreateWindow("GL_Cylinder");//GLUI���ΏۂƂ���E�B���h�E
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
	
	NX = region.nMesh;//���a����������
	NY = 2 * NX;     //���ʊp����
	
	//�~���̔��a��1�ɌŒ�
	float xiMax = log(region.Radius);//�v�Z�ʂɂ�����O�����a(�~�����a��1.0�ɌŒ�j
	DX = xiMax / (float)NX;//���a��������
	DY = 2.0 * M_PI / (float)NY; //���ʊp��������
	DX2 = DX * DX;
	DY2 = DY * DY;
	DXi2 = 1.0 / DX2;
	DYi2 = 1.0 / DY2;
	fct = 1.0 / (2.0 * DX2 + 2.0 * DY2);

	int i, j;
	//��������
	for(j = 0; j <= NY; j++)
		for (i = 0; i <= NX; i++)
		{
			//����֐�
			Psi[i][j] = exp((float)i * DX) * sin((float)j * DY - M_PI);//������𕉂̂����Ƃ��Ă���
			//�Q�x
			Omg[i][j] = 0.0;
			gx[i][j] = 0.0;
			gy[i][j] = 0.0;
		}

	//�i�q�_�̑��x�x�N�g��(�㉺���E�̗���֐��̍��ŋ��߂�)
	float ex;
	int jm, jp;
	for(i = 1; i <= NX; i++)
	{
		ex = exp(- (float)i * DX);
		for (j = 0; j < NY; j++)
		{
			if(j == 0) jm = NY -1; else jm = j - 1;
			if(j == NY-1) jp = 0; else jp = j + 1;
			//�|�e���V�����̍���������Ⴂ����(�t�j
			Vel[i][j].x = ex * (Psi[i][jp] - Psi[i][jm]) / (2.0*(float)DY);//���a����
			Vel[i][j].y = ex * (Psi[i-1][j] - Psi[i+1][j]) / (2.0*(float)DX);//���ʊp����
		}
	}

	//�i�q�_���W�i���炩���ߌv�Z�j
	float r, theta;
    for (i = 0; i <= NX; i++)
    {
		r = exp((float)i * DX);
		for (j = 0; j <= NY; j++)
        {
			theta = (float)j * DY - M_PI; //�����������

			Pos[i][j].x = r * cos(theta);//�����
			Pos[i][j].y = r * sin(theta);
		}
	}

	minOmega0 = minPsi0 = 1000.0;
	maxOmega0 = maxPsi0 = -1000.0;
	maxVel = -1000.0;

	elapseTime2 = 0.0;
	elapseTime3 = 0.0;
	count = 0;//�v�Z�̉�
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
		elapseTime3 += deltaT;//������
		calculate(deltaT); 
		count++;
		if(flagStep) flagFreeze = true;

//if(count == 1000) flagFreeze = 1;
//if(count == 2500) flagFreeze = 1;
//if(count == 5000) flagFreeze = 1;

	}

	if( flagStream ) drawContour((float)minPsi, (float)maxPsi , Psi, BLACK);
	if( flagOmega ) drawContour( minOmega, maxOmega, Omg, RED);
	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();

	//���U���Ȃ����߂̖ڈ���m�邽��
	float Courant, diff_num;
	if(DX < DY)
	{
		Courant = deltaT / DX;	
		diff_num = (1.0 / Re) * deltaT / (DX * DX);//�g�U��
	}
	else
	{
		Courant = deltaT / DY;	
		diff_num = (1.0 / Re) * deltaT / (DY * DY);//�g�U��
	}

	if(flagParameter)
	{
		drawParam("f_rate=%d[fps]", fps0, -0.95, 0.87);
		drawParam("t_step=%2.5f[s]", 1.0/(float)fps0, -0.6, 0.87);
		drawParam("e_time=%3.3f[s]", elapseTime2, -0.1, 0.87);
		drawParam("n_time=%3.3f", elapseTime3, 0.3, 0.87);
		drawParam("Courant=%1.4f", Courant, -0.95, 0.77);
		drawParam("diff_num=%1.4f", diff_num, -0.6, 0.77);
		drawParam("Re=%1.4f", Re, -0.15, 0.77);

	}
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

void calculate(float deltaT)
{
	int i, j;
	float error = 0.0;
	float maxError = 0.0;
	float pp, ex2;

	//���E����
	//�~����
	for (j = 0; j < NY; j++) {
		Omg[0][j] = -2.0 * Psi[1][j] * DXi2;
		Psi[0][j] = 0.0;
	}
	//�O�����E
	for (j = 0; j < NY; j++) {
		//Psi[NX][j] = Pos[NX][j].y;
		Omg[NX][j] = Omg[NX-1][j];//Neumann
	}
//printf("Psi0 = %f \n", Psi[0][10]);
	//Poisson�̕�����������
	int jm, jp;
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		for (i = 1; i < NX; i++)
		{
			ex2 = exp(2.0 * (float)i * DX);
			for (j = 0; j < NY; j++)
			{

				jm = j - 1;
				if(jm == -1) jm = NY-1; //else jm = j - 1;
				jp = j + 1;
				if(jp == NY) jp = 0; //else jp = j + 1;

				pp = ( (Psi[i - 1][j] + Psi[i + 1][j] ) * DY2
				     + (Psi[i][jm] + Psi[i][jp]) * DX2
				     + Omg[i][j] * ex2 * DX2 * DY2 ) * fct ;
				error = fabs(pp - Psi[i][j]);
				if (error > maxError) maxError = error;
				Psi[i][j] = pp;//�X�V
			}
		}
		if (maxError < tolerance) break;
		cnt++;
	}

	if(flagParameter)
	{
		drawParam("cnt=%d", cnt, 0.2, 0.77);
		drawParam("maxError=%2.7f", maxError, 0.4, 0.77);
	}

	//���x�x�N�g���̌v�Z
	//�i�q�_�̑��x�x�N�g��(�㉺���E�̗���֐��̍��ŋ��߂�)
	float ex;
	for(i = 1; i < NX; i++)
	{
		ex = exp(- (float)i * DX);
		for (j = 0; j < NY; j++)
		{	        
			if(j == 0) jm = NY -1; else jm = j - 1;
			if(j == NY-1) jp = 0; else jp = j + 1;
			//�|�e���V�����̒Ⴂ�����獂������
			Vel[i][j].x = ex * (Psi[i][jp] - Psi[i][jm]) / (DY * 2.0);//���a����
			Vel[i][j].y = ex * (Psi[i-1][j] - Psi[i+1][j]) / (DX * 2.0);//���ʊp����
		}
	}

	//�Q�x�A��������������
	methodCIP();

	for(i = 1; i < NX; i++)
	{
		for (j = 0; j < NY; j++)
		{	        

			//�ő�l�E�ŏ��l
			if(Psi[i][j] < minPsi0) minPsi0 = Psi[i][j];
			if(Psi[i][j] > maxPsi0) maxPsi0 = Psi[i][j];
			if(Omg[i][j] < minOmega0) minOmega0 = Psi[i][j];
			if(Omg[i][j] > maxOmega0) maxOmega0 = Psi[i][j];
		}
	}

//printf("minPsi=%f maxPsi=%f maxVel=%f \n", minPsi0, maxPsi0, maxVel);
//printf("minOmega=%f maxOmega=%f  \n", minOmega0, maxOmega0);
}

void methodCIP()
{
	float newOmg[NUM_MAX][NUM_MAX];//�Q�x
	float newGx[NUM_MAX][NUM_MAX];//x��������
	float newGy[NUM_MAX][NUM_MAX];//y��������

	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dy, dx2, dy2, dx3, dy3, ex, ex2; 

	int i, j, iup, jup;
	for(i = 1; i < NX; i++)
	{
		ex = exp(-(float)i * DX);
		ex2 = ex * ex;
		for(j = 0; j < NY; j++)
		{
			if(Vel[i][j].x >= 0.0) sx = 1.0; else sx = -1.0;
			if(Vel[i][j].y >= 0.0) sy = 1.0; else sy = -1.0;

			x = - Vel[i][j].x * deltaT;
			y = - Vel[i][j].y * deltaT;
			iup = i - (int)sx;//����_
			jup = j - (int)sy;
			//if(jup == NY) jup = 0;
			dx = sx * DX;
			dy = sy * DY;
			dx2 = dx * dx;
			dy2 = dy * dy;
			dx3 = dx2 * dx;
			dy3 = dy2 * dy;

			c30 = ((gx[iup][j] + gx[i][j]) * dx - 2.0 * (Omg[i][j] - Omg[iup][j]) ) / dx3;
			c20 = (3.0 * (Omg[iup][j] - Omg[i][j])  + (gx[iup][j] + 2.0 * gx[i][j]) * dx) / dx2;
			c03 = ((gy[i][jup] + gy[i][j]) * dy - 2.0 * (Omg[i][j] - Omg[i][jup]) ) / dy3;
			c02 = (3.0 * (Omg[i][jup] - Omg[i][j]) + (gy[i][jup] + 2.0 * gy[i][j]) * dy) / dy2;
			a =( Omg[i][j] - Omg[i][jup] - Omg[iup][j] + Omg[iup][jup]) ;
			b = gy[iup][j] - gy[i][j];
			c12 = (-a - b * dy) / (dx * dy2);
			c21 = (-a - (gx[i][jup] - gx[i][j]) * dx) / (dx2*dy);
			c11 = -b / dx + c21 * dx;

			newOmg[i][j] = Omg[i][j] + (((c30 * x + c21 * y + c20) * x + c11 * y + gx[i][j]) * x
				       + ((c03 * y + c12 * x + c02) * y + gy[i][j]) * y) * ex;

			newGx[i][j] = gx[i][j] + ((3.0 * c30 * x + 2.0 * (c21 * y + c20)) * x + (c12 * y + c11) * y) * ex;
			newGy[i][j] = gy[i][j] + ((3.0 * c03 * y + 2.0 * (c12 * x + c02)) * y + (c21 * x + c11) * x) * ex;
	
			//�S�����ɒ�������
			newOmg[i][j] += deltaT * ( (Omg[i-1][j] + Omg[i+1][j] - 2.0 * Omg[i][j]) / dx2 
				           +  (Omg[i][j-1] + Omg[i][j+1] - 2.0 * Omg[i][j]) / dy2 ) * ex2 / Re;
		}
	}
	//�X�V
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			Omg[i][j] = newOmg[i][j];
			gx[i][j] = newGx[i][j];
			gy[i][j] = newGy[i][j];
			
			
if(Omg[i][j] > maxOmega0) maxOmega0 = Omg[i][j];
if(Omg[i][j] < minOmega0) minOmega0 = Omg[i][j];
		}
	
}


void drawRegion()
{
	//Cavity
	//����
	glLineWidth(2.0);
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

	scale.x = 2.0 * region.scale * s1 / region.Radius * 0.8;// 100.0;//R=100��1���x�ɂȂ�悤�ɑS�̕\��
	scale.y = 2.0 * region.scale * s2 / region.Radius * 0.8;//100.0;
	float sx = scale.x * region.Radius ;
	float sy = scale.y * region.Radius ;

	drawCylinder();
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

void drawContour(float minP, float maxP, float PP[][NUM_MAX], Color color)
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
	int i, j, k, m, jp;
	float OUT_REGION = -1000.0;

	//�O�p�`�Z���ɕ���
	for (k = 0; k < nLine; k++)
	{
		pp = minP + (float)(k + 1) * dp0;

		for(i = 0; i < NX; i++)
		{
			for(j = 0; j < NY; j++)
			{
				if(j == NY-1) jp = 0; else jp = j + 1;
				//�O�p�`�Z���ɕ���
				p[0] = PP[i][j];     x[0] = Pos[i][j].x; y[0] = Pos[i][j].y;
				p[1] = PP[i+1][j];   x[1] =  Pos[i+1][j].x; y[1] =  Pos[i+1][j].y;
				p[2] = PP[i+1][jp]; x[2] =  Pos[i+1][jp].x; y[2] =  Pos[i+1][jp].y;
				p[3] = PP[i][jp];   x[3] = Pos[i][jp].x; y[3] =  Pos[i][jp].y;
				p[4] = p[0]; x[4] = x[0]; y[4] = y[0];//0�Ԗڂɓ���
				//���S
				p[5] = (p[0] + p[1] + p[2] + p[3]) / 4.0;
				x[5] = (x[0] + x[1] + x[2] + x[3]) / 4.0;
				y[5] = (y[0] + y[1] + y[2] + y[3]) / 4.0;

				for(m = 0; m < 4; m++)//�e�O�p�`�ɂ���
				{
					x1 = OUT_REGION; y1 = OUT_REGION; 
					
					if((p[m] <= pp && pp < p[m+1]) || (p[m] >= pp && pp > p[m+1]))
					{
						x1 = x[m] + (x[m+1] - x[m]) * (pp - p[m]) / (p[m+1] - p[m]);
						y1 = y[m] + (y[m+1] - y[m]) * (pp - p[m]) / (p[m+1] - p[m]);
					}
					if((p[m] <= pp && pp < p[5]) || (p[m] >= pp && pp > p[5]))
					{
						if(x1 == OUT_REGION)//�܂���_�Ȃ�
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
						if(x1 == OUT_REGION)//�܂���_�Ȃ�
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

					if(x1 == OUT_REGION) continue;
draw_line:;
					glBegin(GL_LINES);
					glVertex2f(region.pos0.x + scale.x * x1, region.pos0.y + scale.y * y1);
					glVertex2f(region.pos0.x + scale.x * x2, region.pos0.y + scale.y * y2);
					glEnd();
				}//m
			}//j
		}//i
	}//k
}

void drawVelocity()
{
	int i, j;

	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
	//�`��
	float theta, mag,  x, y, r, alpha;
	for(i = 1; i < NX; i++)
	{
		//r = Exp[i];//
		r = exp((float)i * DX);
		for (j = 0; j < NY; j++)
		{
			if((i/thinningV) * thinningV != i) continue;
			if((j/thinningV) * thinningV != j) continue;
			alpha = (float)j * DY - M_PI;//���a�̊p�x(�������180���j
			x = r * cos(alpha); y = r * sin(alpha);//�i�q�_�ʒu
			theta = atan2(Vel[i][j].y, Vel[i][j].x);
			theta += alpha;//����������̕Ίp
			theta *=  RAD_TO_DEG;//degree
			mag = Vel[i][j].magnitude();
			glPushMatrix();
			glTranslatef(region.pos0.x + scale.x * x, region.pos0.y + scale.y * y, 0.0);
			glRotatef(theta, 0.0, 0.0, 1.0);
			glScalef(scaleArrow * mag, scaleArrow * mag, 0.0);
			drawArrow();
			glPopMatrix();
		}
	}
}
void drawArrow()
{
	glLineWidth(widthArrow);
	glColor3f(0.5, 0.0, 0.5);

	glBegin(GL_LINES);
	glVertex2f(-0.5, 0.0);
	glVertex2f( 0.5, 0.0);

	glVertex2f( 0.0, 0.15);
	glVertex2f( 0.5, 0.0);

	glVertex2f( 0.0,-0.15);
	glVertex2f( 0.5, 0.0);
	glEnd();
}

void drawGrid()
{
	//�ɍ��W�i���a�����ɍ��W�ϊ��j
	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 1.0);

	int i, j;
	float x1, y1, x2, y2;
	//�S�~
	for(i = 0; i <= NX; i++)//���a������
	{
		glBegin(GL_LINE_STRIP);
		for(j = 0; j <= NY; j++)
		{

			x1 = Pos[i][j].x;
			y1 = Pos[i][j].y; 
			glVertex2f(region.pos0.x + scale.x * x1, region.pos0.y + scale.y * y1);

		}
		glEnd();
	}
	//���a
	for(j = 0; j <= NY; j++)//���ʊp�ƕ���
	{
		x1 = Pos[0][j].x; 
		y1 = Pos[0][j].y;
		x2 = Pos[NX][j].x;
		y2 = Pos[NX][j].y;
		glBegin(GL_LINES);
		glVertex2f(region.pos0.x + scale.x * x1, region.pos0.y + scale.y * y1);
		glVertex2f(region.pos0.x + scale.x * x2, region.pos0.y + scale.y * y2);
		glEnd();
	}
}

void drawCylinder()
{
	int nn = 20;
	float theta0 = 2.0 * M_PI / (float)nn;
	Vector2D pos0, pos1, pos2;
	pos0 = Vector2D(0.0, 0.0);//���_

	glPolygonMode(GL_FRONT, GL_FILL);
	glColor3f(0.6, 0.8, 0.8);

	int j;
	for(j = 0; j < nn; j++)
	{

		pos1.x = cos((float)j * theta0);
		pos1.y = sin((float)j * theta0);
		pos2.x = cos((float)(j+1) * theta0);
		pos2.y = sin((float)(j+1) * theta0);
	
		glBegin(GL_TRIANGLES);
		glVertex2f(region.pos0.x + scale.x * pos0.x, region.pos0.y + scale.y * pos0.y);
		glVertex2f(region.pos0.x + scale.x * pos1.x, region.pos0.y + scale.y * pos1.y);
		glVertex2f(region.pos0.x + scale.x * pos2.x, region.pos0.y + scale.y * pos2.y);
		glEnd();
	}

	//�֊s
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(2.0);
	glColor3f(0.0, 0.0, 0.0);
	for(j = 0; j < nn; j++)
	{
		pos1.x = cos((float)j * theta0);
		pos1.y = sin((float)j * theta0);
		pos2.x = cos((float)(j+1) * theta0);
		pos2.y = sin((float)(j+1) * theta0);
		glBegin(GL_LINES);
		glVertex2f(region.pos0.x + scale.x * pos1.x, region.pos0.y + scale.y * pos1.y);
		glVertex2f(region.pos0.x + scale.x * pos2.x, region.pos0.y + scale.y * pos2.y);
		glEnd();
	}
		
}
