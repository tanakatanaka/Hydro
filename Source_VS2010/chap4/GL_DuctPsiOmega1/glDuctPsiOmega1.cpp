/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
障害物が中間にある平行平板ダクト
流れ関数-渦度法
渦度のPoisson方程式と渦度の輸送方程式を交互に計算
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
void drawObstacle();
void resize(int w, int h);
void calculate(float dt);
void methodCIP(float deltaT);
void calcVelocity();
void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Psi[NUM_MAX_X][NUM_MAX_Y];//流れ関数
float Omega[NUM_MAX_X][NUM_MAX_Y];//渦度
float gx[NUM_MAX_X][NUM_MAX_Y];//x方向微分
float gy[NUM_MAX_X][NUM_MAX_Y];//y方向微分

Vector2D Vel[NUM_MAX_X][NUM_MAX_Y];//格子点の速度
Vector2D scale;//Window左下を(-1.0, -1.0)としたデカルト座標に表示するときのスケール調整

float maxPsi0, minPsi0, maxOmg0, minOmg0;

enum Type{INSIDE, INLET, OUTLET, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM, OBS_RIGHT, OBSTACLE, CORNER_UL, CORNER_UR, CORNER_LL, CORNER_LR};
Type type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ
int NX, NY;

Particle2D p[MAX_PARTICLE];
bool flagResetting = false;

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
	main_window = glutCreateWindow("GL_DuctPsiOmega1");
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
	glEnable(GL_POINT_SMOOTH);
	//表示ウィンドウに文字をレンダリングする準備
	setFontBitmaps();
	
	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元）
	
	NX = rect.nMeshX;
	NY = rect.nMeshY;
	rect.delta.x = rect.size.x / (float)NX;
	rect.delta.y = rect.size.y / (float)NY;
	
	//float obsTop, obsBottom;
	//obsTop = rect.size.y/2.0 + rect.obs_width/2.0;
	//obsBottom = rect.size.y/2.0 - rect.obs_width/2.0;
	//障害物境界の分割点番号
	float eps = 0.5;
	int nMeshX1 = floor((rect.obs_left - rect.obs_thick/2.0) / rect.delta.x + eps);//障害物左端位置
	int nMeshX2 = floor((rect.obs_left + rect.obs_thick/2.0) / rect.delta.x + eps);//障害物右端位置
	int nMeshY2 = floor((rect.size.y + rect.obs_width) / (2.0 * rect.delta.y) + eps);//障害物上端位置
	int nMeshY1 = floor((rect.size.y - rect.obs_width) / (2.0 * rect.delta.y) + eps);//障害物下端位置

	int nMeshY_ObsW = floor(rect.obs_width / rect.delta.y + eps);
	//格子間隔の整数倍か
	float left1, left2, bottom1, bottom2;
	left1 = rect.delta.x * (float)(nMeshX1);//左端から障害物左端まで
	left2 = rect.obs_left - rect.obs_thick/2.0;
	bottom1 = rect.delta.y * (float)nMeshY1;
	bottom2 = (rect.size.y - rect.obs_width)/2.0;
	printf("left1=%f, left2=%f, bottom1=%f, bottom2=%f \n", left1, left2, bottom1, bottom2);
	eps = 0.00001;
	if( fabs(left1 - left2) > eps || fabs(bottom1 - bottom2) > eps )
	{
		printf("障害物の位置，サイズを設定しなおせ! \n");
		flagResetting = true;
	}
	else flagResetting = false;

	//格子点のタイプ
	int i, j;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			type[i][j] = INSIDE;//内点
			if(j == 0) type[i][j] = BOTTOM;//下側壁面
			if(j == rect.nMeshY) type[i][j] = TOP;//上側壁面
			if(i == 0) type[i][j] = INLET;//流入端
			if(i == rect.nMeshX) type[i][j] = OUTLET;//流出端
			if(i == nMeshX1 && j > nMeshY1 && j < nMeshY2) type[i][j] = OBS_LEFT;//障害物左端
			if(i == nMeshX2 && j > nMeshY1 && j < nMeshY2) type[i][j] = OBS_RIGHT;//障害物右端
			if(i >= nMeshX1 && i <= nMeshX2 && j == nMeshY2) type[i][j] = OBS_TOP;//障害物上端
			if(i >= nMeshX1 && i <= nMeshX2 && j == nMeshY1) type[i][j] = OBS_BOTTOM;//障害物下端
			if(i > nMeshX1 && i < nMeshX2 && j > nMeshY1 && j < nMeshY2) type[i][j] = OBSTACLE;//障害物内部
			if(i == nMeshX1 && j == nMeshY1) type[i][j] = CORNER_LL;
			if(i == nMeshX1 && j == nMeshY2) type[i][j] = CORNER_UL;
			if(i == nMeshX2 && j == nMeshY1) type[i][j] = CORNER_LR;
			if(i == nMeshX2 && j == nMeshY2)  type[i][j] = CORNER_UR;
		}

	//初期値
	for (i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			//流れ関数
			Psi[i][j] = (float)j * rect.delta.y;;
			//渦度
			Omega[i][j] = 0.0;
			//速度
			Vel[i][j].x = 1.0;
			if(type[i][j] == BOTTOM || type[i][j] == TOP || type[i][j] >= OBS_LEFT) Vel[i][j].x = 0.0;
			Vel[i][j].y = 0.0;
			gx[i][j] = 0.0;
			gy[i][j] = 0.0;
		}
	//境界，初期条件
	//左端入口/出口は流速1(psi = y)
	//下の壁は psi = 0，上の壁は psi = rect.size.y
	for (i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			if(type[i][j] == BOTTOM) Psi[i][j] = 0.0;
			else if(type[i][j] == TOP) Psi[i][j] = rect.size.y;
			else if(type[i][j] == INLET || type[i][j] == OUTLET) Psi[i][j] = (float)j * rect.delta.y;
			else if(type[i][j] >= OBS_LEFT) Psi[i][j] = rect.size.y / 2.0;//障害物表面
		}

	calcVelocity();

	maxPsi0 = -1000.0; minPsi0 = 1000.0;
	maxOmg0 = -1000.0; minOmg0 = 1000.0;

	countP = 0;
	for(i = 0; i < MAX_PARTICLE; i++) p[i] = Particle2D();

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
		elapseTime2 += dt;
		elapseTime3 += deltaT ;//無次元
		calculate(deltaT); 
		
		if(flagStep) flagFreeze = true;
	}

	if( flagStream ) drawContour(Psi, minPsi, maxPsi, BLACK);
	if( flagVorticity ) drawContour(Omega, minOmg, maxOmg, RED);

	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();

	if(flagP_Start) drawParticle(deltaT);//粒子アニメーション

	drawObstacle();
	//発散しないための目安を知るため
	float Courant, diff_num;
	if(rect.delta.x < rect.delta.y)
	{
		Courant = deltaT / rect.delta.x;	
		diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.x);//拡散数
	}
	else
	{
		Courant = deltaT / rect.delta.y;	
		diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.y);//拡散数
	}

	if(flagParameter)
	{
		drawParam("f_rate=%d[fps]", fps0, -0.99, 0.9);
		drawParam("t_step=%2.5f[s]", dt, -0.6, 0.9);
		drawParam("e_time=%3.3f[s]", elapseTime2, -0.2, 0.9);
		drawParam("deltaT=%1.3f", deltaT, 0.2, 0.9);
		drawParam("n_time=%3.3f", elapseTime3, 0.5, 0.9);
		drawParam("Courant=%1.4f", Courant, -0.99, 0.8);
		drawParam("diff_num=%1.4f", diff_num, -0.6, 0.8);
		drawParam("Re=%5.1f", Re, -0.1, 0.8);
	}
	if(flagResetting) drawParam("Reset obstacle size",  -0.5, 0.0);
	//バッファの切り替え
	glutSwapBuffers();
}

