/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
正方形領域のCavity
流れ関数-渦度法
渦度のPoisson方程式と渦度の輸送方程式を交互に計算
輸送方程式にCIP法を適用
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
void methodCIP(float deltaT);
void drawContour(float PP[][NUM_MAX], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Psi[NUM_MAX][NUM_MAX];//流れ関数
float Omega[NUM_MAX][NUM_MAX];//渦度
float gx[NUM_MAX][NUM_MAX];//x方向微分
float gy[NUM_MAX][NUM_MAX];//y方向微分

Vector2D Vel[NUM_MAX][NUM_MAX];//格子点の速度
Vector2D scale;//Window左下を(-1.0, -1.0)としたデカルト座標に表示するときのスケール調整

float Courant, diff_num;
float maxPsi0, minPsi0, maxOmg0, minOmg0;

Particle2D p[MAX_PARTICLE];
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
	main_window = glutCreateWindow("GL_CavityPsiOmega");
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
	elapseTime3 = 0.0;//start後の総経過時間(無次元)
	
	rect.delta.x = rect.size.x / (float)rect.nMesh;//解析領域は[0, 1]に固定）
	rect.delta.y = rect.size.y / (float)rect.nMesh;//解析領域は[0, 1]に固定）

	int i, j;
	//境界，初期条件
	for(j = 0; j <= rect.nMesh; j++)
		for (i = 0; i <= rect.nMesh; i++)
		{
			//流れ関数
			Psi[i][j] = 0.0;
			//渦度
			Omega[i][j] = 0.0;
			//速度
			Vel[i][j].x = 0.0;
			Vel[i][j].y = 0.0;
			gx[i][j] = 0.0;
			gy[i][j] = 0.0;
		}

	count = 0;//タイムステップ数

	countP = 0;//粒子個数のカウント
	for(i = 0; i < MAX_PARTICLE; i++) p[i] = Particle2D();
	//粒子初期配置
	for(j = 1; j < rect.nMesh; j++)
		for (i = 1; i < rect.nMesh; i++)
		{
			p[countP].pos = Vector2D((float)i * rect.delta.x, (float)j * rect.delta.y);
			p[countP].size = sizeParticle;
			p[countP].speedC = 1.0;
			if(i <= rect.nMesh/3) p[countP].color = RED;
			else if(i <= 2 * rect.nMesh/3)  p[countP].color = GREEN;			
			else  p[countP].color = BLUE;
			countP ++;
			if(countP > MAX_PARTICLE) printf("MAX_PARTICLE を大きくしてください \n");
		}


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
		elapseTime2 += dt;
		elapseTime3 += deltaT;//無次元
		calculate(deltaT); 

		if(flagStep) flagFreeze = true;
		count++;
	}

	if( flagStream ) drawContour( Psi, minPsi, maxPsi,BLACK);
	if( flagVorticity ) drawContour( Omega, minOmg, maxOmg,RED);

	if( flagVelocity ) drawVelocity();
	if( flagGrid ) drawGrid();

	if(flagP_Start) drawParticle(deltaT);//粒子アニメーション

	//発散しないための目安を知るため
	float Courant = 1.0 * deltaT / rect.delta.x;	
	float diff_num = (1.0 / Re) * deltaT / (rect.delta.x * rect.delta.x);//拡散数

	if(flagParameter)
	{
		drawParam("f_rate=%d[fps]", fps0, -0.99, 0.9);
		drawParam("t_step=%2.5f[s]", dt, -0.5, 0.9);
		drawParam("e_time=%3.3f[s]", elapseTime2, 0.05, 0.9);
		drawParam("n_time=%3.3f", elapseTime3, 0.55, 0.9);
		drawParam("Courant=%1.4f", Courant, -0.99, 0.8);
		drawParam("diff_num=%1.4f", diff_num, -0.5, 0.8);
		drawParam("Re=%5.1f", Re, 0.0, 0.8);
	}
	//バッファの切り替え
	glutSwapBuffers();
}

