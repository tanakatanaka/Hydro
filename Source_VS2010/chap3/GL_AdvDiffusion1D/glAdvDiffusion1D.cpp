/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

1次元拡散方程式
1次精度風上差分とCIPの比較
*/
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include "../../support2D.h"
#include "../../myMath6.h"
#include "../../simultaneous.h"
#include "myGLUI.h"

//関数のプロトタイプ宣言
void idle();
void display();
void drawRegion();
void resize(int w, int h);
void intWave();
void upwind(float dt, float c, float d);
void methodCIP(float dt, float d);
void methodImplicit(float dt, float c, float d);
void drawProfile();
void drawTimeSeries();
void setFontBitmaps();
void drawParam(const char *character, int param, float xx, float yy);
void drawParam(const char *character, float param, float xx, float yy);
void drawParam(const char *character, double param, float xx, float yy);

//格子間隔
float deltaX;//Δx

#define NUM_MAX  501
float f0[NUM_MAX];//物理量（温度，濃度 etc.）計算前
float f1[NUM_MAX];//物理量（温度，濃度 etc.）計算後
float g0[NUM_MAX];//微分値（CIP法で使用）計算前
float g1[NUM_MAX];//微分値（CIP法で使用）計算後
float f_t[4][NUM_MAX][10];//時系列データ
float time[] = {0.0, 2.0, 4.0, 6.0, 8.0};//その時刻
int mark[4][10];
int nTime = 5;
Vector2D scale;
int nMesh, nWidth, nCenter;
int count = 0;//厳密解を描画する際に，丁度格子間隔を通過したときだけデータを更新するためのカウンタ
float tt = 0;//厳密解を描画するときの経過時間
float hh = 0.3;//1つのプロファイルの表示枠の高さ
Sle sle = Sle();//Sleは連立1次方程式のクラス(simultaneous.hに実装)

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//表示モード
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	//表示ウィンドウのサイズ
	glutInitWindowSize(win_width, win_height);
	//左上の位置
	glutInitWindowPosition(win_x0, win_y0);
	//ウィンドウ作成
	main_window = glutCreateWindow("GL_AdvDiffusion1D");//GLUIが対象とするウィンドウ
	//ウィンドウのサイズ変更
	glutReshapeFunc(resize);
	//表示
	glutDisplayFunc(display);

	//初期設定
	init();
	setupGLUI();//myGLUI.hに実装

	GLUI_Master.set_glutIdleFunc( idle );
	//イベント処理ループに入る
	glutMainLoop();
	return 0;
}
void idle(void)
{
	//Redisplayメッセージがmai_windowになるようにセット
	if(glutGetWindow() != main_window ) glutSetWindow(main_window);  
	//再描画
	glutPostRedisplay();
}

void init(void)
{
	//背景色
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	//表示ウィンドウに文字をレンダリングする準備
	setFontBitmaps();

	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start後の総経過時間

	int i, j, k;
	nMesh = rect.nMesh;
	deltaX = rect.size.x / (float)nMesh;//解析領域は[0, 1]に固定(rect.size.xの単位を仮に[m]とする）
	nWidth = widthInit / deltaX;
	nCenter = centerInit / deltaX;

	if(flagReset == 1)
	{
		for(k = 0; k < nTime; k++)
		{
			for(int i = 0; i <= nMesh ; i++)
			{
				for(j = 0; j < 4; j++) 
				{
					f_t[j][i][k] = 0.0;
					mark[j][k] = 0;
				}
			}
		}
	}

	//初期値
	//f[0] = 0.0;
	for (i = 0; i <= nMesh; i++)
	{
		if(i < nCenter - nWidth/ 2) f0[i] = 0.0;
		else if(i <= nCenter + nWidth / 2) f0[i] = 1.0;
		else  f0[i] = 0.0;
		
		for(j = 0; j < 4; j++)f_t[j][i][0] = f1[i] = f0[i];//初期値（時系列データ）
	}
	//微分の初期値(CIP)
	g0[0] = 0.0; g0[nMesh] = 0.0;
	for (i = 1; i < nMesh; i++)
	{
		g0[i] = g1[i] = (f0[i+1] - f0[i-1]) / (2.0 * deltaX);//0でもほとんど同じ
	}

	for(k = 0; k < nTime; k++)
	{
		for(int i = 0; i <= nMesh ; i++)
			for(j = 0; j < 4; j++) mark[j][k] = 0;//確定済みのとき1
		
		time[k] = (float)k / fabs(speed) / (float)nTime;//speed=0.1のとき2s間隔
	}
	count = 0;
	flagReset = 0;//Risetボタンが押されるまでリセットしない
}

