/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
水面の渦を表現（粒子を水面に浮かべる)
屈折環境マッピングを追加
投影マッピングによるコースティックスの追加
サイズ20×20，分割数200×200に固定
移動障害物（円柱，直線往復運動，等速円運動）
境界は固定壁面
速度-圧力法（フラクショナル・ステップ法）
レギュラー格子
GPGPU(数値計算をGPU側で実行）
CIPを使用
速度のPoisson方程式を厳密には解いてない（１回だけ）
GLSLによるシェーディング
鉛直軸はｚ軸
*/

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
void setTextureCaus();
void drawNumberingPoints();
void renewVelX();
void renewVelY();
void renewWaveVel();
void renewWavePos();
void renewParticle();
void renderingP();
void drawPlate();
void drawParticles();
void setTextureParticle();
void setFramebufferParticle();
void calcObsPos();
void setFramebufferCube();
void setCubeMap();
void makeTextureOfCubeMapping();
void refractMapping();
void makeUpperTank();
void makeLowerTank();
void setTextureMatrix();

//仮想生物アニメーション
#include "../../myFish1.h"
CFish1 fish1 ;
#include "../../myFish2.h"
CFish2 fish2 ;

//解析用データのサイズ
int texWidth, texHeight;//全格子数

//解析用データ配列
float *velX;//速度VelXと圧力
float *velY;//速度VelYと渦度
float *wave;//波の高さの速度，位置
GLubyte *caus;
float *particle;//粒子位置
GLuint texID[6];//texture object
GLuint fbo[5];//frame buffer object
GLuint rbo;   //render buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;
GLenum type = GL_FLOAT;
//CubeMap用
#define CUBE_WIDTH  512
#define CUBE_HEIGHT 512

//シェーダプログラム名
GLuint shader1, shader2, shader3, shader4, shader5, shader6, shader7, shader8 ;

enum Type{INSIDE, LEFT, RIGHT, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
          OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ

int NX, NY, NX1, NY1, NX2, NY2;
float DX, DY;
float direction = 1.0;//障害物直線移動方向

//テクスチャー構造体(cube map)
struct Target{
	GLenum name;//ターゲット名
	GLdouble cx, cy, cz;//注視点
	GLdouble ux, uy, uz;//アップベクトル
};
Target c_target[] = {
	{//右
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//左
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		-1.0, 0.0, 0.0,
		0.0, -1.0, 0.0,
	},
	{//奥
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
	},
	{//手前
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		0.0, -1.0, 0.0,
		0.0, 0.0, -1.0,
	},
	{//上方
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		0.0, 0.0, 1.0,
		0.0, -1.0, 0.0,
	},
	{//床面
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		0.0, 0.0, -1.0,
		0.0, -1.0, 0.0,
	}
};

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
	main_window = glutCreateWindow("GP_Caustics1");
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
	initGlsl(&shader1, "simulation.vert", "calcVelocityX.frag");
	initGlsl(&shader2, "simulation.vert", "calcVelocityY.frag");
	initGlsl(&shader3, "simulation.vert", "waveVel.frag");
	initGlsl(&shader4, "simulation.vert", "wavePos.frag");
	initGlsl(&shader5, "rendering.vert", "rendering.frag");
	initGlsl(&shader6, "simulation.vert", "particle.frag");//粒子用
	initGlsl(&shader7, "renderingP.vert");//粒子用
	initGlsl(&shader8, "projection.vert", "projection.frag");//投影マッピング用

	setupGLUI();//myGLUI.hに実装

	//イベント処理ループに入る
	glutMainLoop();
	//リソースの解放
 	free(velX);
	free(velY);
	free(wave);
	free(caus);
	free(particle);
	glDeleteTextures(6, texID);
	glDeleteFramebuffersEXT(5, fbo);
	glDeleteRenderbuffersEXT(1, &rbo);
	glDeleteProgram(shader1);
	glDeleteProgram(shader2);
	glDeleteProgram(shader3);
	glDeleteProgram(shader4);
	glDeleteProgram(shader5);
	glDeleteProgram(shader6);
	glDeleteProgram(shader7);
	glDeleteProgram(shader8);
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
	glClearColor(0.5, 0.7, 1.0, 1.0);
	setCamera();//視点を求める
	setLight(); //光源設定　
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//カメラと視体積の初期値の変更（Viewの宣言はsetup.h)
	view.dist = 30.0;
	view.vCnt = Vector3D(0.0, 0.5, waveHeight);
	view.theta = 30.0;
	view.phi = -90.0;//右が +x軸
	setCamera();
	view0 = view;
	lightPos[2]  = 20.0;
	lightPos0[2] = 20.0;

	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元）

	glGenTextures(6, texID);//テクスチャオブジェクトの名前付け
	glGenFramebuffersEXT(5, fbo);//フレームバッファオブジェクトの生成

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

	NX = rect.meshX;
	NY = rect.meshY;         
	
	DX = rect.size.x / (float)NX;
	DY = rect.size.y / (float)NY;

	//格子点のタイプ
	int i, j;
	float eps = DX/2.0;
	float x, y, x1, x2, y1, y2, x0, y0, r, r0;
	x0 = rect.obs_left;//円柱の中心
	y0 = rect.size.y/2.0;
	r0 = rect.obs_radius;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{

			g_type[i][j] = INSIDE;//内点
			if(j == 0) g_type[i][j] = BOTTOM;//下側(-y)
			if(j == NY) g_type[i][j] = TOP;  //上側(+y)
			if(i == 0) g_type[i][j] = LEFT;  //左端(-x)
			if(i == NX) g_type[i][j] = RIGHT;//右端(+x)
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
	wave = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//水面のｚ軸方向速度と変位
	caus = (GLubyte*)malloc(3 * texWidth * texHeight * sizeof(GLubyte));//caustics

	//initialize
	int k;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;

			//速度X
			velX[4 * k + 0] = 0.0;
			velX[4 * k + 1] = 0.0;//x微分
			velX[4 * k + 2] = 0.0;//y微分
			velX[4 * k + 3] = 0.0;//圧力
			//速度Y
			velY[4 * k + 0] = 0.0;
			velY[4 * k + 1] = 0.0;//x微分
			velY[4 * k + 2] = 0.0;//y微分
			velY[4 * k + 3] = 0.0;//表示用渦度（GPU側だけで利用)
			//波
			wave[4 * k + 0] = 0.0;//波のｚ軸方向速度
			wave[4 * k + 1] = 0.0;//水面の変位
			wave[4 * k + 2] = 0.0;//渦＋波
			wave[4 * k + 3] = 0.0;//未使用
			//caustics
			caus[3 * k + 0] = 127.0;//R
			caus[3 * k + 1] = 127.0;//G
			caus[3 * k + 2] = 127.0;//B
		}

	setTextureVelX();
	setFramebufferVelX();
	setTextureVelY();
	setFramebufferVelY();
	setTextureWave();
	setFramebufferWave();
	setFramebufferCube();
	setCubeMap();
	setTextureCaus();

	numRigid = 11;
	for(i = 0; i < numRigid; i++) rigid[i] = CRigid();
	//障害物（円柱，球）
	rigid[0].kind = CYLINDER;
	rigid[0].color = RED;
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, -rect.size.y/2.0 + rect.obs_posY, waveHeight+rect.obs_length/4.0);
	rigid[0].vSize = Vector3D(rect.obs_radius*2.0, rect.obs_radius*2.0, rect.obs_length/2.0);

	//下半分(水面下の障害物）
	rigid[1].kind = CYLINDER;
	rigid[1].color = BLUE;
	rigid[1].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, -rect.size.y/2.0 + rect.obs_posY, waveHeight-rect.obs_length/4.0);
	rigid[1].vSize = Vector3D(rect.obs_radius*2.0, rect.obs_radius*2.0, rect.obs_length/2.0);
	makeLowerTank();
	makeUpperTank();

	elapseTime1 = 0.0;//1sec間以内の経過時間
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元)

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
			particle[4 * k + 3] = 0.0;//未使用
		}
	setTextureParticle();
	setFramebufferParticle();

}
void initObjectLength()
{
	//GLUIウィンドウで水面の高さを変えたとき障害物のｚ軸方向長さを変える
	rigid[0].vPos.z =  waveHeight + rect.obs_length/4.0;
	rigid[1].vPos.z =  waveHeight - rect.obs_length/4.0;
	makeLowerTank();
	makeUpperTank();
}

