/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
���ʂ̉Q�Ɣg��\���i���q�𐅖ʂɕ����ׂ�)
�T�C�Y20�~10�C������200�~100�ɌŒ�
��Q�������Ԃɂ��镽�s���_�N�g
���x-���͖@�i�t���N�V���i���E�X�e�b�v�@�j
���M�����[�i�q
GPGPU(���l�v�Z��GPU���Ŏ��s�j
CIP���g�p
���x��Poisson�������������ɂ͉����ĂȂ��i�P�񂾂��j
GLSL�ɂ��V�F�[�f�B���O
�������͂���
*/
#define MAX_NUM0 50//��x�ɕ��o���闱�q��

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
void drawNumberingPoints();
void renewVelX();
void renewVelY();
void renewWaveVel();
void renewWavePos();
void renewParticle();
void renderingP();
void drawPlate();
void drawParticles();
void decideRegion();
float regionFlag(float x, float y);
void setTextureParticle();
void setFramebufferParticle();

//��͗p�f�[�^�̃T�C�Y
int texWidth, texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *velX;//���xVelX�ƈ���
float *velY;//���xVelY�ƉQ�x
float *wave;//�g��
float *particle;//���q�ʒu
GLuint texID[4];//texture object
GLuint fbo[4];//framebuffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;//�����ʂ�vel��r,g,b����ш���vel.a
GLenum type = GL_FLOAT;
//�V�F�[�_�v���O������
GLuint shader_prog1, shader_prog2, shader_prog3, shader_prog4, shader_prog5, shader_prog6, shader_prog7;

enum Type{INSIDE, LEFT, RIGHT, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
          OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //�i�q�_�̃^�C�v

int NX, NY, NX1, NX2, NY1, NY2;
float DX, DY;

float maxPrs0, minPrs0, maxVel0, minVel0;
bool flagResetting = false;

int nDummy;
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
	main_window = glutCreateWindow("GP_VortexWave1");
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
	initGlsl(&shader_prog1, "simulation.vert", "calcVelocityX.frag");
	initGlsl(&shader_prog2, "simulation.vert", "calcVelocityY.frag");
	initGlsl(&shader_prog3, "simulation.vert", "waveVel.frag");
	initGlsl(&shader_prog4, "simulation.vert", "wavePos.frag");
	initGlsl(&shader_prog5, "rendering.vert", "rendering.frag");
	initGlsl(&shader_prog6, "simulation.vert", "particle.frag");//���q�p
	initGlsl(&shader_prog7, "renderingP.vert");//���q�p

	setupGLUI();//myGLUI.h�Ɏ���

	//�C�x���g�������[�v�ɓ���
	glutMainLoop();
	//���\�[�X�̉��
 	free(velX);
	free(velY);
	free(wave);
	free(particle);
	glDeleteTextures(4, texID);
	glDeleteFramebuffersEXT(4, fbo);
	glDeleteProgram(shader_prog1);
	glDeleteProgram(shader_prog2);
	glDeleteProgram(shader_prog3);
	glDeleteProgram(shader_prog4);
	glDeleteProgram(shader_prog5);
	glDeleteProgram(shader_prog6);
	glDeleteProgram(shader_prog7);
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
	glClearColor(0.2, 0.2, 0.3, 1.0);
	setCamera();//���_�����߂�
	setLight(); //�����ݒ�@
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	
	//�J�����Ǝ��̐ς̏����l�̕ύX�iView�̐錾��setup.h)
	view.dist = 25.0;
	view.vCnt = Vector3D(0.0, 0.5, 0.0);
	view.theta = 90.0;//��O�������i���_���^��j
	view.phi = -90.0;//�E�� +x��
	setCamera();
	view0 = view;
	ortho_scale = 10.0;

	//���Ԋ֌W
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(�������j

	glGenTextures(4, texID);//�e�N�X�`���I�u�W�F�N�g�̖��O�t��
	glGenFramebuffersEXT(4, fbo);//�t���[���o�b�t�@�I�u�W�F�N�g�̐���

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
	DX = rect.size.x / (float)rect.meshX;
	DY = rect.size.y / (float)rect.meshY;

	nDummy = 20;//�ő�30
	NX = rect.meshX + 2 * nDummy;
	NY = rect.meshY + 2 * nDummy;         

	rect.obs_posY = rect.size.y / 2.0;
	
	//��Q�����E�̕����_�ԍ�
	float eps = 0.5;
	int NX1 = nDummy + floor((rect.obs_left - rect.obs_thick / 2.0) / DX + eps);//��Q�����[�ʒu
	int NX2 = nDummy + floor((rect.obs_left + rect.obs_thick / 2.0) / DX + eps);//��Q���E�[�ʒu
	int NY2 = nDummy + floor((rect.obs_posY + rect.obs_width/2.0) / DY + eps);//��Q����[�ʒu
	int NY1 = nDummy + floor((rect.obs_posY - rect.obs_width/2.0) / DY + eps);//��Q�����[�ʒu

	//�i�q�_�̃^�C�v
	int i, j;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			g_type[i][j] = INSIDE;//���_
			if(j == 0) g_type[i][j] = BOTTOM;//�����ǖ�
			if(j == NY) g_type[i][j] = TOP;//�㑤�ǖ�
			if(i == 0) g_type[i][j] = LEFT;//�����[
			if(i == NX) g_type[i][j] = RIGHT;//���o�[
			if(i == NX1 && j > NY1 && j < NY2) g_type[i][j] = OBS_LEFT;//��Q�����[
			if(i == NX2 && j > NY1 && j < NY2) g_type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i >= NX1 && i <= NX2 && j == NY2) g_type[i][j] = OBS_TOP;//��Q����[
			if(i >= NX1 && i <= NX2 && j == NY1) g_type[i][j] = OBS_BOTTOM;//��Q�����[
			if(i > NX1 && i < NX2 && j > NY1 && j < NY2) g_type[i][j] = OBSTACLE;//��Q������
			if(i == NX1 && j == NY1) g_type[i][j] = OBS_LL;
			if(i == NX1 && j == NY2) g_type[i][j] = OBS_UL;
			if(i == NX2 && j == NY1) g_type[i][j] = OBS_LR;
			if(i == NX2 && j == NY2)  g_type[i][j] = OBS_UR;
		}

	//�����ʃe�N�Z���T�C�Y
	texWidth  = NX + 1;//���W�e�N�X�`���̉��T�C�Y
	texHeight = NY + 1;//���W�e�N�X�`���̏c�T�C�Y
	
	//�����ʔz��
	velX = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//X���x���̔�������ш���(A����)
	velY = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//Y���x�Ƃ��̔���(A�����͕\���p�Q�x)
	wave = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//���ʂ̂����������x�ƕψʗ�

	//initialize
	int k, I, J;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//I = i - nDummy; J = j - nDummy;//nDummy���l�����Ȃ��Ƃ��̊i�q�ԍ�
			//���xX
			velX[4 * k + 0] = 1.0;
			velX[4 * k + 1] = 0.0;//x����
			velX[4 * k + 2] = 0.0;//y����
			velX[4 * k + 3] = 0.0;//����
			if(g_type[i][j] == BOTTOM || g_type[i][j] == TOP || g_type[i][j] >= OBS_LEFT) velX[4 * k + 0] = 0.0;//��Q���\��
			//���xY
			velY[4 * k + 0] = 0.0;
			velY[4 * k + 1] = 0.0;//x����
			velY[4 * k + 2] = 0.0;//y����
			velY[4 * k + 3] = 0.0;//�\���p�Q�x�iGPU�������ŗ��p)
			//�e�i�q�_�ɂ����鐅�ʂ̍����Ƃ����������x
			wave[k * 4 + 0] = 0.0;//���ʂ̂����������x
			wave[k * 4 + 1] = 0.0;//���ʂ̕ψʗ�
			wave[k * 4 + 2] = 0.0;
			wave[k * 4 + 3] = 1.0;//��Q�����O����ɗ��p		
		}

	setTextureVelX();
	setFramebufferVelX();
	setTextureVelY();
	setFramebufferVelY();

	//��Q��
	//�S�Ẵp�^�[���ɋ���
	numRigid = 1;
	for(i = 0; i < RIGID_NUM_MAX; i++)	rigid[i] = CRigid();
	
	rigid[0].kind = CUBE;
	rigid[0].color = DARK_YELLOW;
	rigid[0].vSize = Vector3D(rect.obs_thick, rect.obs_width, 1.0);
//	rigid[0].vPos = Vector3D(0.0, 0.0, 0.0);
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, 0.0, 0.0);
//printf("left=%f \n",rect.obs_left); 
	float wid = 0.4;
	float hig = 0.5;
	if(pattern == 1)
	{
		numRigid = 3;
		rigid[1].kind = CUBE;
		rigid[1].color = GRAY;
		rigid[1].vSize = Vector3D(rect.size.x, wid, hig);
		rigid[1].vPos = Vector3D(0.0, rect.size.y/2.0 + 0.5*wid , 0.0);

		rigid[2].kind = CUBE;
		rigid[2].color = GRAY;
		rigid[2].vSize = Vector3D(rect.size.x, wid, hig);
		rigid[2].vPos = Vector3D(0.0, -rect.size.y/2.0 - 0.5*wid , 0.0);
	}
	decideRegion();
	setTextureWave();
	setFramebufferWave();

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
			particle[4 * k + 3] = 0.0;//�i�q�_�̉t�ʂ̍���
		}
	setTextureParticle();
	setFramebufferParticle();

	elapseTime1 = 0.0;//1sec�Ԉȓ��̌o�ߎ���
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(������)

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
	
	int i, j, k, k1;

	if(flagStart==true && flagFreeze == false) 
	{
		for(int m = 0; m < dispSpeed; m++)//�������^�C���X�e�b�vdeltaT�͕ς����ɕ\�����x�����ς���
		{
			//���x���E����
			for (j = 0; j <= NY; j++) 
			{
				//������
				k = 4 * (j * texWidth);
				velX[k] = 1.0;
				velY[k] = 0.0;
				//���o��
				k = 4 * (NX + j * texWidth);
				k1 = 4 * (NX - 1 + j * texWidth);
				velX[k] = velX[k1]; 
				velY[k] = velY[k1]; 
			}
			//��Q���\�ʂ���ѓ���
			for (i = 1; i < NX; i++)
				for (j = 0; j <= NY; j++) 
				{
					if(g_type[i][j] < OBS_LEFT) continue;
					k = 4 * (i + j * texWidth);
					velX[k] = 0.0;
					velY[k] = 0.0;
				}
			
			//���͋��E����(������Q�������j
			//�S�̈�̋��E�ɑ΂��Ă�Dirichlet
			for (i = 0; i <= NX; i++)
				for (j = 0; j <= NY; j++) 
				{
					k = 4 * (i + j * texWidth) + 3;//����
					if(g_type[i][j] == INSIDE) continue;
					else if(g_type[i][j] == LEFT)  velX[k] = 0.0;
					else if(g_type[i][j] == RIGHT) velX[k] = 0.0;
					else if(g_type[i][j] == TOP) velX[k] = 0.0;
					else if(g_type[i][j] == BOTTOM) velX[k] = 0.0;
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
			renewWavePos();//�g�̐��ʕψʗʂ̍X�V

			//particle
			for(j = 0; j < texHeight; j++)
			for(i = 0; i < texWidth; i++)
			{
				k = i + j * texWidth;
				if(k > numParticle) break;
				//�̈�O�̗��q�͗����[��
				if(particle[4 * k + 0] > rect.size.x/2.0 || particle[4 * k + 1] > rect.size.y/2.0 || particle[4 * k + 1] < -rect.size.y/2.0)
				{
					particle[4 * k + 0] = -rect.size.x/2.0;
					particle[4 * k + 1] = getRandom(-rect.size.y/2.0, rect.size.y/2.0);
				}
			}
			setTextureParticle();
			setFramebufferParticle();

			renewParticle();//���q�ʒu�̍X�V
		}
		if(flagStep) flagFreeze = true;
		elapseTime2 += dt;
		elapseTime3 += deltaT;
	}

	//�J���[�o�b�t�@,�f�v�X�o�b�t�@�̃N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	resize(win_width, win_height);

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

	rendering();

	if(flagShowParticle) renderingP();
	
	for(k = 0; k < numRigid; k++) rigid[k].draw();

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
	//if(flagResetting) drawParam("Reset obstacle size",  200, 200.0);
	if(flagCoordShow) drawWorldCoordinates(5.0);

	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();	
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
	glUseProgram(shader_prog1);
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog1, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog1, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint texWLoc = glGetUniformLocation(shader_prog1, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog1, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader_prog1, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader_prog1, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog1, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader_prog1, "Re");
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
	glUseProgram(shader_prog2);
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog2, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog2, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint texWLoc = glGetUniformLocation(shader_prog2, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog2, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader_prog2, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader_prog2, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog2, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader_prog2, "Re");
	glUniform1f(ReLoc, Re);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, velY);//�f�[�^��CPU����

	//printf("vel[5][NY/8] = %f  vel[10][NY/8] = %f \n", vel[3*(5 +(NY/8) * texWidth)], vel[3*(10 +(NY/8) * texWidth)]);
	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewWaveVel()
{
	//framebuffer object2��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[2] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[2]);//wave[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader_prog3);
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog3, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE2��K�p
	GLint texWidthLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint meshXLoc = glGetUniformLocation(shader_prog3, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog3, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint dummyLoc = glGetUniformLocation(shader_prog3, "nDummy");
	glUniform1i(dummyLoc, nDummy);
	GLint sizeXLoc = glGetUniformLocation(shader_prog3, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog3, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint ampLoc = glGetUniformLocation(shader_prog3, "amp");
	glUniform1f(ampLoc, amp0);
	GLint freqLoc = glGetUniformLocation(shader_prog3, "freq");
	glUniform1f(freqLoc, freq0);
	GLint timeLoc = glGetUniformLocation(shader_prog3, "time");
	glUniform1f(timeLoc, elapseTime2);
	GLint nSourceLoc = glGetUniformLocation(shader_prog3, "nSource");
	glUniform1i(nSourceLoc, nSource0);
	GLint sourceXLoc = glGetUniformLocation(shader_prog3, "sourceX");
	glUniform1i(sourceXLoc, sourceX0);
	GLint intervalLoc = glGetUniformLocation(shader_prog3, "interval");
	glUniform1i(intervalLoc, interval0);
	GLint dragLoc = glGetUniformLocation(shader_prog3, "drag0");
	glUniform1f(dragLoc, drag0);
	GLint waveSpeedLoc = glGetUniformLocation(shader_prog3, "waveSpeed");
	glUniform1f(waveSpeedLoc, waveSpeed);
	GLint dtLoc = glGetUniformLocation(shader_prog3, "dt");
	glUniform1f(dtLoc, deltaT);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	//glReadPixels(0, 0, texWidth, texHeight, format, type, wave);
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
	glUseProgram(shader_prog4);
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog4, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog4, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog4, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE0��K�p
	GLint texWidthLoc = glGetUniformLocation(shader_prog4, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog4, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint maxOmgLoc = glGetUniformLocation(shader_prog4, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader_prog4, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
	GLint dtLoc = glGetUniformLocation(shader_prog4, "dt");
	glUniform1f(dtLoc, deltaT);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`���i�����ʁj��\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	//glReadPixels(0, 0, texWidth, texHeight, format, type, wave);
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
	glUseProgram(shader_prog5);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]

	GLint samplerWaveLoc = glGetUniformLocation(shader_prog5, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2��K�p
	GLint texWLoc = glGetUniformLocation(shader_prog5, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog5, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint sizeXLoc = glGetUniformLocation(shader_prog5, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog5, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint meshXLoc = glGetUniformLocation(shader_prog5, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog5, "meshY");
	glUniform1i(meshYLoc, rect.meshY);

	GLint adjustHLoc = glGetUniformLocation(shader_prog5, "adjustH");
	glUniform1f(adjustHLoc, adjustH);

	static float diffuse[] = { 0.1, 0.3, 0.9, 1.0};
	static float specular[] = { 0.7, 0.7, 0.7, 1.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS, 100.0);

	if(flagWireframe == 0)
		drawGridPlate((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);
	else
		drawGridLines((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);
	glUseProgram(0);
}

void renewParticle()
{
	//framebuffer object��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[3] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(target, texID[3]);//particle[]

	glUseProgram(shader_prog6);
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog6, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog6, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog6, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2��K�p
	GLint samplerPosLoc = glGetUniformLocation(shader_prog6, "samplerPosP");
	glUniform1i(samplerPosLoc, 3);//GL_TEXTURE3��K�p
/*	GLint texWidthLoc = glGetUniformLocation(shader_prog6, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog6, "texHeight");
	glUniform1i(texHeightLoc, texHeight);*/
	GLint meshXLoc = glGetUniformLocation(shader_prog6, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog6, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint dummyLoc = glGetUniformLocation(shader_prog6, "nDummy");
	glUniform1i(dummyLoc, nDummy);
	GLint adjustHLoc = glGetUniformLocation(shader_prog6, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
/*
	GLint maxOmgLoc = glGetUniformLocation(shader_prog6, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader_prog6, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
*/
	GLint sizeXLoc = glGetUniformLocation(shader_prog6, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog6, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint numParticleLoc = glGetUniformLocation(shader_prog6, "numParticle");
	glUniform1i(numParticleLoc, numParticle);

	GLint dtLoc = glGetUniformLocation(shader_prog6, "dt");
	glUniform1f(dtLoc, deltaT);

	drawNumberingPoints();
	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	//�X�V�f�[�^�i���x�ƈʒu�j���e�N�X�`���������ɃR�s�[
	//glCopyTexSubImage2D(target, 0, 0, 0, 0, 0, texWidth, texHeight);
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
	glUseProgram(shader_prog7);
	GLint samplerLoc = glGetUniformLocation(shader_prog7, "samplerPosP");
	glUniform1i(samplerLoc, 0);//GL_TEXTURE0��K�p

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
void decideRegion()
{
	int i, j, k;
	float x, y;

	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//�i�q�_�̍��W
			x = (float)(i - texWidth / 2) * DX;
			y = (float)(j - texHeight / 2) * DY;
			wave[k * 4 + 3] = regionFlag(x, y);
			if(wave[k * 4 + 3] == 0.0) g_type[i][j] = OBSTACLE;
		}
}

float regionFlag(float x, float y)
{//�Ώې}�`�̊�{�p���Ŕ���
	Vector3D vPoint0 = Vector3D(x, y, 0.0);
	Vector3D vPoint;

	for(int k = 0; k < numRigid; k++)
	{
		//�Ώې}�`�̒��S�ɕ��s�ړ������̉�]
		vPoint = vPoint0 - rigid[k].vPos;
		vPoint.rotZ(-rigid[k].vEuler.z);
		if(rigid[k].kind == CUBE)
		{
			if( vPoint.x >= - rigid[k].vSize.x / 2.0 && vPoint.x <= rigid[k].vSize.x / 2.0
			 && vPoint.y >= - rigid[k].vSize.y / 2.0 && vPoint.y <= rigid[k].vSize.y / 2.0) return 0.0;//��Q������
		}
		else if(rigid[k].kind == SPHERE || rigid[k].kind == CYLINDER)
		{
			//�ȉ~�`����
			float a = rigid[k].vSize.x * rigid[k].vSize.x / 4.0;
			float b = rigid[k].vSize.y * rigid[k].vSize.y / 4.0;
			float f =  vPoint.x*vPoint.x / a + vPoint.y*vPoint.y / b - 1.0;
			if(f <= 0.0) return 0.0;
		}
	}
	return 1.0;//�O��(����)
}

