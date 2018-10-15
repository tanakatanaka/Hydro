/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����Laplace1
����֐��̃��v���X��������L�������@�ŉ���
*/
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glui.h>//GLUI���g�p����
#include "../../support2D.h"
#include "../../myMath6.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾
void idle();
void display();
void drawRegion();
void resize(int w, int h);
void calculate();
void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();

float Psi[NUM_MAX_X][NUM_MAX_Y];//����֐�
int type[NUM_MAX_X][NUM_MAX_Y]; //�i�q�_�̃^�C�v
//�i�q�_�̑��x
Vector2D Vel[NUM_MAX_X][NUM_MAX_Y];
Vector2D scale;

//�i�q���i��Q���j
int nMeshX1;//���[�����Q�����[�܂ł̕�����
int nMeshX2;//���[�����Q���E�[�܂ł̕�����
int nMeshX_ObsT;//��Q�������̕�����
int nMeshY_ObsW;

//Vector2D mPos;//�}�E�X���N���b�N���ꂽ�ʒu
enum Type{INSIDE, INLET, OUTLET, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_RIGHT, OBSTACLE};
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
	main_window = glutCreateWindow("GL_Laplace1");//GLUI���ΏۂƂ���E�B���h�E
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(resize);
	//�\��
	glutDisplayFunc(display);
	//�}�E�X����
//	glutMouseFunc(mouse);
	//glutMotionFunc(motion);

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

	rect.delta.x = rect.size.x / (float)rect.nMeshX;
	rect.delta.y = rect.size.y / (float)rect.nMeshY;
	nMeshX1 = floor(rect.obs_left / rect.delta.x);
	nMeshX2 = floor((rect.obs_left + rect.obs_thickness) / rect.delta.x);
	nMeshX_ObsT = nMeshX2 - nMeshX1;
	nMeshY_ObsW = floor(rect.obs_width / rect.delta.y);

	//�i�q�_�̃^�C�v
	int i, j;
	for(i = 0; i <= rect.nMeshX; i++)
		for(j = 0; j <= rect.nMeshY; j++)
		{
			type[i][j] = INSIDE;//���_
			if(j == 0) type[i][j] = BOTTOM;//�����ǖ�
			if(j == rect.nMeshY) type[i][j] = TOP;//�㑤�ǖ�
			if(i == 0) type[i][j] = INLET;//�����[(���[�j
			if(i == rect.nMeshX) type[i][j] = OUTLET;//���o�[�i�E�[�j
			if(i == nMeshX1 && j < nMeshY_ObsW) type[i][j] = OBS_LEFT;//��Q�����[
			if(i >= nMeshX1 && i <= nMeshX2 && j == nMeshY_ObsW) type[i][j] = OBS_TOP;//��Q����[
			if(i == nMeshX2 && j < nMeshY_ObsW) type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i > nMeshX1 && i < nMeshX2 && j < nMeshY_ObsW) type[i][j] = OBSTACLE;//��Q������
		}

	for(i = 0; i <= rect.nMeshX; i++)
		for(j = 0; j <= rect.nMeshY; j++) Psi[i][j] = 0.0;

	//���Ԋ֌W
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start��̑��o�ߎ���
}

