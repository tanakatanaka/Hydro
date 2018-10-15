/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
移動障害物（直線往復運動，等速円運動）
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
void drawNumberingPoints();
void renewVelX();
void renewVelY();
void calcObsPos();

//解析用データのサイズ
int texWidth, texHeight;//全格子数

//解析用データ配列
float *velX;//速度VelXと圧力
float *velY;//速度VelYと渦度
GLuint texID[2];//texture object
GLuint fbo[2];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;//物理量はvelのr,g,bおよび圧力vel.a
GLenum type = GL_FLOAT;
//シェーダプログラム名
GLuint shader_prog1, shader_prog2, shader_prog3;

enum Type{INSIDE, LEFT, RIGHT, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
          OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ

int NX, NY, NX1, NY1, NX2, NY2;
float DX, DY;

float maxPrs0, minPrs0, maxVel0, minVel0;
bool flagResetting = false;
float direction = 1.0;//障害物直線移動方向


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
	main_window = glutCreateWindow("GP_MovingObstacle");
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
	initGlsl(&shader_prog3, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.hに実装

	//イベント処理ループに入る
	glutMainLoop();
	//リソースの解放
 	free(velX);
	free(velY);
	glDeleteTextures(2, texID);
	glDeleteFramebuffersEXT(2, fbo);
	glDeleteProgram(shader_prog1);
	glDeleteProgram(shader_prog2);
	glDeleteProgram(shader_prog3);
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
	view.dist = 35.0;
	view.vCnt = Vector3D(0.0, 0.0, 0.0);
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

	glGenTextures(2, texID);//テクスチャオブジェクトの名前付け
	glGenFramebuffersEXT(2, fbo);//フレームバッファオブジェクトの生成

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

	float obsTop, obsBottom;
	obsTop = rect.obs_posY + rect.obs_width/2.0;
	obsBottom = rect.obs_posY - rect.obs_width/2.0;
	
	//障害物境界の分割点番号
	float eps = 0.5;
	NX1 = floor((rect.obs_left - rect.obs_thick / 2.0) / DX + eps);//障害物左端位置
	NX2 = floor((rect.obs_left + rect.obs_thick / 2.0) / DX + eps);//障害物右端位置
	NY2 = floor(obsTop / DY + eps);//障害物上端位置
	NY1 = floor(obsBottom / DY + eps);//障害物下端位置

	//格子点のタイプ
	int i, j;
	for(i = 0; i <= NX; i++)
		for(j = 0; j <= NY; j++)
		{
			g_type[i][j] = INSIDE;//内点
			if(j == 0) g_type[i][j] = BOTTOM;//下側壁面
			if(j == NY) g_type[i][j] = TOP;//上側壁面
			if(i == 0) g_type[i][j] = LEFT;//流入端
			if(i == NX) g_type[i][j] = RIGHT;//流出端
			if(i == NX1 && j > NY1 && j < NY2) g_type[i][j] = OBS_LEFT;//障害物左端
			if(i == NX2 && j > NY1 && j < NY2) g_type[i][j] = OBS_RIGHT;//障害物右端
			if(i >= NX1 && i <= NX2 && j == NY2) g_type[i][j] = OBS_TOP;//障害物上端
			if(i >= NX1 && i <= NX2 && j == NY1) g_type[i][j] = OBS_BOTTOM;//障害物下端
			if(i > NX1 && i < NX2 && j > NY1 && j < NY2) g_type[i][j] = OBSTACLE;//障害物内部
			if(i == NX1 && j == NY1) g_type[i][j] = OBS_LL;
			if(i == NX1 && j == NY2) g_type[i][j] = OBS_UL;
			if(i == NX2 && j == NY1) g_type[i][j] = OBS_LR;
			if(i == NX2 && j == NY2)  g_type[i][j] = OBS_UR;
		}

	//物理量テクセルサイズ
	texWidth  = NX + 1;//座標テクスチャの横サイズ
	texHeight = NY + 1;//座標テクスチャの縦サイズ
	
	//物理量配列
	velX = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//X速度その微分と圧力
	velY = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//Y速度とその微分(A成分は表示用渦度)

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
		}

	setTextureVelX();
	setFramebufferVelX();
	setTextureVelY();
	setFramebufferVelY();

