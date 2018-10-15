/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
正方形領域のCavity
スタガード格子のFS法（Fractional Step Method)
圧力のPoisson方程式と速度の輸送方程式を交互に計算
速度の輸送方程式にCIPを使用
*/
#include <windows.h>
#include <GL/glut.h>
#include <GL/glui.h>//GLUIを使用する
#include <math.h>
#include "../../support2D.h"
#include "../../myMath6.h"
#include "../../particle2D.h"
#include "myGLUI.h"

//関数のプロトタイプ宣言
void idle();
void display();
void drawRegion();
void resize(int w, int h);
void calculate(float dt);
void methodCIP(float f[][NUM_MAX],  float gx[][NUM_MAX], float gy[][NUM_MAX], float vx[][NUM_MAX], float vy[][NUM_MAX]);
void drawContour(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawPressureC();
void drawPressureI(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawPsi(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Prs[NUM_MAX][NUM_MAX];//圧力
float velX[NUM_MAX][NUM_MAX];//staggered格子点のx速度
float velY[NUM_MAX][NUM_MAX];//staggered格子点のy速度
float velXgx[NUM_MAX][NUM_MAX];//速度微分
float velXgy[NUM_MAX][NUM_MAX];//速度微分
float velYgx[NUM_MAX][NUM_MAX];//速度微分
float velYgy[NUM_MAX][NUM_MAX];//速度微分
Vector2D Vel[NUM_MAX][NUM_MAX];//格子点の速度（表示格子点は圧力と同じ格子点）
float Psi[NUM_MAX][NUM_MAX];//流れ関数（y速度で計算）
float Omg[NUM_MAX][NUM_MAX];//渦度（x,y速度で計算）
Vector2D scale;//Window左下を(-1.0, -1.0)としたデカルト座標に表示するときのスケール調整

int NX, NY;//解析するときの格子数
float DX, DY, A1, A2, A3;
int count;
float Courant, diff_num;
float maxPsi0, minPsi0, maxPrs0, minPrs0, maxOmg0, minOmg0;

Particle2D p[MAX_PARTICLE];

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
	main_window = glutCreateWindow("GL_CavityFS");
	//ウィンドウのサイズ変更
	glutReshapeFunc(resize);
	//表示
	glutDisplayFunc(display);
	//マウス操作
//	glutMouseFunc(mouse);

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
	glEnable(GL_POINT_SMOOTH);
	//表示ウィンドウに文字をレンダリングする準備
	setFontBitmaps();
	
	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//無次元時間
	
	DX = rect.delta.x = rect.size.x / (float)rect.mesh;
	DY = rect.delta.y = rect.size.y / (float)rect.mesh;

	int i, j;
	NX = rect.mesh + 2;//解析の格子点は壁の内部に1つずつ
	NY = NX;//(正方形）
	//step1(初期条件)
	for(j = 0; j <= NY; j++)
		for (i = 0; i <= NX; i++)
		{
			
			Prs[i][j] = 0.0;//圧力
			velX[i][j] = 0.0;//解析用速度
			velY[i][j] = 0.0;//解析用速度
			velXgx[i][j] = 0.0;//CIPで利用する速度の微分
			velXgy[i][j] = 0.0;//
			velYgx[i][j] = 0.0;//
			velYgy[i][j] = 0.0;//
			Vel[i][j] = Vector2D(0.0, 0.0);//表示用速度
			Psi[i][j] = 0.0;//流れ関数
			Omg[i][j] = 0.0;//渦度

		}
		
	float DX2, DY2;
	DX2 = DX * DX;
	DY2 = DY * DY;
	A1 = 0.5 * DY2 / (DX2 + DY2);
	A2 = 0.5 * DX2 / (DX2 + DY2);
	A3 = 0.5 * DY2 / (1.0 + DY2 / DX2);

	count = 0;//タイムステップ数

	countP = 0;//粒子個数のカウント
	for(i = 0; i < MAX_PARTICLE; i++) p[i] = Particle2D();
	//粒子初期配置
	for(j = 1; j < rect.mesh; j++)
		for (i = 1; i < rect.mesh; i++)
		{
			p[countP].pos = Vector2D((float)i * rect.delta.x, (float)j * rect.delta.y);
			p[countP].size = sizeParticle;
			p[countP].speedC = 1.0;
			if(i <= rect.mesh/3) p[countP].color = RED;
			else if(i <= 2 * rect.mesh/3)  p[countP].color = GREEN;			
			else  p[countP].color = BLUE;
			countP ++;
			if(countP > MAX_PARTICLE) printf("MAX_PARTICLE を大きくしてください \n");
		}

	maxPrs0 = -1000.0; minPrs0 = 1000.0;
	maxPsi0 = -1000.0; minPsi0 = 1000.0;
	maxOmg0 = -1000.0; minOmg0 = 1000.0;

}

void display(void)
{
	//時間計測
	QueryPerformanceFrequency( &freq );
	QueryPerformanceCounter( &currentTime);
	pTime = (double)((currentTime.QuadPart - lastTime.QuadPart) * 1000000 / freq.QuadPart);
	float dt = pTime / 1000000.0;//[sec]
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

	//計算領域描画
	drawRegion();
	
	if(flagStart && !flagFreeze) 
	{ 
		calculate(deltaT); 
		elapseTime2 += dt;
		elapseTime3 += deltaT;
		count++;
		if(flagStep) flagFreeze = true;
	}

	if( flagPressureColor) drawPressureC(); 
	if( flagPressureIsobar) drawPressureI(Prs, minPrs, maxPrs, BLACK); 
	if( flagStream ) drawContour(Psi, minPsi, maxPsi, BLACK);
	if( flagVorticity ) drawContour(Omg, minOmg, maxOmg, RED);

	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();
	
	if(flagP_Start) drawParticle(deltaT);//粒子アニメーション

	//発散しないための目安を知るため
	float Courant = 1.0 * deltaT / rect.delta.x;	
	float diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.x);//拡散数

	if(flagParameter)
	{
		drawParam("f_rate=%d[fps]", fps0, -0.99, 0.9);
		drawParam("t_step=%2.5f[s]", 1.0/(float)fps0, -0.5, 0.9);
		drawParam("e_time=%3.2f[s]", elapseTime2, 0.0, 0.9);
		drawParam("n_time=%3.3f", elapseTime3, 0.55, 0.9);
		drawParam("Courant=%1.4f", Courant, -0.99, 0.8);
		drawParam("diff_num=%1.4f", diff_num, -0.45, 0.8);
		drawParam("Re=%5.1f", Re, 0.1, 0.8);
	}
	//バッファの切り替え
	glutSwapBuffers();
}

void calculate(float deltaT)
{
	int i, j;
	float error, maxError=0.0;
	float a, b, pp;

	//step2(速度の境界条件）
	//上下
	for (i = 0; i <= NX; i++) {
		velY[i][0] = velY[i][2];
		velY[i][1] = 0.0;
		velX[i][0] = - velX[i][1];

		velX[i][NY-1]= 2.0 - velX[i][NY-2];//上境界の速度を1とする(平均値が1となる)
		velY[i][NY] =  velY[i][NY-2];
		velY[i][NY-1] = 0.0;
	}
	//左右
	for (j = 0; j <= NY; j++) {
		velX[0][j] = velX[2][j];
		velX[1][j] = 0.0;
		velY[0][j] = - velY[1][j];

		velX[NX][j] =   velX[NX-2][j];
		velX[NX-1][j] = 0.0;
		velY[NX-1][j] = -velY[NX-2][j];
	}

	//step3(CIPによる速度輸送方程式)
	float vel[NUM_MAX][NUM_MAX];
	//x方向速度定義点における速度
	for(i = 1; i < NX; i++)
	for(j = 1; j < NY; j++) 
		vel[i][j] = (velY[i-1][j] + velY[i][j] + velY[i-1][j+1] + velY[i][j+1]) / 4.0;

	methodCIP(velX, velXgx, velXgy, velX, vel);
	//y成分
	//y方向速度定義点における速度
	for(i = 1; i < NX; i++)
	for(j = 1; j < NY; j++)
		vel[i][j] = (velX[i][j] + velX[i][j-1] + velX[i+1][j-1] + velX[i+1][j]) / 4.0;
	
	methodCIP(velY, velYgx, velYgy, vel, velY);

	//step4(Poisson方程式の解)
	//Poisson方程式の右辺
	float D[NUM_MAX][NUM_MAX];
	for (j = 1; j < NY-1; j++)
		for (i = 1; i < NX-1; i++)
		{
			a = (velX[i+1][j] - velX[i][j]) / DX;
			b = (velY[i][j+1] - velY[i][j]) / DY;
			D[i][j] = A3 * (a + b) / deltaT;
		}

	//反復法
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		//圧力境界値
		for (j = 1; j < NY; j++) {
			Prs[0][j] = Prs[1][j] - 2.0 * velX[0][j] / (DX * Re);//左端
			Prs[NX-1][j] = Prs[NX-2][j] + 2.0 * velX[NX][j] / (DX * Re);//右端
		}
		for (i = 1; i < NX; i++){
			Prs[i][0] = Prs[i][1] - 2.0 * velY[i][0] / (DY * Re) ;//下端
			Prs[i][NY-1] = Prs[i][NY-2] + 2.0 * velY[i][NY] / (DY * Re);//上端
		}				
	
		for (j = 1; j < NY-1; j++)
			for (i = 1; i < NX-1; i++)
			{
				pp = A1 * (Prs[i+1][j] + Prs[i-1][j]) + A2 * (Prs[i][j+1] + Prs[i][j-1]) - D[i][j];
				error = fabs(pp -  Prs[i][j]);
				if (error > maxError) maxError = error;
				Prs[i][j] = pp;//更新 
			}
		if (maxError < tolerance) break;

		cnt++;
	}
	if(flagParameter) drawParam("cnt=%d", cnt, 0.5, 0.8);

	//step5(スタガード格子点の速度ベクトルの更新）
	for (j = 1; j < NY-1; j++)
		for(i = 2; i < NX-1; i++)
		{	        
			velX[i][j] += - deltaT * (Prs[i][j] - Prs[i-1][j]) / DX;
		}
	for (j = 2; j < NY-1; j++)
		for(i = 1; i < NX-1; i++)
		{
			velY[i][j] += - deltaT * (Prs[i][j] - Prs[i][j-1]) / DY;
		}

	//表示のための速度は圧力と同じ位置で
	for(j = 1; j <= NY-2; j++)
		for(i = 1; i <= NX-2; i++)
		{
			Vel[i][j].x = (velX[i][j] + velX[i+1][j]) / 2.0;
			Vel[i][j].y = (velY[i][j] + velY[i][j+1]) / 2.0;
		}

	//Psi
	for(j = 0; j < NY-1; j++)
	{
		Psi[0][j] = 0.0;
		for (i = 1; i <= NX-1; i++)
			Psi[i][j] = Psi[i-1][j] - DX * (velY[i-1][j] + velY[i][j]) / 2.0;
	}
	//Omega
	for(i = 1; i <= NX-1; i++)
		for (j = 1; j <= NY-1; j++) 
		{
			Omg[i][j] = 0.5 * ((Vel[i+1][j].y - Vel[i-1][j].y) / DX - (Vel[i][j+1].x - Vel[i][j-1].x) / DY);
		}

	//流れ関数，圧力の最大値・最小値
	for(i = 1; i < NX-1; i++)
		for (j = 1; j < NY-1; j++)
		{
			if(Psi[i][j] > maxPsi0) maxPsi0 = Psi[i][j];
			if(Psi[i][j] < minPsi0) minPsi0 = Psi[i][j];
			if(Prs[i][j] > maxPrs0) maxPrs0 = Prs[i][j];
			if(Prs[i][j] < minPrs0) minPrs0 = Prs[i][j];
			if(Omg[i][j] > maxOmg0) maxOmg0 = Omg[i][j];
			if(Omg[i][j] < minOmg0) minOmg0 = Omg[i][j];
		}

//printf("maxPrs=%f minPrs=%f \n", maxPrs0, minPrs0);
//printf("maxPsi=%f minPsi=%f \n", maxPsi0, minPsi0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);
}