void makeLowerTank()
{
	float bHeight = waveHeight;
	//float bWidth = 1.0;
	//右側
	rigid[2].kind = PLATE;
	rigid[2].color = DARK_YELLOW;
	rigid[2].vPos = Vector3D(rect.size.x/2.0, 0.0, bHeight / 2.0);
	rigid[2].vSize = Vector3D( bHeight, rect.size.y, 1.0);
	rigid[2].vEuler = Vector3D(0.0, -90.0, 0.0);
	//左側
	rigid[3].kind = PLATE;
	rigid[3].color = DARK_YELLOW;
	rigid[3].vPos = Vector3D(-rect.size.x/2.0, 0.0, bHeight / 2.0);
	rigid[3].vSize = Vector3D(bHeight, rect.size.y, 1.0);
	rigid[3].vEuler = Vector3D(0.0, 90.0, 0.0);
	//奥
	rigid[4].kind = PLATE;
	rigid[4].color = DARK_CYAN;
	rigid[4].vPos = Vector3D(0.0, rect.size.y/2.0, bHeight / 2.0);
	rigid[4].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[4].vEuler = Vector3D(90.0, 0.0, 0.0);
	//手前
	rigid[5].kind = PLATE;
	rigid[5].color = DARK_CYAN;
	rigid[5].vPos = Vector3D(0.0, -rect.size.y/2.0, bHeight / 2.0);
	rigid[5].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[5].vEuler = Vector3D(-90.0, 0.0, 0.0);
}
void makeUpperTank()
{
	float bHeight = 10.0 - waveHeight;
	float hh0 = waveHeight + bHeight / 2.0;
	//右側
	rigid[6].kind = PLATE;
	rigid[6].color = DARK_YELLOW;
	rigid[6].vPos = Vector3D(rect.size.x/2.0 , 0.0, hh0);
	rigid[6].vSize = Vector3D(bHeight, rect.size.y, 1.0);
	rigid[6].vEuler = Vector3D(0.0, -90.0, 0.0);
	//左側
	rigid[7].kind = PLATE;
	rigid[7].color = DARK_YELLOW;
	rigid[7].vPos = Vector3D(-rect.size.x/2.0, 0.0, hh0);
	rigid[7].vSize = Vector3D(bHeight, rect.size.y, 1.0);
	rigid[7].vEuler = Vector3D(0.0, 90.0, 0.0);
	//奥
	rigid[8].kind = PLATE;
	rigid[8].color = DARK_CYAN;
	rigid[8].vPos = Vector3D(0.0, rect.size.y/2.0, hh0);
	rigid[8].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[8].vEuler = Vector3D(90.0, 0.0, 0.0);
	//奥
	rigid[9].kind = PLATE;
	rigid[9].color = DARK_CYAN;
	rigid[9].vPos = Vector3D(0.0, -rect.size.y/2.0, hh0);
	rigid[9].vSize = Vector3D(rect.size.x, bHeight, 1.0);
	rigid[9].vEuler = Vector3D(-90.0, 0.0, 0.0);
	//LIGHT
	rigid[10].kind = BOX;
	rigid[10].color = WHITE;
	rigid[10].vPos = Vector3D(lightPos[0], lightPos[1], lightPos[2] + 1.0);
	rigid[10].vSize = Vector3D(3.0, 3.0, 0.1);

}

