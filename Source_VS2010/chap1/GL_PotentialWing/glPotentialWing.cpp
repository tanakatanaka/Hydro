/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元ポテンシャル流れ
一様流れ+ダブレット+自由渦の速度ポテンシャル，流れ関数，速度ベクトル
を写像変換しJoukowski profileがあるときのポテンシャル流れを求める
圧力のカラー表示
粒子を速度ベクトルで動かす
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
//void init();
void idle();
void display();
void drawRegion();
void resize(int w, int h);
void calculate();
void drawPotential();
void drawStream();
void drawVelocity();
void drawArrow();
void drawPressure();
void drawGrid();
void drawCylinder();
void drawParticle(float dt);
void mouse(int button, int state, int x, int y);
Vector2D getVelocityParticle(Vector2D pos);
void mapping(Vector2D& pos);
void drawPoint(Vector2D pos);

float Phi[NUM_MAX][NUM_MAX];//ポテンシャル
float Psi[NUM_MAX][NUM_MAX];//流れ関数
Vector2D Vel[NUM_MAX][NUM_MAX];//速度
Vector2D VelM[NUM_MAX][NUM_MAX];//写像後の速度
float Press[NUM_MAX][NUM_MAX];//圧力
Vector2D scale;//Window左下を(-1.0, -1.0)としたデカルト座標に表示するときのスケール調整
Vector2D mPos;//マウスがクリックされた位置

Particle2D p[MAX_PARTICLE];

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//表示モード
	glutInitDisplayMode(GLUT_RGBA  | GLUT_DOUBLE);
	//表示ウィンドウのサイズ
	glutInitWindowSize(win_width, win_height);
	//左上の位置
	glutInitWindowPosition(win_x0, win_y0);
	//ウィンドウ作成
	main_window = glutCreateWindow("GL_PotentialWing");//GLUIが対象とするウィンドウ
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
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glEnable(GL_POINT_SMOOTH);
	//表示ウィンドウに文字をレンダリングする準備
	setFontBitmaps();

	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	elapseTime2 = 0.0;//start後の総経過時間

	//初期化
	for(int i = 0; i <= rect.nMesh; i++)
		for(int j = 0; j <= rect.nMesh; j++) { Psi[i][j] = 0.0; Phi[i][j] = 0.0; }

	//粒子
	countP = 0;	
	for(int i = 0; i < MAX_PARTICLE; i++){
		p[i] = Particle2D();
		p[i].size = sizeParticle;
		p[i].speedC = speedCoef;
	}
/*	//[Start]をクリックしたとき移動させる粒子の初期条件
	for(int i = 0; i < numParticle; i++)
	{
		p[i].size = sizeParticle;
		p[i].speedC = speedCoef;

		p[i].pos.x = -rect.size.x / 2;	//粒子の初期位置(矩形領域中心を原点とした座標系）
		p[i].pos.y = rect.size.y * ((float)(i + 1) / (float)(numParticle + 1) - 0.5) ;
		p[i].pos.rotZ_deg(alpha);
	}*/

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
	scale.x = 2.0 * rect.scale * s1;
	scale.y = 2.0 * rect.scale * s2;
	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	//左下基準点
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;
	//格子間隔
	rect.delta.x = rect.size.x / (float)rect.nMesh;
	rect.delta.y = rect.size.y / (float)rect.nMesh;
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
//printf("dt = %f \n", dt);
	//カラーバッファのクリア
	glClear(GL_COLOR_BUFFER_BIT);

	alpha = (float)alpha_int;

	//計算領域描画
	drawRegion();

	if( flagExecute ) calculate();
	if( flagPressure) drawPressure(); 
	if( flagPotential ) drawPotential();
	if( flagStream ) drawStream();
	if( flagVelocity ) drawVelocity();
	if( flagCylinder == 1 ) drawCylinder();
	if( flagGrid ) drawGrid();

	if(flagStart) drawParticle(dt);//粒子移動
