/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2�����|�e���V��������
��l����+�N�o��(�܂��͋z����)+���R�Q�̑��x�|�e���V�����C����֐��C���x�x�N�g��
���q�𑬓x�x�N�g���œ�����
*/
#include <windows.h>
#include <GL/glut.h>
#include <GL/glui.h>
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
void calculate();
void drawContour(float PP[][NUM_MAX], Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
void drawParticle(float dt);
void mouse(int button, int state, int x, int y);
Vector2D getVelocityParticle(Vector2D pos);

float Phi[NUM_MAX][NUM_MAX];//�|�e���V����
float Psi[NUM_MAX][NUM_MAX];//����֐�
//�i�q�_�̑��x
Vector2D Vel[NUM_MAX][NUM_MAX];//���x
Vector2D scale;//Window������(-1.0, -1.0)�Ƃ����f�J���g���W�ɕ\������Ƃ��̃X�P�[������
Vector2D mPos;//�}�E�X���N���b�N���ꂽ�ʒu

Particle2D p[MAX_PARTICLE];

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//�\�����[�h
	glutInitDisplayMode(GLUT_RGBA  | GLUT_DOUBLE);
	//�\���E�B���h�E�̃T�C�Y
	glutInitWindowSize(win_width, win_height);
	//����̈ʒu
	glutInitWindowPosition(win_x0, win_y0);
	//�E�B���h�E�쐬
	main_window = glutCreateWindow("GL_PotentialFlow1");//GLUI���ΏۂƂ���E�B���h�E
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(resize);
	//�\��
	glutDisplayFunc(display);
	//�}�E�X����
	glutMouseFunc(mouse);

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

	//������
	for(int i = 0; i <= rect.nMesh; i++)
		for(int j = 0; j <= rect.nMesh; j++) { Psi[i][j] = 0.0; Phi[i][j] = 0.0; }

	//���q
	countP = 0;	
	for(int i = 0; i < MAX_PARTICLE; i++){
		p[i] = Particle2D();
		p[i].size = sizeParticle;
		p[i].speedC = speedCoef;
	}
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

	alpha = (float)alpha_int;
	//�v�Z�̈�`��
	drawRegion();

	if( flagExecute ) calculate();
	if( flagPotential ) drawContour(Phi, RED);
	if( flagStream ) drawContour(Psi, BLACK);
	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();

	if(flagStart) drawParticle(dt);

	if(!flagStart && flagMouse)//�}�E�X�Ŏw�肵���ʒu����1�����ړ�
	{

		p[0].size = sizeParticle;
		p[0].speedC = speedCoef;
		p[0].vel = getVelocityParticle(p[0].pos);
		if(!flagFreeze) p[0].update(dt);
		if(p[0].pos.x >= 0.0 && p[0].pos.x < rect.size.x && p[0].pos.y >= 0.0 && p[0].pos.y < rect.size.y) p[0].show(rect.left0, scale);
		elapseTime2 += dt;
	}

	if(flagParameter)
	{
		drawParam("framerate=%d[fps]", fps0, -0.9, 0.9);
		drawParam("timestep=%1.5f[s]", dt, -0.2, 0.9);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 0.5, 0.9);
		drawParam("alpha=%3.1f[deg]", alpha, -0.9,0.8);
	}

	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

void drawRegion()
{
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

	scale.x = 2.0 * rect.scale * s1;
	scale.y = 2.0 * rect.scale * s2;
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
	//�i�q�Ԋu
	rect.delta.x = rect.size.x / (float)rect.nMesh;
	rect.delta.y = rect.size.y / (float)rect.nMesh;

	//x-y���W
	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 1.0);
	//x���W
	glBegin(GL_LINES);
	glVertex2f(rect.left0.x, rect.pos0.y );
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.pos0.y );
	//y���W
	glVertex2f(rect.pos0.x, rect.left0.y);
	glVertex2f(rect.pos0.x, rect.left0.y + scale.y * rect.size.y);
	glEnd();

}
void resize(int w, int h)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport(tx, ty, tw, th);
	//�\���E�B���h�E�̃T�C�Y
	win_width = tw;//w;
	win_height = th;//h;
	win_ratio = (float)win_height / (float)win_width;

	glutPostRedisplay();
}

