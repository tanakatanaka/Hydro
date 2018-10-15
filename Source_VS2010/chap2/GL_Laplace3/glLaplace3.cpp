/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元Laplace3
速度ポテンシャルおよび流れ関数のラプラス方程式を有限差分法で解く
粒子アニメーション
*/
#include <windows.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glui.h>//GLUIを使用する
#include "../../support2D.h"
#include "../../myMath6.h"
#include "../../particle2D.h"
#include "myGLUI.h"

//関数のプロトタイプ宣言
void idle();
void display();
void drawRegion();
void resize(int w, int h);
void calculatePhi();
void calculatePsi();
void drawContour(float PP[][NUM_MAX_Y], float minP, float maxP, Color color);
void drawVelocity();
void drawArrow();
void drawGrid();
void mouse(int button, int state, int x, int y);
Vector2D getVelocityParticle(Vector2D pos);
void drawParticle(float dt);

float Phi[NUM_MAX_X][NUM_MAX_Y];//速度ポテンシャル
float Psi[NUM_MAX_X][NUM_MAX_Y];//流れ関数
int type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ
//格子点の速度
Vector2D Vel[NUM_MAX_X][NUM_MAX_Y];
Vector2D scale;

int NX, NY;
//格子数（障害物）
int nMeshX1;//左端から障害物左端までの分割数
int nMeshX2;//左端から障害物右端までの分割数
int nMeshX_ObsT;//障害物部分の分割数
int nMeshY_ObsW;
Vector2D mPos;//マウスがクリックされた位置
Particle2D p[MAX_PARTICLE];

enum Type{INSIDE, INLET, OUTLET, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM, OBS_RIGHT, OBSTACLE, CORNER_UL, CORNER_UR};

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
	main_window = glutCreateWindow("GL_Laplace3");//GLUIが対象とするウィンドウ
	//ウィンドウのサイズ変更
	glutReshapeFunc(resize);
	//表示
	glutDisplayFunc(display);
	//マウス操作
	glutMouseFunc(mouse);

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

	NX = rect.nMeshX;
	NY = rect.nMeshY;
	rect.delta.x = rect.size.x / (float)NX;
	rect.delta.y = rect.size.y / (float)NY;

	//障害物境界の分割点番号
	float eps = 0.00001;
	nMeshX1 = floor(rect.obs_left / rect.delta.x + eps); //障害物左端位置
	nMeshX2 = floor((rect.obs_left + rect.obs_thickness) / rect.delta.x + eps);//障害物右端位置
	nMeshX_ObsT = nMeshX2 - nMeshX1;
	nMeshY_ObsW = floor(rect.obs_width / rect.delta.y + eps);

	//格子点のタイプ
	int i, j;
	for(i = 0; i <= rect.nMeshX; i++)
		for(j = 0; j <= rect.nMeshY; j++)
		{
			type[i][j] = INSIDE;//内点
			if(j == 0) type[i][j] = BOTTOM;//下側壁面
			if(j == rect.nMeshY) type[i][j] = TOP;//上側壁面
			if(i == 0) type[i][j] = INLET;//流入端
			if(i == rect.nMeshX) type[i][j] = OUTLET;//流出端
			if(i == nMeshX1 && j < nMeshY_ObsW) type[i][j] = OBS_LEFT;//障害物左端
			if(i >= nMeshX1 && i <= nMeshX2 && j == nMeshY_ObsW) type[i][j] = OBS_TOP;//障害物上端
			if(i == nMeshX2 && j < nMeshY_ObsW) type[i][j] = OBS_RIGHT;//障害物右端
			if(i > nMeshX1 && i < nMeshX2 && j < nMeshY_ObsW) type[i][j] = OBSTACLE;//障害物内部
			if(i == nMeshX1 && j == 0) type[i][j] = CORNER_UL;
			if(i == nMeshX1 && j == nMeshY_ObsW)  type[i][j] = CORNER_UL;
			if(i == nMeshX2 && j == 0) type[i][j] = CORNER_UR;
			if(i == nMeshX2 && j == nMeshY_ObsW)  type[i][j] = CORNER_UR;
		}

	//内部点
	for(i = 0; i <= rect.nMeshX; i++)
		for(j = 0; j <= rect.nMeshY; j++) { Phi[i][j] = 0.0; Psi[i][j] = 0.0; }

	//粒子
	countP = 0;	
	for(int i = 0; i < MAX_PARTICLE; i++){
		p[i] = Particle2D();
		p[i].size = sizeParticle;
		p[i].speedC = speedCoef;
	}

		
	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start後の総経過時間
}

