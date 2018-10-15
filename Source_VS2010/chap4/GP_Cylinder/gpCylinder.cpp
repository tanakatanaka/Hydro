/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2����FDM
�~���܂��̗���(�S�~�j
�ɍ��W�ϊ��C����ɓ��a�����ɕs���Ԋu�i�q
����֐�-�Q�x�@
GPGPU(���l�v�Z��GPU���Ŏ��s�j
CIP���g�p
�Q�x��Poisson�������������ɂ͉����ĂȂ��i�P�񂾂��j

GLSL�ɂ��V�F�[�f�B���O
�������͂���
(�����ݒ�Fx���͎�O�Cy���͉E�����j
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
void setTextureOmega();
void setFramebufferOmega();
void setTexturePsi();
void setFramebufferPsi();
void drawNumberingPoints();
void renewPsi();
void renewOmega(float deltaT);
//void createFloor();

//��͗p�f�[�^�̃T�C�Y
int texWidth, texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *omg;//�Q�x
float *psi;//����֐��C���x
GLuint texID[2];//texture object
GLuint fbo[2];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGB;//�����ʂ�psi[]�����omg��r,g,b
GLenum type = GL_FLOAT;
//�V�F�[�_�v���O������
GLuint shader_prog1, shader_prog2, shader_prog3;
int NX, NY;
float DX, DY;
int count = 0;

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
	main_window = glutCreateWindow("GP_Cylinder");
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
	initGlsl(&shader_prog1, "simulation.vert", "calcPsi.frag");
	initGlsl(&shader_prog2, "simulation.vert", "calcOmega.frag");
	initGlsl(&shader_prog3, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.h�Ɏ���

	//�C�x���g�������[�v�ɓ���
	glutMainLoop();
	//���\�[�X�̉��
 	free(omg);
	free(psi);
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
	view.dist = 80.0;
	view.vCnt = Vector3D(10.0, 0.0, 0.0);
	view.theta = 90.0;//��O�������i���_���^��j
	view.phi = -90.0;//�E�� +x��
	setCamera();
	view0 = view;
	ortho_scale = 10.0;
	//parameters

	//���Ԋ֌W
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

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

	NX = region.nMesh;//���a����������
	NY = 2 * NX;     //���ʊp����
	
	//�~���̔��a��1�ɌŒ�
	float xiMax = log(region.Radius);//�v�Z�ʂɂ�����O�����a(�~�����a��1.0�ɌŒ�j
	DX = xiMax / (float)NX;//���a��������
	DY = 2.0 * M_PI / (float)(NY); //���ʊp��������

	//�����ʃe�N�Z���T�C�Y
	texWidth  = NX + 1;//���W�e�N�X�`���̉��T�C�Y
	texHeight = NY + 1;//���W�e�N�X�`���̏c�T�C�Y
	
	//�����ʔz��̐錾
	psi = (float*)malloc(3 * texWidth * texHeight * sizeof(float));//����֐��Ƒ��x
	omg = (float*)malloc(3 * texWidth * texHeight * sizeof(float));//�Q�x�Ƃ��̔���
	//initialize
	int i, j, k, km, kp;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//�Q�x
			//if(j >= 5 && j <= 7 ) omg[3 * k + 0] = 10.0;
			//else
			omg[3 * k + 0] = 0.0;
			omg[3 * k + 1] = 0.0;//x����
			omg[3 * k + 2] = 0.0;//y����
			psi[3 * k + 0] = exp((float)i * DX) * sin((float)j * DY - M_PI);//����֐�(������������j
		}
	//�i�q�_�̑��x�x�N�g��(�㉺���E�̗���֐��̍��ŋ��߂�)
	float ex;
	int jm, jp;
	for(i = 1; i < NX; i++)
	{
		ex = exp(- (float)i * DX);
		for (j = 0; j <= NY; j++)
		{
			k = i + j * texWidth;
			if(j == 0) jm = NY -1; else 
			jm = j - 1;
			//if(j == NY-1) jp = 0; else 
				jp = j + 1;
			if(j == NY) jp = 1;
			kp = i + jp * texWidth;
			km = i + jm * texWidth;
			//�|�e���V�����̒Ⴂ�����獂������
			psi[3 * k + 1] = ex * (psi[3*kp] - psi[3*km]) / (2.0*DY);//���a����
			psi[3 * k + 2] = -ex * (psi[3*(i+1 + j*texWidth)] - psi[3*(i-1 + j*texWidth)]) / (2.0*DX);//���ʊp����
		}
	}
	setTexturePsi();
	setFramebufferPsi();
	setTextureOmega();
	setFramebufferOmega();

	numRigid = 1;
	for(i = 0; i < numRigid; i++) rigid[i] = CRigid();
	//�~��
	rigid[0].kind = CYLINDER;
	rigid[0].color = GRAY;
	rigid[0].vPos = Vector3D(0.0, 0.0, 0.0);
	rigid[0].vSize = Vector3D(2.0, 2.0, 1.0);//���a1.0

	elapseTime1 = 0.0;//1sec�Ԉȓ��̌o�ߎ���
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;//start��̑��o�ߎ���(������)
	count = 0;
}

void setTexturePsi()
{
	glBindTexture(target, texID[0]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, psi);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureOmega()
{
	glBindTexture(target, texID[1]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, omg);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferPsi()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0 );
	//framebuffer object �̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void setFramebufferOmega()
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
	
	if(flagStart && flagFreeze == false)
	{
		for(int n = 0; n < dispSpeed; n++)
		{
			for (int j = 0; j < NY; j++) 
			{
				//�~�����E��
				int k = j * texWidth;
				omg[3 * k] = -2.0 * psi[3*(1 + j * texWidth)] /(DX*DX);
				psi[3 * k] = 0.0;
			
				//�O�����E
				//i == NX
				k = NX + j * texWidth;
				omg[3 * k] = omg[3 * (NX-1 + j * texWidth)];//Neumann 
				//psi[3 * k] = region.Radius * sin((float)j * DY - M_PI);
			}

			setTexturePsi();
			setFramebufferPsi();
			setTextureOmega();
			setFramebufferOmega();
			
			renewPsi();         //����֐��̍X�V
			renewOmega(deltaT); //�Q�x�̍X�V
		}
		if(flagStep) flagFreeze = 1;
		elapseTime2 += dt;
		elapseTime3 += deltaT * dispSpeed;
		count += dispSpeed;
//printf("count = %d \n", count);
//if(count == 1000) flagFreeze = 1;
//if(count == 2500) flagFreeze = 1;
//if(count == 50000) flagFreeze = 1;

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
	
//	if(flagCoordShow) drawWorldCoordinates(0.6);
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
		drawParam("t_step=%1.5f[s]", dt, 120, 30);
		drawParam("e_time=%3.3f[s]", (float)elapseTime2, 280, 30);
		drawParam("n_time=%3.3f", elapseTime3, 440, 30);
		drawParam("Courant=%1.4f", Courant, 10, 60);
		drawParam("diff_num=%1.4f", diff_num, 150, 60);
		drawParam("Re=%3.1f", Re, 300, 60);
	}

	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();	
}

void renewPsi()
{
	//framebuffer object1��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//psi[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//omg[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader_prog1);
	GLint samplerPsiLoc = glGetUniformLocation(shader_prog1, "samplerPsi");
	glUniform1i(samplerPsiLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerOmgLoc = glGetUniformLocation(shader_prog1, "samplerOmg");
	glUniform1i(samplerOmgLoc, 1);//GL_TEXTURE1��K�p
	GLint texWLoc = glGetUniformLocation(shader_prog1, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog1, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint DXLoc = glGetUniformLocation(shader_prog1, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog1, "DY");
	glUniform1f(DYLoc, DY);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, psi);

	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewOmega(float deltaT)
{
	//framebuffer object0��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[1] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//psi[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//omg[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader_prog2);
	GLint samplerPsiLoc = glGetUniformLocation(shader_prog2, "samplerPsi");
	glUniform1i(samplerPsiLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerOmgLoc = glGetUniformLocation(shader_prog2, "samplerOmg");
	glUniform1i(samplerOmgLoc, 1);//GL_TEXTURE1��K�p
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
	glReadPixels(0, 0, texWidth, texHeight, format, type, omg);

	//printf("omg[5][NY/8] = %f  omg[10][NY/8] = %f \n", omg[3*(5 +(NY/8) * texWidth)], omg[3*(10 +(NY/8) * texWidth)]);
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
	glBindTexture(target, texID[0]);//psi[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//omg[]

	GLint samplerPsiLoc = glGetUniformLocation(shader_prog3, "samplerPsi");
	glUniform1i(samplerPsiLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerOmgLoc = glGetUniformLocation(shader_prog3, "samplerOmg");
	glUniform1i(samplerOmgLoc, 1);//GL_TEXTURE1��K�p
	GLint texWLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint adjustHLoc = glGetUniformLocation(shader_prog3, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint radiusLoc = glGetUniformLocation(shader_prog3, "Radius");
	glUniform1f(radiusLoc, region.Radius);
	GLint colorLoc = glGetUniformLocation(shader_prog3, "color");
	glUniform1i(colorLoc, color);
	GLint kindLoc = glGetUniformLocation(shader_prog3, "kind");
	glUniform1i(kindLoc, kind);
	GLint DXLoc = glGetUniformLocation(shader_prog3, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog3, "DY");
	glUniform1f(DYLoc, DY);
	GLint maxOmegaLoc = glGetUniformLocation(shader_prog3, "maxOmega");
	glUniform1f(maxOmegaLoc, maxOmega);
	GLint maxPsiLoc = glGetUniformLocation(shader_prog3, "maxPsi");
	glUniform1f(maxPsiLoc, maxPsi);
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