void display(void)
{
	//時間計測
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

	//カラーバッファのクリア
	glClear(GL_COLOR_BUFFER_BIT);
  
	glLoadIdentity();

	//計算領域描画
	drawRegion();

	float deltaT = deltaT0 / (float)thinningN;
	float c = speed * deltaT / deltaX;//クーラン数(speed<0のときcも見かけ上負になる）
	float d = diff_coef * deltaT / (deltaX * deltaX);//拡散数

	if(flagStart && !flagFreeze) 
	{ 
		elapseTime2 += deltaT;
		for(int i = 0; i < thinningN; i++)
		{
			if(method == 0) upwind( deltaT, c, d ); 
			else if(method == 1) methodCIP(deltaT, d);
			else methodImplicit(deltaT, c, d);
		}
	}

	drawProfile();
	drawTimeSeries();

	if(flagParameter)
	{
		drawParam("framerate=%d[fps]", fps0, -0.9, 0.92);
		drawParam("timestep=%1.5f[s]", dt, -0.2, 0.92);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 0.5, 0.92);

		drawParam("deltaX=%f", deltaX, -0.9, 0.85);
		drawParam("deltaT=%1.4f[s]", deltaT, -0.4, 0.85);
		drawParam("Courant=%1.4f", c, 0.1, 0.85);
		drawParam("diffusion=%1.4f", d, 0.6, 0.85);
	}
	//バッファの切り替え
	glutSwapBuffers();
}

void upwind(float deltaT, float cou, float dif)
{
	int im1 = 0, ip1 = 0, im2 = 0, ip2 = 0;
	float fm1 = 0.0, fp1 = 0.0, fm2 = 0.0, fp2 = 0.0;

	//常に中心温度が1となるように設定
	if(flagConstant)
	{
		for (int i = 0; i <= nMesh; i++)
		{
			if(i >= nCenter - nWidth/2 && i <= nCenter + nWidth/2) f0[i] = 1.0;
		}
	}

	for(int i = 0 ; i <= nMesh; i++)
	{		
		if(boundary==0) { f0[0] = 0.0; f0[nMesh] = 0.0;}
		else if(boundary == 1) { f0[0] = f0[1]; f0[nMesh] = f0[nMesh-1];}
	
		im1 = i-1; ip1 = i+1;
		if(im1 < 0) fm1 = 0.0; else fm1 = f0[im1];
		if(ip1 > nMesh) fp1 = 0.0; else fp1 = f0[ip1];

		//時間に前進差分，空間の移流項に後退（上流）差分
		f1[i] = f0[i] + 0.5 * ( cou * (fm1 - fp1) + fabs(cou) * (fp1 + fm1 - 2.0 * f0[i]));
		//拡散
		f1[i] += dif * (fm1 - 2.0 * f0[i] + fp1);
//		if(boundary==0) { f1[0] = 0.0; f1[nMesh] = 0.0;}
//		else if(boundary == 1) { f1[0] = f1[1]; f1[nMesh] = f1[nMesh-1];}
	}
	for(int i = 0 ; i <= nMesh; i++) f0[i] = f1[i];//次のステップで使うデータ


	//時系列データの保存
	for(int k = 1; k < nTime; k++)
	{
		if(mark[method][k] == 1) continue;
		if(elapseTime2 >= time[k] / (float)thinningN)
		{
			for(int i = 0; i <= nMesh; i++) f_t[method][i][k] = f0[i];
			mark[method][k] = 1;
		}
	}
}