/*
	{
		for(int k = 0; k < numParticle; k++)
		{
			p[k].vel = getVelocityParticle(p[k].pos);

			if(!flagFreeze) p[k].update(dt);

			if(!flagMapping)
			{
				if(p[k].pos.x >= -rect.size.x/2.0 && p[k].pos.x < rect.size.x/2.0 
					&& p[k].pos.y >= -rect.size.y/2.0 && p[k].pos.y < rect.size.y/2.0) p[k].draw(rect.pos0, scale);
			}
			else
			{
				Vector2D pos = p[k].pos;
				mapping(pos);
				drawPoint(pos);
			}

		}
	}*/

	if(!flagStart && flagMouse)
	{
		p[0].vel = getVelocityParticle(p[0].pos);
		if(!flagFreeze) p[0].update(dt);
		if(!flagMapping)
		{
			if(p[0].pos.x >= -rect.size.x/2.0 && p[0].pos.x < rect.size.x/2.0 
				&& p[0].pos.y >= -rect.size.y/2.0 && p[0].pos.y < rect.size.y/2.0) p[0].show(rect.pos0, scale);
		}
		else
		{
			Vector2D pos = p[0].pos;
			mapping(pos);
			drawPoint(pos);
		}
		
	}
	if(flagParameter)
	{
		drawParam("framerate=%d[fps]", fps0, -0.9, 0.9);
		drawParam("timestep=%1.5f[s]", dt, -0.2, 0.9);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 0.5, 0.9);
		drawParam("alpha=%3.1f[deg]", alpha, -0.9,0.8);
	}

	//バッファの切り替え
	glutSwapBuffers();
}

void drawRegion()
{
	//線幅
	glLineWidth(1.0);
	//色
	glColor3f(0.0, 0.0, 0.0);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//GL_FILLにすると塗りつぶす

	float sx = scale.x * rect.size.x / 2.0;
	float sy = scale.y * rect.size.y / 2.0;

	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y + sy);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y + sy);
	glEnd();

	if(flagCoord)
	{
		//x-y座標
		glLineWidth(1.0);
		glColor3f(0.0, 0.0, 1.0);

		//x座標
		glBegin(GL_LINES);
		glVertex2f(rect.left0.x, rect.pos0.y );
		glVertex2f(rect.left0.x + scale.x * rect.size.x, rect.pos0.y );
		//y座標
		glVertex2f(rect.pos0.x, rect.left0.y);
		glVertex2f(rect.pos0.x, rect.left0.y + scale.y * rect.size.y);
		glEnd();
	}

}
void resize(int w, int h)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport(tx, ty, tw, th);
	//表示ウィンドウのサイズ
	win_width = tw;//w;
	win_height = th;//h;
	win_ratio = (float)win_height / (float)win_width;

	init();
	glutPostRedisplay();
}

