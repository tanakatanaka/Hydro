void resize(int w, int h);

//Window�̃T�C�Y,�ʒu
int win_width  = 600;
int win_height = 500;
int win_x0 = 500;
int win_y0 = 100;

//�o�ߎ���
LARGE_INTEGER freq, lastTime, currentTime;
double pTime;
double elapseTime1, elapseTime2, elapseTime3;

//�}�E�X����
int xStart, yStart;
bool flagMouse = false;
bool flagSlide = false;
bool flagStart = false; //�A�j���[�V�����X�^�[�g�t���O
bool flagFreeze = false;//�ꎞ��~�t���O
bool flagFreeze0 = false;
bool flagStep = false;

//�^�C���X�e�b�v�̋}���ȕω��ɑΉ������邽�߂����Ұ�
float timeStep, timeStep0, timeStep1;
//�����e
int flagOrtho = 0;
float ortho_scale = 5.0;//�����e�̂Ƃ��̃T�C�Y���W�� 

//����
float lightPos[] =  {0.0, 0.0, 100.0, 1.0};//�����ʒu
//float lightPos0[] = {50.0, -50.0, 100.0, 1.0};//�����ʒu(�����l)
float lightPos0[] =  {0.0, 0.0, 100.0, 1.0};//�����ʒu

void setLight()
{
	float lightAmbient0[] = {0.7, 0.7, 0.7, 1.0}; //����
	float lightDiffuse0[] = {0.9, 0.9, 0.9, 1.0}; //�g�U��
	float lightSpecular0[] = {0.9, 0.9, 0.9, 1.0};//���ʌ�
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

//�J�����Ǝ��̐�
struct View
{
	//�J����
	Vector3D vPos;//�ʒu�i���_�j
	Vector3D vCnt;//�����_
	float dist;  //�����_���王�_�܂ł̋���(R)
	float theta; //�p�i�����ʂƂ̕Ίp�j
	float phi;   //���ʊp
	//���̐�
	float fovY;  //����p
	float nearZ; //�O���N���b�v��(�ߕ���)
	float farZ;  //����N���b�v��(������)
};
View view = 
{ 
	Vector3D(),             //pos(���_,���ݒ�j
	Vector3D(0.0, 0.0, 7.0),//cnt(�����_�j 
	100.0, 15.0, 0.0,       //dist, theta, phi
	//40.0, 0.1, 20.0       //fovY,nearZ,farZ
	30.0, 1.0, 1000.0       //fovY,nearZ,farZ
};
View view0 = view;

void resize(int w, int h)
{
	//�r���[�|�[�g�ϊ�
	glViewport(0, 0, w, h);
	//�v���W�F�N�V�����s��̎w��
	glMatrixMode(GL_PROJECTION);
	//�v���W�F�N�V�����s��̏�����
	glLoadIdentity();
	//�������e�s��̐ݒ�i���e�ϊ��j
	float aspect = (double)w/(double)h;
	if(flagOrtho) glOrtho(-ortho_scale*aspect, ortho_scale*aspect, -ortho_scale, ortho_scale, view.nearZ, view.farZ);
	else gluPerspective(view.fovY, aspect, view.nearZ, view.farZ);
	glMatrixMode(GL_MODELVIEW);
	//���f���r���[�s��̏�����
	glLoadIdentity();
	//�\���E�B���h�E�̃T�C�Y
	win_width = w;
	win_height = h;
}


void setCamera()
{
	view.vPos.x = view.vCnt.x + view.dist * cos(DEG_TO_RAD * view.theta) * cos(DEG_TO_RAD * view.phi);//x
	view.vPos.y = view.vCnt.y + view.dist * cos(DEG_TO_RAD * view.theta) * sin(DEG_TO_RAD * view.phi);//y
	view.vPos.z = view.vCnt.z + view.dist * sin(DEG_TO_RAD * view.theta);//z
	resize(win_width, win_height);
}

//�ȉ��̂R�̊֐��̓}�E�X����ɂ�鎋�_�̕ύX�ɕK�v
void mouse(int button, int state, int x, int y)
{
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
	  xStart = x; yStart = y;
	  flagMouse = true;
	  if(x > win_width/4 && x < 3*win_width/4 && y > win_height/4 && y < 3*win_height/4)//dolly
	  {
		  if(!flagOrtho) view.dist -= 0.5; //�߂Â�
		  else ortho_scale -= 0.5;
	  }
  }
  else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
  {
	  if(x > win_width/4 && x < 3*win_width/4 && y > win_height/4 && y < 3*win_height/4)//dolly
	  {
		  if(!flagOrtho) view.dist += 0.5; //��������
		  else ortho_scale += 0.5;
	  }
	  else if(( x < win_width/4 || x > 3*win_width/4) && (y > win_height/4 && y < 3*win_height/4))//pan
	  {
		  if(x < win_width/4 ) view.phi -= 1.0;
		  else view.phi += 1.0;
		  view.vCnt.x = view.vPos.x - view.dist * cos(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
		  view.vCnt.y = view.vPos.y - view.dist * sin(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
	  }
	  else if((x > win_width/4 && x < 3*win_width/4) && (y < win_height/4 || y > 3*win_height/4))//tilt
	  {
		  if( y < win_height/4)
			  view.theta += 1.0; 
		  else
			  view.theta -= 1.0;
		  
		  view.vCnt.x = view.vPos.x - view.dist * (float)(cos(DEG_TO_RAD * view.theta) * cos(DEG_TO_RAD * view.phi));
		  view.vCnt.y = view.vPos.y - view.dist * (float)(cos(DEG_TO_RAD * view.theta) * sin(DEG_TO_RAD * view.phi));
		  view.vCnt.z = view.vPos.z - view.dist * (float)sin(DEG_TO_RAD * view.theta);
	  }
	  else if(x < win_width/8 && y > 7*win_height/8) view.fovY -= 1.0;//zoom in
	  else if(x > 7*win_width/8 && y > 7*win_height/8) view.fovY += 1.0;//zoom out
  }
  else flagMouse = false;
  if(state == GLUT_DOWN) setCamera();
}

void motion(int x, int y)
{
  if(!flagMouse) return;
	if(y < win_height / 2)
		view.phi += 0.2 * (float)(x - xStart) ;//tumble
	else
		view.phi -= 0.2 * (float)(x - xStart) ;//tumble

  view.theta += 0.2 * (float)(y - yStart) ;//crane

  setCamera();
  xStart = x;
  yStart = y;
}
//------------------------------------------------------------------------
//�`��E�B���h�E�ɕ���/���W�\�����邽�߂̏����ݒ�Ɗ֐�
char text[100];
GLuint listBase;

void setFontBitmaps()
{//�����ݒ�
	HDC hdc = wglGetCurrentDC();
	HGLRC hglrc = wglGetCurrentContext();
	wglMakeCurrent(hdc, hglrc);
	SelectObject(hdc, GetStockObject(SYSTEM_FONT));
	wglUseFontBitmaps(hdc, 0, 255, 1000);
	glListBase(1000);
}

Vector3D getWorldPos(int x, int y, int win_width, int win_height)
{//2�����E�B���h�E���W->3�������[���h���W�ϊ�
  //���_���璍���_�֌������x�N�g��
  Vector3D vDirView = view.vCnt - view.vPos;
  //�E�B���h�E���S�_�̃��[���h���W
	//(�\���ʒu�����̐ς̒��ɓ���悤�ɋߕ��ʂ̋����𒷂����Čv�Z�j
  Vector3D vCenterWindow
        = ((view.nearZ+0.1)/vDirView.magnitude() - 1.0) * vDirView + view.vCnt;
  //���̐ς̋ߕ��ʂ̍���
  float heightNearPlane = 2.0 * (view.nearZ+0.1) * tan(DEG_TO_RAD * view.fovY / 2.0);
//printf("h=%f \n", heightNearPlane);
  //window�̏�����x�N�g��
  Vector3D vUp;
  vUp = vDirView.norm2();
  vUp.rotZ(-view.phi);
  vUp.rotY(90.0);
  vUp.rotZ(view.phi);
  //�E�����x�N�g��
  Vector3D vRight = vCross(vDirView, vUp);
  vRight.norm();
  float ratio = heightNearPlane / (float)win_height;

  float rx, ry; //�X�N���[���̒��S����̋���(world���W�ɕϊ�)
  rx = ((float)x - (float)win_width / 2.0) * ratio;
  ry = ((float)win_height / 2.0 - (float)y) * ratio;

  Vector3D vPosWorld = vRight * rx + vUp * ry + vCenterWindow;
  return vPosWorld ;
}

void drawParam(const char *character, int param, int xx, int yy)
{
	int nn = sprintf_s(text, character, param);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	Vector3D vPosW = getWorldPos(xx, yy, win_width, win_height);
	glRasterPos3f(vPosW.x, vPosW.y, vPosW.z);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	glEnable(GL_LIGHTING);
}
void drawParam(const char *character, float param, int xx, int yy)
{
	int nn = sprintf_s(text, character, param);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	Vector3D vPosW = getWorldPos(xx, yy, win_width, win_height);
	glRasterPos3f(vPosW.x, vPosW.y, vPosW.z);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	glEnable(GL_LIGHTING);
}
void drawParam(const char *character, double param, int xx, int yy)
{
	int nn = sprintf_s(text, character, param);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	Vector3D vPosW = getWorldPos(xx, yy, win_width, win_height);
	glRasterPos3f(vPosW.x, vPosW.y, vPosW.z);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	glEnable(GL_LIGHTING);
}
void drawParam(const char *character, int xx, int yy)
{
	int nn = sprintf_s(text, character);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	Vector3D vPosW = getWorldPos(xx, yy, win_width, win_height);
	glRasterPos3f(vPosW.x, vPosW.y, vPosW.z);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	glEnable(GL_LIGHTING);
}

void drawScale(float widthX, float widthY, float height)
{
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_LINES);
	glVertex3f(-0.5*widthX, -0.5*widthY, -0.2*height); glVertex3f(-0.5*widthX, -0.5*widthY, height);//z
	glVertex3f(-0.5*widthX,  0.5*widthY, -0.2*height); glVertex3f(-0.5*widthX,  0.5*widthY, height);//z
	glVertex3f( 0.5*widthX,  0.5*widthY, -0.2*height); glVertex3f( 0.5*widthX,  0.5*widthY, height);//z
	glEnd();
	glBegin(GL_LINES);
	for(int i = 0; i < 7; i++)
	{
		glVertex3f(-0.5*widthX, -0.5*widthY, 2.0*(-0.1 + 0.1*(float)i)*height); glVertex3f(-0.5*widthX, 0.5*widthY, 2.0*(-0.1 + 0.1*(float)i)*height);
	}
	glEnd();
	glBegin(GL_LINES);
	for(int i = 0; i < 7; i++)
	{
		glVertex3f( 0.5*widthX, 0.5*widthY, 2.0*(-0.1 + 0.1*(float)i)*height); glVertex3f(-0.5*widthX, 0.5*widthY, 2.0*(-0.1 + 0.1*(float)i)*height);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void drawWorldCoordinates(float len)
{
	glDisable(GL_LIGHTING);
	glLineWidth(2.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.01); glVertex3f(len, 0.0, 0.005);//x(0.005��floor�ɏd�Ȃ�Ȃ��悤�Ɏ����グ�j
		glVertex3f(0.0, 0.0, 0.01); glVertex3f(0.0, len, 0.01);//y
		glVertex3f(0.0, 0.0, 0.0); glVertex3f(0.0, 0.0, len);//z
	glEnd();
	int nn = sprintf_s(text, "x");
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glRasterPos3f(len + 0.03, 0.0, 0.0);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	nn = sprintf_s(text, "y");
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glRasterPos3f(0.0, len + 0.03, 0.0);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
	nn = sprintf_s(text, "z");
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glRasterPos3f(0.0, 0.0, len + 0.03);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);

	glEnable(GL_LIGHTING);
}