void display(void)
{
	//時間計測
	QueryPerformanceFrequency( &freq );
	QueryPerformanceCounter( &currentTime);
	pTime = (float)((currentTime.QuadPart - lastTime.QuadPart) * 1000000 / freq.QuadPart);
	float dt = pTime / 1000000.0;//[sec]
	elapseTime1 += dt;
	elapseTime2 += dt;
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

	if(flagExecute == 1) 
	{
		calculatePhi();
		calculatePsi();
	}

	if(flagPotential == 1) drawContour(Phi, 0.0, rect.size.x, RED);
	if(flagStream == 1) drawContour(Psi, 0.0, rect.size.x, BLACK);
	if(flagVelocity == 1) drawVelocity();
	if(flagGrid == 1) drawGrid();

	if(flagStart) drawParticle(dt);//粒子アニメーション

	if(!flagStart && flagMouse)//マウスで指定した位置から1個だけ移動
	{
		p[0].size = sizeParticle;
		p[0].speedC = speedCoef;
		p[0].vel = getVelocityParticle(p[0].pos);
		if(!flagFreeze) p[0].update(dt);
		if(p[0].pos.x >= 0.0 && p[0].pos.x < rect.size.x && p[0].pos.y >= 0.0 && p[0].pos.y < rect.size.y) p[0].show(rect.left0, scale);
		elapseTime2 += dt;
	}

	if(flagParameter)
	{
		drawParam("framerate=%d[fps]", fps0, -0.9, 0.9);
		drawParam("timestep=%1.5f[s]", dt, -0.2, 0.9);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 0.5, 0.9);
	}
	//バッファの切り替え
	glutSwapBuffers();
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

	scale.x = rect.scale * s1 * 1.5;
	scale.y = rect.scale * s2 * 1.5;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//左下基準点
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;

	//障害物
	glColor3f(0.7, 0.7, 0.7);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * rect.obs_width);
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * rect.obs_width);
	glEnd();

	//全体枠
	glLineWidth(1.0);
	//色
	glColor3f(0.0, 0.0, 0.0);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glEnd();

	glLineWidth(3.0);
	glBegin(GL_LINES);
	//左下側
	glVertex2f(rect.left0.x, rect.left0.y); 
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y);
	//障害物左端
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * rect.obs_width);
	//障害物上部
	glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * rect.obs_width);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * rect.obs_width);
	//障害物右端
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * rect.obs_width);
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y);
	//右下側
	glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y);
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y);
	//上部
	glVertex2f(rect.left0.x, rect.left0.y + scale.y * rect.size.y); 
	glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * rect.size.y);
	glEnd();
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

