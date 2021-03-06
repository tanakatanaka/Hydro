/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
障害物が中間にある平行平板ダクト
速度-圧力法（フラクショナル・ステップ法）
スタガード格子
圧力のPoisson方程式と速度の輸送方程式を交互に計算
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
void methodCIP(float f[][NUM_MAX_Y],  float gx[][NUM_MAX_Y], float gy[][NUM_MAX_Y], float vx[][NUM_MAX_Y], float vy[][NUM_MAX_Y]);
void calcVelocity();
void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color);
void drawPressureC();
void drawPressureI(float PP[][NUM_MAX_Y], float minP, float maxP, Color color);
void drawOmegaC();
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Prs[NUM_MAX_X][NUM_MAX_Y];//圧力
float velX[NUM_MAX_X][NUM_MAX_Y];//格子点の速度(解析用）
float velY[NUM_MAX_X][NUM_MAX_Y];//速度微分
float velXgx[NUM_MAX_X][NUM_MAX_Y];//速度微分
float velXgy[NUM_MAX_X][NUM_MAX_Y];//速度微分
float velYgx[NUM_MAX_X][NUM_MAX_Y];//速度微分
float velYgy[NUM_MAX_X][NUM_MAX_Y];//速度微分
float Omg[NUM_MAX_X][NUM_MAX_Y];//渦度

Vector2D Vel[NUM_MAX_X][NUM_MAX_Y];//格子点の速度（表示用）Window左下を(-1.0, -1.0)としたデカルト座標に表示するときのスケール調整
Vector2D scale;//Window左下を(-1.0, -1.0)としたデカルト座標に表示するときのスケール調整

float minPrs0, maxPrs0, minOmg0, maxOmg0;

enum Type{INSIDE, INLET, OUTLET, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM, OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ
int NX, NY, NX1, NX2, NY1, NY2;
float DX, DY, A1, A2, A3;

Particle2D p[MAX_PARTICLE];
bool flagResetting = false;
int count;

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
	main_window = glutCreateWindow("GL_DuctFS2");
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
	
	DX = rect.delta.x = rect.size.x / (float)rect.meshX;
	DY = rect.delta.y = rect.size.y / (float)rect.meshY;
	NX = rect.meshX + 2;//解析の格子点は壁の内部に1つずつ
	NY = rect.meshY + 2;
	A1 = 0.5 * DY*DY / (DX*DX + DY*DY);
	A2 = 0.5 * DX*DX / (DX*DX + DY*DY);
	A3 = 0.5 * DY*DY / (1.0 + DY*DY / (DX*DX));

	float obsTop, obsBottom;
	obsTop = rect.obs_posY + rect.obs_width/2.0;
	obsBottom = rect.obs_posY - rect.obs_width/2.0;
	//障害物境界の分割点番号
	float eps = 0.00001;
	NX1 = floor((rect.obs_left - rect.obs_thick/2.0) / rect.delta.x + eps)+1;//障害物左端位置
	NX2 = floor((rect.obs_left + rect.obs_thick/2.0) / rect.delta.x + eps)+1;//障害物右端位置
	NY2 = floor((rect.obs_posY + rect.obs_width/2.0) / rect.delta.y + eps)+1;//障害物上端位置
	NY1 = floor((rect.obs_posY - rect.obs_width/2.0) / rect.delta.y + eps)+1;//障害物下端位置