void calculate()
{
	int i, j;
    //ポテンシャル，流れ関数のクリア
    for (i = 0; i <= rect.nMesh; i++)
    {
        for (j = 0; j <= rect.nMesh; j++)
        {
            Phi[i][j] = 0.0;
            Psi[i][j] = 0.0;
			Vel[i][j] = Vector2D();
        }
    }
	
	Vector2D z ;
	float rr = 0.0, c1 = 0.0, c2 = 0.0;;
	float r2 = 0.0;//２乗距離
	float r4 = 0.0;//４乗距離
	float radius2 = radCylinder * radCylinder;//円柱の半径の２乗
	float ang = alpha * DEG_TO_RAD; 
	float maxVel2 = maxVelocity * maxVelocity;

	for (i = 0; i <= rect.nMesh; i++)
	{
		z.x = rect.delta.x * (float)(i - rect.nMesh / 2);//中心のポテンシャルを0
		for (j = 0; j <= rect.nMesh; j++)
		{
			z.y = rect.delta.y * (float)(j - rect.nMesh / 2);
			//一様流れ
			{
				Phi[i][j] += flowVelocity * (z.x * cos(ang) + z.y * sin(ang));
				Psi[i][j] += flowVelocity * (z.y * cos(ang) - z.x * sin(ang));
				Vel[i][j].x += flowVelocity * cos(ang);
				Vel[i][j].y += flowVelocity * sin(ang);
			}

			//ダブレット（円柱）
			{
                if (z.x == 0.0 && z.y == 0.0)
                {//原点は対数的特異点
                    z = rect.delta / 1000.0f;
                }
				rr = z.magnitude();
				r2 = z.magnitude2();
				r4 = r2 * r2;

                Phi[i][j] += flowVelocity * radius2 *(cos(ang) * z.x + sin(ang) * z.y) / r2;
                Psi[i][j] += flowVelocity * radius2 *(sin(ang) * z.x - cos(ang) * z.y) / r2;
				c1 = flowVelocity * radius2 * ( z.x * z.x - z.y * z.y ) / r4;
				c2 = 2.0 * flowVelocity * radius2 * z.x * z.y / r4;
				Vel[i][j].x -= c1 * cos(ang) + c2 * sin(ang);
				Vel[i][j].y += c1 * sin(ang) - c2 * cos(ang);
            }

			if (flagVoltex)//うず
			{
                if (z.x == 0.0f && z.y == 0.0f) 
				{ 
                    z = rect.delta / 1000.0f;
				}
				r2 = z.magnitude2();
                Psi[i][j] -= Gamma * (float)(log(r2) / (4.0 * M_PI));
                Phi[i][j] += Gamma * (float)atan2(z.y, z.x) / (float)(2.0 * M_PI);
				Vel[i][j].x -= Gamma * z.y / r2 / (2.0 * M_PI);
				Vel[i][j].y += Gamma * z.x / r2 / (2.0 * M_PI);
            }
			Press[i][j] = 1.0 - Vel[i][j].magnitude2() / maxVel2;
			if(Press[i][j] < 0.0) Press[i][j] = 0.0;
        }
    }

	flagStart = 0;
}

void drawParticle(float dt)
{
	int k, kk;
	if(!flagFreeze && countInterval == 0.0)
	{
		for(k = 0; k < numP0; k++)
		{
			kk = countP + k;
			p[kk].size = sizeParticle;
			p[kk].speedC = speedCoef;
			p[kk].pos.x = -rect.size.x / 2;	//粒子の初期位置(矩形領域中心を原点とした座標系）
			p[kk].pos.y = rect.size.y * getRandom(-0.5, 0.5);

		}
		countP += numP0;
	}

	for(k = 0; k < MAX_PARTICLE; k++)
	{
		p[k].vel = getVelocityParticle(p[k].pos);

		if(!flagFreeze) p[k].update(dt);
			if(!flagMapping)
			{
				if(p[k].pos.x >= -rect.size.x/2.0 && p[k].pos.x < rect.size.x/2.0 
					&& p[k].pos.y >= -rect.size.y/2.0 && p[k].pos.y < rect.size.y/2.0) p[k].show(rect.pos0, scale);
			}
			else
			{
				Vector2D pos = p[k].pos;
				mapping(pos);
				drawPoint(pos);
			}

//		if(p[k].pos.x >= 0.0 && p[k].pos.x < rect.size.x 
//			&& p[k].pos.y >= 0.0 && p[k].pos.y < rect.size.y) p[k].show(rect.left0, scale);
	}
	if(countP > MAX_PARTICLE - numP0) countP = 0;
	elapseTime2 += dt;
	countInterval += dt;
	if(countInterval > intervalP) {
		countInterval = 0.0;
	}
}