void calculatePhi()
{
	int i, j;

	//境界条件と内部格子点の初期条件
	for(i = 0; i <= rect.nMeshX; i++)
	{
		for(j = 0; j <= rect.nMeshY; j++)
		{
			if(type[i][j] == INLET) Phi[i][j] = 0.0;
			else if(type[i][j] == OUTLET)  Phi[i][j] = rect.size.x;//一様流れの流速を1とする
			else
			{//内点は線形補間
				Phi[i][j] = float(i) * rect.delta.x;
			}
		}
	}

	//差分法
	int cnt = 0;
	float error = 0.0;
	float maxError = 0.0;
	float dx2 = rect.delta.x * rect.delta.x ;
	float dy2 = rect.delta.y * rect.delta.y ;
	float pp;
	while (cnt < iteration)
	{
		//Neumann boundary conditionは毎回与える
		for(i = 1; i < rect.nMeshX; i++)
		for(j = 0; j <= rect.nMeshY; j++)
		{
			if(type[i][j] == TOP) Phi[i][j] = Phi[i][j-1];
			else if(type[i][j] == BOTTOM)   Phi[i][j] = Phi[i][j+1];
			else if(type[i][j] == OBS_LEFT) Phi[i][j] = Phi[i-1][j];
			else if(type[i][j] == OBS_TOP)  Phi[i][j] = Phi[i][j+1];
			else if(type[i][j] == OBS_RIGHT) Phi[i][j] = Phi[i+1][j];
			else if(type[i][j] == CORNER_UL)  Phi[i][j] = Phi[i-1][j+1];
			else if(type[i][j] == CORNER_UR)  Phi[i][j] = Phi[i+1][j+1];
		}

		maxError = 0.0;
		for (i = 1; i < rect.nMeshX; i++)
			for (j = 1; j < rect.nMeshY; j++)
			{
				if(type[i][j] != INSIDE) continue;
				pp = dy2 * (Phi[i - 1][j] + Phi[i + 1][j]) + dx2 *( Phi[i][j - 1] + Phi[i][j + 1]);
				pp /= 2.0 * (dx2 + dy2);
				error = fabs(pp - Phi[i][j]);
				if (error > maxError) maxError = error;
				Phi[i][j] = pp;
			}
		if (maxError < tolerance) break;
		cnt++;
	}


	for(i = 0; i <= rect.nMeshX; i++){
		for (j = 0; j <= rect.nMeshY; j++)
		{	        
			Vel[i][j].x = 0.5 * (Phi[i+1][j] - Phi[i-1][j]) / rect.delta.x;
			Vel[i][j].y = 0.5 * (Phi[i][j+1] - Phi[i][j-1]) / rect.delta.y;
			if(i == 0 || i == rect.nMeshX) Vel[i][j].x = 1.0;
			if(j == 0 || j == rect.nMeshY) Vel[i][j].y = 0.0;
			if(type[i][j] == OBS_LEFT || type[i][j] == OBS_RIGHT) Vel[i][j].x = 0.0;
			if(type[i][j] == OBS_TOP) Vel[i][j].y = 0.0;
			if(type[i][j] == CORNER_UL || type[i][j] == CORNER_UR) {Vel[i][j].x = 0.0; Vel[i][j].y = 0.0;}

		}
	}
}

void calculatePsi()
{
	int i, j;

	//境界条件と内部格子点の初期条件
	for(i = 0; i <= rect.nMeshX; i++)
	for(j = 0; j <= rect.nMeshY; j++)
	{
		if(type[i][j] >= BOTTOM) Psi[i][j] = 0.0;
		else if(type[i][j] == TOP)  Psi[i][j] = rect.size.y;//一様流れの流速を1とする
		else
		{//内点および入口・出口は線形補間
			if(i < nMeshX1 || i > nMeshX2)   Psi[i][j] = float(j) * rect.delta.y;
			else  Psi[i][j] = rect.size.y * float(j - nMeshY_ObsW) / (float)(rect.nMeshY - nMeshY_ObsW);
		}
	}

	//差分法
	int cnt = 0;
	float error = 0.0;
	float maxError = 0.0;
	float dx2 = rect.delta.x * rect.delta.x ;
	float dy2 = rect.delta.y * rect.delta.y ;
	
	float pp;
	while (cnt < iteration)
	{
		maxError = 0.0;
		for (i = 1; i < rect.nMeshX; i++)
			for (j = 0; j < rect.nMeshY; j++)
			{
				if(type[i][j] != INSIDE) continue;
				pp = dy2 * (Psi[i - 1][j] + Psi[i + 1][j]) + dx2 *( Psi[i][j - 1] + Psi[i][j + 1]);
				pp /= 2.0 * (dx2 + dy2);
				error = fabs(pp - Psi[i][j]);
				if (error > maxError) maxError = error;
				Psi[i][j] = pp;
			}
		if (maxError < tolerance) break;
		cnt++;
	}

	flagExecute = 0;

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
	{
		for (j = 1; j < rect.nMeshY; j++)
		{
			if(type[i][j] != INSIDE) continue;
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
		if(i <= nMeshX1 || i >= nMeshX2)
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
			glEnd();
		}
		else
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.obs_width);
			glVertex2f(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y * rect.size.y);
			glEnd();
		}

	}
	for(j = 1; j < rect.nMeshY; j++)
	{
		if(j > nMeshY_ObsW)
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
			glEnd();
		}
		else
		{
			glBegin(GL_LINES);
			glVertex2f(rect.left0.x , rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * rect.obs_left, rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * (rect.obs_left + rect.obs_thickness), rect.left0.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.left0.y + scale.y * (float)j * rect.delta.y);
			glEnd();
		}	
	}
}