void methodCIP(float deltaT, float dif)
{
	if(flagConstant)
	{
		for (int i = 0; i <= nMesh; i++)
		{
			if(i >= nCenter - nWidth/2 && i <= nCenter + nWidth/2) f0[i] = 1.0;
		}
	}

	int ip, im;
	float Fp, Gp, fm1, fp1;
	float c3, c2, x;
	float s;

	if( speed > 0.0) s = 1.0;
	else s = -1.0;
	for(int i = 1; i < nMesh; i++)
	{
		ip = i-(int)s;//上流点
		
		if(ip < 0.0 || ip > nMesh){
			Fp = 0.0;
			Gp = 0.0;
		}
		else{ 
			Fp = f0[ip];
			Gp = g0[ip];
		}

		float dx = s * deltaX;
		float dx2 = dx * dx;
		float dx3 = dx2 * dx;

		c3 = (g0[i] + Gp) / dx2 - 2.0 * (f0[i] - Fp) / dx3;
		c2 = 3.0 * (Fp - f0[i]) / dx2 + (2.0 * g0[i] + Gp) / dx;
		x = - speed * deltaT;

		f1[i] = f0[i] + ( (c3 * x + c2) * x + g0[i] ) * x ;
		g1[i] = g0[i] + ( 3.0 * c3 * x + 2.0 * c2 ) * x ;	
		
		//拡散
		im = i-1; ip = i+1;
		if(im < 0.0) fm1 = 0.0; else fm1 = f0[im];
		if(ip > nMesh) fp1 = 0.0; else fp1 = f0[ip];
		f1[i] += dif * (fm1 - 2.0 * f0[i] + fp1);
		
		if(boundary == 0) { f1[0] = 0.0; f1[nMesh] = 0.0;}
		else if(boundary == 1) { f1[0] = f1[1]; f1[nMesh] = f1[nMesh-1];}

	}
	for(int i = 0; i <= nMesh; i++) { f0[i] = f1[i]; g0[i] = g1[i]; }

	//時系列データの保存
	for(int k = 1; k < nTime; k++)
	{
		if(mark[method][k] == 1) continue;
		if(elapseTime2 >= time[k] / (float)thinningN)
		{
			for(int i = 0; i <= nMesh; i++) f_t[method][i][k] = f0[i];
			mark[method][k] = 1;
		}
	}
}

void methodImplicit(float deltaT, float c, float d)
{
	float a[3], A, B, C;
	int im = 0, ip = 0;
	float fm1 = 0.0, fp1 = 0.0;

	//常に中心温度が1となるように設定
	if(flagConstant)
	{
		for (int i = 0; i <= nMesh; i++)
		{
			if(i >= nCenter - nWidth/2 && i <= nCenter + nWidth/2) f0[i] = 1.0;
		}
	}

	if(boundary==0) { f0[0] = 0.0; f0[nMesh] = 0.0;}//Dirichlet
	else if(boundary == 1) { f0[0] = f0[1]; f0[nMesh] = f0[nMesh-1];}//Neumann

	//1次風上差分
	if(method == 2)//半陰解法（Crank-Nicolson)
	{
		a[0] = -0.5 * ((c + fabs(c))/2.0 + d);
		a[1] = 1.0 + fabs(c)/2.0 + d;
		a[2] = 0.5 * ((c - fabs(c))/2.0 - d);
		A = 0.5 * ((c + fabs(c))/2.0 + d) ;
		B = 1.0 - fabs(c)/2.0 - d;
		C =  - 0.5 * ((c - fabs(c))/2.0 - d);
		f1[0] =  B * f0[0] + C * f0[1];
		f1[nMesh] = B * f0[nMesh] + A * f0[nMesh - 1];
		
		for(int i = 0 ; i <= nMesh; i++) {
			im = i-1; ip = i+1;
			if(im < 0) fm1 = 0.0; else fm1 = f0[im];
			if(ip > nMesh) fp1 = 0.0; else fp1 = f0[ip];

			f1[i] = A * fm1 + B * f0[i] + C * fp1;
		}
		sle.Thomas(a, f1, nMesh, boundary);
		for(int i = 0 ; i <= nMesh; i++) f0[i] = f1[i];
	}

	else if(method == 3)//純陰解法
	{
		a[0] = -(c + fabs(c)) / 2.0 - d;
		a[1] = 1.0 + fabs(c) + 2.0 * d;
		a[2] = (c- fabs(c)) / 2.0 - d;

		sle.Thomas(a, f0, nMesh, boundary);
	}
	//時系列データの保存
	for(int k = 1; k < nTime; k++)
	{
		if(mark[method][k] == 1) continue;
		if(elapseTime2 >= time[k] / (float)thinningN)
		{
			for(int i = 0; i <= nMesh; i++) f_t[method][i][k] = f0[i];
			mark[method][k] = 1;
		}
	}
}