void drawPotential()
{
	float minPhi =  - flowVelocity * range;
	float maxPhi =    flowVelocity * range;
	float dp0 = (maxPhi - minPhi) / (float)(nLine +1);//ポテンシャル間隔

	glLineWidth(2.0);
	glColor3f(1.0, 0.0, 0.0);

	float pp;
	Vector2D pos, pos1, pos2, dir;
	Vector2D pos0 = rect.size / 2.0;//計算領域の中心
	float rad, rr1, rr2, eps = radCylinder / 50.0;

	int i, j, k;

    for (k = 0; k < nLine; k++)
    {
        pp = (float)(k+1) * dp0 + minPhi;
        for (i = 0; i < rect.nMesh; i++)
        {
            for (j = 0; j < rect.nMesh; j++)
            {

				pos1 = Vector2D(-1.0, -1.0);
				pos2 = Vector2D(-1.0, -1.0);

                //格子の左辺を調査
                if ((pp >= Phi[i][j] && pp <= Phi[i][j + 1]) || (pp <= Phi[i][j] && pp >= Phi[i][j + 1]))
                {
                    pos1.x = (float)i;
                    pos1.y = (float)j + (pp - Phi[i][j]) / (Phi[i][j + 1] - Phi[i][j]);
                }

                //上辺を調査
                if ((pp >= Phi[i][j + 1] && pp <= Phi[i + 1][j + 1]) || (pp <= Phi[i][j + 1] && pp >= Phi[i + 1][j + 1]))
                {
                    pos.x = (float)i + (pp - Phi[i][j + 1]) / (Phi[i + 1][j + 1] - Phi[i][j + 1]);
                    pos.y = (float)(j + 1);
                    if (pos1.x < 0.0 || pos1.y < 0.0)
                    {
						pos1 = pos;
                    }
                    else
                    {
						pos2 = pos;
						goto drawLine;
                    }
                }
                //右辺を調査
                if ((pp >= Phi[i + 1][j] && pp <= Phi[i + 1][j + 1]) || (pp <= Phi[i + 1][j] && pp >= Phi[i + 1][j + 1]))
                {
                    pos.x = (float)(i + 1) ;
                    pos.y = (float)j  + (pp - Phi[i + 1][j]) / (Phi[i + 1][j + 1] - Phi[i + 1][j]);

                    if (pos1.x < 0.0 || pos1.y < 0.0)
                    {
						pos1 = pos;
                    }
                    else
                    {
						pos2 = pos; goto drawLine;
                    }
                }
                //下辺を調査

                if ((pp >= Phi[i][j] && pp <= Phi[i + 1][j]) || (pp <= Phi[i][j] && pp >= Phi[i + 1][j]))
                {
                    pos2.x = (float)i + (pp - Phi[i][j]) / (Phi[i + 1][j] - Phi[i][j]);
                    pos2.y = (float)j;
                }
            drawLine: ;
                if (pos1.x < 0.0 || pos1.y < 0.0 || pos2.x < 0.0 || pos2.y < 0.0) continue;
				pos1 = pos1 * rect.delta - pos0;//矩形領域中心を原点とした座標系に変換
				pos2 = pos2 * rect.delta - pos0;

				rr1 = pos1.magnitude();
				rr2 = pos2.magnitude(); 

				if( rr1 < radCylinder && rr2 < radCylinder) continue; 
				
				//円柱内部に残った格子点を円柱上に移動
				int count = 0;
				if(rr1 < radCylinder)
				{
					dir = pos1 / rr1;
					rad = rr1;
					while( rad < radCylinder || count > 100)
					{
						rad += eps;
						count++;
					}
					pos1 = dir * rad;
				}
				if(rr2 < radCylinder)
				{
					dir = pos2 / rr2;
					rad = rr2;
					while( rad < radCylinder || count > 100)
					{
						rad += eps;
						count++;
					}
					pos2 = dir * rad;
				}

				if(flagMapping)
				{
					mapping(pos1);
					mapping(pos2);
				}

				glBegin(GL_LINES);
				glVertex2f(rect.pos0.x + scale.x * pos1.x, rect.pos0.y + scale.y * pos1.y);
				glVertex2f(rect.pos0.x + scale.x * pos2.x, rect.pos0.y + scale.y * pos2.y);
				glEnd();
            }
        }
    }
}