void drawLowerObject()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//背面は描画しない
	for(int i = 1; i <= 5; i++)
	{
		//テクスチャに対するモデリング変換
		setTextureMatrix();
		//glPushMatrix();
		//現在位置
		glTranslatef(rigid[i].vPos.x, rigid[i].vPos.y, rigid[i].vPos.z);//平行移動
		glRotatef(rigid[i].vEuler.z, 0.0, 0.0, 1.0);//回転角はdeg
		glRotatef(rigid[i].vEuler.y, 0.0, 1.0, 0.0);//回転角はdeg
		glRotatef(rigid[i].vEuler.x, 1.0, 0.0, 0.0);//回転角はdeg
		//スケーリング
		glScalef(rigid[i].vSize.x, rigid[i].vSize.y, rigid[i].vSize.z);

		//通常のモデルビュー変換に戻す 
		glMatrixMode(GL_MODELVIEW);

		rigid[i].draw();
	}
	setTextureMatrix();
	glMatrixMode(GL_MODELVIEW);
	glCallList(floor0);//床・地面
	glDisable(GL_CULL_FACE);
}
void drawUpperObject()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//背面は描画しない
	rigid[0].draw();//障害物水面上
	rigid[10].vPos = Vector3D(lightPos[0], lightPos[1], lightPos[2] + 2.0);
	for(int i = 6; i < 11; i++) rigid[i].draw();
	glDisable(GL_CULL_FACE);
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

