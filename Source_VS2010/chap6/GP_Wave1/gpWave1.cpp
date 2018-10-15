/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

�g��������
GPGPU(���l�v�Z��GPU���Ŏ��s�j

GLSL�ɂ��V�F�[�f�B���O
�������͂���
(�����ݒ�Fx���͉E�����Cy���͉��j
*/

#include <windows.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glui.h>
#include "../../myGlsl.h"
#include "../../myMath6.h"
#include "../../myPrimitive6.h"
#include "../../support3D.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾
void init();
void idle();
void display();
void rendering();
void setTextureWave();
void setFramebufferVel();
void setFramebufferWave();
void drawNumberingPoints();
void renewVelocity();
void renewPosition();

//��͗p�f�[�^�̃T�C�Y
int texWidth, texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *wave;
GLuint texID[1];//texture object
GLuint fbo[1];  //framebuffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;
GLenum type = GL_FLOAT;
//�V�F�[�_�v���O������
GLuint shader_prog1, shader_prog2, shader_prog3;

//�^�C���X�e�b�v�̋}���ȕω��ɑΉ������邽�߂����Ұ�
//double timeStep, timeStep0, timeStep1;
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
	main_window = glutCreateWindow("GP_Wave1");
	//�E�B���h�E�̃T�C�Y�ύX
	glutReshapeFunc(resize);
	//�\��
	glutDisplayFunc(display);
	//�}�E�X����
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//�ĕ`��
	glutIdleFunc(idle);
	//�����ݒ�
	glewInit();//framebuffer�𗘗p����Ƃ��ɕK�v
	init();
	initGlsl(&shader_prog1, "simulation.vert", "waveVel.frag");
	initGlsl(&shader_prog2, "simulation.vert", "wavePos.frag");
	initGlsl(&shader_prog3, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.h�Ɏ���

	//�C�x���g�������[�v�ɓ���
	glutMainLoop();
	//���\�[�X�̉��
 	free(wave);
	glDeleteTextures(1, texID);
	glDeleteFramebuffersEXT(1, fbo);
	glDeleteProgram(shader_prog1);
	glDeleteProgram(shader_prog2);
	glDeleteProgram(shader_prog3);
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
	view.dist= 25.0;
	view.vCnt = Vector3D(0.0, 0.0, 0.0);
	view.theta = 70.0;
	view.phi = -90.0;
	setCamera();
	view0 = view;

	lightPos[1] = 200.0;
	//parameters
	//timeRate = 1.0;

	//���Ԋ֌W
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	glGenTextures(1, texID);//�e�N�X�`���I�u�W�F�N�g�̖��O�t��
	glGenFramebuffersEXT(1, fbo);//�t���[���o�b�t�@�I�u�W�F�N�g�̐���

	//�\���E�B���h�E�ɕ����������_�����O���鏀��
	setFontBitmaps();

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
	//flagFreeze = false;
	//flagStep = false;

	if(boundary == B_NON) nDummy = 20;//�_�~�[�i�q��(�Б�)
	else nDummy = 0;

	//�����ʃe�N�Z���T�C�Y
	texWidth  = rect.meshX + 2 * nDummy + 1;//���W�e�N�X�`���̉��T�C�Y
	texHeight = rect.meshY + 2 * nDummy + 1;//���W�e�N�X�`���̉��T�C�Y
	//texHeight = texWidth;                //���W�e�N�X�`���̏c�T�C�Y
	
	//�����ʔz��̐錾
	wave = (float*)malloc(4 * texWidth * texHeight * sizeof(float));

	//initialize
	int i, j, k;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//�e�i�q�_�̐��ʂ̍����Ƃ����������x
			wave[k * 4 + 0] = 0.0;//���ʂ̂����������x
			wave[k * 4 + 1] = 0.0;//���ʂ̕ψ�
			wave[k * 4 + 2] = 0.0;
			wave[k * 4 + 3] = 0.0;		
		}

	setTextureWave();
	setFramebufferWave();

	elapseTime1 = 0.0;//1sec�Ԉȓ��̌o�ߎ���
	elapseTime2 = 0.0;//start��̑��o�ߎ���
	elapseTime3 = 0.0;
}

void setTextureWave()
{
	glBindTexture(target, texID[0]);
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
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0 );
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

	static int countStrobo = 0;
	if(elapseTime1 >= 1.0)
	{
		elapseTime1 = 0.0;
		fps0 = fps;
		fps = 0;
	}
	QueryPerformanceCounter( &lastTime);

	if(flagStart==true && flagFreeze == false)
	{
		//step1:���x�̍X�V
		renewVelocity();

		//step2:�ʒu�̍X�V
		renewPosition();

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

	if(flagCoordShow) drawWorldCoordinates(5.0);//���[���h��Ԃ̍��W��
	
	if(flagParamShow) 
	{
		drawParam("f_rate=%d[fps]", fps0, 10, 30);
		drawParam("t_step=%1.5f[s]", dt, 150, 30);
		drawParam("e_time=%3.3f[s]", elapseTime2, 300, 30);
		drawParam("n_time=%3.3f", elapseTime3, 460, 30);
	}
	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();	
}

void renewVelocity()
{
	//framebuffer object0��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//wave[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader_prog1);
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog1, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE0��K�p
	GLint texWidthLoc = glGetUniformLocation(shader_prog1, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog1, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint sizeXLoc = glGetUniformLocation(shader_prog1, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog1, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint meshXLoc = glGetUniformLocation(shader_prog1, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog1, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint dummyLoc = glGetUniformLocation(shader_prog1, "nDummy");
	glUniform1i(dummyLoc, nDummy);

	GLint ampLoc = glGetUniformLocation(shader_prog1, "amp");
	glUniform1f(ampLoc, amp0);
	GLint freqLoc = glGetUniformLocation(shader_prog1, "freq");
	glUniform1f(freqLoc, freq0);
	GLint timeLoc = glGetUniformLocation(shader_prog1, "time");
	glUniform1f(timeLoc, elapseTime2);
	GLint nSourceLoc = glGetUniformLocation(shader_prog1, "nSource");
	glUniform1i(nSourceLoc, nSource0);
	GLint sourceXLoc = glGetUniformLocation(shader_prog1, "sourceX");
	glUniform1i(sourceXLoc, sourceX0);
	GLint intervalLoc = glGetUniformLocation(shader_prog1, "interval");
	glUniform1i(intervalLoc, interval0);
	GLint dragLoc = glGetUniformLocation(shader_prog1, "drag0");
	glUniform1f(dragLoc, drag0);
	GLint waveSpeedLoc = glGetUniformLocation(shader_prog1, "waveSpeed");
	glUniform1f(waveSpeedLoc, waveSpeed);
	GLint dtLoc = glGetUniformLocation(shader_prog1, "dt");
	glUniform1f(dtLoc, deltaT);
	GLint modeLoc = glGetUniformLocation(shader_prog1, "mode");
	glUniform1i(modeLoc, mode);
	GLint boundaryLoc = glGetUniformLocation(shader_prog1, "boundary");
	glUniform1i(boundaryLoc, boundary);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	//glReadPixels(0, 0, texWidth, texHeight, format, type, wave);
	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewPosition()
{
	//framebuffer object0��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//wave[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader_prog2);
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog2, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE0��K�p
	GLint texWidthLoc = glGetUniformLocation(shader_prog2, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog2, "texHeight");
	glUniform1i(texHeightLoc, texHeight);

	GLint boundaryLoc = glGetUniformLocation(shader_prog2, "boundary");
	glUniform1i(boundaryLoc, boundary);
	GLint dtLoc = glGetUniformLocation(shader_prog2, "dt");
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
		for(int i = 0; i < texWidth; i++)
	   glVertex2f((float)i, (float)j);//�_���W��2�����i�q���_�ԍ�
	glEnd();

}

void rendering()
{
	glUseProgram(shader_prog3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//wave[]

	GLint samplerWaveLoc = glGetUniformLocation(shader_prog3, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE1��K�p
	GLint texWidthLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint dummyLoc = glGetUniformLocation(shader_prog3, "nDummy");
	glUniform1i(dummyLoc, nDummy);

	GLint meshXLoc = glGetUniformLocation(shader_prog3, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog3, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint sizeXLoc = glGetUniformLocation(shader_prog3, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog3, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);

	static float diffuse[] = { 0.2, 0.5, 0.9, 1.0};
	static float ambient[] = { 0.1, 0.2, 0.3, 1.0};
	static float specular[]= { 0.8, 0.8, 0.8, 1.0};

	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS,100.0);
	if(flagWireframe == 0)
		drawGridPlate((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);
	else
		drawGridLines((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);

	glUseProgram(0);
}