void drawStream()
{
	float minPsi =  - flowVelocity * range;
	float maxPsi =    flowVelocity * range;
	float dp0 = (maxPsi - minPsi) / (float)(nLine +1);//ポテンシャル間隔

	glLineWidth(2.0);
	glColor3f(0.0, 0.0, 0.0);

	float pp;
	Vector2D pos, pos1, pos2;
	Vector2D pos0 = rect.size / 2.0;//計算領域の中心

	int i, j, k;

    for (k = 0; k < nLine; k++)
    {
        pp = (float)(k+1) * dp0 + minPsi;
        for (i = 0; i < rect.nMesh; i++)
        {
            for (j = 0; j < rect.nMesh; j++)
            {

				pos1 = Vector2D(-1.0, -1.0);
				pos2 = Vector2D(-1.0, -1.0);

                //格子の左辺を調査
                if ((pp >= Psi[i][j] && pp <= Psi[i][j + 1]) || (pp <= Psi[i][j] && pp >= Psi[i][j + 1]))
                {
                    pos1.x = (float)i;
                    pos1.y = (float)j + (pp - Psi[i][j]) / (Psi[i][j + 1] - Psi[i][j]);
                }

                //上辺を調査
                if ((pp >= Psi[i][j + 1] && pp <= Psi[i + 1][j + 1]) || (pp <= Psi[i][j + 1] && pp >= Psi[i + 1][j + 1]))
                {
                    pos.x = (float)i + (pp - Psi[i][j + 1]) / (Psi[i + 1][j + 1] - Psi[i][j + 1]);
                    pos.y = (float)(j + 1);
                    if (pos1.x < 0.0 || pos1.y < 0.0)
                    {
						pos1 = pos;
                    }
                    else
                    {
						pos2 = pos;
						goto drawLine;
                    }
                }
                //右辺を調査
                if ((pp >= Psi[i + 1][j] && pp <= Psi[i + 1][j + 1]) || (pp <= Psi[i + 1][j] && pp >= Psi[i + 1][j + 1]))
                {
                    pos.x = (float)(i + 1) ;
                    pos.y = (float)j  + (pp - Psi[i + 1][j]) / (Psi[i + 1][j + 1] - Psi[i + 1][j]);

                    if (pos1.x < 0.0 || pos1.y < 0.0)
                    {
						pos1 = pos;
                    }
                    else
                    {
						pos2 = pos; goto drawLine;
                    }
                }
                //下辺を調査

                if ((pp >= Psi[i][j] && pp <= Psi[i + 1][j]) || (pp <= Psi[i][j] && pp >= Psi[i + 1][j]))
                {
                    pos2.x = (float)i + (pp - Psi[i][j]) / (Psi[i + 1][j] - Psi[i][j]);
                    pos2.y = (float)j;
                }
            drawLine: ;
                if (pos1.x < 0.0 || pos1.y < 0.0 || pos2.x < 0.0 || pos2.y < 0.0) continue;
				pos1 = pos1 * rect.delta - pos0;//矩形領域中心を原点とした座標系に変換
				pos2 = pos2 * rect.delta - pos0;

				if( pos1.magnitude() < radCylinder || pos2.magnitude() < radCylinder) continue; 
				
				if(flagMapping)
				{//計算空間から物理空間へ
					mapping(pos1);
					mapping(pos2);
				}

				glBegin(GL_LINES);
				glVertex2f(rect.pos0.x + scale.x * pos1.x, rect.pos0.y + scale.y * pos1.y);
				glVertex2f(rect.pos0.x + scale.x * pos2.x, rect.pos0.y + scale.y * pos2.y);
				glEnd();
            }
        }
    }
}

