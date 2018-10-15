/*
//Window�̃T�C�Y,�ʒu
int win_width  = 600;
int win_height = 500;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 300;
int win_y0 = 100;
*/
//�o�ߎ���
LARGE_INTEGER freq, lastTime, currentTime;
double pTime;
double elapseTime1, elapseTime2, elapseTime3;
//�F�̖���
enum Color { RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, BLACK,
             DARK_RED, DARK_GREEN, DARK_BLUE, DARK_YELLOW, DARK_CYAN, DARK_MAGENTA};

//---------------------------------------------------------------
//�`��E�B���h�E�ɕ����\�����邽�߂̏����ݒ�Ɗ֐�
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

void drawParam(const char *character, int param, float xx, float yy)
{
	int nn = sprintf_s(text, character, param);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(xx, yy);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
}

void drawParam(const char *character, float param, float xx, float yy)
{
	int nn = sprintf_s(text, character, param);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(xx, yy);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
}

void drawParam(const char *character, double param, float xx, float yy)
{
	int nn = sprintf_s(text, character, param);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(xx, yy);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
}
void drawParam(const char *character, float xx, float yy)
{
	int nn = sprintf_s(text, character);
	listBase = (GLuint)glGenLists((GLsizei)strlen(text));
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(xx, yy);
	glBitmap(0, 0, 0.0, 0.0, 0.0, 5.0, NULL);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
}