void drawRegion()
{
	//全体枠
	//線幅
	glLineWidth(1.0);
	//色
	glColor3f(0.0, 0.0, 0.0);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	scale.x = 2.0 * rect.scale * s1 ;
	scale.y = 1.7 * rect.scale * s2 ;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//左下基準点
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;

	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glEnd();

	hh = rect.size.y / 4.0;//1つ当たりの表示枠の高さ
	int i;
	//各段の横軸
	for(i = 0; i <= 3; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x - 0.04 , rect.left0.y + (float)i * hh * scale.y );
		glVertex2f(rect.left0.x + rect.size.x * scale.x, rect.left0.y + (float)i * hh * scale.y );
		glEnd();
	}
	//横軸5分割線を縦軸に平行に引く（0.2間隔，2秒間隔）
	for(i = 1; i <= 4; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + (float)i * 0.2 * rect.size.x * scale.x, rect.left0.y);
		glVertex2f(rect.left0.x + (float)i * 0.2 * rect.size.x * scale.x, rect.left0.y + rect.size.y * scale.y);
		glEnd();
	}
	//目盛表示
	drawParam("0", 0 , rect.left0.x - 0.01, rect.left0.y - 0.08);
	drawParam("1", 0 , rect.left0.x - 0.01 + rect.size.x * scale.x, rect.left0.y - 0.08);
	drawParam("x", 0 , rect.left0.x + rect.size.x * scale.x + 0.02, rect.left0.y - 0.03);
	//縦軸目盛
	for(i = 0; i <= 4; i++)
	{
		//f = 1
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x,        rect.left0.y + ((float)i * hh + hh * 0.8) * scale.y);
		glVertex2f(rect.left0.x - 0.04, rect.left0.y + ((float)i * hh + hh * 0.8) * scale.y);
		glEnd();
		//f = 0.5
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x,        rect.left0.y + ((float)i * hh + hh * 0.4) * scale.y);
		glVertex2f(rect.left0.x - 0.02, rect.left0.y + ((float)i * hh + hh * 0.4) * scale.y);
		glEnd();
	}
	drawParam("f", 0 , rect.left0.x - 0.03, rect.left0.y  + 4.0 * hh * scale.y -0.05);
	drawParam("upwind", 0 , rect.left0.x + 0.05, rect.left0.y  + 4.0 * hh * scale.y -0.08);
	drawParam("CIP method", 0 , rect.left0.x + 0.05, rect.left0.y  + 3.0 * hh * scale.y -0.08);
	drawParam("semi-implicit", 0 , rect.left0.x + 0.05, rect.left0.y  + 2.0 * hh * scale.y -0.08);
	drawParam("fully-implicit", 0 , rect.left0.x + 0.05, rect.left0.y  + 1.0 * hh * scale.y -0.08);
}

void resize(int w, int h)
{
	//ビューポート変換
	glViewport(0, 0, w, h);

	//表示ウィンドウのサイズ
	win_width = w;
	win_height = h;
	win_ratio = (float)win_height / (float)win_width;
}

void drawProfile()
{
	float HH;
	
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);

	for(int i = 0; i < nMesh; i++)
	{
		HH = (3.0 - (float)method) * hh;
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i* deltaX, rect.left0.y + scale.y * (HH + (float)f0[i] * hh * 0.8));
		glVertex2f(rect.left0.x + scale.x * (float)(i+1) * deltaX , rect.left0.y + scale.y * (HH + (float)f0[i+1] * hh * 0.8));
		glEnd();
	}
}


void drawTimeSeries()
{
	float HH;
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);

	for(int j = 0; j < 4; j++)
	for(int k = 0; k < nTime; k++)
	{
		HH = (3.0 - (float)j) * hh;
		for(int i = 0; i < nMesh; i++)
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x + scale.x * (float)i * deltaX, rect.left0.y + scale.y * (HH + (float)f_t[j][i][k] * hh * 0.8));
			glVertex2f(rect.left0.x + scale.x * (float)(i+1) * deltaX, rect.left0.y + scale.y * (HH + (float)f_t[j][i+1][k] * hh * 0.8));
			glEnd();
		}
	}

}