void drawVelocity()
{
	int i, j;

	float rr;//中心からの距離
	Vector2D pos, pos1, pos2;//, vel;

	//描画
	float theta, mag;
	for(i = 0; i <= rect.nMesh; i++)
	{
		for (j = 0; j <= rect.nMesh; j++)
		{
			//矩形領域中心を原点とした座標系
			pos.x = rect.delta.x * (float)i - rect.size.x / 2.0;
			pos.y = rect.delta.y * (float)j - rect.size.y / 2.0;
			rr = pos.magnitude();
			if( rr < radCylinder) continue;//円柱の内部は表示しない

			//if(Vel[i][j].magnitude() > 5.0) continue;//中心付近の極端に大きな場合を除く
			
			if(flagMapping)
			{
				pos1 = pos - Vel[i][j] / 1000.0;//速度ベクトルの始点(誤差を小さくするため1000で割る）
				pos2 = pos + Vel[i][j] / 1000.0;//速度ベクトルの終点
				mapping(pos);
				mapping(pos1);
				mapping(pos2);
				VelM[i][j] = (pos2 - pos1) *1000.0;//変換後の速度ベクトル
			}
			
			if(flagMapping)
			{
				theta = atan2(VelM[i][j].y, VelM[i][j].x) * RAD_TO_DEG;
				mag = VelM[i][j].magnitude();//速度の大きさ
			}
			else
			{
				theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;
				mag = Vel[i][j].magnitude();//速度の大きさ
			}
			glPushMatrix();
			glTranslatef(rect.pos0.x + scale.x * pos.x, rect.pos0.y + scale.y * pos.y, 0.0);
			glRotatef(theta, 0.0, 0.0, 1.0);
			glScalef(scaleArrow * mag, scaleArrow * mag, 0.0);
			drawArrow();
			glPopMatrix();
		}
	}
}
void drawArrow()
{
	if(arrow == 0) widthArrow = 1.0;
	else widthArrow = 2.0;
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

void drawPressure()
{
	float minPress = 0.0;
	float maxPress = 1.0;

	glPolygonMode(GL_FRONT, GL_FILL);


	Vector2D pos[4], dir ;
	float pp[4], rr[4], gg[4], bb[4];
	float rad, eps = radCylinder / 50.0;
	int i, j, k;

    for (i = 0; i < rect.nMesh; i++)
    {
        for (j = 0; j < rect.nMesh; j++)
        {

			pos[0].x = (float)i * rect.delta.x;     pos[0].y = (float)j * rect.delta.y;
			pos[1].x = (float)(i+1) * rect.delta.x; pos[1].y = (float)j * rect.delta.y;
			pos[2].x = (float)(i+1) * rect.delta.x; pos[2].y = (float)(j+1) * rect.delta.y;
			pos[3].x = (float)i * rect.delta.x;     pos[3].y = (float)(j+1) * rect.delta.y;
			for(k = 0; k < 4; k++) pos[k] -= rect.size / 2.0;

			//４個とも円柱内部ならば表示しない
			if(pos[0].magnitude() < radCylinder && pos[1].magnitude() < radCylinder && 
				pos[2].magnitude() < radCylinder && pos[3].magnitude() < radCylinder) continue;

			//円柱内部に残った格子点を円柱上に移動
			int count = 0;
			for(k = 0; k < 4; k++)
			{
				rad = pos[k].magnitude();
				if(rad < radCylinder)
				{
					dir = pos[k] / rad;
					count = 0;
					while( rad < radCylinder || count > 100)
					{
						rad += eps;
						count++;
					}
					pos[k] = dir * rad;
				}
			}

			pp[0] = Press[i][j]; pp[1] = Press[i+1][j]; 
			pp[2] = Press[i+1][j+1]; pp[3] = Press[i][j+1];

			pp[0] = Press[i][j]; pp[1] = Press[i+1][j]; pp[2] = Press[i+1][j+1]; pp[3] = Press[i][j+1];

			for(k = 0; k < 4; k++)
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

/*
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
*/
			if(flagMapping)
			{
				for(k = 0; k < 4; k++) mapping(pos[k]);
			}
			glBegin(GL_QUADS);
			glColor3f(rr[0], gg[0], bb[0]);
			glVertex2f(rect.pos0.x + scale.x * pos[0].x, rect.pos0.y + scale.y * pos[0].y);
			glColor3f(rr[1], gg[1], bb[1]);
			glVertex2f(rect.pos0.x + scale.x * pos[1].x, rect.pos0.y + scale.y * pos[1].y);
			glColor3f(rr[2], gg[2], bb[2]);
			glVertex2f(rect.pos0.x + scale.x * pos[2].x, rect.pos0.y + scale.y * pos[2].y);
			glColor3f(rr[3], gg[3], bb[3]);
			glVertex2f(rect.pos0.x + scale.x * pos[3].x, rect.pos0.y + scale.y * pos[3].y);
			glEnd();
        }
    }
    
}

void drawCylinder()
{
	int nMesh = 30;
	float theta0 = 2.0 * M_PI / (float)nMesh;
	Vector2D pos1, pos2;
	
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor3f(0.9, 0.9, 0.9);

	int i;
	if(!flagMapping)
	{
		for(i = 0;i < nMesh; i++)
		{
			pos1.x = radCylinder * cos((float)i * theta0);
			pos1.y = radCylinder * sin((float)i * theta0);
			pos2.x = radCylinder * cos((float)(i+1) * theta0);
			pos2.y = radCylinder * sin((float)(i+1) * theta0);
		
			glBegin(GL_TRIANGLES);
			glVertex2f(rect.pos0.x, rect.pos0.y);
			glVertex2f(rect.pos0.x + scale.x * pos1.x, rect.pos0.y + scale.y * pos1.y);
			glVertex2f(rect.pos0.x + scale.x * pos2.x, rect.pos0.y + scale.y * pos2.y);
			glEnd();
		}
	}

	//輪郭
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(4.0);
	glColor3f(0.0, 0.0, 0.0);
	for(i = 0;i < nMesh; i++)
	{
		pos1.x = radCylinder * cos((float)i * theta0);
		pos1.y = radCylinder * sin((float)i * theta0);
		pos2.x = radCylinder * cos((float)(i+1) * theta0);
		pos2.y = radCylinder * sin((float)(i+1) * theta0);
		if(flagMapping)
		{//物理面のplane
			mapping(pos1);
			mapping(pos2);
		}
		glBegin(GL_LINES);
		glVertex2f(rect.pos0.x + scale.x * pos1.x, rect.pos0.y + scale.y * pos1.y);
		glVertex2f(rect.pos0.x + scale.x * pos2.x, rect.pos0.y + scale.y * pos2.y);
		glEnd();
	}	
}

void drawGrid()
{
	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 1.0);

	Vector2D left = rect.pos0 - scale * rect.size / 2.0;
	Vector2D pos[4], dir ;
	float rad, eps = radCylinder / 50.0;

	int i, j, k;

	if(!flagMapping)//ζ-plane
	{
		for(i = 0; i < rect.nMesh; i++)
		{
			glBegin(GL_LINES);
			glVertex2f(left.x + scale.x * (float)i * rect.delta.x, left.y);
			glVertex2f(left.x + scale.x * (float)i * rect.delta.x, left.y + scale.y * rect.size.y);
			glEnd();
		}
		for(j = 0; j < rect.nMesh; j++)
		{
			glBegin(GL_LINES);
			glVertex2f(left.x, left.y + scale.y * (float)j * rect.delta.y);
			glVertex2f(left.x + scale.x * rect.size.x, left.y + scale.y * (float)j * rect.delta.y);
			glEnd();
		}
	}
	else//物理平面(z-plane)にマッピング
	{
		for(i = 0; i < rect.nMesh; i++)
		{
			for(j = 0; j < rect.nMesh; j++)
			{

				pos[0].x = (float)i * rect.delta.x;     pos[0].y = (float)j * rect.delta.y;
				pos[1].x = (float)(i+1) * rect.delta.x; pos[1].y = (float)j * rect.delta.y;
				pos[2].x = (float)(i+1) * rect.delta.x; pos[2].y = (float)(j+1) * rect.delta.y;
				pos[3].x = (float)i * rect.delta.x;     pos[3].y = (float)(j+1) * rect.delta.y;
				for(k = 0; k < 4; k++) pos[k] -= rect.size / 2.0;

				//４個とも円柱内部ならば表示しない
				if(pos[0].magnitude() < radCylinder && pos[1].magnitude() < radCylinder && 
					pos[2].magnitude() < radCylinder && pos[3].magnitude() < radCylinder) continue;

				//円柱内部に残った格子点を円柱上に移動
				int count = 0;
				for(k = 0; k < 4; k++)
				{
					rad = pos[k].magnitude();
					if(rad < radCylinder)
					{
						dir = pos[k] / rad;
						count = 0;
						while( rad < radCylinder || count > 100)
						{
							rad += eps;
							count++;
						}
	//printf("count=%d rad=%f \n", count, rad);
						pos[k] = dir * rad;
					}
				}

				for(int k = 0; k < 4; k++) mapping(pos[k]);

				glBegin(GL_LINES);
				glVertex2f(rect.pos0.x + scale.x * pos[0].x, rect.pos0.y + scale.y * pos[0].y);
				glVertex2f(rect.pos0.x + scale.x * pos[1].x, rect.pos0.y + scale.y * pos[1].y);
				
				glVertex2f(rect.pos0.x + scale.x * pos[1].x, rect.pos0.y + scale.y * pos[1].y);
				glVertex2f(rect.pos0.x + scale.x * pos[2].x, rect.pos0.y + scale.y * pos[2].y);

				glVertex2f(rect.pos0.x + scale.x * pos[2].x, rect.pos0.y + scale.y * pos[2].y);
				glVertex2f(rect.pos0.x + scale.x * pos[3].x, rect.pos0.y + scale.y * pos[3].y);

				glVertex2f(rect.pos0.x + scale.x * pos[3].x, rect.pos0.y + scale.y * pos[3].y);
				glVertex2f(rect.pos0.x + scale.x * pos[1].x, rect.pos0.y + scale.y * pos[1].y);
				glEnd();
			}
		}	
	}
}