	int NX_ObsT = NX2 - NX1;
	int NY_ObsW = floor(rect.obs_width / rect.delta.y + eps);
	printf("NX1=%d, NX2=%d, NY1=%d, NY2=%d ObsT=%d ObsW=%d \n", NX1, NX2, NY1, NY2, NX_ObsT, NY_ObsW);
	//格子間隔の整数倍か
	float left, thick, width, top, bottom, height;
	thick = DX * (float) NX_ObsT;
	width = DY * (float) NY_ObsW;
	left = DX * (float)NX1;//左端から障害物左端までの距離
	height = DY * (float)(NY1 + NY_ObsW/2);
	top = DY * (float)NY2;
	bottom = rect.delta.y * (float)NY1;
printf("AAA left=%f, thick=%f, width=%f, height=%f \n", rect.obs_left, rect.obs_thick, rect.obs_width, rect.obs_posY);
printf("BBB left=%f, thick=%f, width=%f, height=%f \n", left, thick, width, height);
	if( (rect.obs_left - rect.obs_thick/2.0 - left) > eps || (rect.obs_thick - thick) > eps || (rect.obs_posY - height) > eps
		|| (rect.obs_width - width) > eps || (obsTop - top) > eps || (obsBottom - bottom) > eps)
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
/*
			if(j == 0) type[i][j] = BOTTOM;//下側壁面
			if(j == rect.meshY) type[i][j] = TOP;//上側壁面
			if(i == 0) type[i][j] = INLET;//流入端
			if(i == NX) type[i][j] = OUTLET;//流出端
*/
			if(j == 1) type[i][j] = BOTTOM;//下側壁面
			if(j == NY-1) type[i][j] = TOP;//上側壁面
			if(i == 1) type[i][j] = INLET;//流入端
			if(i == NX-1) type[i][j] = OUTLET;//流出端

			if(i == NX1 && j > NY1 && j < NY2) type[i][j] = OBS_LEFT;//障害物左端
			if(i == NX2 && j > NY1 && j < NY2) type[i][j] = OBS_RIGHT;//障害物右端
			if(i > NX1 && i < NX2 && j == NY2) type[i][j] = OBS_TOP;//障害物上端
			if(i > NX1 && i < NX2 && j == NY1) type[i][j] = OBS_BOTTOM;//障害物下端
			if(i > NX1 && i < NX2 && j > NY1 && j < NY2) type[i][j] = OBSTACLE;//障害物内部
			if(i == NX1 && j == NY1) type[i][j] = OBS_LL;//左下
			if(i == NX1 && j == NY2) type[i][j] = OBS_UL;//左上
			if(i == NX2 && j == NY1) type[i][j] = OBS_LR;//右下
			if(i == NX2 && j == NY2)  type[i][j] = OBS_UR;//右上
		}

	//step1(初期条件）
	for (i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			//圧力
			Prs[i][j] = 0.0;
			//速度
			velX[i][j] = 1.0;
			//if(type[i][j] == BOTTOM || type[i][j] == TOP || type[i][j] >= OBS_LEFT) velX[i][j] = 0.0;
			velY[i][j] = 0.0;
			velYgx[i][j] = 0.0;
			velYgy[i][j] = 0.0;
			Vel[i][j].x = velX[i][j];
			Vel[i][j].y = velY[i][j];
			Omg[i][j] = 0.0;//渦度
		}

	maxPrs0 = -1000.0; minPrs0 = 1000.0;
	maxOmg0 = -1000.0; minOmg0 = 1000.0;

	count = 0;//タイムステップ数
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
		count++;
		//if(count >= 10000) flagFreeze = 1;
	}