void calculate(float deltaT)
{
	int i, j;
	float error = 0.0;
	float maxError = 0.0;
	float pp;
	float DX = rect.delta.x;
	float DY = rect.delta.y;
	float DX2 = DX * DX;
	float DY2 = DY * DY;
	float DD2 = DX2 + DY2;//corner

	//渦度境界条件
	float psi_obs = rect.size.y/2.0;
	for (i = 0; i <= NX; i++)
		for (j = 0; j <= NY; j++) 
		{
			if(type[i][j] == INSIDE) continue;
			else if(type[i][j] == INLET) Omega[i][j] = 0.0;//Dirichlet(流入口）
			else if(type[i][j] == OUTLET) Omega[i][j] = Omega[i-1][j];//Neumann（流出口）
			else if(type[i][j] == TOP) Omega[i][j] = -2.0 *( Psi[i][NY-1] - rect.size.y) / DY2;
			else if(type[i][j] == BOTTOM) Omega[i][j] = -2.0 * Psi[i][1] / DY2;
			else if(type[i][j] == OBS_LEFT) Omega[i][j] = -2.0 * (Psi[i-1][j] - psi_obs) / DX2;
			else if(type[i][j] == OBS_RIGHT) Omega[i][j] = -2.0 *( Psi[i+1][j] - psi_obs) / DX2;
			else if(type[i][j] == OBS_TOP) Omega[i][j] = -2.0 *( Psi[i][j+1] - psi_obs) / DY2;
			else if(type[i][j] == OBS_BOTTOM) Omega[i][j] = -2.0 * (Psi[i][j-1] - psi_obs) / DY2;
			else if(type[i][j] == CORNER_UL) Omega[i][j] = -2.0 * (Psi[i-1][j+1] - psi_obs) / DD2;
			else if(type[i][j] == CORNER_UR) Omega[i][j] = -2.0 * (Psi[i+1][j+1] - psi_obs) / DD2;
			else if(type[i][j] == CORNER_LL) Omega[i][j] = -2.0 * (Psi[i-1][j-1] - psi_obs) / DD2;
			else if(type[i][j] == CORNER_LR) Omega[i][j] = -2.0 * (Psi[i+1][j-1] - psi_obs) / DD2;
		}
	
	//Poissonの方程式を解く
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		for (i = 1; i < NX; i++)
			for (j = 1; j < NY; j++)
			{
				if(type[i][j] >= OBS_LEFT) continue;
				pp = ( DY2 * (Psi[i - 1][j] + Psi[i + 1][j]) + DX2 *(Psi[i][j - 1] + Psi[i][j + 1])
					 + Omega[i][j] * DX2 * DY2 ) / (2.0 * (DX2 + DY2));
				error = fabs(pp - Psi[i][j]);
				if (error > maxError) maxError = error;
				Psi[i][j] = pp;//更新
			}
		if (maxError < tolerance) break;
		cnt++;
	}

	if(flagParameter)
	{
		drawParam("cnt=%d", cnt, 0.2, 0.8);
		drawParam("maxError=%2.7f", maxError, 0.5, 0.8);
	}

	calcVelocity();//速度の更新(//ψ→ω変換)

	//渦度輸送方程式を解く
	methodCIP(deltaT);

	//流れ関数，渦度の最小値，最大値
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			if(Psi[i][j] > maxPsi0) maxPsi0 = Psi[i][j];
			if(Psi[i][j] < minPsi0) minPsi0 = Psi[i][j];
			if(Omega[i][j] > maxOmg0) maxOmg0 = Omega[i][j];
			if(Omega[i][j] < minOmg0) minOmg0 = Omega[i][j];
		}