void setFramebufferCube()
{
	//renderbuffer
	glGenRenderbuffersEXT( 1, &rbo );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, rbo );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, CUBE_WIDTH, CUBE_HEIGHT );
	//framebuffer
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[4] );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbo );

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void setCubeMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[4]);
	for(int i = 0; i < 6; i++)
	glTexImage2D(c_target[i].name,0,GL_RGB,CUBE_WIDTH,CUBE_HEIGHT,0,GL_RGB,GL_UNSIGNED_BYTE,0);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void setTextureCaus()
{
	glBindTexture(target, texID[5]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texWidth,texHeight,0,GL_RGB,GL_UNSIGNED_BYTE, caus);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glBindTexture(target, 0);
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
	
	int i, j, k;

	if(flagStart==true && flagFreeze == false) 
	{
		//移動障害物表面の速度境界条件
		for (i = 1; i < NX; i++)
			for (j = 0; j <= NY; j++) 
			{
				if(g_type[i][j] < OBS_LEFT) continue;
				k = 4 * (i + j * texWidth);
				velX[k] = rigid[0].vVel.x;
				velY[k] = rigid[0].vVel.y;
			}

		//圧力境界条件
		for (i = 0; i <= NX; i++)
			for (j = 0; j <= NY; j++) 
			{
				k = 4 * (i + j * texWidth) + 3;//圧力
				if(g_type[i][j] == INSIDE) continue;
				else if(g_type[i][j] == LEFT)  velX[k] = 0.0;//-x
				else if(g_type[i][j] == RIGHT) velX[k] = 0.0;//+x
				else if(g_type[i][j] == TOP) velX[k] = 0.0;//+y
				else if(g_type[i][j] == BOTTOM) velX[k] = 0.0;//-y
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
		renewWavePos();//波の高さの更新

/*
for(i=50;i<150; i++) 
{
	k3 = 3*(i + texWidth * texHeight/2);
	float aa = (float)caus[k3];
	if(aa > 200.0) printf("i=%d caus=%f \n", i, aa);
}
*/
		//particle
		for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			if(k > numParticle) break;
			//速度X
			if(particle[4 * k + 0] > rect.size.x/2.0 || particle[4 * k + 0] < -rect.size.x/2.0 || 
			   particle[4 * k + 1] > rect.size.y/2.0 || particle[4 * k + 1] < -rect.size.y/2.0)
			{
				particle[4 * k + 0] = getRandom(-rect.size.x/2.0, rect.size.x/2.0);
				particle[4 * k + 1] = getRandom(-rect.size.y/2.0, rect.size.y/2.0);
			}
		}
		setTextureParticle();
		setFramebufferParticle();

		renewParticle();//粒子位置の更新

		if(flagStep) flagFreeze = true;
		elapseTime2 += dt;
		elapseTime3 += deltaT;

		calcObsPos();

	}

	//環境マップのテクスチャ作成
	makeTextureOfCubeMapping();

	//通常のシーンの描画
	//カラーバッファ,デプスバッファのクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	resize(win_width, win_height);
	glMatrixMode(GL_MODELVIEW);
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

	rendering();//水面

	if(flagShowParticle) renderingP();
	
	drawUpperObject();

	//投影マッピング
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[5]);//caus[]
	// シェーダプログラムの適用 
	glUseProgram(shader8);
	GLint texLoc = glGetUniformLocation(shader8, "samplerCaus");
	glUniform1i(texLoc, 0);//GL_TEXTURE0を適用

	drawLowerObject();
	fish1.motion(elapseTime2);
	fish2.motion(elapseTime2);
	// シェーダプログラムの適用を解除	
	glUseProgram(0);

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