//printf("dt = %f \n", dt);

	if( flagPressureColor ) drawPressureC(); 
	if( flagOmegaColor ) drawOmegaC();
	if( flagPressureIsobar ) drawPressureI(Prs, minPrs, maxPrs, BLACK);
	if( flagOmega ) drawContour(Omg, minOmg, maxOmg, RED);

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

	//step2(速度の境界条件）
	//左右
	for (j = 0; j <= NY; j++) {
		//流入口
		velX[0][j] = velX[2][j];
		velX[1][j] = 1.0;
		velY[0][j] = - velY[1][j];
		//流出口
		velX[NX][j] =   velX[NX-2][j];
		velX[NX-1][j] = 1.0;
		velY[NX-1][j] = -velY[NX-2][j];
	}
	//上下
	for (i = 0; i <= NX; i++) {
		velX[i][0] = - velX[i][1];
		velY[i][0] = velY[i][2];
		velY[i][1] = 0.0;

		velX[i][NY-1] = - velX[i][NY-2];
		velY[i][NY] =  velY[i][NY-2];
		velY[i][NY-1] = 0.0;
	}

	//障害物左右
	for(j = NY1; j <= NY2; j++)
	{
		//左端
		velX[NX1+1][j] =  velX[NX1-1][j];
		velX[NX1][j]   =  0.0;
		velY[NX1][j]   = -velY[NX-1][j];
		//右端
		velX[NX2-1][j] =  velX[NX2+1][j];
		velX[NX2][j]   =  0.0;
		velY[NX2-1][j] = -velY[NX2][j];
	}
	//障害物上下
	for(i = NX1; i <= NX2; i++)
	{
		//上端
		velX[i][NY2] = - velX[i][NY2+1];
		velY[i][NY2-1] = velY[i][NY2+1];
		velY[i][NY2]   = 0.0;
		//下端
		velX[i][NY1+1] = - velX[i][NY1];
		velY[i][NY1+1] = velY[i][NY1-1];
		velY[i][NY1]   = 0.0;
	}

	//step3(CIPによる速度輸送方程式)
	float vel[NUM_MAX_X][NUM_MAX_Y];
	//x方向速度定義点における速度y
	for(i = 1; i < NX; i++)
	for(j = 1; j < NY; j++) 
		vel[i][j] = (velY[i-1][j] + velY[i][j] + velY[i-1][j+1] + velY[i][j+1]) / 4.0;

	methodCIP(velX, velXgx, velXgy, velX, vel);
	//y成分
	//y方向速度定義点における速度x
	for(i = 1; i < NX; i++)
	for(j = 1; j < NY; j++)
		vel[i][j] = (velX[i][j] + velX[i][j-1] + velX[i+1][j-1] + velX[i+1][j]) / 4.0;
	
	methodCIP(velY, velYgx, velYgy, vel, velY);

	//Poisson方程式の右辺
	float D[NUM_MAX_X][NUM_MAX_Y];
	for (j = 1; j < NY; j++)
		for (i = 1; i < NX; i++)
		{
			if(type[i][j] >= OBS_LEFT) continue;
			//if(i >= NX1 && i < NX2 && j >= NY1 && j < NY2) continue;
			D[i][j] = A3 * ((velX[i+1][j] - velX[i][j]) / DX + (velY[i][j+1] - velY[i][j]) / DY) / deltaT;
		}

	//反復法
	int cnt = 0;
	float error, maxError;
	//float maxError = 0.0;
	float pp;
	while (cnt < iteration)
	{
		maxError = 0.0;
		//圧力境界値
		for (j = 1; j < NY; j++) {//左右
			Prs[0][j] = Prs[1][j];//左端
			Prs[NX-1][j] = Prs[NX-2][j];//右端
		}
		for (i = 0; i <= NX; i++){//上下
			Prs[i][0] = Prs[i][1];//下端
			Prs[i][NY-1] = Prs[i][NY-2];//上端
		}	

		//障害物
		for(j = NY1; j < NY2; j++)
		{
			Prs[NX1][j] = Prs[NX1-1][j];//左端
			Prs[NX2-1][j] = Prs[NX2][j];//右端
		}
		for(i = NX1; i < NX2; i++)
		{
			Prs[i][NY1] = Prs[i][NY1-1];//下端
			Prs[i][NY2-1] = Prs[i][NY2];//上端
		}

		//角
		Prs[NX1][NY1] = Prs[NX1-1][NY1-1];//左下
		Prs[NX1][NY2-1] = Prs[NX1-1][NY2];//左上
		Prs[NX2-1][NY2-1] = Prs[NX2][NY2];//右上
		Prs[NX2-1][NY1] = Prs[NX2][NY1-1];//右下

		for (j = 1; j < NY-1; j++)
			for (i = 1; i < NX-1; i++)
			{
				if(type[i][j] >= OBS_LEFT) continue;
				//if(i >= NX1 && i < NX2 && j >= NY1 && j < NY2) continue;
				pp = A1 * (Prs[i+1][j] + Prs[i-1][j]) + A2 * (Prs[i][j+1] + Prs[i][j-1]) - D[i][j];
				error = fabs(pp -  Prs[i][j]);
				if (error > maxError) maxError = error;
				Prs[i][j] = pp;//更新 
			}
		if (maxError < tolerance) break;
		cnt++;
	}

	if(flagParameter)
	{
		drawParam("cnt=%d", cnt, 0.2, 0.8);
		drawParam("maxError=%2.7f", maxError, 0.5, 0.8);
	}

	//step5(スタガード格子点の速度ベクトルの更新）
	for (j = 1; j < NY-1; j++)
		for(i = 2; i < NX-1; i++)
		{	        
			//if(type[i][j] >= OBS_LEFT) continue;
			velX[i][j] += - deltaT * (Prs[i][j] - Prs[i-1][j]) / DX;
		}
	for (j = 2; j < NY-1; j++)
		for(i = 1; i < NX-1; i++)
		{
			//if(type[i][j] >= OBS_LEFT) continue;
			velY[i][j] += - deltaT * (Prs[i][j] - Prs[i][j-1]) / DY;
		}

	//表示のための速度は圧力と同じ位置で
	for(j = 1; j <= NY-2; j++)
		for(i = 1; i <= NX-2; i++)
		{
			//if(type[i][j] >= OBS_LEFT) continue;
			Vel[i][j].x = (velX[i][j] + velX[i+1][j]) / 2.0;
			Vel[i][j].y = (velY[i][j] + velY[i][j+1]) / 2.0;
		}

	//Omega
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++) 
		{
			Omg[i][j] = 0.5 * ((Vel[i+1][j].y - Vel[i-1][j].y) / DX - (Vel[i][j+1].x - Vel[i][j-1].x) / DY);
		}
	
	//流れ関数，圧力の最大値・最小値
	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			if(Prs[i][j] > maxPrs0) maxPrs0 = Prs[i][j];
			if(Prs[i][j] < minPrs0) minPrs0 = Prs[i][j];
			if(Omg[i][j] > maxOmg0) maxOmg0 = Omg[i][j];
			if(Omg[i][j] < minOmg0) minOmg0 = Omg[i][j];
