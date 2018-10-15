/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

�g��������
GPGPU(���l�v�Z��GPU���Ŏ��s�j
�ړ���Q��
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
#include "../../rigid.h"
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
void decideRegion();
float regionFlag(float x, float y);
void calcObsPos();

//��͗p�f�[�^�̃T�C�Y
int texWidth, texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *wave;
GLuint texID[1];//texture object
GLuint fbo[1];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;
GLenum type = GL_FLOAT;
//�V�F�[�_�v���O������
GLuint shader_prog1, shader_prog2, shader_prog3;

int nDummy;
float direction = 1.0;//��Q�������ړ�����

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
	main_window = glutCreateWindow("GP_Wave3");
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
	view.dist= 25.0;
	view.vCnt = Vector3D(0.0, 0.0, 0.0);
	view.theta = 30.0;
	view.phi = -90.0;
	setCamera();
	view0 = view;

	lightPos[1] = 200.0;
	for(int i = 0; i < 3; i++) lightPos0[i] = lightPos[i];
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
	nDummy = 30;

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
			//�e�i�q�_�ɂ����鐅�ʂ̍����Ƃ����������x
			wave[k * 4 + 0] = 0.0;//���ʂ̂����������x
			wave[k * 4 + 1] = 0.0;//���ʂ̍���
			wave[k * 4 + 2] = 0.0;
			wave[k * 4 + 3] = 1.0;//��Q�����O����ɗ��p		
		}

	//��Q��
	//�S�Ẵp�^�[���ɋ���
	numRigid = 1;
	for(i = 0; i < RIGID_NUM_MAX; i++)	rigid[i] = CRigid();
	
	rigid[0].kind = CUBE;
	rigid[0].color = DARK_YELLOW;
	rigid[0].flagFixed = false;
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, 0.0, 0.0);
	rigid[0].vSize = Vector3D(rect.obs_thick, rect.obs_width, 1.0);

	if(pattern == 1)
	{
		numRigid = 3;
		rigid[1].kind = CUBE;
		rigid[1].color = GRAY;
		rigid[1].flagFixed = true;
		rigid[1].vSize = Vector3D(rect.size.x, 0.4, 1.0);
		rigid[1].vPos = Vector3D(0.0, rect.size.y/2.0 + 0.5 , 0.0);

		rigid[2].kind = CUBE;
		rigid[2].color = GRAY;
		rigid[2].flagFixed = true;
		rigid[2].vSize = Vector3D(rect.size.x, 0.4, 1.0);
		rigid[2].vPos = Vector3D(0.0, -rect.size.y/2.0 - 0.5 , 0.0);
	}
	decideRegion();
	setTextureWave();
	setFramebufferWave();

/*
j=texHeight /2;
for(i=0;i < rect.meshX;i++)
{
	k=i+j*texWidth;
	printf("i = %d , a = %f \n", i, wave[k*4+3]);
}*/

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

		calcObsPos();

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
	for(int k = 0; k < numRigid; k++) rigid[k].draw();

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
	GLint meshXLoc = glGetUniformLocation(shader_prog1, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog1, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint dummyLoc = glGetUniformLocation(shader_prog1, "nDummy");
	glUniform1i(dummyLoc, nDummy);
	GLint sizeXLoc = glGetUniformLocation(shader_prog1, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog1, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint ampLoc = glGetUniformLocation(shader_prog1, "amp");
	glUniform1f(ampLoc, amp0);
	GLint freqLoc = glGetUniformLocation(shader_prog1, "freq");
	glUniform1f(freqLoc, freq0);
	GLint timeLoc = glGetUniformLocation(shader_prog1, "time");
	glUniform1f(timeLoc, elapseTime2);
	GLint dragLoc = glGetUniformLocation(shader_prog1, "drag0");
	glUniform1f(dragLoc, drag0);
	GLint waveSpeedLoc = glGetUniformLocation(shader_prog1, "waveSpeed");
	glUniform1f(waveSpeedLoc, waveSpeed);
	GLint dtLoc = glGetUniformLocation(shader_prog1, "dt");
	glUniform1f(dtLoc, deltaT);

	GLint posObjLoc = glGetUniformLocation(shader_prog1, "posObs");
	float pos[] ={ rigid[0].vPos.x, rigid[0].vPos.y, rigid[0].vPos.z};
	glUniform3fv(posObjLoc, 1, pos);
	GLint sizeObjLoc = glGetUniformLocation(shader_prog1, "sizeObs");
	float size[] = { rigid[0].vSize.x, rigid[0].vSize.y, rigid[0].vSize.z};
	glUniform3fv(sizeObjLoc, 1, size);
	GLint velObjLoc = glGetUniformLocation(shader_prog1, "velObs");
	float vel[] = { rigid[0].vVel.x, rigid[0].vVel.y, rigid[0].vVel.z};
	glUniform3fv(velObjLoc, 1, vel);

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

	static float diffuse[] = { 0.4, 0.6, 0.9, 1.0};
	static float ambient[] = { 0.1, 0.2, 0.3, 1.0};
	static float specular[]= { 0.5, 0.5, 0.5, 1.0};

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

void decideRegion()
{
	int i, j, k;
	float x, y;
	float dx = rect.size.x / (float)rect.meshX;
	float dy = rect.size.y / (float)rect.meshY;

	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//�i�q�_�̍��W
			x = (float)(i - texWidth / 2) * dx;
			y = (float)(j - texHeight / 2) * dy;
			wave[k * 4 + 3] = regionFlag(x, y);
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
			 && vPoint.y >= - rigid[k].vSize.y / 2.0 && vPoint.y <= rigid[k].vSize.y / 2.0) //return 0.0;//��Q������
			{
				if(rigid[k].flagFixed == true) return 0.0;//�Œ��Q������
				else                           return 1.0;//�ړ���Q���͐��ʂƓ�������
			}
		}
		else if(rigid[k].kind == SPHERE || rigid[k].kind == CYLINDER)
		{
			//�ȉ~�`����
			float a = rigid[k].vSize.x * rigid[k].vSize.x / 4.0;
			float b = rigid[k].vSize.y * rigid[k].vSize.y / 4.0;
			float f =  vPoint.x*vPoint.x / a + vPoint.y*vPoint.y / b - 1.0;
			if(f <= 0.0) //return 0.0;
			{
				if(rigid[k].flagFixed == true) return 0.0;//�Œ��Q������
				else                           return 1.0;//�ړ���Q���͐��ʂƓ�������
			}
		}
	}
	return 1.0;//�O��(����)
}

void calcObsPos()
{
	//if(flagObsStop) { rigid[0].vVel = Vector3D();  return; }

	//else rigid[0].vVel.x = velocity0;
	Vector3D left;//�_�N�g�̍��[����щ��[����̋���

	float R = rect.size.x / 2.0 - rect.obs_left;//��]���a
	//float theta;
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
	left.x = rigid[0].vPos.x + rect.size.x/2.0;
	left.y = rigid[0].vPos.y + rect.size.y/2.0;
}
