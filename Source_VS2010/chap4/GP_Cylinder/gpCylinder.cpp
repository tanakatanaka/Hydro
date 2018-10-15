/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元FDM
円柱まわりの流れ(全円）
極座標変換，さらに動径方向に不等間隔格子
流れ関数-渦度法
GPGPU(数値計算をGPU側で実行）
CIPを使用
渦度のPoisson方程式を厳密には解いてない（１回だけ）

GLSLによるシェーディング
鉛直軸はｚ軸
(初期設定：x軸は手前，y軸は右方向）
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
//void initData();
void idle();
void display();
void rendering();
void setTextureOmega();
void setFramebufferOmega();
void setTexturePsi();
void setFramebufferPsi();
void drawNumberingPoints();
void renewPsi();
void renewOmega(float deltaT);
//void createFloor();

//解析用データのサイズ
int texWidth, texHeight;//全格子数

//解析用データ配列
float *omg;//渦度
float *psi;//流れ関数，速度
GLuint texID[2];//texture object
GLuint fbo[2];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGB;//物理量はpsi[]およびomgのr,g,b
GLenum type = GL_FLOAT;
//シェーダプログラム名
GLuint shader_prog1, shader_prog2, shader_prog3;
int NX, NY;
float DX, DY;
int count = 0;

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
	main_window = glutCreateWindow("GP_Cylinder");
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
	initGlsl(&shader_prog1, "simulation.vert", "calcPsi.frag");
	initGlsl(&shader_prog2, "simulation.vert", "calcOmega.frag");
	initGlsl(&shader_prog3, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.hに実装

	//イベント処理ループに入る
	glutMainLoop();
	//リソースの解放
 	free(omg);
	free(psi);
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
	view.dist = 80.0;
	view.vCnt = Vector3D(10.0, 0.0, 0.0);
	view.theta = 90.0;//手前がｚ軸（視点が真上）
	view.phi = -90.0;//右が +x軸
	setCamera();
	view0 = view;
	ortho_scale = 10.0;
	//parameters

	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

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

	NX = region.nMesh;//動径方向分割数
	NY = 2 * NX;     //方位角方向
	
	//円柱の半径は1に固定
	float xiMax = log(region.Radius);//計算面における外側半径(円柱半径は1.0に固定）
	DX = xiMax / (float)NX;//動径方向刻み
	DY = 2.0 * M_PI / (float)(NY); //方位角方向刻み

	//物理量テクセルサイズ
	texWidth  = NX + 1;//座標テクスチャの横サイズ
	texHeight = NY + 1;//座標テクスチャの縦サイズ
	
	//物理量配列の宣言
	psi = (float*)malloc(3 * texWidth * texHeight * sizeof(float));//流れ関数と速度
	omg = (float*)malloc(3 * texWidth * texHeight * sizeof(float));//渦度とその微分
	//initialize
	int i, j, k, km, kp;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//渦度
			//if(j >= 5 && j <= 7 ) omg[3 * k + 0] = 10.0;
			//else
			omg[3 * k + 0] = 0.0;
			omg[3 * k + 1] = 0.0;//x微分
			omg[3 * k + 2] = 0.0;//y微分
			psi[3 * k + 0] = exp((float)i * DX) * sin((float)j * DY - M_PI);//流れ関数(分岐線を左側）
		}
	//格子点の速度ベクトル(上下左右の流れ関数の差で求める)
	float ex;
	int jm, jp;
	for(i = 1; i < NX; i++)
	{
		ex = exp(- (float)i * DX);
		for (j = 0; j <= NY; j++)
		{
			k = i + j * texWidth;
			if(j == 0) jm = NY -1; else 
			jm = j - 1;
			//if(j == NY-1) jp = 0; else 
				jp = j + 1;
			if(j == NY) jp = 1;
			kp = i + jp * texWidth;
			km = i + jm * texWidth;
			//ポテンシャルの低い方から高い方へ
			psi[3 * k + 1] = ex * (psi[3*kp] - psi[3*km]) / (2.0*DY);//動径方向
			psi[3 * k + 2] = -ex * (psi[3*(i+1 + j*texWidth)] - psi[3*(i-1 + j*texWidth)]) / (2.0*DX);//方位角方向
		}
	}
	setTexturePsi();
	setFramebufferPsi();
	setTextureOmega();
	setFramebufferOmega();

	numRigid = 1;
	for(i = 0; i < numRigid; i++) rigid[i] = CRigid();
	//円柱
	rigid[0].kind = CYLINDER;
	rigid[0].color = GRAY;
	rigid[0].vPos = Vector3D(0.0, 0.0, 0.0);
	rigid[0].vSize = Vector3D(2.0, 2.0, 1.0);//半径1.0

	elapseTime1 = 0.0;//1sec間以内の経過時間
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元)
	count = 0;
}

void setTexturePsi()
{
	glBindTexture(target, texID[0]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, psi);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureOmega()
{
	glBindTexture(target, texID[1]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, omg);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferPsi()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0 );
	//framebuffer object の無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
void setFramebufferOmega()
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
	
	if(flagStart && flagFreeze == false)
	{
		for(int n = 0; n < dispSpeed; n++)
		{
			for (int j = 0; j < NY; j++) 
			{
				//円柱境界上
				int k = j * texWidth;
				omg[3 * k] = -2.0 * psi[3*(1 + j * texWidth)] /(DX*DX);
				psi[3 * k] = 0.0;
			
				//外部境界
				//i == NX
				k = NX + j * texWidth;
				omg[3 * k] = omg[3 * (NX-1 + j * texWidth)];//Neumann 
				//psi[3 * k] = region.Radius * sin((float)j * DY - M_PI);
			}

			setTexturePsi();
			setFramebufferPsi();
			setTextureOmega();
			setFramebufferOmega();
			
			renewPsi();         //流れ関数の更新
			renewOmega(deltaT); //渦度の更新
		}
		if(flagStep) flagFreeze = 1;
		elapseTime2 += dt;
		elapseTime3 += deltaT * dispSpeed;
		count += dispSpeed;
//printf("count = %d \n", count);
//if(count == 1000) flagFreeze = 1;
//if(count == 2500) flagFreeze = 1;
//if(count == 50000) flagFreeze = 1;

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
	rigid[0].draw();
	
//	if(flagCoordShow) drawWorldCoordinates(0.6);
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
		drawParam("t_step=%1.5f[s]", dt, 120, 30);
		drawParam("e_time=%3.3f[s]", (float)elapseTime2, 280, 30);
		drawParam("n_time=%3.3f", elapseTime3, 440, 30);
		drawParam("Courant=%1.4f", Courant, 10, 60);
		drawParam("diff_num=%1.4f", diff_num, 150, 60);
		drawParam("Re=%3.1f", Re, 300, 60);
	}

	//バッファの切り替え
	glutSwapBuffers();	
}

void renewPsi()
{
	//framebuffer object1を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//psi[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//omg[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog1);
	GLint samplerPsiLoc = glGetUniformLocation(shader_prog1, "samplerPsi");
	glUniform1i(samplerPsiLoc, 0);//GL_TEXTURE0を適用
	GLint samplerOmgLoc = glGetUniformLocation(shader_prog1, "samplerOmg");
	glUniform1i(samplerOmgLoc, 1);//GL_TEXTURE1を適用
	GLint texWLoc = glGetUniformLocation(shader_prog1, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog1, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint DXLoc = glGetUniformLocation(shader_prog1, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog1, "DY");
	glUniform1f(DYLoc, DY);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, psi);

	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void renewOmega(float deltaT)
{
	//framebuffer object0を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[1] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//psi[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//omg[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog2);
	GLint samplerPsiLoc = glGetUniformLocation(shader_prog2, "samplerPsi");
	glUniform1i(samplerPsiLoc, 0);//GL_TEXTURE0を適用
	GLint samplerOmgLoc = glGetUniformLocation(shader_prog2, "samplerOmg");
	glUniform1i(samplerOmgLoc, 1);//GL_TEXTURE1を適用
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
	glReadPixels(0, 0, texWidth, texHeight, format, type, omg);

	//printf("omg[5][NY/8] = %f  omg[10][NY/8] = %f \n", omg[3*(5 +(NY/8) * texWidth)], omg[3*(10 +(NY/8) * texWidth)]);
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
	glBindTexture(target, texID[0]);//psi[]
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(target, texID[1]);//omg[]

	GLint samplerPsiLoc = glGetUniformLocation(shader_prog3, "samplerPsi");
	glUniform1i(samplerPsiLoc, 0);//GL_TEXTURE0を適用
	GLint samplerOmgLoc = glGetUniformLocation(shader_prog3, "samplerOmg");
	glUniform1i(samplerOmgLoc, 1);//GL_TEXTURE1を適用
	GLint texWLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint adjustHLoc = glGetUniformLocation(shader_prog3, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint radiusLoc = glGetUniformLocation(shader_prog3, "Radius");
	glUniform1f(radiusLoc, region.Radius);
	GLint colorLoc = glGetUniformLocation(shader_prog3, "color");
	glUniform1i(colorLoc, color);
	GLint kindLoc = glGetUniformLocation(shader_prog3, "kind");
	glUniform1i(kindLoc, kind);
	GLint DXLoc = glGetUniformLocation(shader_prog3, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader_prog3, "DY");
	glUniform1f(DYLoc, DY);
	GLint maxOmegaLoc = glGetUniformLocation(shader_prog3, "maxOmega");
	glUniform1f(maxOmegaLoc, maxOmega);
	GLint maxPsiLoc = glGetUniformLocation(shader_prog3, "maxPsi");
	glUniform1f(maxPsiLoc, maxPsi);
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