//if(i==15) printf("j=%d prs=%f \n", j, Prs[i][j]);
		}

//printf("maxPrs=%f minPrs=%f \n", maxPrs0, minPrs0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);

}

void methodCIP(float f[][NUM_MAX_Y],  float gx[][NUM_MAX_Y], float gy[][NUM_MAX_Y], float vx[][NUM_MAX_Y] , float vy[][NUM_MAX_Y])
{
	float newF[NUM_MAX_X][NUM_MAX_Y];//関数
	float newGx[NUM_MAX_X][NUM_MAX_Y];//x方向微分
	float newGy[NUM_MAX_X][NUM_MAX_Y];//y方向微分

	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dy, dx2, dy2, dx3, dy3; 

	int i, j, ip, jp;
	for(i = 1; i < NX; i++)
		for(j = 1; j < NY; j++)
		{
			//if(type[i][j] >= OBS_LEFT) continue;
			//if(type[i][j] == OBSTACLE) continue;
if(i >=NX1 && i < NX2 && j >= NY1 && j < NY2) continue;

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
				               +  (f[i][j-1] + f[i][j+1] - 2.0 * f[i][j]) / dy2 ) / Re;

		}

	//更新
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
if(i >=NX1 && i < NX2 && j >= NY1 && j < NY2) continue;
			//if(type[i][j] >= OBS_LEFT) continue;
//			if(type[i][j] == OBSTACLE) continue;
			f[i][j] = newF[i][j];
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
	//glLineWidth(3.0);

	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
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

	glLineWidth(2.0);
	glBegin(GL_LINES);
	//下側
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	//上側
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	//障害物左端
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY + rect.obs_width/2.0));
	//障害物右端
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY + rect.obs_width/2.0));
	//障害物上部
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY + rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY + rect.obs_width/2.0));
	//障害物下部
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY - rect.obs_width/2.0));
	glEnd();

}

void drawObstacle()
{
	//障害物
	glColor3f(0.7, 0.7, 0.7);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY - rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY + rect.obs_width/2.0));
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left - rect.obs_thick/2.0), rect.left0.y + scale.y * (rect.obs_posY + rect.obs_width/2.0));
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
			{
				//1つでも内点なら描画
				if( type[i][j] != INSIDE && type[i+1][j] != INSIDE 
					&& type[i+1][j+1] != INSIDE && type[i][j+1] != INSIDE ) continue;
 			
				//三角形セルに分割
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
					//glVertex2f(rect.left0.x + scale.x * (x1), rect.left0.y + scale.y * (y1 ));
					//glVertex2f(rect.left0.x + scale.x * (x2), rect.left0.y + scale.y * (y2 ));
					glEnd();
				}//m
			}//j
		}//i
	}//k
}