//printf("maxPsi=%f minPsi=%f \n", maxPsi0, minPsi0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);
}

void calcVelocity()
{
	float DX = rect.delta.x;
	float DY = rect.delta.y;
	int i, j;
	//速度ベクトルの計算
	//格子点の速度ベクトル(上下左右の流れ関数の差で求める)
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			//ポテンシャルの低い方から高い方へ
			Vel[i][j].x = (Psi[i][j+1] - Psi[i][j-1]) / (DY * 2.0);
			Vel[i][j].y = (Psi[i-1][j] - Psi[i+1][j]) / (DX * 2.0);
		}
}
void methodCIP(float deltaT)
{
	float newOmega[NUM_MAX_X][NUM_MAX_Y];//渦度
	float newGx[NUM_MAX_X][NUM_MAX_Y];//x方向微分
	float newGy[NUM_MAX_X][NUM_MAX_Y];//y方向微分

	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dx2, dx3, dy, dy2, dy3; 

	int i, j, ip, jp;
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			if(type[i][j] >= OBS_LEFT) continue;

			if(Vel[i][j].x >= 0.0) sx = 1.0; else sx = -1.0;
			if(Vel[i][j].y >= 0.0) sy = 1.0; else sy = -1.0;

			x = - Vel[i][j].x * deltaT;
			y = - Vel[i][j].y * deltaT;
			ip = i - (int)sx;//上流点
			jp = j - (int)sy;
			dx = sx * rect.delta.x;
			dy = sy * rect.delta.y;
			dx2 = dx * dx;
			dx3 = dx2 * dx;
			dy2 = dy * dy;
			dy3 = dy2 * dy;

			c30 = ((gx[ip][j] + gx[i][j]) * dx - 2.0 * (Omega[i][j] - Omega[ip][j])) / dx3;
			c20 = (3.0 * (Omega[ip][j] - Omega[i][j]) + (gx[ip][j] + 2.0 * gx[i][j]) * dx) / dx2;
			c03 = ((gy[i][jp] + gy[i][j]) * dy - 2.0 * (Omega[i][j] - Omega[i][jp])) / dy3;
			c02 = (3.0 * (Omega[i][jp] - Omega[i][j]) + (gy[i][jp] + 2.0 * gy[i][j]) * dy) / dy2;
			a = Omega[i][j] - Omega[i][jp] - Omega[ip][j] + Omega[ip][jp];
			b = gy[ip][j] - gy[i][j];
			c12 = (-a - b * dy) / (dx * dy2);
			c21 = (-a - (gx[i][jp] - gx[i][j]) * dx) / (dx2*dy);
			c11 = - b / dx + c21 * dx;

			newOmega[i][j] = Omega[i][j] + ((c30 * x + c21 * y + c20) * x + c11 * y + gx[i][j]) * x
				        + ((c03 * y + c12 * x + c02) * y + gy[i][j]) * y;

			newGx[i][j] = gx[i][j] + (3.0 * c30 * x + 2.0 * (c21 * y + c20)) * x + (c12 * y + c11) * y;
			newGy[i][j] = gy[i][j] + (3.0 * c03 * y + 2.0 * (c12 * x + c02)) * y + (c21 * x + c11) * x;

			//粘性項に中央差分
			newOmega[i][j] += deltaT * ( (Omega[i-1][j] - 2.0 * Omega[i][j] + Omega[i+1][j]) / dx2
							 + (Omega[i][j-1] - 2.0 * Omega[i][j] + Omega[i][j+1]) / dy2 ) / Re;

		}
	//更新
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			Omega[i][j] = newOmega[i][j];
			gx[i][j] = newGx[i][j];
			gy[i][j] = newGy[i][j];
		}
			
}