void calculate()
{
	int i, j;
    //�|�e���V�����C����֐�,���x�̃N���A
    for (i = 0; i <= rect.nMesh; i++)
    {
        for (j = 0; j <= rect.nMesh; j++)
        {
            Phi[i][j] = 0.0;
            Psi[i][j] = 0.0;
			Vel[i][j] = Vector2D();
        }
    }
	
	Vector2D z;
	float r2 = 0.0;
	float rad0 = 0.001;

	for (i = 0; i <= rect.nMesh; i++)
	{
		z.x = rect.delta.x * (float)(i - rect.nMesh / 2);//���S�̃|�e���V������0
		for (j = 0; j <= rect.nMesh; j++)
		{
			z.y = rect.delta.y * (float)(j - rect.nMesh / 2);
			if( flagUniform )
			{
				Phi[i][j] = flowVelocity * (z.x * cos(alpha * DEG_TO_RAD) + z.y * sin(alpha * DEG_TO_RAD));
				Psi[i][j] = flowVelocity * (z.y * cos(alpha * DEG_TO_RAD) - z.x * sin(alpha * DEG_TO_RAD));
				Vel[i][j].x = flowVelocity * cos(alpha * DEG_TO_RAD);
				Vel[i][j].y = flowVelocity * sin(alpha * DEG_TO_RAD);
			}

			if (flagSource)//�N���o��(�z�����݁j
			{
                if (z.x == 0.0 && z.y == 0.0)
                {//���_�͑ΐ��I���ٓ_
                    z = rect.delta / 1000.0f;
                }
				r2 = z.magnitude2();
				if(r2 < rad0) r2 = rad0;//���S�t�߂̑��x��}���邽��
                Phi[i][j] += Q_Value * log(r2) / (4.0 * M_PI);
                Psi[i][j] += Q_Value * atan2(z.y, z.x) / (2.0 * M_PI);
				Vel[i][j].x += Q_Value * z.x / r2 / (2.0 * M_PI);
				Vel[i][j].y += Q_Value * z.y / r2 / (2.0 * M_PI);

            }

			if (flagVortex)//����
			{
                if (z.x == 0.0 && z.y == 0.0) 
				{ 
                    z = rect.delta / 1000.0;
				}
				r2 = z.magnitude2();
				if(r2 < rad0) r2 = rad0;//���S�t�߂̑��x��}���邽��
                Psi[i][j] -= Gamma * (float)(log(r2) / (4.0 * M_PI));
                Phi[i][j] += Gamma * (float)atan2(z.y, z.x) / (float)(2.0 * M_PI);
				Vel[i][j].x -= Gamma * z.y / r2 / (2.0 * M_PI);
				Vel[i][j].y += Gamma * z.x / r2 / (2.0 * M_PI);
            }
        }
    }
	flagStart = 0;
}

void drawContour(float PP[][NUM_MAX], Color color)
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

	float maxP =  flowVelocity * range;
	float minP = -flowVelocity * range;
	float dp0 = (maxP - minP) / (float)(nLine +1);//�����Ԋu
	float pp;

	float x1, y1, x2, y2;
	float r1, r2, theta1, theta2, c1, s1, c2, s2;
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

	//�`��
	float theta, mag;
	for(i = 1; i < rect.nMesh; i++)
		for (j = 1; j < rect.nMesh; j++)
		{
			if(Vel[i][j].magnitude() > 100.0) continue;
			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;
			mag = Vel[i][j].magnitude();//���x�̑傫��
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
	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
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
	if(!flagFreeze && countInterval==0.0)
	{
		for(k = 0; k < numP0; k++)
		{
			kk = countP + k;
			p[kk].size = sizeParticle;
			p[kk].speedC = speedCoef;
			p[kk].pos.x = 0.01;
			if(flagSource && Q_Value > 0.0)
			{
				if((kk / 2) * 2 == kk)
				{
					p[kk].pos.y = rect.size.y * getRandom(0.0, 1.0);
					p[kk].color = BLUE;
				}
				else
				{
					p[kk].pos = getRandomVectorXY(0.02) + rect.size/2.0;//left0����̒��S�ʒu
					p[kk].color = RED;
				}
			}
			else
				p[kk].pos.y = rect.size.y * getRandom(0.0, 1.0);

		}
		countP += numP0;
	}

	for(k = 0; k < MAX_PARTICLE; k++)
	{
		p[k].vel = getVelocityParticle(p[k].pos);

		if(!flagFreeze) p[k].update(dt);

		if(p[k].pos.x >= 0.0 && p[k].pos.x < rect.size.x 
			&& p[k].pos.y >= 0.0 && p[k].pos.y < rect.size.y) p[k].show(rect.left0, scale);
	}
	if(countP > MAX_PARTICLE - numP0) countP = 0;
	elapseTime2 += dt;
	countInterval += dt;
	if(countInterval > intervalP) countInterval = 0.0;
}

void mouse(int button, int state, int x, int y)
{
	if(state == GLUT_UP) return;
	//x, y �̓E�B���h�E�������ɂ����������W
	//������(-1.0, -1.0)�Ƃ����f�J���g���W�ɕϊ�
	float xx, yy;
	xx =  2.0 * (float)(x - win_width/2) / (float)win_width;
	yy = -2.0 * (float)(y - win_height/2) / (float)win_height; 
	//��͋�`�̈捶������ɂ����̈���W�n(��rect.size.x, ����rect.size.y)�ɕϊ�
	mPos.x = (xx - rect.left0.x) / scale.x;
	mPos.y = (yy - rect.left0.y) / scale.y;
	//�i�q�ԍ����擾
	int I = 0, J = 0;
	for(int i = 0; i < rect.nMesh; i++)
	{
		if((float)i * rect.delta.x < mPos.x && (float)(i+1) * rect.delta.x > mPos.x) I = i;
	}
	for(int j = 0; j < rect.nMesh; j++)
	{
		if((float)j * rect.delta.y < mPos.y && (float)(j+1) * rect.delta.y > mPos.y) J = j;
	}

	p[0].pos = mPos;
	flagMouse = 1;
	flagStart = 0;
}

Vector2D getVelocityParticle(Vector2D pos)
{
	
	if( pos.x < 0.0 ) return Vector2D(-10.0, -10.0);
	if( pos.x > rect.size.x ) return Vector2D( 10.0, 10.0);
	if(pos.y < 0.0 ) return Vector2D(-10.0, -10.0);
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