	numRigid = 1;
	for(i = 0; i < numRigid; i++) rigid[i] = CRigid();
	//障害物（直方体）
	rigid[0].kind = CUBE;
	rigid[0].color = GRAY;
	//rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, 0.0, 0.0);
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, -rect.size.y/2.0 + rect.obs_posY, 0.0);
	rigid[0].vSize = Vector3D(rect.obs_thick, rect.obs_width, 2.0);
	rigid[0].vVel = Vector3D(obsSpeed, 0.0, 0.0);
	
	elapseTime1 = 0.0;//1sec間以内の経過時間
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元)

	maxPrs0 = -1000.0; minPrs0 = 1000.0;
	maxVel0 = -1000.0; minVel0 = 1000.0;

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

		//境界における速度境界条件
	//速度境界条件
	//壁境界に対してはすべてDirihlet条件なのでinitObject()ルーチンで与えておけばよい

/*
		for (i = 0; i <= NX; i++) 
		{
			//下端
			k = 4 * i;
			k1 = 4 * (i + texWidth);
			velX[k] = 0.0;//velX[k1];
			velY[k] = 0.0;//velY[k1];
			//上端
			k = 4 * (i + NY * texWidth);
			k1 = 4 * (i + (NY-1) * texWidth);
			velX[k] = 0.0;//velX[k1]; 
			velY[k] = 0.0;//velY[k1]; 
		}
		for (j = 0; j <= NY; j++) 
		{
			//左端
			k = 4 * (j * texWidth);
			//k1 = 4 * (1 + j * texWidth);
			velX[k] = 0.0;//velX[k1];
			velY[k] = 0.0;//velY[k1];
			//右端
			k = 4 * (NX + j * texWidth);
			//k1 = 4 * (NX - 1 + j * texWidth);
			velX[k] = 0.0;//velX[k1]; 
			velY[k] = 0.0;//velY[k1]; 
		}
*/
		//障害物表面の速度境界条件
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
				else if(g_type[i][j] == LEFT)  velX[k] = 0.0;// velX[4*(i+1 + j * texWidth) + 3];//Neumann(左端）
				else if(g_type[i][j] == RIGHT) velX[k] = 0.0;//Dirichlet（右端）
				else if(g_type[i][j] == TOP) velX[k] = 0.0;//velX[4 * (i + (NY-1) * texWidth) + 3];
				else if(g_type[i][j] == BOTTOM) velX[k] =0.0;// velX[4 * (i + texWidth) + 3];
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
		renewVelY(); //速度Yの更新
		
		if(flagStep) flagFreeze = true;
		elapseTime2 += dt;
		elapseTime3 += deltaT;

		calcObsPos();
	}

	for(i = 1; i < NX; i++)
		for (j = 1; j < NY; j++)
		{
			k = 4 * (i + j * texWidth) + 3;

			if(g_type[i][j] >= OBS_LEFT) continue;
			if(velX[k] > maxPrs0) maxPrs0 = velX[k];
			if(velX[k] < minPrs0) minPrs0 = velX[k];
			if(velY[k] > maxVel0) maxVel0 = velY[k];
			if(velY[k] < minVel0) minVel0 = velY[k];
		}

//printf("maxPrs=%f minPrs=%f \n", maxPrs0, minPrs0);
//printf("maxOmg=%f minOmg=%f \n", maxVel0, minVel0);

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
	
	rigid[0].draw();
	
	//発散しないための目安を知るため
	float Courant, diff_num;
	if(DX < DY)
	{
		Courant = obsSpeed * deltaT / DX;	
		diff_num = (1.0 / Re) * deltaT / (DX * DX);//拡散数
	}
	else
	{
		Courant = obsSpeed * deltaT / DY;	
		diff_num = (1.0 / Re) * deltaT / (DY * DY);//拡散数
	}
	 
	if(flagParamShow && !flagOrtho) 
	{
		drawParam("f_rate=%d[fps]", fps0, 10, 30);
		drawParam("t_step=%1.5f[s]", dt, 130, 30);
		drawParam("e_time=%3.3f[s]", elapseTime2, 270, 30);
		drawParam("n_time=%3.3f", elapseTime3, 400, 30);
		drawParam("Courant=%1.4f", Courant, 10, 50);
		drawParam("diff_num=%1.4f", diff_num, 150, 50);
		drawParam("Re=%3.1f", Re,300, 50);
	}

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

	//printf("vel[5][NY/8] = %f  vel[10][NY/8] = %f \n", vel[3*(5 +(NY/8) * texWidth)], vel[3*(10 +(NY/8) * texWidth)]);
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
		for(int i = 0; i < texWidth; i++)
	   glVertex2f((float)i, (float)j);//点座標が2次元格子頂点番号
	glEnd();
}

