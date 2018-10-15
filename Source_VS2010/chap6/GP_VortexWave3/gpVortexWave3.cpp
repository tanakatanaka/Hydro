/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
水面の渦と波を表現（粒子を水面に浮かべる)
サイズ20×10，分割数200×100に固定
障害物(円柱）が中間にある平行平板ダクト
速度-圧力法（フラクショナル・ステップ法）
レギュラー格子
GPGPU(数値計算をGPU側で実行）
CIPを使用
速度のPoisson方程式を厳密には解いてない（１回だけ）
GLSLによるシェーディング
鉛直軸はｚ軸
*/
#define MAX_NUM0 50//一度に放出する粒子個数

#include <windows.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glui.h>
#include "../../myGlsl.h"
#include "../../myMath6.h"
#include "../../myPrimitive6.h"
#include "../../rigid.h"
#include "../../support3D.h"
#include "myGLUI.h"

//関数のプロトタイプ宣言
void init();
void idle();
void display();
void rendering();
void setTextureVelX();
void setFramebufferVelX();
void setTextureVelY();
void setFramebufferVelY();
void setTextureWave();
void setFramebufferWave();
void drawNumberingPoints();
void renewVelX();
void renewVelY();
void renewWaveVel();
void renewWavePos();
void renewParticle();
void renderingP();
void drawPlate();
void drawParticles();
void decideRegion();
float regionFlag(float x, float y);
void setTextureParticle();
void setFramebufferParticle();
void calcObsPos();

//解析用データのサイズ
int texWidth, texHeight;//全格子数

//解析用データ配列
float *velX;//速度VelXと圧力
float *velY;//速度VelYと渦度
float *wave;//波動
float *particle;//粒子位置
GLuint texID[4];//texture object
GLuint fbo[4];//framebuffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;//物理量はvelのr,g,bおよび圧力vel.a
GLenum type = GL_FLOAT;
//シェーダプログラム名
GLuint shader_prog1, shader_prog2, shader_prog3, shader_prog4, shader_prog5, shader_prog6, shader_prog7;

enum Type{INSIDE, LEFT, RIGHT, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
          OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ

int NX, NY, NX1, NX2, NY1, NY2;
float DX, DY;

//bool flagResetting = false;
float direction = 1.0;//障害物直線移動方向

int nDummy;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	//表示モード
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//表示ウィンドウのサイズ
	glutInitWindowSize(win_width, win_height);
	//左上の位置
	glutInitWindowPosition(win_x0, win_y0);
	//ウィンドウ作成
	main_window = glutCreateWindow("GP_VortexWave3");
	//ウィンドウのサイズ変更
	glutReshapeFunc(resize);
	//表示
	glutDisplayFunc(display);
	//マウス操作
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//再描画
	GLUI_Master.set_glutIdleFunc( idle );
	//初期設定
	glewInit();//framebufferを利用するときに必要
	init();
	initGlsl(&shader_prog1, "simulation.vert", "calcVelocityX.frag");
	initGlsl(&shader_prog2, "simulation.vert", "calcVelocityY.frag");
	initGlsl(&shader_prog3, "simulation.vert", "waveVel.frag");
	initGlsl(&shader_prog4, "simulation.vert", "wavePos.frag");
	initGlsl(&shader_prog5, "rendering.vert", "rendering.frag");
	initGlsl(&shader_prog6, "simulation.vert", "particle.frag");//粒子用
	initGlsl(&shader_prog7, "renderingP.vert");//粒子用

	setupGLUI();//myGLUI.hに実装

	//イベント処理ループに入る
	glutMainLoop();
	//リソースの解放
 	free(velX);
	free(velY);
	free(wave);
	free(particle);
	glDeleteTextures(4, texID);
	glDeleteFramebuffersEXT(4, fbo);
	glDeleteProgram(shader_prog1);
	glDeleteProgram(shader_prog2);
	glDeleteProgram(shader_prog3);
	glDeleteProgram(shader_prog4);
	glDeleteProgram(shader_prog5);
	glDeleteProgram(shader_prog6);
	glDeleteProgram(shader_prog7);
	deleteDisplayLists();
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
	glClearColor(0.2, 0.2, 0.3, 1.0);
	setCamera();//視点を求める
	setLight(); //光源設定　
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	
	//カメラと視体積の初期値の変更（Viewの宣言はsetup.h)
	view.dist = 25.0;
	view.vCnt = Vector3D(0.0, 0.5, 0.0);
	view.theta = 90.0;//手前がｚ軸（視点が真上）
	view.phi = -90.0;//右が +x軸
	setCamera();
	view0 = view;
	ortho_scale = 10.0;

	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元）

	glGenTextures(4, texID);//テクスチャオブジェクトの名前付け
	glGenFramebuffersEXT(4, fbo);//フレームバッファオブジェクトの生成

	//表示ウィンドウに文字をレンダリングする準備
	setFontBitmaps();

	//display list作成
	createDisplayLists();

	initData();

	//操作キーの説明
	printf("マウス操作で視点変更可 \n");
	printf("  dolly：中央付近を左ボタンクリックで近づき，右ボタンクリックで遠ざかる \n");
	printf("  pan：左横および右横を右ボタンクリックで注視点が左右に変化する \n");
	printf("  tilt：真上および真下を右ボタンクリックで注視点が上下に変化する \n");
	printf("  tumble：左右にドラッグすると視点が左右に変化する \n");
	printf("  crane：上下にドラッグすると視点が上下に変化する \n");
	printf("  zoom：左下を右ボタンクリックでズームイン \n");
	printf("        右下を右ボタンクリックでズームアウト \n");
}

void initData()
{	
	DX = rect.size.x / (float)rect.meshX;
	DY = rect.size.y / (float)rect.meshY;

	nDummy = 20;//最大30
	NX = rect.meshX + 2 * nDummy;
	NY = rect.meshY + 2 * nDummy;         

	rect.obs_posY = rect.size.y / 2.0;
	
	//格子点のタイプ
	int i, j;
	float eps = DX/2.0;
	float x, y, x1, x2, y1, y2, x0, y0, r, r0;
	x0 = rect.obs_left + (float)nDummy * DX;//円柱の中心
	y0 = rect.size.y/2.0 + (float)nDummy * DY;
	r0 = rect.obs_radius;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			g_type[i][j] = INSIDE;//内点
			if(j == 0) g_type[i][j] = BOTTOM;//下側境界
			if(j == NY) g_type[i][j] = TOP;//上側境界
			if(i == 0) g_type[i][j] = LEFT;//左端
			if(i == NX) g_type[i][j] = RIGHT;//右端
			x = (float)i * DX;
			y = (float)j * DY;
			if(y < y0 - r0 - DY) continue;
			if(y > y0 + r0 + DY) continue;
			if(x < x0 - r0 - DX) continue;
			if(x > x0 + r0 + DX) continue;

			r = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
			if(r > r0 + eps) continue;
			if( r <= r0 ) g_type[i][j] = OBSTACLE;//ここでは境界を含む

			if( fabs(y - y0 - r0) < eps ) g_type[i][j] = OBS_TOP;
			else if( fabs(y - y0 + r0) < eps ) g_type[i][j] = OBS_BOTTOM;
			else if( fabs(x - x0 + r0) < eps ) g_type[i][j] = OBS_LEFT;
			else if( fabs(x - x0 - r0) < eps ) g_type[i][j] = OBS_RIGHT;
			else
			{
				x1 = x0 - sqrt(r0 * r0 - (y - y0) * (y - y0));//左側交点
				x2 = x0 + sqrt(r0 * r0 - (y - y0) * (y - y0));//右側交点
				y1 = y0 - sqrt(r0 * r0 - (x - x0) * (x - x0));//下側交点
				y2 = y0 + sqrt(r0 * r0 - (x - x0) * (x - x0));//上側交点
				if(i == int(x1/DX + 0.5) && y > y0 ) g_type[i][j] = OBS_UL;//左上
				if(j == int(y2/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_UL;//左上
				if(i == int(x1/DX + 0.5) && y < y0 ) g_type[i][j] = OBS_LL;//左下
				if(j == int(y1/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_LL;//左下
				if(i == int(x2/DX + 0.5) && y > y0) g_type[i][j] = OBS_UR;//右上
				if(j == int(y2/DY + 0.5) && x > x0) g_type[i][j] = OBS_UR;//右上
				if(i == int(x2/DX + 0.5) && y < y0) g_type[i][j] = OBS_LR;//右下
				if(j == int(y1/DY + 0.5) && x > x0) g_type[i][j] = OBS_LR;//右下
			}
		}

	//物理量テクセルサイズ
	texWidth  = NX + 1;//座標テクスチャの横サイズ
	texHeight = NY + 1;//座標テクスチャの縦サイズ
	
	//物理量配列
	velX = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//X速度その微分および圧力(A成分)
	velY = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//Y速度とその微分(A成分は表示用渦度)
	wave = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//水面のｚ軸方向速度と変位量

	//initialize
	int k;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//I = i - nDummy; J = j - nDummy;//nDummyを考慮しないときの格子番号
			//速度X
			velX[4 * k + 0] = 0.0;
			velX[4 * k + 1] = 0.0;//x微分
			velX[4 * k + 2] = 0.0;//y微分
			velX[4 * k + 3] = 0.0;//圧力
			if(g_type[i][j] == BOTTOM || g_type[i][j] == TOP || g_type[i][j] >= OBS_LEFT) velX[4 * k + 0] = 0.0;//障害物表面
			//速度Y
			velY[4 * k + 0] = 0.0;
			velY[4 * k + 1] = 0.0;//x微分
			velY[4 * k + 2] = 0.0;//y微分
			velY[4 * k + 3] = 0.0;//表示用渦度（GPU側だけで利用)
			//各格子点における水面の高さとｚ軸方向速度
			wave[k * 4 + 0] = 0.0;//水面のｚ軸方向速度
			wave[k * 4 + 1] = 0.0;//水面の変位量
			wave[k * 4 + 2] = 0.0;//渦＋波
			wave[k * 4 + 3] = 1.0;//障害物内外判定に利用		
		}

	setTextureVelX();
	setFramebufferVelX();
	setTextureVelY();
	setFramebufferVelY();

	//障害物
	//全てのパターンに共通
	numRigid = 1;
	for(i = 0; i < RIGID_NUM_MAX; i++)	rigid[i] = CRigid();
	
	rigid[0].kind = CYLINDER;
	rigid[0].color = DARK_YELLOW;
	rigid[0].flagFixed = false;
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, -rect.size.y/2.0 + rect.obs_posY, 0.0);
	rigid[0].vSize = Vector3D(rect.obs_radius*2.0, rect.obs_radius*2.0, rect.obs_radius*2.0);
	float wid = 0.4;
	float hig = 0.5;
	if(pattern == 1)
	{
		numRigid = 3;
		rigid[1].kind = CUBE;
		rigid[1].color = GRAY;
		rigid[1].flagFixed = true;
		rigid[1].vSize = Vector3D(rect.size.x, wid, hig);
		rigid[1].vPos = Vector3D(0.0, rect.size.y/2.0 + 0.5*wid , 0.0);

		rigid[2].kind = CUBE;
		rigid[2].color = GRAY;
		rigid[2].flagFixed = true;
		rigid[2].vSize = Vector3D(rect.size.x, wid, hig);
		rigid[2].vPos = Vector3D(0.0, -rect.size.y/2.0 - 0.5*wid , 0.0);

	}
	decideRegion();
	setTextureWave();
	setFramebufferWave();

	//粒子
	particle = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//粒子の位置

	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			if(k > numParticle) break;
			particle[4 * k + 0] = getRandom(-rect.size.x/2.0, rect.size.x/2.0);
			particle[4 * k + 1] = getRandom(-rect.size.y/2.0, rect.size.y/2.0);
			particle[4 * k + 2] = 0.0;
			particle[4 * k + 3] = 0.0;//格子点の液面の高さ
		}
	setTextureParticle();
	setFramebufferParticle();

	elapseTime1 = 0.0;//1sec間以内の経過時間
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元)

}

void setTextureVelX()
{
	glBindTexture(target, texID[0]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, velX);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureVelY()
{
	glBindTexture(target, texID[1]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, velY);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferVelX()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0 );
	//framebuffer object の無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void setFramebufferVelY()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[1] );
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[1], 0 );
	//framebuffer object の無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void setTextureWave()
{
	glBindTexture(target, texID[2]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, wave);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferWave()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[2] );
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[2], 0 );
	//framebuffer object の無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void setTextureParticle()
{
	glBindTexture(target, texID[3]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, GL_FLOAT, particle);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferParticle()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[3] );
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[3], 0 );
	//framebuffer object の無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
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
	
	int i, j, k, k1;

	if(flagStart==true && flagFreeze == false) 
	{
		//速度境界条件
		if(pattern == 1)
		{
			for (i = 0; i <= NX; i++) 
			{
				//手前障害物の壁面
				k = 4 * (i + nDummy * texWidth);
				velX[k] = 0.0;
				velY[k] = 0.0;
				//奥
				k = 4 * (i + (NY - nDummy) * texWidth);
				velX[k] = 0.0; 
				velY[k] = 0.0; 
			}
		}
		//移動障害物表面および内部
		for (i = 1; i < NX; i++)
			for (j = 0; j <= NY; j++) 
			{
				if(g_type[i][j] < OBS_LEFT) continue;
				k = 4 * (i + j * texWidth);
				velX[k] = rigid[0].vVel.x;
				velY[k] = rigid[0].vVel.y;
			}
		
		//圧力境界条件(移動障害物に対してだけ）
		//全領域境界ではすべてDirichlet
		for (i = 0; i <= NX; i++)
			for (j = 0; j <= NY; j++) 
			{
				k = 4 * (i + j * texWidth) + 3;//圧力
				if(g_type[i][j] == INSIDE) continue;
				else if(g_type[i][j] == LEFT)  velX[k] = 0.0;
				else if(g_type[i][j] == RIGHT) velX[k] = 0.0;
				else if(g_type[i][j] == TOP) velX[k] = 0.0;
				else if(g_type[i][j] == BOTTOM) velX[k] = 0.0;
				else if(g_type[i][j] == OBS_LEFT) velX[k] = velX[4 * (i-1 + j * texWidth) + 3];
				else if(g_type[i][j] == OBS_RIGHT) velX[k] = velX[4 * (i+1 + j * texWidth) + 3];
				else if(g_type[i][j] == OBS_TOP) velX[k] = velX[4 * (i + (j+1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_BOTTOM) velX[k] = velX[4 * (i + (j-1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_UL) velX[k] = velX[4 * (i-1 + (j+1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_UR) velX[k] = velX[4 * (i+1 + (j+1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_LL) velX[k] = velX[4 * (i-1 + (j-1) * texWidth) + 3];
				else if(g_type[i][j] == OBS_LR) velX[k] = velX[4 * (i+1 + (j-1) * texWidth) + 3];
			}

		setTextureVelX();
		setFramebufferVelX();
		setTextureVelY();
		setFramebufferVelY();
		
		renewVelX(); //速度Xと圧力の更新
		renewVelY(); //速度Yと渦度の更新

		renewWaveVel();//波のｚ軸方向速度の更新
		renewWavePos();//波の水面変位量の更新

		//particle
		for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			if(k > numParticle) break;
			//領域外の粒子は領域内に
			if(particle[4 * k + 0] < -rect.size.x/2.0 || particle[4 * k + 0] > rect.size.x/2.0 
				|| particle[4 * k + 1] > rect.size.y/2.0 || particle[4 * k + 1] < -rect.size.y/2.0)
			{
				particle[4 * k + 0] = getRandom(-rect.size.x/2.0, rect.size.x/2.0);
				particle[4 * k + 1] = getRandom(-rect.size.y/2.0, rect.size.y/2.0);
			}
		}
		setTextureParticle();
		setFramebufferParticle();

		calcObsPos();//障害物位置計算

		renewParticle();//粒子位置の更新
		
		if(flagStep) flagFreeze = true;
		elapseTime2 += dt;
		elapseTime3 += deltaT;
	}

	//カラーバッファ,デプスバッファのクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	resize(win_width, win_height);

	glLoadIdentity();//視点を変えるときはこの位置に必要

	if(cos(M_PI * view.theta /180.0) >= 0.0)//カメラ仰角90度でﾋﾞｭｰｱｯﾌﾟﾍﾞｸﾄﾙ切替
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, 1.0);
	else
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, -1.0);

	//光源設定
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	if(flagWireframe == 1)//'w'でwireframeとsolid model切り替え
	{
	  glPolygonMode(GL_FRONT,GL_LINE);
	  glPolygonMode(GL_BACK,GL_POINT);
	}
	else 
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	rendering();

	if(flagShowParticle) renderingP();
	
	for(k = 0; k < numRigid; k++) rigid[k].draw();

	//発散しないための目安を知るため
	float Courant, diff_num;
	if(DX < DY)
	{
		Courant = deltaT / DX;	
		diff_num = (1.0 / Re) * deltaT / (DX * DX);//拡散数
	}
	else
	{
		Courant = deltaT / DY;	
		diff_num = (1.0 / Re) * deltaT / (DY * DY);//拡散数
	}
	 
	if(flagParamShow && !flagOrtho) 
	{
		drawParam("f_rate=%d[fps]", fps0, 10, 30);
		drawParam("t_step=%1.5f[s]", dt, 150, 30);
		drawParam("e_time=%3.3f[s]", elapseTime2, 300, 30);
		drawParam("n_time=%3.3f", elapseTime3, 460, 30);
		drawParam("Courant=%1.4f", Courant, 10, 50);
		drawParam("diff_num=%1.4f", diff_num, 150, 50);
		drawParam("Re=%3.1f", Re,300, 50);
	}
	if(flagCoordShow) drawWorldCoordinates(5.0);

	//バッファの切り替え
	glutSwapBuffers();	
}

void renewVelX()
{
	//framebuffer object1を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog1);
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog1, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog1, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint texWLoc = glGetUniformLocation(shader_prog1, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog1, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader_prog1, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader_prog1, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog1, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader_prog1, "Re");
	glUniform1f(ReLoc, Re);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, velX);//データをCPU側へ

	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewVelY()
{
	//framebuffer object1を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[1] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//veYl[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog2);
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog2, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog2, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint texWLoc = glGetUniformLocation(shader_prog2, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog2, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader_prog2, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader_prog2, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog2, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader_prog2, "Re");
	glUniform1f(ReLoc, Re);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, velY);//データをCPU側へ

	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewWaveVel()
{
	//framebuffer object2を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[2] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[2]);//wave[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog3);
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog3, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE2を適用
	GLint texWidthLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint meshXLoc = glGetUniformLocation(shader_prog3, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog3, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint dummyLoc = glGetUniformLocation(shader_prog3, "nDummy");
	glUniform1i(dummyLoc, nDummy);
	GLint sizeXLoc = glGetUniformLocation(shader_prog3, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog3, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint ampLoc = glGetUniformLocation(shader_prog3, "amp");
	glUniform1f(ampLoc, amp0);
	GLint freqLoc = glGetUniformLocation(shader_prog3, "freq");
	glUniform1f(freqLoc, freq0);
	GLint timeLoc = glGetUniformLocation(shader_prog3, "time");
	glUniform1f(timeLoc, elapseTime2);
	GLint dragLoc = glGetUniformLocation(shader_prog3, "drag0");
	glUniform1f(dragLoc, drag0);
	GLint waveSpeedLoc = glGetUniformLocation(shader_prog3, "waveSpeed");
	glUniform1f(waveSpeedLoc, waveSpeed);
	GLint dtLoc = glGetUniformLocation(shader_prog3, "dt");
	glUniform1f(dtLoc, deltaT);

	GLint posObjLoc = glGetUniformLocation(shader_prog3, "posObs");
	float pos[] ={ rigid[0].vPos.x, rigid[0].vPos.y, rigid[0].vPos.z};
	glUniform3fv(posObjLoc, 1, pos);
	GLint sizeObjLoc = glGetUniformLocation(shader_prog3, "sizeObs");
	float size[] = { rigid[0].vSize.x, rigid[0].vSize.y, rigid[0].vSize.z};
	glUniform3fv(sizeObjLoc, 1, size);
	GLint velObjLoc = glGetUniformLocation(shader_prog3, "velObs");
	float vel[] = { rigid[0].vVel.x, rigid[0].vVel.y, rigid[0].vVel.z};
	glUniform3fv(velObjLoc, 1, vel);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	//glReadPixels(0, 0, texWidth, texHeight, format, type, wave);
	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewWavePos()
{
	//framebuffer object2を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[2] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//veYl[]
	glActiveTexture(GL_TEXTURE2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog4);
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog4, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog4, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog4, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE0を適用
	GLint texWidthLoc = glGetUniformLocation(shader_prog4, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog4, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint maxOmgLoc = glGetUniformLocation(shader_prog4, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader_prog4, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
	GLint dtLoc = glGetUniformLocation(shader_prog4, "dt");
	glUniform1f(dtLoc, deltaT);

	drawNumberingPoints();//数値解析用のテクスチャ（物理量）を貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	//glReadPixels(0, 0, texWidth, texHeight, format, type, wave);
	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void drawNumberingPoints()
{
	//ビューポートのサイズ設定
	glViewport(0, 0, texWidth, texHeight);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//正投影行列の設定
	gluOrtho2D(0.0, texWidth, 0.0, texHeight);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//物理量をテクスチャとして貼り付けるための仮想オブジェクト
	glBegin(GL_POINTS);
	for(int j = 0; j < texHeight; j++)
		for(int i = 0; i < texWidth; i++) glVertex2f((float)i, (float)j);//点座標が2次元格子頂点番号
	glEnd();

}

void rendering()
{
	glUseProgram(shader_prog5);
/*
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY[]
*/
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
/*
	GLint samplerVelXLoc = glGetUniformLocation(shader_prog5, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog5, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
*/
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog5, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2を適用
	GLint texWLoc = glGetUniformLocation(shader_prog5, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog5, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint sizeXLoc = glGetUniformLocation(shader_prog5, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog5, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint meshXLoc = glGetUniformLocation(shader_prog5, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog5, "meshY");
	glUniform1i(meshYLoc, rect.meshY);

	GLint adjustHLoc = glGetUniformLocation(shader_prog5, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
/*
	GLint maxOmgLoc = glGetUniformLocation(shader_prog5, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);

	GLint maxPrsLoc = glGetUniformLocation(shader_prog5, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
*/
	static float diffuse[] = { 0.1, 0.3, 0.9, 1.0};
	static float specular[] = { 0.7, 0.7, 0.7, 1.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS, 100.0);

	if(flagWireframe == 0)
		drawGridPlate((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);
	else
		drawGridLines((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);
	glUseProgram(0);
}

void renewParticle()
{
	//framebuffer objectを有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[3] );

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(target, texID[3]);//particle[]

	glUseProgram(shader_prog6);

	GLint samplerVelXLoc = glGetUniformLocation(shader_prog6, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog6, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog6, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2を適用
	GLint samplerPosLoc = glGetUniformLocation(shader_prog6, "samplerPosP");
	glUniform1i(samplerPosLoc, 3);//GL_TEXTURE3を適用
	GLint meshXLoc = glGetUniformLocation(shader_prog6, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog6, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint dummyLoc = glGetUniformLocation(shader_prog6, "nDummy");
	glUniform1i(dummyLoc, nDummy);
//	GLint adjustHLoc = glGetUniformLocation(shader_prog6, "adjustH");
//	glUniform1f(adjustHLoc, adjustH);
	GLint maxOmgLoc = glGetUniformLocation(shader_prog6, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader_prog6, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
	GLint sizeXLoc = glGetUniformLocation(shader_prog6, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog6, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint numParticleLoc = glGetUniformLocation(shader_prog6, "numParticle");
	glUniform1i(numParticleLoc, numParticle);

	GLint dtLoc = glGetUniformLocation(shader_prog6, "dt");
	glUniform1f(dtLoc, deltaT);

	drawNumberingPoints();
	// シェーダプログラムの解除 
	glUseProgram(0);
	//更新データ（速度と位置）をテクスチャメモリにコピー
	//glCopyTexSubImage2D(target, 0, 0, 0, 0, 0, texWidth, texHeight);
	glReadPixels(0, 0, texWidth, texHeight, format, type, particle);//データをCPU側へ
	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

}

void renderingP()
{
	//描画	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[3]);
	glUseProgram(shader_prog7);
	GLint samplerLoc = glGetUniformLocation(shader_prog7, "samplerPosP");
	glUniform1i(samplerLoc, 0);//GL_TEXTURE0を適用

	drawParticles();
	glUseProgram(0);

	glBindTexture(target, 0);
}
void drawParticles()
{
	glDisable(GL_LIGHTING);
	glPointSize(sizeParticle);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_POINTS);
	for(int j = 0; j < texHeight; j++)
		for(int i = 0; i < texWidth; i++) glVertex3f((float)i, (float)j, 0.0);
	glEnd();
	glPointSize(1);
	glEnable(GL_LIGHTING);
}
void decideRegion()
{
	int i, j, k;
	float x, y;

	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//格子点の座標
			x = (float)(i - texWidth / 2) * DX;
			y = (float)(j - texHeight / 2) * DY;
			wave[k * 4 + 3] = regionFlag(x, y);
			if(wave[k * 4 + 3] == 0.0) g_type[i][j] = OBSTACLE;
		}
}

float regionFlag(float x, float y)
{//対象図形の基本姿勢で判定
	Vector3D vPoint0 = Vector3D(x, y, 0.0);
	Vector3D vPoint;

	for(int k = 0; k < numRigid; k++)
	{
		//対象図形の中心に平行移動し負の回転
		vPoint = vPoint0 - rigid[k].vPos;
		vPoint.rotZ(-rigid[k].vEuler.z);
		if(rigid[k].kind == CUBE)
		{
			if( vPoint.x >= - rigid[k].vSize.x / 2.0 && vPoint.x <= rigid[k].vSize.x / 2.0
			 && vPoint.y >= - rigid[k].vSize.y / 2.0 && vPoint.y <= rigid[k].vSize.y / 2.0)
			{
				if(rigid[k].flagFixed == true) return 0.0;//固定障害物内部
				else                           return 1.0;//移動障害物は水面と同じ扱い
			}
		}
		else if(rigid[k].kind == SPHERE || rigid[k].kind == CYLINDER)
		{
			//楕円形も可
			float a = rigid[k].vSize.x * rigid[k].vSize.x / 4.0;
			float b = rigid[k].vSize.y * rigid[k].vSize.y / 4.0;
			float f =  vPoint.x*vPoint.x / a + vPoint.y*vPoint.y / b - 1.0;
			if(f <= 0.0) //return 0.0;
			{
				if(rigid[k].flagFixed == true) return 0.0;//固定障害物内部
				else                           return 1.0;//移動障害物は水面と同じ扱い
			}
		}
	}
	return 1.0;//外部(水面)
}

void calcObsPos()
{
	if(flagObsStop) { rigid[0].vVel = Vector3D();  return; }


	//else rigid[0].vVel.x = velocity0;
	Vector3D left;//ダクトの左端および下端からの距離

	float R = rect.size.x / 2.0 - rect.obs_left;//回転半径
	//float theta;
	if(flagRotation)
	{//初期位置が左側のとき右回り，右側のとき左回り
		rigid[0].vVel.x = obsSpeed * rigid[0].vPos.y / R;
		rigid[0].vVel.y = -obsSpeed * rigid[0].vPos.x / R;
	}
	else//直進
	{
		if(rigid[0].vPos.x >=  rect.size.x / 2.0 - rect.obs_left) direction = -1.0;
		if(rigid[0].vPos.x <= -rect.size.x / 2.0 + rect.obs_left) direction =  1.0;
		rigid[0].vVel.x = direction * obsSpeed;
	}
	rigid[0].vPos += rigid[0].vVel * deltaT;
	left.x = rigid[0].vPos.x + rect.size.x/2.0 + (float)nDummy * DX;
	left.y = rigid[0].vPos.y + rect.size.y/2.0 + (float)nDummy * DY;

	//格子点のタイプ
	int i, j;
	float eps = DX/2.0;
	float x, y, x1, x2, y1, y2, x0, y0, r, r0;
	x0 = left.x;//rect.obs_left;//円柱の中心
	y0 = left.y;//rect.size.y/2.0;
	r0 = rect.obs_radius;
	for(i = 1; i < NX; i++)
		for(j = 1; j < NY; j++)
		{
			g_type[i][j] = INSIDE;//内点
			x = (float)i * DX;
			y = (float)j * DY;
			if(y < y0 - r0 - DY) continue;
			if(y > y0 + r0 + DY) continue;
			if(x < x0 - r0 - DX) continue;
			if(x > x0 + r0 + DX) continue;

			r = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
			if(r > r0 + eps) continue;
			if( r <= r0 ) g_type[i][j] = OBSTACLE;//ここでは境界を含む

			if( fabs(y - y0 - r0) < eps ) g_type[i][j] = OBS_TOP;
			else if( fabs(y - y0 + r0) < eps ) g_type[i][j] = OBS_BOTTOM;
			else if( fabs(x - x0 + r0) < eps ) g_type[i][j] = OBS_LEFT;
			else if( fabs(x - x0 - r0) < eps ) g_type[i][j] = OBS_RIGHT;
			else
			{
				x1 = x0 - sqrt(r0 * r0 - (y - y0) * (y - y0));//左側交点
				x2 = x0 + sqrt(r0 * r0 - (y - y0) * (y - y0));//右側交点
				y1 = y0 - sqrt(r0 * r0 - (x - x0) * (x - x0));//下側交点
				y2 = y0 + sqrt(r0 * r0 - (x - x0) * (x - x0));//上側交点
				if(i == int(x1/DX + 0.5) && y > y0 ) g_type[i][j] = OBS_UL;//左上
				if(j == int(y2/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_UL;//左上
				if(i == int(x1/DX + 0.5) && y < y0 ) g_type[i][j] = OBS_LL;//左下
				if(j == int(y1/DY + 0.5) && x < x0 ) g_type[i][j] = OBS_LL;//左下
				if(i == int(x2/DX + 0.5) && y > y0) g_type[i][j] = OBS_UR;//右上
				if(j == int(y2/DY + 0.5) && x > x0) g_type[i][j] = OBS_UR;//右上
				if(i == int(x2/DX + 0.5) && y < y0) g_type[i][j] = OBS_LR;//右下
				if(j == int(y1/DY + 0.5) && x > x0) g_type[i][j] = OBS_LR;//右下
			}
		}
}


