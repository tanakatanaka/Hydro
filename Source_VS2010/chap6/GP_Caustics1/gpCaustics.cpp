/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
���ʂ̉Q��\���i���q�𐅖ʂɕ����ׂ�)
���܊��}�b�s���O��ǉ�
���e�}�b�s���O�ɂ��R�[�X�e�B�b�N�X�̒ǉ�
�T�C�Y20�~20�C������200�~200�ɌŒ�
�ړ���Q���i�~���C���������^���C�����~�^���j
���E�͌Œ�ǖ�
���x-���͖@�i�t���N�V���i���E�X�e�b�v�@�j
���M�����[�i�q
GPGPU(���l�v�Z��GPU���Ŏ��s�j
CIP���g�p
���x��Poisson�������������ɂ͉����ĂȂ��i�P�񂾂��j
GLSL�ɂ��V�F�[�f�B���O
�������͂���
*/

#include <windows.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glui.h>
#include "../../myGlsl.h"
#include "../../myMath6.h"
#include "../../myPrimitive6.h"
#include "../../rigid.h"
#include "../../support3D.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾
void init();
void idle();
void display();
void rendering();
void setTextureVelX();
void setFramebufferVelX();
void setTextureVelY();
void setFramebufferVelY();
void setTextureWave();
void setFramebufferWave();
void setTextureCaus();
void drawNumberingPoints();
void renewVelX();
void renewVelY();
void renewWaveVel();
void renewWavePos();
void renewParticle();
void renderingP();
void drawPlate();
void drawParticles();
void setTextureParticle();
void setFramebufferParticle();
void calcObsPos();
void setFramebufferCube();
void setCubeMap();
void makeTextureOfCubeMapping();
void refractMapping();
void makeUpperTank();
void makeLowerTank();
void setTextureMatrix();

//���z�����A�j���[�V����
#include "../../myFish1.h"
CFish1 fish1 ;
#include "../../myFish2.h"
CFish2 fish2 ;

//��͗p�f�[�^�̃T�C�Y
int texWidth, texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *velX;//���xVelX�ƈ���
float *velY;//���xVelY�ƉQ�x
float *wave;//�g�̍����̑��x�C�ʒu
GLubyte *caus;
float *particle;//���q�ʒu
GLuint texID[6];//texture object
GLuint fbo[5];//frame buffer object
GLuint rbo;   //render buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;
GLenum type = GL_FLOAT;
//CubeMap�p
#define CUBE_WIDTH  512
#define CUBE_HEIGHT 512

//�V�F�[�_�v���O������
GLuint shader1, shader2, shader3, shader4, shader5, shader6, shader7, shader8 ;

enum Type{INSIDE, LEFT, RIGHT, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
          OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //�i�q�_�̃^�C�v

int NX, NY, NX1, NY1, NX2, NY2;
float DX, DY;
float direction = 1.0;//��Q�������ړ�����

//�e�N�X�`���[�\����(cube map)
struct Target{
	GLenum name;//�^�[�Q�b�g��
	GLdouble cx, cy, cz;//�����_
	GLdouble ux, uy, uz;//�A�b�v�x�N�g��
};
Target c_target[] = {
	{//�E
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//��
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		-1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//��
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
	},
	{//��O
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		0.0, -1.0, 0.0,
		0.0, 0.0, -1.0,
	},
	{//���
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		0.0, 0.0, 1.0,
		0.0, -1.0, 0.0,
	},
	{//����
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		0.0, 0.0, -1.0,
		0.0, -1.0, 0.0,
	}
};

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	//�\�����[�h
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//�\���E�B���h�E�̃T�C�Y
	glutInitWindowSize(win_width, win_height);
	//����̈ʒu
	glutInitWindowPosition(win_x0, win_y0);
	//�E�B���h�E�쐬
	main_window = glutCreateWindow("GP_Caustics1");
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(resize);
	//�\��
	glutDisplayFunc(display);
	//�}�E�X����
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//�ĕ`��
	GLUI_Master.set_glutIdleFunc( idle );
	//�����ݒ�
	glewInit();//framebuffer�𗘗p����Ƃ��ɕK�v
	init();
	initGlsl(&shader1, "simulation.vert", "calcVelocityX.frag");
	initGlsl(&shader2, "simulation.vert", "calcVelocityY.frag");
	initGlsl(&shader3, "simulation.vert", "waveVel.frag");
	initGlsl(&shader4, "simulation.vert", "wavePos.frag");
	initGlsl(&shader5, "rendering.vert", "rendering.frag");
	initGlsl(&shader6, "simulation.vert", "particle.frag");//���q�p
	initGlsl(&shader7, "renderingP.vert");//���q�p
	initGlsl(&shader8, "projection.vert", "projection.frag");//���e�}�b�s���O�p

	setupGLUI();//myGLUI.h�Ɏ���

	//�C�x���g�������[�v�ɓ���
	glutMainLoop();
	//���\�[�X�̉��
 	free(velX);
	free(velY);
	free(wave);
	free(caus);
	free(particle);
	glDeleteTextures(6, texID);
	glDeleteFramebuffersEXT(5, fbo);
	glDeleteRenderbuffersEXT(1, &rbo);
	glDeleteProgram(shader1);
	glDeleteProgram(shader2);
	glDeleteProgram(shader3);
	glDeleteProgram(shader4);
	glDeleteProgram(shader5);
	glDeleteProgram(shader6);
	glDeleteProgram(shader7);
	glDeleteProgram(shader8);
	deleteDisplayLists();
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
	glClearColor(0.5, 0.7, 1.0, 1.0);
	setCamera();//���_�����߂�
	setLight(); //�����ݒ�@
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//�J�����Ǝ��̐ς̏����l�̕ύX�iView�̐錾��setup.h)
	view.dist = 30.0;
	view.vCnt = Vector3D(0.0, 0.5, waveHeight);
	view.theta = 30.0;
	view.phi = -90.0;//�E�� +x��
	setCamera();
	view0 = view;
	lightPos[2]  = 20.0;
	lightPos0[2] = 20.0;

	//���Ԋ֌W
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(�������j

	glGenTextures(6, texID);//�e�N�X�`���I�u�W�F�N�g�̖��O�t��
	glGenFramebuffersEXT(5, fbo);//�t���[���o�b�t�@�I�u�W�F�N�g�̐���

	//�\���E�B���h�E�ɕ����������_�����O���鏀��
	setFontBitmaps();

	//display list�쐬
	createDisplayLists();

	initData();

	//����L�[�̐���
	printf("�}�E�X����Ŏ��_�ύX�� \n");
	printf("  dolly�F�����t�߂����{�^���N���b�N�ŋ߂Â��C�E�{�^���N���b�N�ŉ������� \n");
	printf("  pan�F��������щE�����E�{�^���N���b�N�Œ����_�����E�ɕω����� \n");
	printf("  tilt�F�^�エ��ѐ^�����E�{�^���N���b�N�Œ����_���㉺�ɕω����� \n");
	printf("  tumble�F���E�Ƀh���b�O����Ǝ��_�����E�ɕω����� \n");
	printf("  crane�F�㉺�Ƀh���b�O����Ǝ��_���㉺�ɕω����� \n");
	printf("  zoom�F�������E�{�^���N���b�N�ŃY�[���C�� \n");
	printf("        �E�����E�{�^���N���b�N�ŃY�[���A�E�g \n");
}

void initData()
{

	NX = rect.meshX;
	NY = rect.meshY;         
	
	DX = rect.size.x / (float)NX;
	DY = rect.size.y / (float)NY;

	//�i�q�_�̃^�C�v
	int i, j;
	float eps = DX/2.0;
	float x, y, x1, x2, y1, y2, x0, y0, r, r0;
	x0 = rect.obs_left;//�~���̒��S
	y0 = rect.size.y/2.0;
	r0 = rect.obs_radius;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{

			g_type[i][j] = INSIDE;//���_
			if(j == 0) g_type[i][j] = BOTTOM;//����(-y)
			if(j == NY) g_type[i][j] = TOP;  //�㑤(+y)
			if(i == 0) g_type[i][j] = LEFT;  //���[(-x)
			if(i == NX) g_type[i][j] = RIGHT;//�E�[(+x)
			x = (float)i * DX;
			y = (float)j * DY;
			if(y < y0 - r0 - DY) continue;
			if(y > y0 + r0 + DY) continue;
			if(x < x0 - r0 - DX) continue;
			if(x > x0 + r0 + DX) continue;

			r = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
			if(r > r0 + eps) continue;
			if( r <= r0 ) g_type[i][j] = OBSTACLE;//�����ł͋��E���܂�

			if( fabs(y - y0 - r0) < eps ) g_type[i][j] = OBS_TOP;
			else if( fabs(y - y0 + r0) < eps ) g_type[i][j] = OBS_BOTTOM;
			else if( fabs(x - x0 + r0) < eps ) g_type[i][j] = OBS_LEFT;
			else if( fabs(x - x0 - r0) < eps ) g_type[i][j] = OBS_RIGHT;
			else
			{
				x1 = x0 - sqrt(r0 * r0 - (y - y0) * (y - y0));//������_
				x2 = x0 + sqrt(r0 * r0 - (y - y0) * (y - y0));//�E����_
				y1 = y0 - sqrt(r0 * r0 - (x - x0) * (x - x0));//������_
				y2 = y0 + sqrt(r0 * r0 - (x - x0) * (x - x0));//�㑤��_
				if(i == int(x1/DX + 0.5) && y > y0 ) g_type[i][j] = OBS_UL;//����
				if(j == int(y2/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_UL;//����
				if(i == int(x1/DX + 0.5) && y < y0 ) g_type[i][j] = OBS_LL;//����
				if(j == int(y1/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_LL;//����
				if(i == int(x2/DX + 0.5) && y > y0) g_type[i][j] = OBS_UR;//�E��
				if(j == int(y2/DY + 0.5) && x > x0) g_type[i][j] = OBS_UR;//�E��
				if(i == int(x2/DX + 0.5) && y < y0) g_type[i][j] = OBS_LR;//�E��
				if(j == int(y1/DY + 0.5) && x > x0) g_type[i][j] = OBS_LR;//�E��
			}
		}

	//�����ʃe�N�Z���T�C�Y
	texWidth  = NX + 1;//���W�e�N�X�`���̉��T�C�Y
	texHeight = NY + 1;//���W�e�N�X�`���̏c�T�C�Y
	
	//�����ʔz��
	velX = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//X���x���̔�������ш���(A����)
	velY = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//Y���x�Ƃ��̔���(A�����͕\���p�Q�x)
	wave = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//���ʂ̂����������x�ƕψ�
	caus = (GLubyte*)malloc(3 * texWidth * texHeight * sizeof(GLubyte));//caustics

	//initialize
	int k;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;

			//���xX
			velX[4 * k + 0] = 0.0;
			velX[4 * k + 1] = 0.0;//x����
			velX[4 * k + 2] = 0.0;//y����
			velX[4 * k + 3] = 0.0;//����
			//���xY
			velY[4 * k + 0] = 0.0;
			velY[4 * k + 1] = 0.0;//x����
			velY[4 * k + 2] = 0.0;//y����
			velY[4 * k + 3] = 0.0;//�\���p�Q�x�iGPU�������ŗ��p)
			//�g
			wave[4 * k + 0] = 0.0;//�g�̂����������x
			wave[4 * k + 1] = 0.0;//���ʂ̕ψ�
			wave[4 * k + 2] = 0.0;//�Q�{�g
			wave[4 * k + 3] = 0.0;//���g�p
			//caustics
			caus[3 * k + 0] = 127.0;//R
			caus[3 * k + 1] = 127.0;//G
			caus[3 * k + 2] = 127.0;//B
		}

	setTextureVelX();
	setFramebufferVelX();
	setTextureVelY();
	setFramebufferVelY();
	setTextureWave();
	setFramebufferWave();
	setFramebufferCube();
	setCubeMap();
	setTextureCaus();

	numRigid = 11;
	for(i = 0; i < numRigid; i++) rigid[i] = CRigid();
	//��Q���i�~���C���j
	rigid[0].kind = CYLINDER;
	rigid[0].color = RED;
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, -rect.size.y/2.0 + rect.obs_posY, waveHeight+rect.obs_length/4.0);
	rigid[0].vSize = Vector3D(rect.obs_radius*2.0, rect.obs_radius*2.0, rect.obs_length/2.0);

	//������(���ʉ��̏�Q���j
	rigid[1].kind = CYLINDER;
	rigid[1].color = BLUE;
	rigid[1].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, -rect.size.y/2.0 + rect.obs_posY, waveHeight-rect.obs_length/4.0);
	rigid[1].vSize = Vector3D(rect.obs_radius*2.0, rect.obs_radius*2.0, rect.obs_length/2.0);
	makeLowerTank();
	makeUpperTank();

	elapseTime1 = 0.0;//1sec�Ԉȓ��̌o�ߎ���
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(������)

	//���q
	particle = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//���q�̈ʒu

	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			if(k > numParticle) break;
			particle[4 * k + 0] = getRandom(-rect.size.x/2.0, rect.size.x/2.0);
			particle[4 * k + 1] = getRandom(-rect.size.y/2.0, rect.size.y/2.0);
			particle[4 * k + 2] = 0.0;
			particle[4 * k + 3] = 0.0;//���g�p
		}
	setTextureParticle();
	setFramebufferParticle();

}
void initObjectLength()
{
	//GLUI�E�B���h�E�Ő��ʂ̍�����ς����Ƃ���Q���̂�������������ς���
	rigid[0].vPos.z =  waveHeight + rect.obs_length/4.0;
	rigid[1].vPos.z =  waveHeight - rect.obs_length/4.0;
	makeLowerTank();
	makeUpperTank();
}

void makeLowerTank()
{
	float bHeight = waveHeight;
	//float bWidth = 1.0;
	//�E��
	rigid[2].kind = PLATE;
	rigid[2].color = DARK_YELLOW;
	rigid[2].vPos = Vector3D(rect.size.x/2.0, 0.0, bHeight / 2.0);
	rigid[2].vSize = Vector3D( bHeight, rect.size.y, 1.0);
	rigid[2].vEuler = Vector3D(0.0, -90.0, 0.0);
	//����
	rigid[3].kind = PLATE;
	rigid[3].color = DARK_YELLOW;
	rigid[3].vPos = Vector3D(-rect.size.x/2.0, 0.0, bHeight / 2.0);
	rigid[3].vSize = Vector3D(bHeight, rect.size.y, 1.0);
	rigid[3].vEuler = Vector3D(0.0, 90.0, 0.0);
	//��
	rigid[4].kind = PLATE;
	rigid[4].color = DARK_CYAN;
	rigid[4].vPos = Vector3D(0.0, rect.size.y/2.0, bHeight / 2.0);
	rigid[4].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[4].vEuler = Vector3D(90.0, 0.0, 0.0);
	//��O
	rigid[5].kind = PLATE;
	rigid[5].color = DARK_CYAN;
	rigid[5].vPos = Vector3D(0.0, -rect.size.y/2.0, bHeight / 2.0);
	rigid[5].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[5].vEuler = Vector3D(-90.0, 0.0, 0.0);
}
void makeUpperTank()
{
	float bHeight = 10.0 - waveHeight;
	float hh0 = waveHeight + bHeight / 2.0;
	//�E��
	rigid[6].kind = PLATE;
	rigid[6].color = DARK_YELLOW;
	rigid[6].vPos = Vector3D(rect.size.x/2.0 , 0.0, hh0);
	rigid[6].vSize = Vector3D(bHeight, rect.size.y, 1.0);
	rigid[6].vEuler = Vector3D(0.0, -90.0, 0.0);
	//����
	rigid[7].kind = PLATE;
	rigid[7].color = DARK_YELLOW;
	rigid[7].vPos = Vector3D(-rect.size.x/2.0, 0.0, hh0);
	rigid[7].vSize = Vector3D(bHeight, rect.size.y, 1.0);
	rigid[7].vEuler = Vector3D(0.0, 90.0, 0.0);
	//��
	rigid[8].kind = PLATE;
	rigid[8].color = DARK_CYAN;
	rigid[8].vPos = Vector3D(0.0, rect.size.y/2.0, hh0);
	rigid[8].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[8].vEuler = Vector3D(90.0, 0.0, 0.0);
	//��
	rigid[9].kind = PLATE;
	rigid[9].color = DARK_CYAN;
	rigid[9].vPos = Vector3D(0.0, -rect.size.y/2.0, hh0);
	rigid[9].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[9].vEuler = Vector3D(-90.0, 0.0, 0.0);
	//LIGHT
	rigid[10].kind = BOX;
	rigid[10].color = WHITE;
	rigid[10].vPos = Vector3D(lightPos[0], lightPos[1], lightPos[2] + 1.0);
	rigid[10].vSize = Vector3D(3.0, 3.0, 0.1);

}

void drawLowerObject()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//�w�ʂ͕`�悵�Ȃ�
	for(int i = 1; i <= 5; i++)
	{
		//�e�N�X�`���ɑ΂��郂�f�����O�ϊ�
		setTextureMatrix();
		//glPushMatrix();
		//���݈ʒu
		glTranslatef(rigid[i].vPos.x, rigid[i].vPos.y, rigid[i].vPos.z);//���s�ړ�
		glRotatef(rigid[i].vEuler.z, 0.0, 0.0, 1.0);//��]�p��deg
		glRotatef(rigid[i].vEuler.y, 0.0, 1.0, 0.0);//��]�p��deg
		glRotatef(rigid[i].vEuler.x, 1.0, 0.0, 0.0);//��]�p��deg
		//�X�P�[�����O
		glScalef(rigid[i].vSize.x, rigid[i].vSize.y, rigid[i].vSize.z);

		//�ʏ�̃��f���r���[�ϊ��ɖ߂� 
		glMatrixMode(GL_MODELVIEW);

		rigid[i].draw();
	}
	setTextureMatrix();
	glMatrixMode(GL_MODELVIEW);
	glCallList(floor0);//���E�n��
	glDisable(GL_CULL_FACE);
}
void drawUpperObject()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//�w�ʂ͕`�悵�Ȃ�
	rigid[0].draw();//��Q�����ʏ�
	rigid[10].vPos = Vector3D(lightPos[0], lightPos[1], lightPos[2] + 2.0);
	for(int i = 6; i < 11; i++) rigid[i].draw();
	glDisable(GL_CULL_FACE);
}

void setTextureVelX()
{
	glBindTexture(target, texID[0]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, velX);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
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
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
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
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferVelX()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0 );
	//framebuffer object �̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void setFramebufferVelY()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[1] );
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[1], 0 );
	//framebuffer object �̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void setFramebufferWave()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[2] );
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[2], 0 );
	//framebuffer object �̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void setTextureParticle()
{
	glBindTexture(target, texID[3]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, GL_FLOAT, particle);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferParticle()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[3] );
  //texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[3], 0 );
	//framebuffer object �̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void setFramebufferCube()
{
	//renderbuffer
	glGenRenderbuffersEXT( 1, &rbo );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, rbo );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, CUBE_WIDTH, CUBE_HEIGHT );
	//framebuffer
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[4] );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbo );

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void setCubeMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[4]);
	for(int i = 0; i < 6; i++)
	glTexImage2D(c_target[i].name,0,GL_RGB,CUBE_WIDTH,CUBE_HEIGHT,0,GL_RGB,GL_UNSIGNED_BYTE,0);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void setTextureCaus()
{
	glBindTexture(target, texID[5]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texWidth,texHeight,0,GL_RGB,GL_UNSIGNED_BYTE, caus);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glBindTexture(target, 0);
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
	
	int i, j, k;

	if(flagStart==true && flagFreeze == false) 
	{
		//�ړ���Q���\�ʂ̑��x���E����
		for (i = 1; i < NX; i++)
			for (j = 0; j <= NY; j++) 
			{
				if(g_type[i][j] < OBS_LEFT) continue;
				k = 4 * (i + j * texWidth);
				velX[k] = rigid[0].vVel.x;
				velY[k] = rigid[0].vVel.y;
			}

		//���͋��E����
		for (i = 0; i <= NX; i++)
			for (j = 0; j <= NY; j++) 
			{
				k = 4 * (i + j * texWidth) + 3;//����
				if(g_type[i][j] == INSIDE) continue;
				else if(g_type[i][j] == LEFT)  velX[k] = 0.0;//-x
				else if(g_type[i][j] == RIGHT) velX[k] = 0.0;//+x
				else if(g_type[i][j] == TOP) velX[k] = 0.0;//+y
				else if(g_type[i][j] == BOTTOM) velX[k] = 0.0;//-y
				else if(g_type[i][j] == OBS_LEFT) velX[k] = velX[4 * (i-1 + j * texWidth) + 3];
				else if(g_type[i][j] == OBS_RIGHT) velX[k] = velX[4 * (i+1 + j * texWidth) + 3];
				else if(g_type[i][j] == OBS_TOP) velX[k] = velX[4 * (i + (j+1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_BOTTOM) velX[k] = velX[4 * (i + (j-1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_UL) velX[k] = velX[4 * (i-1 + (j+1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_UR) velX[k] = velX[4 * (i+1 + (j+1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_LL) velX[k] = velX[4 * (i-1 + (j-1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_LR) velX[k] = velX[4 * (i+1 + (j-1) * texWidth) + 3];
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
	k3 = 3*(i + texWidth * texHeight/2);
	float aa = (float)caus[k3];
	if(aa > 200.0) printf("i=%d caus=%f \n", i, aa);
}
*/
		//particle
		for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			if(k > numParticle) break;
			//���xX
			if(particle[4 * k + 0] > rect.size.x/2.0 || particle[4 * k + 0] < -rect.size.x/2.0 || 
			   particle[4 * k + 1] > rect.size.y/2.0 || particle[4 * k + 1] < -rect.size.y/2.0)
			{
				particle[4 * k + 0] = getRandom(-rect.size.x/2.0, rect.size.x/2.0);
				particle[4 * k + 1] = getRandom(-rect.size.y/2.0, rect.size.y/2.0);
			}
		}
		setTextureParticle();
		setFramebufferParticle();

		renewParticle();//���q�ʒu�̍X�V

		if(flagStep) flagFreeze = true;
		elapseTime2 += dt;
		elapseTime3 += deltaT;

		calcObsPos();

	}

	//���}�b�v�̃e�N�X�`���쐬
	makeTextureOfCubeMapping();

	//�ʏ�̃V�[���̕`��
	//�J���[�o�b�t�@,�f�v�X�o�b�t�@�̃N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	resize(win_width, win_height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//���_��ς���Ƃ��͂��̈ʒu�ɕK�v

	if(cos(M_PI * view.theta /180.0) >= 0.0)//�J�����p90�x���ޭ������޸�ِؑ�
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, 1.0);
	else
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, -1.0);

	//�����ݒ�
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	if(flagWireframe == 1)//'w'��wireframe��solid model�؂�ւ�
	{
	  glPolygonMode(GL_FRONT,GL_LINE);
	  glPolygonMode(GL_BACK,GL_POINT);
	}
	else 
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	rendering();//����

	if(flagShowParticle) renderingP();
	
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
	// �V�F�[�_�v���O�����̓K�p������	
	glUseProgram(0);

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
	 
	if(flagParamShow && !flagOrtho) 
	{
		drawParam("f_rate=%d[fps]", fps0, 10, 30);
		drawParam("t_step=%1.5f[s]", dt, 150, 30);
		drawParam("e_time=%3.3f[s]", elapseTime2, 300, 30);
		drawParam("n_time=%3.3f", elapseTime3, 460, 30);
		drawParam("Courant=%1.4f", Courant, 10, 50);
		drawParam("diff_num=%1.4f", diff_num, 150, 50);
		drawParam("Re=%3.1f", Re,300, 50);
	}
	if(flagCoordShow) drawWorldCoordinates(5.0);

	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();	
}

void makeTextureOfCubeMapping()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[4] );

	// �����ϊ��s��̐ݒ� 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.1, 100.0);//����p��90��
	// ���f���r���[�ϊ��s��̐ݒ� 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//���_���猩���V�[�������܃}�b�s���O�ɗ��p 
	for (int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, c_target[i].name, texID[4], 0);
		glViewport(0, 0, CUBE_WIDTH, CUBE_HEIGHT);
		//�J���[�o�b�t�@,�f�v�X�o�b�t�@�̃N���A
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		//���_���猩������̂������_�����O 
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vPos.x+c_target[i].cx, view.vPos.y+c_target[i].cy, view.vPos.z+c_target[i].cz,
				c_target[i].ux, c_target[i].uy, c_target[i].uz);

		//�����ݒ�
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		if(view.vPos.z < waveHeight)//���_�����ʂ�艺
		{
			drawUpperObject();//���ʏ�̃I�u�W�F�N�g
		}

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
		// �V�F�[�_�v���O�����̓K�p������	
		glUseProgram(0);

		glPopMatrix();
	}
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewVelX()
{
	//framebuffer object1��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader1);
	GLint samplerVelXLoc = glGetUniformLocation(shader1, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader1, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint texWLoc = glGetUniformLocation(shader1, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader1, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader1, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader1, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader1, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader1, "Re");
	glUniform1f(ReLoc, Re);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, velX);//�f�[�^��CPU����

	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewVelY()
{
	//framebuffer object1��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[1] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//veYl[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader2);
	GLint samplerVelXLoc = glGetUniformLocation(shader2, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader2, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint texWLoc = glGetUniformLocation(shader2, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader2, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader2, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader2, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader2, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader2, "Re");
	glUniform1f(ReLoc, Re);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, velY);//�f�[�^��CPU����

	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewWaveVel()
{
	//framebuffer object2��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[2] );
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader3);
	GLint samplerWaveLoc = glGetUniformLocation(shader3, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2��K�p
	GLint texWLoc = glGetUniformLocation(shader3, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader3, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader3, "dt");
	glUniform1f(deltaTLoc, deltaT);
	GLint timeLoc = glGetUniformLocation(shader3, "time");
	glUniform1f(timeLoc, elapseTime3);
	GLint DXLoc = glGetUniformLocation(shader3, "dx");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader3, "dy");
	glUniform1f(DYLoc, DY);
	GLint ampLoc = glGetUniformLocation(shader3, "amp");
	glUniform1f(ampLoc, waveAmp);
	GLint freqLoc = glGetUniformLocation(shader3, "freq");
	glUniform1f(freqLoc, waveFreq);
	GLint speedLoc = glGetUniformLocation(shader3, "speed");
	glUniform1f(speedLoc, waveSpeed);
	GLint dragLoc = glGetUniformLocation(shader3, "drag");
	glUniform1f(dragLoc, waveDrag);

	GLint posObsLoc = glGetUniformLocation(shader3, "posObs");
	float pos[] ={ rigid[0].vPos.x, rigid[0].vPos.y, rigid[0].vPos.z};
	glUniform3fv(posObsLoc, 1, pos);
	GLint sizeObsLoc = glGetUniformLocation(shader3, "sizeObs");
	float size[] = { rigid[0].vSize.x, rigid[0].vSize.y, rigid[0].vSize.z};
	glUniform3fv(sizeObsLoc, 1, size);
	GLint velObsLoc = glGetUniformLocation(shader3, "velObs");
	float vel[] = { rigid[0].vVel.x, rigid[0].vVel.y, rigid[0].vVel.z};
	glUniform3fv(velObsLoc, 1, vel);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, wave);//�f�[�^��CPU����

	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void renewWavePos()
{
	//framebuffer object2��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[2] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//veYl[]
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader4);

	GLint samplerVelXLoc = glGetUniformLocation(shader4, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader4, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint samplerWaveLoc = glGetUniformLocation(shader4, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2��K�p
	GLint meshXLoc = glGetUniformLocation(shader4, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader4, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint maxOmgLoc = glGetUniformLocation(shader4, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader4, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
	GLint adjustHLoc = glGetUniformLocation(shader4, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint adjustCLoc = glGetUniformLocation(shader4, "adjustC");
	glUniform1f(adjustCLoc, adjustC);
	GLint deltaTLoc = glGetUniformLocation(shader4, "deltaT");
	glUniform1f(deltaTLoc, deltaT);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, wave);//�f�[�^��CPU����
	//�R�[�X�e�B�N�X�̃e�N�X�`���쐬
	int i, j, k, k3;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			k3 = 3 * k;
			caus[k3] = caus[k3 + 1] = caus[k3 + 2] = 255.0 * wave[k*4 + 3];
		}
	setTextureCaus();

	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void drawNumberingPoints()
{
	//�r���[�|�[�g�̃T�C�Y�ݒ�
	glViewport(0, 0, texWidth, texHeight);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//�����e�s��̐ݒ�
	gluOrtho2D(0.0, texWidth, 0.0, texHeight);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//�����ʂ��e�N�X�`���Ƃ��ē\��t���邽�߂̉��z�I�u�W�F�N�g

	glBegin(GL_POINTS);
	for(int j = 0; j < texHeight; j++)
		for(int i = 0; i < texWidth; i++) glVertex2f((float)i, (float)j);//�_���W��2�����i�q���_�ԍ�
	glEnd();

}

void rendering()
{
	glUseProgram(shader5);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[4]);

	GLint texLoc = glGetUniformLocation(shader5, "smplRefract");
	glUniform1i(texLoc, 3);//GL_TEXTURE3��K�p
	GLint tranLoc = glGetUniformLocation(shader5, "transparency");
	glUniform1f(tranLoc, transparency);
	GLint ratioLoc = glGetUniformLocation(shader5, "nRatio");
	glUniform1f(ratioLoc, nRatio);
	GLint inverseLoc = glGetUniformLocation(shader5, "flagInverse");
	int flagInverse = -1;
	if(view.vPos.z < waveHeight) flagInverse = 1;//���_�����ʈȉ�
	glUniform1i(inverseLoc, flagInverse);
	GLint samplerWaveLoc = glGetUniformLocation(shader5, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE0��K�p
	GLint texWLoc = glGetUniformLocation(shader5, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader5, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint heightLoc = glGetUniformLocation(shader5, "height");
	glUniform1f(heightLoc, waveHeight);

	GLint sizeXLoc = glGetUniformLocation(shader5, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader5, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);

	//���_�̃}�e���A��
	static float diffuse[] = { 0.3, 0.3, 0.3, 1.0};
	static float ambient[] = { 0.1, 0.1, 0.2, 1.0};
	static float specular[]= { 1.0, 1.0, 1.0, 1.0};
//	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS, 100.0);

	if(flagWireframe == 0)
		drawGridPlate((float)NX, (float)NY, NX, NY);
	else
		drawGridLines((float)NX, (float)NY, NX, NY);
	glUseProgram(0);
}

void renewParticle()
{
	//framebuffer object��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[3] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY[]
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(target, texID[3]);//particle[]

	glUseProgram(shader6);

	GLint samplerVelXLoc = glGetUniformLocation(shader6, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader6, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint samplerWaveLoc = glGetUniformLocation(shader6, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2��K�p
	GLint samplerPosLoc = glGetUniformLocation(shader6, "samplerPosP");
	glUniform1i(samplerPosLoc, 3);//GL_TEXTURE3��K�p
	GLint texWidthLoc = glGetUniformLocation(shader6, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader6, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint sizeXLoc = glGetUniformLocation(shader6, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader6, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint dtLoc = glGetUniformLocation(shader6, "dt");
	glUniform1f(dtLoc, deltaT);

	drawNumberingPoints();
	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	//�X�V�f�[�^�i���x�ƈʒu�j���e�N�X�`���������ɃR�s�[
	glReadPixels(0, 0, texWidth, texHeight, format, type, particle);//�f�[�^��CPU����
	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

}

void renderingP()
{
	//�`��	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[3]);
	glUseProgram(shader7);
	GLint samplerLoc = glGetUniformLocation(shader7, "samplerPosP");
	glUniform1i(samplerLoc, 0);//GL_TEXTURE0��K�p
	GLint heightLoc = glGetUniformLocation(shader7, "height");
	glUniform1f(heightLoc, waveHeight);

	drawParticles();
	glUseProgram(0);

	glBindTexture(target, 0);
}
void drawParticles()
{
	glDisable(GL_LIGHTING);
	glPointSize(sizeParticle);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_POINTS);
	for(int j = 0; j < texHeight; j++)
		for(int i = 0; i < texWidth; i++) glVertex3f((float)i, (float)j, 0.0);
	glEnd();
	glPointSize(1);
	glEnable(GL_LIGHTING);
}

void calcObsPos()
{
	if(flagObsStop) { rigid[0].vVel = Vector3D();  return; }

	Vector3D left;//�_�N�g�̍��[����щ��[����̋���
//	float direction = 1.0;//��Q�������ړ�����

	float R = rect.size.x / 2.0 - rect.obs_left;//��]���a
	
	if(flagRotation)
	{//�����ʒu�������̂Ƃ��E���C�E���̂Ƃ������
		rigid[0].vVel.x = obsSpeed * rigid[0].vPos.y / R;
		rigid[0].vVel.y = -obsSpeed * rigid[0].vPos.x / R;
	}
	else//���i
	{
		if(rigid[0].vPos.x >=  rect.size.x / 2.0 - rect.obs_left) direction = -1.0;
		if(rigid[0].vPos.x <= -rect.size.x / 2.0 + rect.obs_left) direction =  1.0;
		rigid[0].vVel.x = direction * obsSpeed;
	}
	rigid[0].vPos += rigid[0].vVel * deltaT;
	//���ʉ��̏�Q���̑�
	rigid[1].vPos = rigid[0].vPos;
	rigid[0].vPos.z = waveHeight + rigid[1].vSize.z/2.0;
	rigid[1].vPos.z = waveHeight - rigid[1].vSize.z/2.0;
	left.x = rigid[0].vPos.x + rect.size.x/2.0;
	left.y = rigid[0].vPos.y + rect.size.y/2.0;

	//�i�q�_�̃^�C�v
	int i, j;
	float eps = DX/2.0;
	float x, y, x1, x2, y1, y2, x0, y0, r, r0;
	x0 = left.x;//rect.obs_left;//�~���̒��S
	y0 = left.y;//rect.size.y/2.0;
	r0 = rect.obs_radius;
	for(i = 1; i < NX; i++)
		for(j = 1; j < NY; j++)
		{
			g_type[i][j] = INSIDE;//���_
			x = (float)i * DX;
			y = (float)j * DY;
			if(y < y0 - r0 - DY) continue;
			if(y > y0 + r0 + DY) continue;
			if(x < x0 - r0 - DX) continue;
			if(x > x0 + r0 + DX) continue;

			r = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
			if(r > r0 + eps) continue;
			if( r <= r0 ) g_type[i][j] = OBSTACLE;//�����ł͋��E���܂�

			if( fabs(y - y0 - r0) < eps ) g_type[i][j] = OBS_TOP;
			else if( fabs(y - y0 + r0) < eps ) g_type[i][j] = OBS_BOTTOM;
			else if( fabs(x - x0 + r0) < eps ) g_type[i][j] = OBS_LEFT;
			else if( fabs(x - x0 - r0) < eps ) g_type[i][j] = OBS_RIGHT;
			else
			{
				x1 = x0 - sqrt(r0 * r0 - (y - y0) * (y - y0));//������_
				x2 = x0 + sqrt(r0 * r0 - (y - y0) * (y - y0));//�E����_
				y1 = y0 - sqrt(r0 * r0 - (x - x0) * (x - x0));//������_
				y2 = y0 + sqrt(r0 * r0 - (x - x0) * (x - x0));//�㑤��_
				if(i == int(x1/DX + 0.5) && y > y0 ) g_type[i][j] = OBS_UL;//����
				if(j == int(y2/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_UL;//����
				if(i == int(x1/DX + 0.5) && y < y0 ) g_type[i][j] = OBS_LL;//����
				if(j == int(y1/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_LL;//����
				if(i == int(x2/DX + 0.5) && y > y0) g_type[i][j] = OBS_UR;//�E��
				if(j == int(y2/DY + 0.5) && x > x0) g_type[i][j] = OBS_UR;//�E��
				if(i == int(x2/DX + 0.5) && y < y0) g_type[i][j] = OBS_LR;//�E��
				if(j == int(y1/DY + 0.5) && x > x0) g_type[i][j] = OBS_LR;//�E��
			}
		}
}

void setTextureMatrix()
{
	//�e�N�X�`���ϊ��s���ݒ肷�� 
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	//���K�����W�� [-0.5,0.5] �͈̔͂��e�N�X�`�����W�͈̔� [0,1] �ɕϊ� 
	glTranslatef(0.5, 0.5, 0.0);
	float fovC = 2.0 * RAD_TO_DEG * atan2(rect.size.x, (lightPos[2] - waveHeight));
	gluPerspective(fovC, 1.0, 1.0, 100.0);
	//���e���S=�����ʒu
	gluLookAt(lightPos[0], lightPos[1], lightPos[2], 0.0, 0.0, waveHeight, 0.0, 1.0, 0.0);
}