void drawRegion()
{
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

	scale.x = 1.5 * rect.scale * s1 ;
	scale.y = 1.5 * rect.scale * s2 ;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//左下基準点
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;

	//全体枠
	glLineWidth(1.0);
	//色
	glColor3f(0.0, 0.0, 0.0);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0);

	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glEnd();

	glLineWidth(3.0);
	glBegin(GL_LINES);
	//下側
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	//上側
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	//障害物左端
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	//障害物右端
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	//障害物上部
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width) / 2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width) / 2.0);
	//障害物下部
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width) / 2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width) / 2.0);
	glEnd();

}

void drawObstacle()
{
	//障害物
	glColor3f(0.7, 0.7, 0.7);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y - rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.size.y + rect.obs_width)/2.0);
	glEnd();
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

void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color)
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
	
	float dp0 = (maxP - minP) / (float)(nLine +1);//ポテンシャル間隔

	float pp;

	float x, y, x1, y1, x2, y2;
	int i, j, k;

    for (k = 0; k < nLine; k++)
    {
        pp = (float)(k+1) * dp0 + minP;
        for (i = 0; i < rect.nMeshX; i++)
        {
            for (j = 0; j < rect.nMeshY; j++)
            {
 				//1つでも内点なら描画
				if( type[i][j] != INSIDE && type[i+1][j] != INSIDE 
					&& type[i+1][j+1] != INSIDE && type[i][j+1] != INSIDE ) continue;
               
				x1 = -1.0f; x2 = -1.0f; y1 = -1.0f; y2 = -1.0f;

                //格子の左辺を調査
                if ((pp >= PP[i][j] && pp < PP[i][j + 1]) || (pp <= PP[i][j] && pp > PP[i][j + 1]))
                {
                    x1 = (float)i;
                    y1 = (float)j + (pp - PP[i][j]) / (PP[i][j + 1] - PP[i][j]);
                }

                //上辺を調査
                if ((pp >= PP[i][j + 1] && pp < PP[i + 1][j + 1]) || (pp <= PP[i][j + 1] && pp > PP[i + 1][j + 1]))
                {
                    x = (float)i + (pp - PP[i][j + 1]) / (PP[i + 1][j + 1] - PP[i][j + 1]);
                    y = (float)(j + 1);
                    if (x1 < 0.0f || y1 < 0.0f)
                    {
                        x1 = x; y1 = y;
                    }
                    else
                    {
                        x2 = x; y2 = y; goto drawLine;
                    }
                }
                //右辺を調査
                if ((pp >= PP[i + 1][j] && pp < PP[i + 1][j + 1]) || (pp <= PP[i + 1][j] && pp > PP[i + 1][j + 1]))
                {
                    x = (float)(i + 1) ;
                    y = (float)j  + (pp - PP[i + 1][j]) / (PP[i + 1][j + 1] - PP[i + 1][j]);

                    if (x1 < 0.0f || y1 < 0.0f)
                    {
                        x1 = x; y1 = y;
                    }
                    else
                    {
                        x2 = x; y2 = y; goto drawLine;
                    }
                }
                //下辺を調査

                if ((pp >= PP[i][j] && pp < PP[i + 1][j]) || (pp <= PP[i][j] && pp > PP[i + 1][j]))
                {
                    x2 = (float)i + (pp - PP[i][j]) / (PP[i + 1][j] - PP[i][j]);
                    y2 = (float)j;
                }
            drawLine: ;
                if (x1 < 0.0 || y1 < 0.0 || x2 < 0.0 || y2 < 0.0) continue;
				x1 *= rect.delta.x; x2 *= rect.delta.x;
				y1 *= rect.delta.y; y2 *= rect.delta.y;
				glBegin(GL_LINES);
				glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
				glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
				glEnd();
            }
        }
    }
}