void display(void)
{
	//���Ԍv��
	QueryPerformanceFrequency( &freq );
	QueryPerformanceCounter( &currentTime);
	pTime = (float)((currentTime.QuadPart - lastTime.QuadPart) * 1000000 / freq.QuadPart);
	float dt = pTime / 1000000.0;//[sec]
	elapseTime1 += dt;
	elapseTime2 += dt;
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
//printf("dt = %f \n", dt);
	//�J���[�o�b�t�@�̃N���A
	glClear(GL_COLOR_BUFFER_BIT);

	//�v�Z�̈�`��
	drawRegion();

	if(flagExecute == 1) calculate();

	if(flagStream == 1) drawContour(Psi, 0.0, rect.size.y, BLACK);
	if(flagVelocity == 1) drawVelocity();
	if(flagGrid == 1) drawGrid();

	if(flagParameter)
	{
		drawParam("framerate=%d[fps]", fps0, -0.9, 0.9);
		drawParam("timestep=%1.5f[s]", dt, -0.2, 0.9);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 0.5, 0.9);
	}
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
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

	scale.x = rect.scale * s1 * 1.5;
	scale.y = rect.scale * s2 * 1.5;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//������_
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;

	//��Q��
	glColor3f(0.7, 0.7, 0.7);
	//���[�h
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * rect.obs_width);
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * rect.obs_width);
	glEnd();

	//�S�̘g
	glLineWidth(1.0);
	//�F
	glColor3f(0.0, 0.0, 0.0);
	//���[�h
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glEnd();

	glLineWidth(3.0);
	glBegin(GL_LINES);
	//������
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y);
	//��Q�����[
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * rect.obs_width);
	//��Q���㕔
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * rect.obs_width);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * rect.obs_width);
	//��Q�����[
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * rect.obs_width);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y);
	//�E����
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	//�㕔
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glEnd();
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

void calculate()
{
	int i, j;

	//���E�����Ɠ����i�q�_�̏�������
	for(i = 0; i <= rect.nMeshX; i++)
	for(j = 0; j <= rect.nMeshY; j++)
	{
		if(type[i][j] >= BOTTOM) Psi[i][j] = 0.0;
		else if(type[i][j] == TOP)  Psi[i][j] = rect.size.y;//��l����̗�����1�Ƃ���
		else
		{//���_����ѓ����E�o���͐��`���
			if(i < nMeshX1 || i > nMeshX2)   Psi[i][j] = float(j) * rect.delta.y;
			else  Psi[i][j] = rect.size.y * float(j - nMeshY_ObsW) / (float)(rect.nMeshY - nMeshY_ObsW);
		}
	}

	//�����@
	int cnt = 0;
	float error = 0.0;
	float maxError = 0.0;
	float dx2 = rect.delta.x * rect.delta.x ;
	float dy2 = rect.delta.y * rect.delta.y ;
	
	float pp;
	while (cnt < iteration)
	{
		maxError = 0.0;
		for (i = 1; i < rect.nMeshX; i++)
			for (j = 0; j < rect.nMeshY; j++)
			{
				if(type[i][j] != INSIDE) continue;
				pp = dy2 * (Psi[i - 1][j] + Psi[i + 1][j]) + dx2 *( Psi[i][j - 1] + Psi[i][j + 1]);
				pp /= 2.0 * (dx2 + dy2);
				error = fabs(pp - Psi[i][j]);
				if (error > maxError) maxError = error;
				Psi[i][j] = pp;
			}
		if (maxError < tolerance) break;
		cnt++;
	}
	//���x�x�N�g���̌v�Z
	//�i�q�_�̑��x�x�N�g��(�㉺���E�̗���֐��̍��ŋ��߂�)
	for(i = 1; i < rect.nMeshX; i++)
		for (j = 1; j < rect.nMeshY; j++)
		{
			if(type[i][j] != INSIDE) continue;
	        
			Vel[i][j].x = 0.5 * (Psi[i][j+1] - Psi[i][j-1]) / (float)rect.delta.y;
			Vel[i][j].y = 0.5 * (Psi[i-1][j] - Psi[i+1][j]) / (float)rect.delta.x;
		}

	flagExecute = 0;
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
	{
		for (j = 1; j < rect.nMeshY; j++)
		{
			if(type[i][j] != INSIDE) continue;
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
		if(i <= nMeshX1 || i >= nMeshX2)
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
			glEnd();
		}
		else
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.obs_width);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
			glEnd();
		}

	}
	for(j = 1; j < rect.nMeshY; j++)
	{
		if(j > nMeshY_ObsW)
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
			glEnd();
		}
		else
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
			glEnd();
		}
			
	}
}