void methodCIP(float f[][NUM_MAX],  float gx[][NUM_MAX], float gy[][NUM_MAX], float vx[][NUM_MAX], float vy[][NUM_MAX])
{
	float newF[NUM_MAX][NUM_MAX];//関数
	float newGx[NUM_MAX][NUM_MAX];//x方向微分
	float newGy[NUM_MAX][NUM_MAX];//y方向微分
	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dy, dx2, dy2, dx3, dy3; 

	int i, j, ip, jp;
	for(i = 1; i < NX; i++)
		for(j = 1; j < NY; j++)
		{
			if(vx[i][j] >= 0.0) sx = 1.0; else sx = -1.0;
			if(vy[i][j] >= 0.0) sy = 1.0; else sy = -1.0;

			x = - vx[i][j] * deltaT;
			y = - vy[i][j] * deltaT;
			ip = i - (int)sx;//上流点
			jp = j - (int)sy;
			dx = sx * rect.delta.x;
			dy = sy * rect.delta.y;
			dx2 = dx * dx;
			dy2 = dy * dy;
			dx3 = dx2 * dx;
			dy3 = dy2 * dy;

			c30 = ((gx[ip][j] + gx[i][j]) * dx - 2.0 * (f[i][j] - f[ip][j])) / dx3;
			c20 = (3.0 * (f[ip][j] - f[i][j]) + (gx[ip][j] + 2.0 * gx[i][j]) * dx) / dx2;
			c03 = ((gy[i][jp] + gy[i][j]) * dy - 2.0 * (f[i][j] - f[i][jp])) / dy3;
			c02 = (3.0 * (f[i][jp] - f[i][j]) + (gy[i][jp] + 2.0 * gy[i][j]) * dy) / dy2;
			a = f[i][j] - f[i][jp] - f[ip][j] + f[ip][jp];
			b = gy[ip][j] - gy[i][j];
			c12 = (-a - b * dy) / (dx * dy2);
			c21 = (-a - (gx[i][jp] - gx[i][j]) * dx) / (dx2*dy);
			c11 = -b / dx + c21 * dx;

			newF[i][j] = f[i][j] + ((c30 * x + c21 * y + c20) * x + c11 * y + gx[i][j]) * x
				       + ((c03 * y + c12 * x + c02) * y + gy[i][j]) * y;

			newGx[i][j] = gx[i][j] + (3.0 * c30 * x + 2.0 * (c21 * y + c20)) * x + (c12 * y + c11) * y;
			newGy[i][j] = gy[i][j] + (3.0 * c03 * y + 2.0 * (c12 * x + c02)) * y + (c21 * x + c11) * x;
	
			//粘性項に中央差分
			newF[i][j] += deltaT * ( (f[i-1][j] + f[i+1][j] - 2.0 * f[i][j]) / dx2 
				               + (f[i][j-1] + f[i][j+1] - 2.0 * f[i][j]) / dy2 ) / Re;
		}

	//更新
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			f[i][j] = newF[i][j];
			gx[i][j] = newGx[i][j];
			gy[i][j] = newGy[i][j];
		}
}