void drawParticle(float dt)
{
	int k, kk;
	if(!flagFreeze && countInterval==0.0)
	{
		for(k = 0; k < numP0; k++)
		{//流入口に粒子発生
			kk = countP + k;
			p[kk].size = sizeParticle;
			p[kk].speedC = speedCoef;
			p[kk].pos.x = 0.01;	
			p[kk].pos.y = rect.size.y * getRandom(0.0, 1.0);

		}
		countP += numP0;
	}

	for(k = 0; k < MAX_PARTICLE; k++)
	{
		p[k].vel = getVelocityParticle(p[k].pos);

		if(!flagFreeze) p[k].update(dt);

		if(p[k].pos.x >= 0.0 && p[k].pos.x < rect.size.x 
			&& p[k].pos.y >= 0.0 && p[k].pos.y < rect.size.y) p[k].show(rect.left0, scale);
	}

	if(countP > MAX_PARTICLE - numP0) countP = 0;
	elapseTime2 += dt;
	countInterval += dt;
	if(countInterval > intervalP) {
		countInterval = 0.0;
	}
}

void mouse(int button, int state, int x, int y)
{
	if(state == GLUT_UP) return;
	//x, y はウィンドウ左上を基準にした物理座標
	//左下を(-1.0, -1.0)としたデカルト座標に変換
	float xx, yy;
	xx =  2.0 * (float)(x - win_width/2) / (float)win_width;
	yy = -2.0 * (float)(y - win_height/2) / (float)win_height; 
	//解析矩形領域左下を基準にした領域座標系(幅sizeX, 高さsizeY)に変換
	mPos.x = (xx - rect.left0.x) / scale.x;
	mPos.y = (yy - rect.left0.y) / scale.y;
//	printf("x=%d  y=%d xx = %f yy = %f mouseX = %f mouseY = %f \n", x, y, xx, yy, mouseX, mouseY);
	//格子番号を取得
	int I = 0, J = 0;
	for(int i = 0; i < rect.nMeshX; i++)
	{
		if((float)i * rect.delta.x < mPos.x && (float)(i+1) * rect.delta.x > mPos.x) I = i;
	}
	for(int j = 0; j < rect.nMeshY; j++)
	{
		if((float)j * rect.delta.y < mPos.y && (float)(j+1) * rect.delta.y > mPos.y) J = j;
	}

	p[0].pos = mPos;
	flagMouse = 1;
	flagStart = 0;
}

Vector2D getVelocityParticle(Vector2D pos)
{
	
	if( pos.x < 0.0 ) return Vector2D(-10.0, -10.0);
	if( pos.x > rect.size.x ) return Vector2D( 10.0, 10.0);
	if(pos.y < 0.0 ) Vector2D(-10.0, -10.0);
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

