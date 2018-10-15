/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元ポテンシャル流れ
一様流れ+ダブレット+自由渦の速度ポテンシャル，流れ関数，速度ベクトル
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
void drawContour(float PP[][NUM_MAX], Color color);
void drawVelocity();
void drawArrow(int i, int j);
void drawPressureI();
void drawPressureC();
void drawGrid();
void drawCylinder();
void drawParticle(float dt);
void mouse(int button, int state, int x, int y);
Vector2D getVelocityParticle(Vector2D pos);

float Phi[NUM_MAX][NUM_MAX];//ポテンシャル
float Psi[NUM_MAX][NUM_MAX];//流れ関数
//格子点の速度
Vector2D Vel[NUM_MAX][NUM_MAX];//速度
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
	main_window = glutCreateWindow("GL_PotentialFlow3");//GLUIが対象とするウィンドウ
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
	//glClearColor(0.0, 0.0, 0.0, 1.0);
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

	//粒子
	countP = 0;	
	for(int i = 0; i < MAX_PARTICLE; i++){
		p[i] = Particle2D();
		p[i].size = sizeParticle;
		p[i].speedC = speedCoef;
	}

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

	alpha = (float)alpha_int;
	//計算領域描画
	drawRegion();

	if( flagExecute ) calculate();
	if( cylinder== 0 ) drawCylinder();
	if( flagPressureColor) drawPressureC(); 
	if( flagPressureIsobar) drawPressureI(); 
	if( flagPotential ) drawContour(Phi, RED);
	if( flagStream ) drawContour(Psi, BLACK);
	if( flagVelocity ) drawVelocity();
	if( cylinder == 1 ) drawCylinder();
	if( flagGrid ) drawGrid();

	if(flagStart) drawParticle(dt);

	if(!flagStart && flagMouse)//マウスで指定した位置から1個だけ移動
	{
		p[0].size = sizeParticle;
		p[0].speedC = speedCoef;
		p[0].vel = getVelocityParticle(p[0].pos);
		if(!flagFreeze) p[0].update(dt);
		if(p[0].pos.x >= 0.0 && p[0].pos.x < rect.size.x && p[0].pos.y >= 0.0 && p[0].pos.y < rect.size.y) p[0].show(rect.left0, scale);
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
	//全体枠
//	glColor3f(0.0, 0.0, 0.0);
	//線幅
	glLineWidth(1.0);
	//色
	glColor3f(0.0, 0.0, 0.0);
	//モード
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//GL_FILLにすると塗りつぶす

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
	glBegin(GL_QUADS);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y - sy);
	glVertex2f(rect.pos0.x + sx, rect.pos0.y + sy);
	glVertex2f(rect.pos0.x - sx, rect.pos0.y + sy);
	glEnd();

	//左下基準点
	rect.left0.x = rect.pos0.x - sx;
	rect.left0.y = rect.pos0.y - sy;
	//格子間隔
	rect.delta.x = rect.size.x / (float)rect.nMesh;
	rect.delta.y = rect.size.y / (float)rect.nMesh;

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
void resize(int w, int h)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport(tx, ty, tw, th);
	//表示ウィンドウのサイズ
	win_width = tw;//w;
	win_height = th;//h;
	win_ratio = (float)win_height / (float)win_width;

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
			if( flagUniform )
			{
				Phi[i][j] = flowVelocity * (z.x * cos(ang) + z.y * sin(ang));
				Psi[i][j] = flowVelocity * (z.y * cos(ang) - z.x * sin(ang));
	//if(i==5)printf("j = %d  Psi = %f \n", j, Psi[i][j]);
				Vel[i][j].x = flowVelocity * cos(ang);
				Vel[i][j].y = flowVelocity * sin(ang);
			}

			if (flagDoublet) //２重吹き出し
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

			if (flagVortex)//うず
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
			//Pressure
			{
				Press[i][j] = 1.0 - Vel[i][j].magnitude2() / maxVel2;
				if(Press[i][j] < 0.0) Press[i][j] = 0.0;
			}
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
			p[kk].pos.x = 0.0;
			/*if(Q_Value > 0.0)
			{
				p[kk].pos.y = rect.size.y * getRandom(0.0, 1.0);
				p[kk].color = BLUE;
			}
			else*/
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