void calculate(float deltaT)
{
	int i, j;
	float error = 0.0;
	float maxError = 0.0;
	float pp;
	float D = rect.delta.x;
	float D2 = D * D;
	int N = rect.nMesh;

	//Poissonの方程式を解くための境界条件
	//左右
	for (j = 0; j <= N; j++) {
		Omega[0][j] = -2.0 * Psi[1][j] / D2;
		Omega[N][j] = -2.0 * Psi[N-1][j] / D2;
	}
	//上下
	for (i = 0; i <= N; i++) {
		Omega[i][0] = -2.0 * Psi[i][1] / D2;//Bottom
		Omega[i][N] = -2.0 *( Psi[i][N-1] + D) / D2;//Top(ｘ方向速度を1）
	}

	//Poissonの方程式を解く
	int cnt = 0;
	while (cnt < iteration)
	{
		maxError = 0.0;

		for (i = 1; i < N; i++)
			for (j = 1; j < N; j++)
			{
				pp = ( (Psi[i - 1][j] + Psi[i + 1][j] + Psi[i][j - 1] + Psi[i][j + 1])
					 + Omega[i][j] * D2 ) / 4.0 ;
				error = fabs(pp - Psi[i][j]);
				if (error > maxError) maxError = error;
				Psi[i][j] = pp;//更新
			}
		if (maxError < tolerance) break;
		cnt++;
	}
	//速度ベクトルの計算
	//格子点の速度ベクトル(上下左右の流れ関数の差で求める)
	for(i = 1; i < N; i++)
		for (j = 1; j < N; j++)
		{	        
			//ポテンシャルの低い方から高い方へ
			Vel[i][j].x = (Psi[i][j+1] - Psi[i][j-1]) / (D * 2.0);
			Vel[i][j].y = (Psi[i-1][j] - Psi[i+1][j]) / (D * 2.0);
		}

	//渦度輸送方程式を解く
	methodCIP(deltaT);

	if(flagParameter)
	{
		drawParam("cnt=%d", cnt, 0.3, 0.8);
	}


	//流れ関数，渦度の最大値・最小値
	for(i = 1; i < N; i++)
		for (j = 1; j < N; j++)
		{
			if(Psi[i][j] > maxPsi0) maxPsi0 = Psi[i][j];
			if(Psi[i][j] < minPsi0) minPsi0 = Psi[i][j];
			if(Omega[i][j] > maxOmg0) maxOmg0 = Omega[i][j];
			if(Omega[i][j] < minOmg0) minOmg0 = Omega[i][j];
		}

//printf("maxPsi=%f minPsi=%f \n", maxPsi0, minPsi0);
//printf("maxOmg=%f minOmg=%f \n", maxOmg0, minOmg0);
}

void methodCIP(float deltaT)
{
	float newOmega[NUM_MAX][NUM_MAX];//渦度
	float newGx[NUM_MAX][NUM_MAX];//x方向微分
	float newGy[NUM_MAX][NUM_MAX];//y方向微分

	int NX = rect.nMesh;
	int NY = rect.nMesh;
	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dx2, dx3, dy, dy2, dy3; 

	int i, j, ip, jp;
	for(j = 1; j < NY; j++)
		for(i = 1; i < NX; i++)
		{
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
			Omega[i][j] = newOmega[i][j];
			gx[i][j] = newGx[i][j];
			gy[i][j] = newGy[i][j];
		}
			
}

void drawRegion()
{
	//Cavity
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

	scale.x = 2.0 * rect.scale * s1 * 0.8;
	scale.y = 2.0 * rect.scale * s2 * 0.8;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;
	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y + sy);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y + sy);
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

void drawContour(float PP[][NUM_MAX], float minP, float maxP, Color color)
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

		for(i = 0; i < rect.nMesh; i++)
		{
			for(j = 0; j < rect.nMesh; j++)
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
					glVertex2f(rect.left0.x + scale.x * x1, rect.left0.y + scale.y * y1);
					glVertex2f(rect.left0.x + scale.x * x2, rect.left0.y + scale.y * y2);
					glEnd();
				}//m
			}//j
		}//i
	}//k
}

void drawVelocity()
{
	int i, j;

	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
	//描画
	float theta, mag;
	for(i = 1; i < rect.nMesh; i++)
		for (j = 1; j < rect.nMesh; j++)
		{
			if((i/thinningV) * thinningV != i) continue;
			if((j/thinningV) * thinningV != j) continue;

			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;// 180.0 / M_PI;
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
	for(i = 1; i < rect.nMesh; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
		glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
		glEnd();
	}
	for(j = 1; j < rect.nMesh; j++)
	{
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
		glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
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
	for(int i = 0; i < rect.nMesh; i++)
	{
		if((float)i * rect.delta.x < pos.x && (float)(i+1) * rect.delta.x > pos.x) I = i;
	}
	for(int j = 0; j < rect.nMesh; j++)
	{
		if((float)j * rect.delta.y < pos.y && (float)(j+1) * rect.delta.y > pos.y) J = j;
	}
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//格子点の速度を線形補間
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}