void drawVelocity()
{
	int i, j;

	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
	//描画
	float theta, mag;
	for(i = 1; i < rect.nMeshX; i++)
		for (j = 1; j < rect.nMeshY; j++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			if((i/thinningV) * thinningV != i) continue;
			if((j/thinningV) * thinningV != j) continue;

			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;
			mag = Vel[i][j].magnitude();
			glPushMatrix();
			glTranslatef(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y *  (float)j * rect.delta.y, 0.0);
			glRotatef(theta, 0.0, 0.0, 1.0);
			glScalef(scaleArrow * mag, scaleArrow * mag, 0.0);
			drawArrow();
			glPopMatrix();
		}
}
void drawArrow()
{
	glLineWidth(widthArrow);
	glColor3f(0.5, 0.0, 0.5);

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
	for(i = 1; i < rect.nMeshX; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
		glEnd();
	}

	for(j = 1; j < rect.nMeshY; j++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glEnd();
	}
}

void drawParticle(float dt)
{
	int k, kk;
	if(!flagFreeze && countInterval == 0.0)
	{
		for(k = 0; k < numP0; k++)
		{
			//流入口に粒子発生
			kk = countP + k;
			p[kk].size = sizeParticle;
			p[kk].speedC = speedCoef;
			p[kk].pos.x = 0.01;	
			p[kk].pos.y = rect.size.y * getRandom(0.0, 1.0);
			if(p[kk].pos.y < rect.size.y / 2.0) p[kk].color = RED;// DARK_YELLOW;
			else p[kk].color = BLUE;//DARK_CYAN;

		}
		countP += numP0;
//printf("size.x=%f , p[0].x = %f, p[1].y = %f \n", rect.size.x,  p[0].pos.x, p[0].pos.y);
	}

	for(k = 0; k < MAX_PARTICLE; k++)
	{
		p[k].vel = getVelocityParticle(p[k].pos);

		if(!flagFreeze) p[k].update(dt);

		if(p[k].pos.x >= 0.0 && p[k].pos.x < rect.size.x 
			&& p[k].pos.y >= 0.0 && p[k].pos.y < rect.size.y) p[k].show(rect.left0, scale);

	}

	if(countP > MAX_PARTICLE - numP0) countP = 0;
	countInterval += dt;
	if(countInterval > intervalP) {
		countInterval = 0.0;
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
	for(int i = 0; i < rect.nMeshX; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.nMeshY; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//格子点の速度を線形補間
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