void drawContour(float PP[][NUM_MAX], Color color)
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
	
	float maxP =  flowVelocity * range;
	float minP = -flowVelocity * range;
	float dp0 = (maxP - minP) / (float)(nLine +1);//流線間隔
	float pp;

	float x, y, x1, y1, x2, y2;
	int i, j, k;

    for (k = 0; k < nLine; k++)
    {
        pp = (float)(k+1) * dp0 + minP;
        for (i = 0; i < rect.nMesh; i++)
        {
            for (j = 0; j < rect.nMesh; j++)
            {
                
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

	float rr;//中心からの距離
	Vector2D pos;

	//描画
	float theta, mag;
	for(i = 1; i < rect.nMesh; i++)
	{
		pos.x = rect.delta.x * (float)i - rect.size.x / 2.0;

		for (j = 1; j < rect.nMesh; j++)
		{
			pos.y = rect.delta.y * (float)j - rect.size.y / 2.0;
			rr = pos.magnitude();
			//if( rr < radCylinder) continue;//円柱の内部は表示しない

			if(Vel[i][j].magnitude() > 5.0) continue;//中心付近の極端に大きな場合を除く
			theta = atan2(Vel[i][j].y, Vel[i][j].x) * RAD_TO_DEG;
			mag = Vel[i][j].magnitude();//sqrt(velX[i][j] * velX[i][j] + velY[i][j] * velY[i][j]);//速度の大きさ
			glPushMatrix();
			glTranslatef(rect.left0.x + scale.x * (float)i * rect.delta.x, rect.left0.y + scale.y *  (float)j * rect.delta.y, 0.0);
			glRotatef(theta, 0.0, 0.0, 1.0);
			glScalef(scaleArrow * mag, scaleArrow * mag, 0.0);
			drawArrow(i, j);
			glPopMatrix();
		}
	}
}
void drawArrow(int i, int j)
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

void drawPressureI()
{
	float minPress = 0.0;
	float maxPress = 1.0;
	float dp0 = (maxPress - minPress) / (float)(nLine +1);

	glLineWidth(2.0);
	glColor3f(0.0, 0.0, 0.0);

	float pp;
	float rr;//中心からの距離
	Vector2D pos;

	float x, y, x1, y1, x2, y2;
	int i, j, k;

    for (k = 0; k < nLine; k++)
    {
		pp = (float)(k+1) * dp0 + minPress;
        for (i = 0; i < rect.nMesh; i++)
        {
			pos.x = rect.delta.x * (float)i - rect.size.x / 2.0;
            for (j = 0; j < rect.nMesh; j++)
            {
				pos.y = rect.delta.y * (float)j - rect.size.y / 2.0;
				rr = pos.magnitude();
				//if( rr < radCylinder) continue;//円柱の内部は表示しない

				x1 = -1.0f; x2 = -1.0f; y1 = -1.0f; y2 = -1.0f;

                //格子の左辺を調査
                if ((pp >= Press[i][j] && pp < Press[i][j + 1]) || (pp < Press[i][j] && pp > Press[i][j + 1]))
                {
                    x1 = (float)i;
                    y1 = (float)j + (pp - Press[i][j]) / (Press[i][j + 1] - Press[i][j]);
                }

                //上辺を調査
                if ((pp >= Press[i][j + 1] && pp < Press[i + 1][j + 1]) || (pp < Press[i][j + 1] && pp > Press[i + 1][j + 1]))
                {
                    x = (float)i + (pp - Press[i][j + 1]) / (Press[i + 1][j + 1] - Press[i][j + 1]);
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
                if ((pp >= Press[i + 1][j] && pp < Press[i + 1][j + 1]) || (pp < Press[i + 1][j] && pp > Press[i + 1][j + 1]))
                {
                    x = (float)(i + 1) ;
                    y = (float)j  + (pp - Press[i + 1][j]) / (Press[i + 1][j + 1] - Press[i + 1][j]);

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

                if ((pp >= Press[i][j] && pp < Press[i + 1][j]) || (pp < Press[i][j] && pp > Press[i + 1][j]))
                {
                    x2 = (float)i + (pp - Press[i][j]) / (Press[i + 1][j] - Press[i][j]);
                    y2 = (float)j;
                }
            drawLine: ;
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

void drawPressureC()
{
	float minPress = 0.0;
	float maxPress = 1.0;

	glPolygonMode(GL_FRONT, GL_FILL);

	float x1, y1, x2, y2, x3, y3, x4, y4;
	float pp[4], rr[4], gg[4], bb[4];
	int i, j, k;

    for (i = 0; i < rect.nMesh; i++)
    {
        for (j = 0; j < rect.nMesh; j++)
        {

			x1 = (float)i * rect.delta.x;     y1 = (float)j * rect.delta.y;
			x2 = (float)(i+1) * rect.delta.x; y2 = (float)j * rect.delta.y;
			x3 = (float)(i+1) * rect.delta.x; y3 = (float)(j+1) * rect.delta.y;
			x4 = (float)i * rect.delta.x;     y4 = (float)(j+1) * rect.delta.y;
			
			pp[0] = Press[i][j]; pp[1] = Press[i+1][j]; pp[2] = Press[i+1][j+1]; pp[3] = Press[i][j+1];

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


void drawCylinder()
{
	int nMesh = 30;
	float theta0 = 2.0 * M_PI / (float)nMesh;
	Vector2D pos0, pos1, pos2;
	pos0 = Vector2D(0.0, 0.0);//原点

	glPolygonMode(GL_FRONT, GL_FILL);
	glColor3f(0.9, 0.9, 0.9);

	int i;
	for(i = 0; i < nMesh; i++)
	{
		pos1.x = radCylinder * cos((float)i * theta0);
		pos1.y = radCylinder * sin((float)i * theta0);
		pos2.x = radCylinder * cos((float)(i+1) * theta0);
		pos2.y = radCylinder * sin((float)(i+1) * theta0);
	
//printf("i=%d,  x=%f, y=%f \n", i, pos1.x, pos1.y);
		
		glBegin(GL_TRIANGLES);
		glVertex2f(rect.pos0.x + scale.x * pos0.x, rect.pos0.y + scale.y * pos0.y);
		glVertex2f(rect.pos0.x + scale.x * pos1.x, rect.pos0.y + scale.y * pos1.y);
		glVertex2f(rect.pos0.x + scale.x * pos2.x, rect.pos0.y + scale.y * pos2.y);
		glEnd();
	}

	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(2.0);
	glColor3f(0.0, 0.0, 0.0);
	for(i = 0; i < nMesh; i++)
	{
		pos1.x = radCylinder * cos((float)i * theta0);
		pos1.y = radCylinder * sin((float)i * theta0);
		//pos2.x = pos0.x + radCylinder * cos((float)(i+1) * theta0);
		pos2.x = radCylinder * cos((float)(i+1) * theta0);
		pos2.y = radCylinder * sin((float)(i+1) * theta0);
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

void mouse(int button, int state, int x, int y)
{
	if(state == GLUT_UP) return;
	//x, y はウィンドウ左上を基準にした物理座標
	//左下を(-1.0, -1.0)としたデカルト座標に変換
	float xx, yy;
	xx =  2.0 * (float)(x - win_width/2) / (float)win_width;
	yy = -2.0 * (float)(y - win_height/2) / (float)win_height; 
	//解析矩形領域左下を基準にした領域座標系(幅rect.size.x, 高さrect.size.y)に変換
	mPos.x = (xx - rect.left0.x) / scale.x;
	mPos.y = (yy - rect.left0.y) / scale.y;
	//格子番号を取得
	int I = 0, J = 0;
	for(int i = 0; i < rect.nMesh; i++)
	{
		if((float)i * rect.delta.x < mPos.x && (float)(i+1) * rect.delta.x > mPos.x) I = i;
	}
	for(int j = 0; j < rect.nMesh; j++)
	{
		if((float)j * rect.delta.y < mPos.y && (float)(j+1) * rect.delta.y > mPos.y) J = j;
	}
//printf("x=%d  y=%d \n", I, J);

	p[0].pos = mPos;
	flagMouse = 1;
	flagStart = 0;
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
//printf("I=%d  J=%d \n", I, J);
//printf("Psi[I][J]=%f Psi[I+1][J]=%f Psi[I+1][J+1]=%f Psi[I][J+1]=%f \n", Psi[I][J], Psi[I+1][J], Psi[I+1][J+1], Psi[I][J+1]);
	float a =  pos.x / rect.delta.x - (float)I;
	float b =  pos.y / rect.delta.y - (float)J;
	//格子点の速度を線形補間
	Vector2D vel = (1.0 - b) * ((1.0 - a) * Vel[I][J] + a * Vel[I+1][J]) + b * ((1.0 - a) * Vel[I][J+1] + a * Vel[I+1][J+1]);
	return vel;
}