void drawRegion()
{
	//Cavity
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

	scale.x = 2.0 * rect.scale * s1 * 0.8;
	scale.y = 2.0 * rect.scale * s2 * 0.8;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//壁境界
	glLineWidth(2.0);//線幅
	//色
	glColor3f(0.0, 0.0, 0.0);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y + sy);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y + sy);
	glEnd();
	//圧力境界
	glLineWidth(1.0);//線幅
	float sxp = scale.x * (rect.size.x + DX) / 2.0;
	float syp = scale.y * (rect.size.y + DY) / 2.0;
	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sxp, rect.pos0.y - syp);
	glVertex2f(rect.pos0.x + sxp, rect.pos0.y - syp);
	glVertex2f(rect.pos0.x + sxp, rect.pos0.y + syp);
	glVertex2f(rect.pos0.x - sxp, rect.pos0.y + syp);
	glEnd();

	//左下基準点
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;
}

void resize(int w, int h)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport(tx, ty, tw, th);

	//ビューポート変換
	//glViewport(0, 0, w, h);
	//表示ウィンドウのサイズ
	win_width = tw;//w;
	win_height = th;//h;
	win_ratio = (float)win_height / (float)win_width;

	glutPostRedisplay();
}

void drawContour(float PP[][NUM_MAX],  float minP, float maxP, Color color)
{
	glLineWidth(2.0);
	if(color == BLACK) glColor3f(0.0, 0.0, 0.0);
	else if(color == RED) glColor3f(1.0, 0.0, 0.0);
	else if(color == GREEN) glColor3f(0.0, 1.0, 0.0);
	else if(color == BLUE) glColor3f(0.0, 0.0, 1.0);
	else if(color == DARK_RED) glColor3f(0.5, 0.0, 0.0);
	else if(color == DARK_GREEN) glColor3f(0.0, 0.5, 0.0);
	else if(color == DARK_BLUE) glColor3f(0.0, 0.0, 0.5);
	else if(color == YELLOW) glColor3f(1.0, 1.0, 0.0);
	else if(color == CYAN) glColor3f(0.0, 1.0, 1.0);
	else if(color == MAGENTA) glColor3f(1.0, 0.0, 1.0);

	float dp0 = (maxP - minP) / (float)(nLine +1);//流線間隔
	float pp;

	float x1, y1, x2, y2;
	float p[6], x[6], y[6];
	int i, j, k, m, j0 = 0;

	//三角形セルに分割
	for (k = 0; k < nLine; k++)
	{
		pp = minP + (double)(k + 1) * dp0;

		for(i = 1; i <= NX-2; i++)
		{//(スタガード格子ではi=1,j=1が壁面）
			for(j = 1; j <= NY-2; j++)
			{//三角形セルに分割
				p[0] = PP[i][j];     x[0] = (float)i * rect.delta.x;     y[0] = (float)j * rect.delta.y;
				p[1] = PP[i][j+1];   x[1] = (float)i * rect.delta.x;     y[1] = (float)(j+1) * rect.delta.y;
				p[2] = PP[i+1][j+1]; x[2] = (float)(i+1) * rect.delta.x; y[2] = (float)(j+1) * rect.delta.y;
				p[3] = PP[i+1][j];   x[3] = (float)(i+1) * rect.delta.x; y[3] = (float)j * rect.delta.y;
				p[4] = p[0]; x[4] = x[0]; y[4] = y[0];//0番目に同じ
				//中心
				p[5] = (p[0] + p[1] + p[2] + p[3]) / 4.0;
				x[5] = (x[0] + x[1] + x[2] + x[3]) / 4.0;
				y[5] = (y[0] + y[1] + y[2] + y[3]) / 4.0;

				for(m = 0; m < 4; m++)//各三角形について
				{
					x1 = -1.0; y1 = -1.0; 
					
					if((p[m] <= pp && pp < p[m+1]) || (p[m] >= pp && pp > p[m+1]))
					{
						x1 = x[m] + (x[m+1] - x[m]) * (pp - p[m]) / (p[m+1] - p[m]);
						y1 = y[m] + (y[m+1] - y[m]) * (pp - p[m]) / (p[m+1] - p[m]);
					}
					if((p[m] <= pp && pp < p[5]) || (p[m] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//まだ交点なし
						{
							x1 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y1 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
						}
						else//x1は見つかった
						{
							x2 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y2 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
							goto draw_line;
						}
						
					}
					if((p[m+1] <= pp && pp < p[5]) || (p[m+1] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//まだ交点なし
						{
							x1 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y1 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
						else//x1は見つかった
						{
							x2 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y2 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
					}

					if(x1 < 0.0) continue;
draw_line:;
					glBegin(GL_LINES);
					glVertex2f(rect.left0.x + scale.x * (x1 - rect.delta.x), rect.left0.y + scale.y * (y1 - rect.delta.y));
					glVertex2f(rect.left0.x + scale.x * (x2 - rect.delta.x), rect.left0.y + scale.y * (y2 - rect.delta.y));
					glEnd();
				}//m
			}//j
		}//i
	}//k
}

void drawPressureI(float PP[][NUM_MAX], float minP, float maxP, Color color)
{
	glLineWidth(2.0);
	if(color == BLACK) glColor3f(0.0, 0.0, 0.0);
	else if(color == RED) glColor3f(1.0, 0.0, 0.0);
	else if(color == GREEN) glColor3f(0.0, 1.0, 0.0);
	else if(color == BLUE) glColor3f(0.0, 0.0, 1.0);
	else if(color == DARK_RED) glColor3f(0.5, 0.0, 0.0);
	else if(color == DARK_GREEN) glColor3f(0.0, 0.5, 0.0);
	else if(color == DARK_BLUE) glColor3f(0.0, 0.0, 0.5);
	else if(color == YELLOW) glColor3f(1.0, 1.0, 0.0);
	else if(color == CYAN) glColor3f(0.0, 1.0, 1.0);
	else if(color == MAGENTA) glColor3f(1.0, 0.0, 1.0);

	float dp0 = (maxP - minP) / (float)(nLine +1);//流線間隔
	float pp;

	float x1, y1, x2, y2;
	float p[6], x[6], y[6];
	int i, j, k, m, j0 = 0;

	//三角形セルに分割
	for (k = 0; k < nLine; k++)
	{
		pp = minP + (double)(k + 1) * dp0;

		for(i = 0; i <= NX-2; i++)
		{
			for(j = 0; j <= NY-2; j++)
			{//三角形セルに分割
				p[0] = PP[i][j];     x[0] = (float)i * rect.delta.x;     y[0] = (float)j * rect.delta.y;
				p[1] = PP[i][j+1];   x[1] = (float)i * rect.delta.x;     y[1] = (float)(j+1) * rect.delta.y;
				p[2] = PP[i+1][j+1]; x[2] = (float)(i+1) * rect.delta.x; y[2] = (float)(j+1) * rect.delta.y;
				p[3] = PP[i+1][j];   x[3] = (float)(i+1) * rect.delta.x; y[3] = (float)j * rect.delta.y;
				p[4] = p[0]; x[4] = x[0]; y[4] = y[0];//0番目に同じ
				//中心
				p[5] = (p[0] + p[1] + p[2] + p[3]) / 4.0;
				x[5] = (x[0] + x[1] + x[2] + x[3]) / 4.0;
				y[5] = (y[0] + y[1] + y[2] + y[3]) / 4.0;

				for(m = 0; m < 4; m++)//各三角形について
				{
					x1 = -1.0; y1 = -1.0; 
					
					if((p[m] <= pp && pp < p[m+1]) || (p[m] >= pp && pp > p[m+1]))
					{
						x1 = x[m] + (x[m+1] - x[m]) * (pp - p[m]) / (p[m+1] - p[m]);
						y1 = y[m] + (y[m+1] - y[m]) * (pp - p[m]) / (p[m+1] - p[m]);
					}
					if((p[m] <= pp && pp < p[5]) || (p[m] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//まだ交点なし
						{
							x1 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y1 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
						}
						else//x1は見つかった
						{
							x2 = x[m] + (x[5] - x[m]) * (pp - p[m]) / (p[5] - p[m]);
							y2 = y[m] + (y[5] - y[m]) * (pp - p[m]) / (p[5] - p[m]);
							goto draw_line;
						}
						
					}
					if((p[m+1] <= pp && pp < p[5]) || (p[m+1] >= pp && pp > p[5]))
					{
						if(x1 < 0.0)//まだ交点なし
						{
							x1 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y1 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
						else//x1は見つかった
						{
							x2 = x[m+1] + (x[5] - x[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
							y2 = y[m+1] + (y[5] - y[m+1]) * (pp - p[m+1]) / (p[5] - p[m+1]);
						}
					}

					if(x1 < 0.0) continue;
draw_line:;
					glBegin(GL_LINES);
					glVertex2f(rect.left0.x + scale.x * (x1 - rect.delta.x/2.0), rect.left0.y + scale.y * (y1 - rect.delta.y/2.0));
					glVertex2f(rect.left0.x + scale.x * (x2 - rect.delta.x/2.0), rect.left0.y + scale.y * (y2 - rect.delta.y/2.0));
					glEnd();
				}//m
			}//j
		}//i
	}//k
}

void drawPressureC()
{
	float pressW = maxPrs -minPrs;//表示圧力最大幅
	glPolygonMode(GL_FRONT, GL_FILL);

	float x1, y1, x2, y2, x3, y3, x4, y4;
	float pp[4], rr[4], gg[4], bb[4];
	int i, j, k;

    for (i = 0; i <= NX-2; i++)
    {
        for (j = 0; j <= NY-2; j++)
        {

			x1 = (float)i * rect.delta.x;     y1 = (float)j * rect.delta.y;
			x2 = (float)(i+1) * rect.delta.x; y2 = (float)j * rect.delta.y;
			x3 = (float)(i+1) * rect.delta.x; y3 = (float)(j+1) * rect.delta.y;
			x4 = (float)i * rect.delta.x;     y4 = (float)(j+1) * rect.delta.y;
			
			pp[0] = (Prs[i][j] - minPrs) / pressW; 
			pp[1] = (Prs[i+1][j] - minPrs) / pressW;
			pp[2] = (Prs[i+1][j+1] - minPrs) / pressW; 
			pp[3] = (Prs[i][j+1] - minPrs) / pressW;
/*
			for(int k = 0; k < 4; k++)
			{
				if(pp[k] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 2.0 * pp[k]; bb[k] = 1.0 - 2.0 * pp[k];
				}
				else
				{
					rr[k] = 2.0 * pp[k] - 1.0; gg[k] = 2.0 * (1.0 - pp[k]); bb[k] = 0.0;
				}
			}

*/
			for(k = 0; k < 4; k++)
			{

				if(pp[k] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * pp[k]; bb[k] = 1.0;
				}
				else if(pp[k] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - pp[k]);
				}
				else if(pp[k] < 0.75)
				{
					rr[k] = 4.0 * (pp[k] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - pp[k]) * 4.0; bb[k] = 0.0;
				}

			}

			glBegin(GL_QUADS);
			glColor3f(rr[0], gg[0], bb[0]);
			glVertex2f(rect.left0.x + scale.x * (x1 - rect.delta.x/2.0), rect.left0.y + scale.y * (y1 - rect.delta.y/2.0));
			glColor3f(rr[1], gg[1], bb[1]);
			glVertex2f(rect.left0.x + scale.x * (x2 - rect.delta.x/2.0), rect.left0.y + scale.y * (y2 - rect.delta.y/2.0));
			glColor3f(rr[2], gg[2], bb[2]);
			glVertex2f(rect.left0.x + scale.x * (x3 - rect.delta.x/2.0), rect.left0.y + scale.y * (y3 - rect.delta.y/2.0));
			glColor3f(rr[3], gg[3], bb[3]);
			glVertex2f(rect.left0.x + scale.x * (x4 - rect.delta.x/2.0), rect.left0.y + scale.y * (y4 - rect.delta.y/2.0));
			glEnd();
        }
    }
    
}


void drawVelocity()
{
	int i, j;

	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
	//描画(スタガード格子ではi=1,j=1は速度格子線の壁面）
	float theta, mag;
	for(i = 1; i <= NX-2; i++)
		for (j = 1; j <= NY-2; j++)
		{
			//間引き表示
			if((i/thinningV) * thinningV != i) continue;
			if((j/thinningV) * thinningV != j) continue;

			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;// 180.0 / M_PI;
			mag = Vel[i][j].magnitude();
			glPushMatrix();
			glTranslatef(rect.left0.x + scale.x * ((float)i-0.5) * rect.delta.x, rect.left0.y + scale.y *  ((float)j-0.5) * rect.delta.y, 0.0);
			//glTranslatef(rect.left0.x + scale.x * ((float)i) * rect.delta.x, rect.left0.y + scale.y *  ((float)j) * rect.delta.y, 0.0);
			glRotatef(theta, 0.0, 0.0, 1.0);
			glScalef(scaleArrow * mag, scaleArrow * mag, 0.0);
			drawArrow();
			glPopMatrix();
		}
}
void drawArrow()
{
	glLineWidth(widthArrow);
	glColor3f(0.0, 0.0, 0.0);

	glBegin(GL_LINES);
	glVertex2f(-0.5, 0.0);
	glVertex2f( 0.5, 0.0);

	glVertex2f( 0.0, 0.2);
	glVertex2f( 0.5, 0.0);

	glVertex2f( 0.0,-0.2);
	glVertex2f( 0.5, 0.0);
	glEnd();
}

void drawGrid()
{
	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 1.0);

	int i, j;
	
	for(i = 1; i < rect.mesh; i++)//このi=0は速度ｘ格子線のI=1に相当
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * (float)rect.mesh * rect.delta.y);
		glEnd();
	}
	for(j = 1; j < rect.mesh; j++)//j=0は速度ｙ格子線のJ=1に相当
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * (float)rect.mesh * rect.delta.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glEnd();
	}
}

void drawParticle(float dt)
{
	int k, kk;

	for(k = 0; k < countP; k++)
	{
		p[k].vel = getVelocityParticle(p[k].pos);

		if(!flagFreeze) p[k].update(dt);

		if(p[k].pos.x >= 0.0 && p[k].pos.x < rect.size.x 
			&& p[k].pos.y >= 0.0 && p[k].pos.y < rect.size.y) p[k].show(rect.left0, scale);

	}

}

Vector2D getVelocityParticle(Vector2D pos)
{
	
	if( pos.x < 0.0 ) return Vector2D(-10.0, -10.0);
	if( pos.x > rect.size.x ) return Vector2D( 10.0, 10.0);
	if( pos.y <  0.0 ) return Vector2D(-10.0, -10.0);
	if( pos.y > rect.size.y ) return Vector2D( 10.0, 10.0);
	//格子番号を取得
	int I = 0, J = 0;
	for(int i = 0; i < rect.mesh; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.mesh; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//格子点の速度を線形補間
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
