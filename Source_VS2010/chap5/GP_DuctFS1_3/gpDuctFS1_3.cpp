/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
3�̏�Q�����u���ꂽ���s���_�N�g
�������̋߂��̒��S�����1�C2�͒��S���ɑ΂��Ώ̂�
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
//void initData();
void idle();
void display();
void rendering();
void setTextureVelX();
void setFramebufferVelX();
void setTextureVelY();
void setFramebufferVelY();
void drawNumberingPoints();
void renewVelX();
void renewVelY();

//��͗p�f�[�^�̃T�C�Y
int texWidth, texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *velX;//���xVelX�ƈ���
float *velY;//���xVelY�ƉQ�x
GLuint texID[2];//texture buffer object
GLuint fbo[2];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;//�����ʂ�vel��r,g,b����ш���vel.a
GLenum type = GL_FLOAT;
//�V�F�[�_�v���O������
GLuint shader_prog1, shader_prog2, shader_prog3;

enum Type{INSIDE, INLET, OUTLET, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
          OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //�i�q�_�̃^�C�v

int NX, NY;
float DX, DY;

float maxPrs0, minPrs0, maxVel0, minVel0;
bool flagResetting = false;

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
	main_window = glutCreateWindow("GP_DuctFS1_3");
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
	initGlsl(&shader_prog3, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.h�Ɏ���

	//�C�x���g�������[�v�ɓ���
	glutMainLoop();
	//���\�[�X�̉��
 	free(velX);
	free(velY);
	glDeleteTextures(2, texID);
	glDeleteFramebuffersEXT(2, fbo);
	glDeleteProgram(shader_prog1);
	glDeleteProgram(shader_prog2);
	glDeleteProgram(shader_prog3);
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

	glGenTextures(2, texID);//�e�N�X�`���I�u�W�F�N�g�̖��O�t��
	glGenFramebuffersEXT(2, fbo);//�t���[���o�b�t�@�I�u�W�F�N�g�̐���

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

	float obsTop, obsBottom;
	float yy0 = rect.size.y / 2.0;//������Q���̒��S
	obsTop = yy0 + rect.obs_width / 2.0;
	obsBottom = yy0 - rect.obs_width / 2.0;
	
	//�����̏�Q�����E�̕����_�ԍ�
	float eps = 0.5;
	int NX1_1 = floor((rect.obs_left - rect.obs_thick/2.0) / DX + eps);//��Q�����[�ʒu
	int NX1_2 = floor((rect.obs_left + rect.obs_thick/2.0) / DX + eps);//��Q���E�[�ʒu
	int NY1_2 = floor(obsTop / DY + eps);//��Q����[�ʒu
	int NY1_1 = floor(obsBottom / DY + eps);//��Q�����[�ʒu

	int NY_ObsW = floor(rect.obs_width / DY + eps);
	//�i�q�Ԋu�̐����{��
	float left1, left2, bottom1, bottom2;
	left1 = DX * (float)(NX1_1);//���[�����Q�����[�܂�
	left2 = rect.obs_left - rect.obs_thick/2.0;
	bottom1 = DY * (float)NY1_1;
	bottom2 = (rect.size.y - rect.obs_width)/2.0;
	printf("left1=%f, left2=%f, bottom1=%f, bottom2=%f \n", left1, left2, bottom1, bottom2);
	eps = 0.00001;
	if( fabs(left1 - left2) > eps || fabs(bottom1 - bottom2) > eps )
	{
		printf("��Q���̈ʒu�C�T�C�Y��ݒ肵�Ȃ���! \n");
		flagResetting = true;
	}
	else flagResetting = false;

/*
	int NX_ObsT = NX1_2 - NX1_1;
	int NY_ObsW = floor(rect.obs_width / DY);
	//�i�q�Ԋu�̐����{��
	float left, thick, width, top, bottom;
	thick = DX * (float) NX_ObsT;
	width = DY * (float) NY_ObsW;
	left = DX * (float)NX1_1;
	top = DY * (float)NY1_2;
	bottom = DY * (float)NY1_1;
//printf("AAA left=%f, thick=%f, width=%f \n", rect.obs_left, rect.obs_thick, rect.obs_width);
//printf("BBB left=%f, thick=%f, width=%f \n", left, thick, width);
	if((rect.obs_left - rect.obs_thick/2.0 - left) > eps || (rect.obs_thick - thick) > eps 
		|| (rect.obs_width - width) > eps || (obsTop - top) > eps || (obsBottom - bottom) > eps)
	{
		printf("��Q���̈ʒu�C�T�C�Y��ݒ肵�Ȃ���! \n");
		flagResetting = true;
	}
	else flagResetting = false;
*/
	//���̏�Q�����E�̕����_�ԍ�
	yy0 = (rect.size.y - rect.obs_dist2) / 2.0;//���̏�Q���̒��S
	int NX2_1 = floor((rect.obs_left + rect.obs_dist1 - rect.obs_thick/2.0) / DX + eps);//��Q�����[�ʒu
	int NX2_2 = floor((rect.obs_left + rect.obs_dist1 + rect.obs_thick/2.0) / DX + eps);//��Q���E�[�ʒu
	int NY2_2 = floor((yy0 + rect.obs_width/2.0) / DY + eps);//��Q����[�ʒu
	int NY2_1 = floor((yy0 - rect.obs_width/2.0) / DY + eps);//��Q�����[�ʒu
	//��̏�Q�����E�̕����_�ԍ�
	yy0 = (rect.size.y + rect.obs_dist2) / 2.0;
	int NX3_1 = floor((rect.obs_left + rect.obs_dist1 - rect.obs_thick/2.0) / DX + eps);//��Q�����[�ʒu
	int NX3_2 = floor((rect.obs_left + rect.obs_dist1 + rect.obs_thick/2.0) / DX + eps);//��Q���E�[�ʒu
	int NY3_2 = floor((yy0 + rect.obs_width/2.0) / DY + eps);//��Q����[�ʒu
	int NY3_1 = floor((yy0 - rect.obs_width/2.0) / DY + eps);//��Q�����[�ʒu

	//�i�q�_�̃^�C�v
	int i, j;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			g_type[i][j] = INSIDE;//���_
			if(j == 0) g_type[i][j] = BOTTOM;//�����ǖ�
			if(j == NY) g_type[i][j] = TOP;//�㑤�ǖ�
			if(i == 0) g_type[i][j] = INLET;//�����[
			if(i == NX) g_type[i][j] = OUTLET;//���o�[
			//�����̏�Q��
			if(i == NX1_1 && j > NY1_1 && j < NY1_2) g_type[i][j] = OBS_LEFT;//��Q�����[
			if(i == NX1_2 && j > NY1_1 && j < NY1_2) g_type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i >= NX1_1 && i <= NX1_2 && j == NY1_2) g_type[i][j] = OBS_TOP;//��Q����[
			if(i >= NX1_1 && i <= NX1_2 && j == NY1_1) g_type[i][j] = OBS_BOTTOM;//��Q�����[
			if(i > NX1_1 && i < NX1_2 && j > NY1_1 && j < NY1_2) g_type[i][j] = OBSTACLE;//��Q������
			if(i == NX1_1 && j == NY1_1) g_type[i][j] = OBS_LL;//����
			if(i == NX1_1 && j == NY1_2) g_type[i][j] = OBS_UL;//����
			if(i == NX1_2 && j == NY1_1) g_type[i][j] = OBS_LR;//�E��
			if(i == NX1_2 && j == NY1_2)  g_type[i][j] = OBS_UR;//�E��
			//���̏�Q��
			if(i == NX2_1 && j > NY2_1 && j < NY2_2) g_type[i][j] = OBS_LEFT;//��Q�����[
			if(i == NX2_2 && j > NY2_1 && j < NY2_2) g_type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i >= NX2_1 && i <= NX2_2 && j == NY2_2) g_type[i][j] = OBS_TOP;//��Q����[
			if(i >= NX2_1 && i <= NX2_2 && j == NY2_1) g_type[i][j] = OBS_BOTTOM;//��Q�����[
			if(i > NX2_1 && i < NX2_2 && j > NY2_1 && j < NY2_2) g_type[i][j] = OBSTACLE;//��Q������
			if(i == NX2_1 && j == NY2_1) g_type[i][j] = OBS_LL;//����
			if(i == NX2_1 && j == NY2_2) g_type[i][j] = OBS_UL;//����
			if(i == NX2_2 && j == NY2_1) g_type[i][j] = OBS_LR;//�E��
			if(i == NX2_2 && j == NY2_2)  g_type[i][j] = OBS_UR;//�E��
			//��̏�Q��
			if(i == NX3_1 && j > NY2_1 && j < NY2_2) g_type[i][j] = OBS_LEFT;//��Q�����[
			if(i == NX3_2 && j > NY2_1 && j < NY2_2) g_type[i][j] = OBS_RIGHT;//��Q���E�[
			if(i >= NX3_1 && i <= NX2_2 && j == NY2_2) g_type[i][j] = OBS_TOP;//��Q����[
			if(i >= NX3_1 && i <= NX2_2 && j == NY2_1) g_type[i][j] = OBS_BOTTOM;//��Q�����[
			if(i > NX3_1 && i < NX3_2 && j > NY3_1 && j < NY3_2) g_type[i][j] = OBSTACLE;//��Q������
			if(i == NX3_1 && j == NY3_1) g_type[i][j] = OBS_LL;//����
			if(i == NX3_1 && j == NY3_2) g_type[i][j] = OBS_UL;//����
			if(i == NX3_2 && j == NY3_1) g_type[i][j] = OBS_LR;//�E��
			if(i == NX3_2 && j == NY3_2)  g_type[i][j] = OBS_UR;//�E��
		}

	//�����ʃe�N�Z���T�C�Y
	texWidth  = NX + 1;//���W�e�N�X�`���̉��T�C�Y
	texHeight = NY + 1;//���W�e�N�X�`���̏c�T�C�Y
	
	//�����ʔz��
	velX = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//X���x���̔����ƈ���
	velY = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//Y���x�Ƃ��̔���(A�����͕\���p�Q�x)

	//initialize
	int k;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
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
		}

	setTextureVelX();
	setFramebufferVelX();
	setTextureVelY();
	setFramebufferVelY();

	numRigid = 3;
	for(i = 0; i < numRigid; i++) rigid[i] = CRigid();
	//�����̏�Q���i�����́j
	rigid[0].kind = CUBE;
	rigid[0].color = GRAY;
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, 0.0, 0.0);
	rigid[0].vSize = Vector3D(rect.obs_thick, rect.obs_width, 1.0);

	//���̏�Q���i�����́j
	rigid[1].kind = CUBE;
	rigid[1].color = GRAY;
	rigid[1].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left + rect.obs_dist1, -rect.obs_dist2/2.0, 0.0);
	rigid[1].vSize = Vector3D(rect.obs_thick, rect.obs_width, 1.0);
	//��̏�Q���i�����́j
	rigid[2].kind = CUBE;
	rigid[2].color = GRAY;
	rigid[2].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left + rect.obs_dist1, rect.obs_dist2/2.0, 0.0);
	rigid[2].vSize = Vector3D(rect.obs_thick, rect.obs_width, 1.0);

	elapseTime1 = 0.0;//1sec�Ԉȓ��̌o�ߎ���
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(������)

	maxPrs0 = -1000.0; minPrs0 = 1000.0;
	maxVel0 = -1000.0; minVel0 = 1000.0;

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

			for (i = 0; i <= NX; i++) //�ǖ�
			{
				k = 4 * i;
				velX[k] = (float)flagIdeal;
				velY[k] = 0.0;
				k = 4 * (i + NY * texWidth);
				velX[k] = (float)flagIdeal;
				velY[k] = 0.0;
			}
			//��Q���\��
			for (i = 1; i < NX; i++)
				for (j = 0; j <= NY; j++) 
				{
					if(g_type[i][j] < OBS_LEFT) continue;
					k = 4 * (i + j * texWidth);
					velX[k] = 0.0;
					velY[k] = 0.0;
				}
