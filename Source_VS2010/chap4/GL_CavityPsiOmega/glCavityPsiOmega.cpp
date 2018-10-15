/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
�����`�̈��Cavity
����֐�-�Q�x�@
�Q�x��Poisson�������ƉQ�x�̗A�������������݂Ɍv�Z
�A����������CIP�@��K�p
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
void methodCIP(float deltaT);
void drawContour(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Psi[NUM_MAX][NUM_MAX];//����֐�
float Omega[NUM_MAX][NUM_MAX];//�Q�x
float gx[NUM_MAX][NUM_MAX];//x��������
float gy[NUM_MAX][NUM_MAX];//y��������

Vector2D Vel[NUM_MAX][NUM_MAX];//�i�q�_�̑��x
Vector2D scale;//Window������(-1.0, -1.0)�Ƃ����f�J���g���W�ɕ\������Ƃ��̃X�P�[������

float Courant, diff_num;
float maxPsi0, minPsi0, maxOmg0, minOmg0;

Particle2D p[MAX_PARTICLE];
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
	main_window = glutCreateWindow("GL_CavityPsiOmega");
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
	elapseTime3 = 0.0;//start��̑��o�ߎ���(������)
	
	rect.delta.x = rect.size.x / (float)rect.nMesh;//��͗̈��[0, 1]�ɌŒ�j
	rect.delta.y = rect.size.y / (float)rect.nMesh;//��͗̈��[0, 1]�ɌŒ�j

	int i, j;
	//���E�C��������
	for(j = 0; j <= rect.nMesh; j++)
		for (i = 0; i <= rect.nMesh; i++)
		{
			//����֐�
			Psi[i][j] = 0.0;
			//�Q�x
			Omega[i][j] = 0.0;
			//���x
			Vel[i][j].x = 0.0;
			Vel[i][j].y = 0.0;
			gx[i][j] = 0.0;
			gy[i][j] = 0.0;
		}

	count = 0;//�^�C���X�e�b�v��

	countP = 0;//���q���̃J�E���g
	for(i = 0; i < MAX_PARTICLE; i++) p[i] = Particle2D();
	//���q�����z�u
	for(j = 1; j < rect.nMesh; j++)
		for (i = 1; i < rect.nMesh; i++)
		{
			p[countP].pos = Vector2D((float)i * rect.delta.x, (float)j * rect.delta.y);
			p[countP].size = sizeParticle;
			p[countP].speedC = 1.0;
			if(i <= rect.nMesh/3) p[countP].color = RED;
			else if(i <= 2 * rect.nMesh/3)  p[countP].color = GREEN;			
			else  p[countP].color = BLUE;
			countP ++;
			if(countP > MAX_PARTICLE) printf("MAX_PARTICLE ��傫�����Ă������� \n");
		}


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
		elapseTime2 += dt;
		elapseTime3 += deltaT;//������
		calculate(deltaT); 

		if(flagStep) flagFreeze = true;
		count++;
	}

	if( flagStream ) drawContour( Psi, minPsi, maxPsi,BLACK);
	if( flagVorticity ) drawContour( Omega, minOmg, maxOmg,RED);

	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();

	if(flagP_Start) drawParticle(deltaT);//���q�A�j���[�V����

	//���U���Ȃ����߂̖ڈ���m�邽��
	float Courant = 1.0 * deltaT / rect.delta.x;	
	float diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.x);//�g�U��

	if(flagParameter)
	{
		drawParam("f_rate=%d[fps]", fps0, -0.99, 0.9);
		drawParam("t_step=%2.5f[s]", dt, -0.5, 0.9);
		drawParam("e_time=%3.3f[s]", elapseTime2, 0.05, 0.9);
		drawParam("n_time=%3.3f", elapseTime3, 0.55, 0.9);
		drawParam("Courant=%1.4f", Courant, -0.99, 0.8);
		drawParam("diff_num=%1.4f", diff_num, -0.5, 0.8);
		drawParam("Re=%5.1f", Re, 0.0, 0.8);
	}
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

void calculate(float deltaT)
{
	int i, j;
	float error = 0.0;
	float maxError = 0.0;
	float pp;
	float D = rect.delta.x;
	float D2 = D * D;
	int N = rect.nMesh;

	//Poisson�̕��������������߂̋��E����
	//���E
	for (j = 0; j <= N; j++) {
		Omega[0][j] = -2.0 * Psi[1][j] / D2;
		Omega[N][j] = -2.0 * Psi[N-1][j] / D2;
	}
	//�㉺
	for (i = 0; i <= N; i++) {
		Omega[i][0] = -2.0 * Psi[i][1] / D2;//Bottom
		Omega[i][N] = -2.0 *( Psi[i][N-1] + D) / D2;//Top(���������x��1�j
	}

	//Poisson�̕�����������
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		for (i = 1; i < N; i++)
			for (j = 1; j < N; j++)
			{
				pp = ( (Psi[i - 1][j] + Psi[i + 1][j] + Psi[i][j - 1] + Psi[i][j + 1])
					 + Omega[i][j] * D2 ) / 4.0 ;
				error = fabs(pp - Psi[i][j]);
				if (error > maxError) maxError = error;
				Psi[i][j] = pp;//�X�V
			}
		if (maxError < tolerance) break;
		cnt++;
	}
	//���x�x�N�g���̌v�Z
	//�i�q�_�̑��x�x�N�g��(�㉺���E�̗���֐��̍��ŋ��߂�)
	for(i = 1; i < N; i++)
		for (j = 1; j < N; j++)
		{	        
			//�|�e���V�����̒Ⴂ�����獂������
			Vel[i][j].x = (Psi[i][j+1] - Psi[i][j-1]) / (D * 2.0);
			Vel[i][j].y = (Psi[i-1][j] - Psi[i+1][j]) / (D * 2.0);
		}

	//�Q�x�A��������������
	methodCIP(deltaT);

	if(flagParameter)
	{
		drawParam("cnt=%d", cnt, 0.3, 0.8);
	}


	//����֐��C�Q�x�̍ő�l�E�ŏ��l
	for(i = 1; i < N; i++)
		for (j = 1; j < N; j++)
		{
			if(Psi[i][j] > maxPsi0) maxPsi0 = Psi[i][j];
			if(Psi[i][j] < minPsi0) minPsi0 = Psi[i][j];
			if(Omega[i][j] > maxOmg0) maxOmg0 = Omega[i][j];
			if(Omega[i][j] < minOmg0) minOmg0 = Omega[i][j];
		}

//printf("maxPsi=%f minPsi=%f \n", maxPsi0, minPsi0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);
}

void methodCIP(float deltaT)
{
	float newOmega[NUM_MAX][NUM_MAX];//�Q�x
	float newGx[NUM_MAX][NUM_MAX];//x��������
	float newGy[NUM_MAX][NUM_MAX];//y��������

	int NX = rect.nMesh;
	int NY = rect.nMesh;
	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dx2, dx3, dy, dy2, dy3; 

	int i, j, ip, jp;
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
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
			Omega[i][j] = newOmega[i][j];
			gx[i][j] = newGx[i][j];
			gy[i][j] = newGy[i][j];
		}
			
}

void drawRegion()
{
	//Cavity
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

	scale.x = 2.0 * rect.scale * s1 * 0.8;
	scale.y = 2.0 * rect.scale * s2 * 0.8;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;
	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y + sy);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y + sy);
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

void drawContour(float PP[][NUM_MAX], float minP, float maxP, Color color)
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

		for(i = 0; i < rect.nMesh; i++)
		{
			for(j = 0; j < rect.nMesh; j++)
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
					glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
					glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
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
	float theta, mag;
	for(i = 1; i < rect.nMesh; i++)
		for (j = 1; j < rect.nMesh; j++)
		{
			if((i/thinningV) * thinningV != i) continue;
			if((j/thinningV) * thinningV != j) continue;

			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;// 180.0 / M_PI;
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
	for(i = 1; i < rect.nMesh; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
		glEnd();
	}
	for(j = 1; j < rect.nMesh; j++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
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
	for(int i = 0; i < rect.nMesh; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.nMesh; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//�i�q�_�̑��x����`���
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