void mouse(int button, int state, int x, int y)
{//ｚ平面に写像しているときは中心付近で大きな誤差が生じる
	if(state == GLUT_UP) return;
	//x, y はウィンドウ左上を基準にした物理座標
	//左下を(-1.0, -1.0)としたデカルト座標に変換
	float xx, yy;
	xx =  2.0 * (float)(x - win_width/2) / (float)win_width;
	yy = -2.0 * (float)(y - win_height/2) / (float)win_height; 
	//解析矩形領域中心を基準にした領域座標系(幅rect.size.x, 高さrect.size.y)に変換
	mPos.x = (xx - rect.pos0.x) / scale.x;
	mPos.y = (yy - rect.pos0.y) / scale.y;

	p[0].pos = mPos;
	p[0].pos.rotZ_deg(alpha);
	flagMouse = 1;
	flagStart = 0;
}

Vector2D getVelocityParticle(Vector2D pos)
{
	//このposは矩形領域中心を原点とした座標系
	//左端下端位置を原点とした座標系に変換
	pos += rect.size / 2.0;

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

void mapping(Vector2D& pos)
{
	//計算平面における拡大・平行移動
	pos *= sqrt((1.0 - xi0)*(1.0 - xi0) + eta0 * eta0);//円を拡大し（xi0,eta0はR=radCylinderで割った値）
	pos += Vector2D(xi0, eta0) * radCylinder;//平行移動
	//計算平面から物理平面への座標変換
	float rad = pos.magnitude();
	float theta = atan2(pos.y, pos.x);

	pos.x = (rad + radCylinder * radCylinder / rad) * cos(theta);
	pos.y = (rad - radCylinder * radCylinder / rad) * sin(theta);
	
	//逆回転し一様流れを水平にする
	pos.rotZ_deg(-alpha);
}


void drawPoint(Vector2D pos)
{
	//写像変換された物理面での粒子描画
	glColor3f(0.0, 0.0, 0.5);
	glPointSize(p[0].size);
	glBegin(GL_POINTS);
		glVertex2f(rect.pos0.x + pos.x * scale.x, rect.pos0.y + pos.y * scale.y);
	glEnd();
}