/*
			for (i = 1; i < NX; i++)
				for (j = 0; j <= NY; j++) 
				{
					k = 4 * (i + j * texWidth);
					if(g_type[i][j] >= OBS_LEFT) //��Q���\��
					{
						velX[k] = 0.0;
						velY[k] = 0.0;
					}
					if(g_type[i][j] == BOTTOM || g_type[i][j] == TOP){//�ǖ�
						velX[k] = (float)flagIdeal;
						velY[k] = 0.0;
					}
				}*/
			//���͋��E����
			for (i = 0; i <= NX; i++)
				for (j = 0; j <= NY; j++) 
				{
					k = 4 * (i + j * texWidth) + 3;//����
					if(g_type[i][j] == INSIDE) continue;
					else if(g_type[i][j] == INLET)  velX[k] = velX[4*(i+1 + j * texWidth) + 3];//Neumann(�������j
					else if(g_type[i][j] == OUTLET) velX[k] = 0.0;//Dirichlet�i���o���j
					else if(g_type[i][j] == TOP) velX[k] = velX[4 * (i + (NY-1) * texWidth) + 3];
					else if(g_type[i][j] == BOTTOM) velX[k] = velX[4 * (i + texWidth) + 3];
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
			renewVelY(); //���xY�̍X�V
		}
		if(flagStep) flagFreeze = true;
		elapseTime2 += dt;
		elapseTime3 += deltaT * (float)dispSpeed;
	}

	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			k = 4 * (i + j * texWidth) + 3;

			if(g_type[i][j] >= OBS_LEFT) continue;
			if(velX[k] > maxPrs0) maxPrs0 = velX[k];
			if(velX[k] < minPrs0) minPrs0 = velX[k];
			if(velY[k] > maxVel0) maxVel0 = velY[k];
			if(velY[k] < minVel0) minVel0 = velY[k];
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
	
	rigid[0].draw();
	rigid[1].draw();
	rigid[2].draw();
	
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
	if(flagResetting) drawParam("Reset obstacle size",  200, 200.0);

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
		for(int i = 0; i < texWidth; i++)
	   glVertex2f((float)i, (float)j);//�_���W��2�����i�q���_�ԍ�
	glEnd();
}

void rendering()
{
	glUseProgram(shader_prog3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY[]

	GLint samplerVelXLoc = glGetUniformLocation(shader_prog3, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog3, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1��K�p
	GLint texWLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint sizeXLoc = glGetUniformLocation(shader_prog3, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog3, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);

	GLint adjustHLoc = glGetUniformLocation(shader_prog3, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint colorLoc = glGetUniformLocation(shader_prog3, "color");
	glUniform1i(colorLoc, color);
	GLint kindLoc = glGetUniformLocation(shader_prog3, "kind");
	glUniform1i(kindLoc, kind);
	GLint maxOmgLoc = glGetUniformLocation(shader_prog3, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader_prog3, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
	GLint maxVelLoc = glGetUniformLocation(shader_prog3, "maxVel");
	glUniform1f(maxVelLoc, maxVel);
	GLint shadingLoc = glGetUniformLocation(shader_prog3, "flagShading");
	glUniform1i(shadingLoc, flagShading);

	static float specular[] = { 0.8, 0.8, 0.8, 1.0};
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS, 100.0);

	if(flagWireframe == 0)
		drawGridPlate((float)NX, (float)NY, NX, NY);
	else
		drawGridLines((float)NX, (float)NY, NX, NY);
	glUseProgram(0);
}