void drawPressureI(float PP[][NUM_MAX_Y], float minP, float maxP, Color color)
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
		{//(スタガード格子ではi=1,j=1が壁面）
			for(j = 0; j <= NY-2; j++)
			{
				if(type[i][j] == OBSTACLE) continue;
				//三角形セルに分割
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
					//glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
					//glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
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

void drawOmegaC()
{
	float omegaW = maxOmg -minOmg;//表示渦度最大幅
	glPolygonMode(GL_FRONT, GL_FILL);

	float x1, y1, x2, y2, x3, y3, x4, y4;
	float pp[4], rr[4], gg[4], bb[4];
	int i, j, k;

    for (i = 0; i < NX-2; i++)
    {
        for (j = 0; j < NY-2; j++)
        {

			x1 = (float)i * rect.delta.x;     y1 = (float)j * rect.delta.y;
			x2 = (float)(i+1) * rect.delta.x; y2 = (float)j * rect.delta.y;
			x3 = (float)(i+1) * rect.delta.x; y3 = (float)(j+1) * rect.delta.y;
			x4 = (float)i * rect.delta.x;     y4 = (float)(j+1) * rect.delta.y;
			
			pp[0] = (Omg[i][j] - minOmg) / omegaW; 
			pp[1] = (Omg[i+1][j] - minOmg) / omegaW;
			pp[2] = (Omg[i+1][j+1] - minOmg) / omegaW; 
			pp[3] = (Omg[i][j+1] - minOmg) / omegaW;
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
			glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
			glColor3f(rr[1], gg[1], bb[1]);
			glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
			glColor3f(rr[2], gg[2], bb[2]);
			glVertex2f(rect.left0.x + scale.x * x3, rect.left0.y + scale.y * y3);
			glColor3f(rr[3], gg[3], bb[3]);
			glVertex2f(rect.left0.x + scale.x * x4, rect.left0.y + scale.y * y4);
			glEnd();
        }
    }
    
}

void drawVelocity()
{
	int i, j;

	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
	//描画(スタガード格子ではi=1,j=1は壁面）
	float theta, mag;
	for(i = 1; i <= NX-2; i++)
		for (j = 1; j <= NY-2; j++)
		{
			//if(type[i][j] >= OBS_LEFT) continue;
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
	for(i = 1; i < NX-2; i++)//このi=0は速度ｘ格子線のI=1に相当
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
		glEnd();
	}

	for(j = 1; j < NY-2; j++)//j=0は速度ｙ格子線のJ=1に相当
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glEnd();
	}

	glPointSize(5);
	for(i = 0; i <= NX; i++)
	for(j = 0; j <= NY; j++)
	{
		//if(type[i][j] >= OBS_LEFT) glColor3f(1.0, 0.0, 0.0);
		//else glColor3f(0.0, 0.0, 1.0);
		if(type[i][j] == INSIDE) glColor3f(0.0, 0.0, 1.0);
		else if(type[i][j] == OBS_TOP || type[i][j] == OBS_BOTTOM || type[i][j] == OBS_LEFT || type[i][j] == OBS_RIGHT) glColor3f(1.0, 0.0, 0.0); 
		else if(type[i][j] == OBS_UL || type[i][j] == OBS_UR || type[i][j] == OBS_LL || type[i][j] == OBS_LR) glColor3f(0.0, 1.0, 0.0); 
		else glColor3f(0.0, 0.0, 0.0);

		glBegin(GL_POINTS);
		glVertex2f(rect.left0.x + scale.x * (float)(i-1) * DX, rect.left0.y + scale.y * (float)(j-1) * DY);
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
	for(int i = 0; i < rect.meshX; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.meshY; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//格子点の速度を線形補間
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