void makeTextureOfCubeMapping()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[4] );

	// 透視変換行列の設定 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.1, 100.0);//視野角を90°
	// モデルビュー変換行列の設定 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//視点から見たシーンを屈折マッピングに利用 
	for (int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, c_target[i].name, texID[4], 0);
		glViewport(0, 0, CUBE_WIDTH, CUBE_HEIGHT);
		//カラーバッファ,デプスバッファのクリア
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		//視点から見えるものをレンダリング 
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vPos.x+c_target[i].cx, view.vPos.y+c_target[i].cy, view.vPos.z+c_target[i].cz,
				c_target[i].ux, c_target[i].uy, c_target[i].uz);

		//光源設定
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		if(view.vPos.z < waveHeight)//視点が水面より下
		{
			drawUpperObject();//水面上のオブジェクト
		}

		//投影マッピング
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(target, texID[5]);//caus[]
		// シェーダプログラムの適用 
		glUseProgram(shader8);
		GLint texLoc = glGetUniformLocation(shader8, "samplerCaus");
		glUniform1i(texLoc, 0);//GL_TEXTURE0を適用

		drawLowerObject();
		fish1.motion(elapseTime2);
		fish2.motion(elapseTime2);
		// シェーダプログラムの適用を解除	
		glUseProgram(0);

		glPopMatrix();
	}
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
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
	glUseProgram(shader1);
	GLint samplerVelXLoc = glGetUniformLocation(shader1, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader1, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint texWLoc = glGetUniformLocation(shader1, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader1, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader1, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader1, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader1, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader1, "Re");
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
	glUseProgram(shader2);
	GLint samplerVelXLoc = glGetUniformLocation(shader2, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader2, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint texWLoc = glGetUniformLocation(shader2, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader2, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader2, "deltaT");
	glUniform1f(deltaTLoc, deltaT);
	GLint DXLoc = glGetUniformLocation(shader2, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader2, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader2, "Re");
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
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	//シェーダプログラムを有効
	glUseProgram(shader3);
	GLint samplerWaveLoc = glGetUniformLocation(shader3, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2を適用
	GLint texWLoc = glGetUniformLocation(shader3, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader3, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint deltaTLoc = glGetUniformLocation(shader3, "dt");
	glUniform1f(deltaTLoc, deltaT);
	GLint timeLoc = glGetUniformLocation(shader3, "time");
	glUniform1f(timeLoc, elapseTime3);
	GLint DXLoc = glGetUniformLocation(shader3, "dx");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader3, "dy");
	glUniform1f(DYLoc, DY);
	GLint ampLoc = glGetUniformLocation(shader3, "amp");
	glUniform1f(ampLoc, waveAmp);
	GLint freqLoc = glGetUniformLocation(shader3, "freq");
	glUniform1f(freqLoc, waveFreq);
	GLint speedLoc = glGetUniformLocation(shader3, "speed");
	glUniform1f(speedLoc, waveSpeed);
	GLint dragLoc = glGetUniformLocation(shader3, "drag");
	glUniform1f(dragLoc, waveDrag);

	GLint posObsLoc = glGetUniformLocation(shader3, "posObs");
	float pos[] ={ rigid[0].vPos.x, rigid[0].vPos.y, rigid[0].vPos.z};
	glUniform3fv(posObsLoc, 1, pos);
	GLint sizeObsLoc = glGetUniformLocation(shader3, "sizeObs");
	float size[] = { rigid[0].vSize.x, rigid[0].vSize.y, rigid[0].vSize.z};
	glUniform3fv(sizeObsLoc, 1, size);
	GLint velObsLoc = glGetUniformLocation(shader3, "velObs");
	float vel[] = { rigid[0].vVel.x, rigid[0].vVel.y, rigid[0].vVel.z};
	glUniform3fv(velObsLoc, 1, vel);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, wave);//データをCPU側へ

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
	glBindTexture(target, texID[2]);//wave[]
	//シェーダプログラムを有効
	glUseProgram(shader4);

	GLint samplerVelXLoc = glGetUniformLocation(shader4, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader4, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint samplerWaveLoc = glGetUniformLocation(shader4, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2を適用
	GLint meshXLoc = glGetUniformLocation(shader4, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader4, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint maxOmgLoc = glGetUniformLocation(shader4, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader4, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
	GLint adjustHLoc = glGetUniformLocation(shader4, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint adjustCLoc = glGetUniformLocation(shader4, "adjustC");
	glUniform1f(adjustCLoc, adjustC);
	GLint deltaTLoc = glGetUniformLocation(shader4, "deltaT");
	glUniform1f(deltaTLoc, deltaT);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, wave);//データをCPU側へ
	//コースティクスのテクスチャ作成
	int i, j, k, k3;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			k3 = 3 * k;
			caus[k3] = caus[k3 + 1] = caus[k3 + 2] = 255.0 * wave[k*4 + 3];
		}
	setTextureCaus();

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
	glUseProgram(shader5);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[4]);

	GLint texLoc = glGetUniformLocation(shader5, "smplRefract");
	glUniform1i(texLoc, 3);//GL_TEXTURE3を適用
	GLint tranLoc = glGetUniformLocation(shader5, "transparency");
	glUniform1f(tranLoc, transparency);
	GLint ratioLoc = glGetUniformLocation(shader5, "nRatio");
	glUniform1f(ratioLoc, nRatio);
	GLint inverseLoc = glGetUniformLocation(shader5, "flagInverse");
	int flagInverse = -1;
	if(view.vPos.z < waveHeight) flagInverse = 1;//視点が水面以下
	glUniform1i(inverseLoc, flagInverse);
	GLint samplerWaveLoc = glGetUniformLocation(shader5, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE0を適用
	GLint texWLoc = glGetUniformLocation(shader5, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader5, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint heightLoc = glGetUniformLocation(shader5, "height");
	glUniform1f(heightLoc, waveHeight);

	GLint sizeXLoc = glGetUniformLocation(shader5, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader5, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);

	//質点のマテリアル
	static float diffuse[] = { 0.3, 0.3, 0.3, 1.0};
	static float ambient[] = { 0.1, 0.1, 0.2, 1.0};
	static float specular[]= { 1.0, 1.0, 1.0, 1.0};
//	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS, 100.0);

	if(flagWireframe == 0)
		drawGridPlate((float)NX, (float)NY, NX, NY);
	else
		drawGridLines((float)NX, (float)NY, NX, NY);
	glUseProgram(0);
}

void renewParticle()
{
	//framebuffer objectを有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[3] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY[]
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(target, texID[3]);//particle[]

	glUseProgram(shader6);

	GLint samplerVelXLoc = glGetUniformLocation(shader6, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader6, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint samplerWaveLoc = glGetUniformLocation(shader6, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE2を適用
	GLint samplerPosLoc = glGetUniformLocation(shader6, "samplerPosP");
	glUniform1i(samplerPosLoc, 3);//GL_TEXTURE3を適用
	GLint texWidthLoc = glGetUniformLocation(shader6, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader6, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint sizeXLoc = glGetUniformLocation(shader6, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader6, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint dtLoc = glGetUniformLocation(shader6, "dt");
	glUniform1f(dtLoc, deltaT);

	drawNumberingPoints();
	// シェーダプログラムの解除 
	glUseProgram(0);
	//更新データ（速度と位置）をテクスチャメモリにコピー
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
	glUseProgram(shader7);
	GLint samplerLoc = glGetUniformLocation(shader7, "samplerPosP");
	glUniform1i(samplerLoc, 0);//GL_TEXTURE0を適用
	GLint heightLoc = glGetUniformLocation(shader7, "height");
	glUniform1f(heightLoc, waveHeight);

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

void calcObsPos()
{
	if(flagObsStop) { rigid[0].vVel = Vector3D();  return; }

	Vector3D left;//ダクトの左端および下端からの距離
//	float direction = 1.0;//障害物直線移動方向

	float R = rect.size.x / 2.0 - rect.obs_left;//回転半径
	
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
	//水面下の障害物の像
	rigid[1].vPos = rigid[0].vPos;
	rigid[0].vPos.z = waveHeight + rigid[1].vSize.z/2.0;
	rigid[1].vPos.z = waveHeight - rigid[1].vSize.z/2.0;
	left.x = rigid[0].vPos.x + rect.size.x/2.0;
	left.y = rigid[0].vPos.y + rect.size.y/2.0;

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

void setTextureMatrix()
{
	//テクスチャ変換行列を設定する 
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	//正規化座標の [-0.5,0.5] の範囲をテクスチャ座標の範囲 [0,1] に変換 
	glTranslatef(0.5, 0.5, 0.0);
	float fovC = 2.0 * RAD_TO_DEG * atan2(rect.size.x, (lightPos[2] - waveHeight));
	gluPerspective(fovC, 1.0, 1.0, 100.0);
	//投影中心=光源位置
	gluLookAt(lightPos[0], lightPos[1], lightPos[2], 0.0, 0.0, waveHeight, 0.0, 1.0, 0.0);
}
