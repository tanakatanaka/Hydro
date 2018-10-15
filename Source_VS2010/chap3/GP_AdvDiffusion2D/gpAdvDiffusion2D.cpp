/*
Microsoft Visual Studio .NET2005 Standard Edition
�v���W�F�N�g�̎�ށFVisualC++�v���W�F�N�g
�e���v���[�g�FWin32�R���\�[���v���W�F�N�g
�A�v���P�[�V�����̎�ށF�R���\�[���A�v���P�[�V����
�ǉ��̃I�v�V�����F��̃v���W�F�N�g

2�����ڗ��g�U������(CIP�g�p)
3D�\��
���x����ԓI�ɕω�(��]���x��j
GPGPU(���l�v�Z��GPU���Ŏ��s�j

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
#include "../../support3D.h"
#include "myGLUI.h"

//�֐��̃v���g�^�C�v�錾
void init();
void idle();
void display();
void rendering();
void setTexturePhi();
void setFramebufferPhi();
void setTextureVel();
void setFramebufferVel();
void drawNumberingPoints();
void renewPhi(float deltaT, float d);
void createFloor();

//��͗p�f�[�^�̃T�C�Y
int texWidth, texHeight;//�S�i�q��

//��͗p�f�[�^�z��
float *phi, *vel;
GLuint texID[2];//texture object
GLuint fbo[2];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGB;//�����ʂ�phi[]��r,g,b
GLenum type = GL_FLOAT;
//�V�F�[�_�v���O������
GLuint shader_prog1, shader_prog2;

int floor0;//floor��display list
float maxSpeed;//��]�^���̍ő呬�x

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
	main_window = glutCreateWindow("GP_AdvDiffusion2D");
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
	initGlsl(&shader_prog1, "simulation.vert", "simulation.frag");
	initGlsl(&shader_prog2, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.h�Ɏ���
	GLUI_Master.set_glutIdleFunc( idle );

	//�C�x���g�������[�v�ɓ���
	glutMainLoop();
	//���\�[�X�̉��
 	free(phi);
	free(vel);
	glDeleteTextures(2, texID);
	glDeleteFramebuffersEXT(2, fbo);
	glDeleteProgram(shader_prog1);
	glDeleteProgram(shader_prog2);
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
	view.dist = 30.0;
	view.vCnt = Vector3D(0.0, 0.0, 2.0);
	view.theta = 10.0;//��O�������i���_���^��j
	view.phi = -90.0;//�E�� +x��
	view.fovY = 20.0;
	view.nearZ =  0.1;
	view.farZ = 200.0;    
	setCamera();
	view0 = view;
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

	initData();
	createFloor();

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
	flagFreeze = 0;
	flagStep = 0;

	rect.delta.x = rect.size.x / (float)rect.nMesh;
	rect.delta.y = rect.size.y / (float)rect.nMesh;

	//�����ʃe�N�Z���T�C�Y
	texWidth  = rect.nMesh + 1;//���W�e�N�X�`���̉��T�C�Y
	texHeight = texWidth;      //���W�e�N�X�`���̏c�T�C�Y
	//�����ʔz��̐錾
	phi = (float*)malloc(3 * texWidth * texHeight * sizeof(float));
	vel = (float*)malloc(3 * texWidth * texHeight * sizeof(float));
	
	int i, j, k;
	float x, y, r;

	maxSpeed = 0.0; 
	float speed;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//���z���S����̋���
			x = (float)i * rect.delta.x - centerInit.x ;
			y = (float)j * rect.delta.y - centerInit.y ;
			if(profile == 0)//Cylinder
			{
				r = sqrt(x * x + y * y);
				if(r < radiusInit) phi[3*k] = 1.0;//�����ʁi���x�C�Z�xetc.)
				else phi[3*k] = 0.0;
			}
			else//Cube
			{
				if(fabs(x) < radiusInit && fabs(y) < radiusInit) phi[3*k] = 1.0;//�����ʁi���x�C�Z�xetc.)
				else phi[3*k] = 0.0;
			}
			//�����l
			phi[3*k + 1] = 0.0;//gx=df/dx
			phi[3*k + 2] = 0.0;//gy=df/dy

			if(flagRotation)
			{
				//��`���S����̋���
				x = (float)(i - texWidth/2) * rect.delta.x ;
				y = (float)(j - texHeight/2) * rect.delta.y ;
				vel[3 * k]     = speedR * y; //���xx����(���v���j
				vel[3 * k + 1] = -speedR * x;//���xy����
				vel[3 * k + 2] = 0.0;//���g�p
				speed = speedR * sqrt(x*x + y*y);
				if(speed > maxSpeed) maxSpeed = speed;
			}
			else//���i���s
			{
				vel[3 * k]     = velocity.x;//���xx����
				vel[3 * k + 1] = velocity.y;//���xy����
				vel[3 * k + 2] = 0.0;//���g�p
			}
		}
	setTexturePhi();
	setFramebufferPhi();
	setTextureVel();
	setFramebufferVel();

	elapseTime1 = 0.0;//1sec�Ԉȓ��̌o�ߎ���
	elapseTime2 = 0.0;//start��̑��o�ߎ���
}

void setTexturePhi()
{
	glBindTexture(target, texID[0]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, phi);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureVel()
{
	glBindTexture(target, texID[1]);
	//�s�N�Z���i�[���[�h
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//�e�N�X�`���̊��蓖�� 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, vel);
	//�e�N�X�`�����g��E�k��������@�̎w�� 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferPhi()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	//texture��framebuffer object�Ɍ��т���
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0 );
	//framebuffer object �̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void setFramebufferVel()
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

	static int countStrobo = 0;
	if(elapseTime1 >= 1.0)
	{
		elapseTime1 = 0.0;
		fps0 = fps;
		fps = 0;
	}
	QueryPerformanceCounter( &lastTime);

	float deltaT = deltaT0 / (float)thinningN;
	float Courant;
	if(flagRotation == 1)  Courant = maxSpeed * deltaT / rect.delta.x;
	else Courant = velocity.magnitude() * deltaT / rect.delta.x;
	
	float diff_num = diff_coef * deltaT / (rect.delta.x * rect.delta.x);//�g�U��
	if(flagStart==true && flagFreeze == false)
	{
		//�����ʂ̍X�V
		for(int i = 0; i < thinningN; i++) renewPhi( deltaT, diff_num ); 

		if(flagStep) flagFreeze = true;
		elapseTime2 += deltaT * (float)thinningN;;
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

	if(flagWireframe == 1)
	{
	  glPolygonMode(GL_FRONT,GL_LINE);
	  glPolygonMode(GL_BACK,GL_POINT);
	}
	else 
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	rendering();
	
	if(flagFloor) glCallList(floor0);//���E�n��
	if(flagScaleShow) drawScale(10.0, 10.0, 5.0);
	if(flagCoordShow) drawWorldCoordinates(5.0);
	 
	if(flagParamShow && !flagOrtho) 
	{
		drawParam("framerate=%d[fps]", fps0, 10, 30);
		drawParam("timestep=%1.5f[s]", dt, 200, 30);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 400, 30);
		drawParam("deltaX=%1.5f[m]", rect.delta.x, 10, 60);
		drawParam("deltaT=%1.5f[s]", deltaT, 150, 60);
		drawParam("Courant=%1.5f", Courant, 300, 60);
		drawParam("diff_num=%1.5f", diff_num, 440, 60);
	}

	//�o�b�t�@�̐؂�ւ�
	glutSwapBuffers();	
}

void renewPhi(float deltaT, float diff_num)
{
	//framebuffer object0��L����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//phi[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//vel[]
	//�V�F�[�_�v���O������L��
	glUseProgram(shader_prog1);
	GLint samplerPhiLoc = glGetUniformLocation(shader_prog1, "samplerPhi");
	glUniform1i(samplerPhiLoc, 0);//GL_TEXTURE0��K�p
	GLint samplerVelLoc = glGetUniformLocation(shader_prog1, "samplerVel");
	glUniform1i(samplerVelLoc, 1);//GL_TEXTURE1��K�p
	GLint texWidthLoc = glGetUniformLocation(shader_prog1, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint deltaTLoc = glGetUniformLocation(shader_prog1, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint diffLoc = glGetUniformLocation(shader_prog1, "diff_num");
	glUniform1f(diffLoc, diff_num);
	GLint sizeLoc = glGetUniformLocation(shader_prog1, "size0");
	glUniform1f(sizeLoc, rect.size.x);
	GLint boundaryLoc = glGetUniformLocation(shader_prog1, "boundary");
	glUniform1i(boundaryLoc, boundary);

	drawNumberingPoints();//���l��͗p�̃e�N�X�`����\��t����I�u�W�F�N�g

	// �V�F�[�_�v���O�����̉��� 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, phi);
	glBindTexture(target, 0);
	//framebuffer object�̖�����
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	if(flagConstant)
	{
		int i, j, k;
		float x, y, r;
		for(j = 0; j < texHeight; j++)
			for(i = 0; i < texWidth; i++)
			{
				k = i + j * texWidth;
				//���z���S����̋���
				x = (float)i * rect.delta.x - centerInit.x ;
				y = (float)j * rect.delta.y - centerInit.y ;
				if(profile == 0)//Cylinder
				{
					r = sqrt(x * x + y * y);
					if(r < radiusInit) phi[3*k] = 1.0;//�����ʁi���x�C�Z�xetc.)
				}
				else//Cube
				{
					if(fabs(x) < radiusInit && fabs(y) < radiusInit) phi[3*k] = 1.0;//�����ʁi���x�C�Z�xetc.)
				}
			}
		setTexturePhi();
		setFramebufferPhi();
	}
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
	glUseProgram(shader_prog2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//phi[]

	GLint samplerLoc = glGetUniformLocation(shader_prog2, "samplerPhi");
	glUniform1i(samplerLoc, 0);//GL_TEXTURE0��K�p
	GLint texWidthLoc = glGetUniformLocation(shader_prog2, "texWidth");
	glUniform1i(texWidthLoc, texWidth);

	GLint nMeshLoc = glGetUniformLocation(shader_prog2, "nMesh");
	glUniform1i(nMeshLoc, rect.nMesh);
	GLint sizeLoc = glGetUniformLocation(shader_prog2, "size0");
	glUniform1f(sizeLoc, rect.size.x);
	GLint adjustHLoc = glGetUniformLocation(shader_prog2, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint colorLoc = glGetUniformLocation(shader_prog2, "color");
	glUniform1i(colorLoc, color);

	if(flagWireframe == 0)
		drawGridPlate((float)rect.nMesh, (float)rect.nMesh, rect.nMesh, rect.nMesh);
	else
		drawGridLines((float)rect.nMesh, (float)rect.nMesh, rect.nMesh, rect.nMesh);

	glUseProgram(0);
}

void createFloor()
{
	floor0 = glGenLists(1);
	if(floor0 == 0) {printf("floor �� display list ���m�ۂł��܂���!\n"); return;}
	glNewList(floor0, GL_COMPILE);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -0.001);
	drawFloorZ(rect.size.x, rect.size.y, 2, 2);
	glPopMatrix();
	glEndList();
}