void rendering()
{
	glUseProgram(shader_prog3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//velX[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//velY[]

	GLint samplerVelXLoc = glGetUniformLocation(shader_prog3, "samplerVelX");
	glUniform1i(samplerVelXLoc, 0);//GL_TEXTURE0を適用
	GLint samplerVelYLoc = glGetUniformLocation(shader_prog3, "samplerVelY");
	glUniform1i(samplerVelYLoc, 1);//GL_TEXTURE1を適用
	GLint texWLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint sizeXLoc = glGetUniformLocation(shader_prog3, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog3, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);

	GLint adjustHLoc = glGetUniformLocation(shader_prog3, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint colorLoc = glGetUniformLocation(shader_prog3, "color");
	glUniform1i(colorLoc, color);
	GLint kindLoc = glGetUniformLocation(shader_prog3, "kind");
	glUniform1i(kindLoc, kind);
	GLint maxOmgLoc = glGetUniformLocation(shader_prog3, "maxOmg");
	glUniform1f(maxOmgLoc, maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader_prog3, "maxPrs");
	glUniform1f(maxPrsLoc, maxPrs);
	GLint maxVelLoc = glGetUniformLocation(shader_prog3, "maxVel");
	glUniform1f(maxVelLoc, maxVel);
	GLint shadingLoc = glGetUniformLocation(shader_prog3, "flagShading");
	glUniform1i(shadingLoc, flagShading);

	static float specular[] = { 0.8, 0.8, 0.8, 1.0};
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS, 100.0);

	if(flagWireframe == 0)
		drawGridPlate((float)NX, (float)NY, NX, NY);
	else
		drawGridLines((float)NX, (float)NY, NX, NY);
	glUseProgram(0);
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
		if(rigid[0].vPos.x >=  7.0) direction = -1.0;
		if(rigid[0].vPos.x <= -7.0) direction =  1.0;
		rigid[0].vVel.x = direction * obsSpeed;
		//rigid[0].vPos += rigid[0].vVel * deltaT;
//		if(rigid[0].vPos.x >=  7.0) rigid[0].vVel.x = -obsSpeed;
//		if(rigid[0].vPos.x <= -7.0) rigid[0].vVel.x =  obsSpeed;
	}
	rigid[0].vPos += rigid[0].vVel * deltaT;
	left.x = rigid[0].vPos.x + rect.size.x/2.0;
	left.y = rigid[0].vPos.y + rect.size.y/2.0;

	float eps = 0.5;
	NX1 = floor((left.x - rect.obs_thick / 2.0) / DX + eps);//障害物左端位置
	NX2 = floor((left.x + rect.obs_thick / 2.0) / DX + eps);//障害物右端位置
	NY2 = floor((left.y + rect.obs_width/2.0) / DY + eps);//障害物上端位置
	NY1 = floor((left.y - rect.obs_width/2.0) / DY + eps);//障害物下端位置

	//障害物の新しい格子点のタイプ
	int i, j;
	for(i = 1; i < NX; i++)
		for(j = 1; j < NY; j++)
		{
			g_type[i][j] = INSIDE;//内点
			if(i == NX1 && j > NY1 && j < NY2) g_type[i][j] = OBS_LEFT;//障害物左端
			if(i == NX2 && j > NY1 && j < NY2) g_type[i][j] = OBS_RIGHT;//障害物右端
			if(i >= NX1 && i <= NX2 && j == NY2) g_type[i][j] = OBS_TOP;//障害物上端
			if(i >= NX1 && i <= NX2 && j == NY1) g_type[i][j] = OBS_BOTTOM;//障害物下端
			if(i > NX1 && i < NX2 && j > NY1 && j < NY2) g_type[i][j] = OBSTACLE;//障害物内部
			if(i == NX1 && j == NY1) g_type[i][j] = OBS_LL;
			if(i == NX1 && j == NY2) g_type[i][j] = OBS_UL;
			if(i == NX2 && j == NY1) g_type[i][j] = OBS_LR;
			if(i == NX2 && j == NY2)  g_type[i][j] = OBS_UR;
		}
}
