/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

波動方程式
GPGPU(数値計算をGPU側で実行）
移動障害物
GLSLによるシェーディング
鉛直軸はｚ軸
(初期設定：x軸は右方向，y軸は奥）
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
void setTextureWave();
void setFramebufferVel();
void setFramebufferWave();
void drawNumberingPoints();
void renewVelocity();
void renewPosition();
void decideRegion();
float regionFlag(float x, float y);
void calcObsPos();

//解析用データのサイズ
int texWidth, texHeight;//全格子数

//解析用データ配列
float *wave;
GLuint texID[1];//texture object
GLuint fbo[1];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RGBA;
GLenum type = GL_FLOAT;
//シェーダプログラム名
GLuint shader_prog1, shader_prog2, shader_prog3;

int nDummy;
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
	main_window = glutCreateWindow("GP_Wave3");
	//ウィンドウのサイズ変更
	glutReshapeFunc(resize);
	//表示
	glutDisplayFunc(display);
	//マウス操作
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//再描画
	glutIdleFunc(idle);
	//初期設定
	glewInit();//framebufferを利用するときに必要
	init();
	initGlsl(&shader_prog1, "simulation.vert", "waveVel.frag");
	initGlsl(&shader_prog2, "simulation.vert", "wavePos.frag");
	initGlsl(&shader_prog3, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.hに実装

	//イベント処理ループに入る
	glutMainLoop();
	//リソースの解放
 	free(wave);
	glDeleteTextures(1, texID);
	glDeleteFramebuffersEXT(1, fbo);
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
	view.dist= 25.0;
	view.vCnt = Vector3D(0.0, 0.0, 0.0);
	view.theta = 30.0;
	view.phi = -90.0;
	setCamera();
	view0 = view;

	lightPos[1] = 200.0;
	for(int i = 0; i < 3; i++) lightPos0[i] = lightPos[i];
	//parameters
	//timeRate = 1.0;

	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	glGenTextures(1, texID);//テクスチャオブジェクトの名前付け
	glGenFramebuffersEXT(1, fbo);//フレームバッファオブジェクトの生成

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
	nDummy = 30;

	//物理量テクセルサイズ
	texWidth  = rect.meshX + 2 * nDummy + 1;//座標テクスチャの横サイズ
	texHeight = rect.meshY + 2 * nDummy + 1;//座標テクスチャの横サイズ
	//texHeight = texWidth;                //座標テクスチャの縦サイズ
	
	//物理量配列の宣言
	wave = (float*)malloc(4 * texWidth * texHeight * sizeof(float));

	//initialize
	int i, j, k;
	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//各格子点における水面の高さとｚ軸方向速度
			wave[k * 4 + 0] = 0.0;//水面のｚ軸方向速度
			wave[k * 4 + 1] = 0.0;//水面の高さ
			wave[k * 4 + 2] = 0.0;
			wave[k * 4 + 3] = 1.0;//障害物内外判定に利用		
		}

	//障害物
	//全てのパターンに共通
	numRigid = 1;
	for(i = 0; i < RIGID_NUM_MAX; i++)	rigid[i] = CRigid();
	
	rigid[0].kind = CUBE;
	rigid[0].color = DARK_YELLOW;
	rigid[0].flagFixed = false;
	rigid[0].vPos = Vector3D(-rect.size.x/2.0 + rect.obs_left, 0.0, 0.0);
	rigid[0].vSize = Vector3D(rect.obs_thick, rect.obs_width, 1.0);

	if(pattern == 1)
	{
		numRigid = 3;
		rigid[1].kind = CUBE;
		rigid[1].color = GRAY;
		rigid[1].flagFixed = true;
		rigid[1].vSize = Vector3D(rect.size.x, 0.4, 1.0);
		rigid[1].vPos = Vector3D(0.0, rect.size.y/2.0 + 0.5 , 0.0);

		rigid[2].kind = CUBE;
		rigid[2].color = GRAY;
		rigid[2].flagFixed = true;
		rigid[2].vSize = Vector3D(rect.size.x, 0.4, 1.0);
		rigid[2].vPos = Vector3D(0.0, -rect.size.y/2.0 - 0.5 , 0.0);
	}
	decideRegion();
	setTextureWave();
	setFramebufferWave();

/*
j=texHeight /2;
for(i=0;i < rect.meshX;i++)
{
	k=i+j*texWidth;
	printf("i = %d , a = %f \n", i, wave[k*4+3]);
}*/

	elapseTime1 = 0.0;//1sec間以内の経過時間
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;
}

void setTextureWave()
{
	glBindTexture(target, texID[0]);
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
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0 );
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

	static int countStrobo = 0;
	if(elapseTime1 >= 1.0)
	{
		elapseTime1 = 0.0;
		fps0 = fps;
		fps = 0;
	}
	QueryPerformanceCounter( &lastTime);

	if(flagStart==true && flagFreeze == false) 
	{
		//step1:速度の更新
		renewVelocity();

		//step2:位置の更新
		renewPosition();

		calcObsPos();

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
	for(int k = 0; k < numRigid; k++) rigid[k].draw();

	if(flagCoordShow) drawWorldCoordinates(5.0);//ワールド空間の座標軸
	
	if(flagParamShow) 
	{
		drawParam("f_rate=%d[fps]", fps0, 10, 30);
		drawParam("t_step=%1.5f[s]", dt, 150, 30);
		drawParam("e_time=%3.3f[s]", elapseTime2, 300, 30);
		drawParam("n_time=%3.3f", elapseTime3, 460, 30);
	}
	//バッファの切り替え
	glutSwapBuffers();	
}

void renewVelocity()
{
	//framebuffer object0を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//wave[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog1);
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog1, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE0を適用
	GLint texWidthLoc = glGetUniformLocation(shader_prog1, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog1, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint meshXLoc = glGetUniformLocation(shader_prog1, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog1, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint dummyLoc = glGetUniformLocation(shader_prog1, "nDummy");
	glUniform1i(dummyLoc, nDummy);
	GLint sizeXLoc = glGetUniformLocation(shader_prog1, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog1, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);
	GLint ampLoc = glGetUniformLocation(shader_prog1, "amp");
	glUniform1f(ampLoc, amp0);
	GLint freqLoc = glGetUniformLocation(shader_prog1, "freq");
	glUniform1f(freqLoc, freq0);
	GLint timeLoc = glGetUniformLocation(shader_prog1, "time");
	glUniform1f(timeLoc, elapseTime2);
	GLint dragLoc = glGetUniformLocation(shader_prog1, "drag0");
	glUniform1f(dragLoc, drag0);
	GLint waveSpeedLoc = glGetUniformLocation(shader_prog1, "waveSpeed");
	glUniform1f(waveSpeedLoc, waveSpeed);
	GLint dtLoc = glGetUniformLocation(shader_prog1, "dt");
	glUniform1f(dtLoc, deltaT);

	GLint posObjLoc = glGetUniformLocation(shader_prog1, "posObs");
	float pos[] ={ rigid[0].vPos.x, rigid[0].vPos.y, rigid[0].vPos.z};
	glUniform3fv(posObjLoc, 1, pos);
	GLint sizeObjLoc = glGetUniformLocation(shader_prog1, "sizeObs");
	float size[] = { rigid[0].vSize.x, rigid[0].vSize.y, rigid[0].vSize.z};
	glUniform3fv(sizeObjLoc, 1, size);
	GLint velObjLoc = glGetUniformLocation(shader_prog1, "velObs");
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

void renewPosition()
{
	//framebuffer object0を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//wave[]
	//シェーダプログラムを有効
	glUseProgram(shader_prog2);
	GLint samplerWaveLoc = glGetUniformLocation(shader_prog2, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE0を適用
	GLint dtLoc = glGetUniformLocation(shader_prog2, "dt");
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
		for(int i = 0; i < texWidth; i++)
	   glVertex2f((float)i, (float)j);//点座標が2次元格子頂点番号
	glEnd();

}

void rendering()
{
	glUseProgram(shader_prog3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//wave[]

	GLint samplerWaveLoc = glGetUniformLocation(shader_prog3, "samplerWave");
	glUniform1i(samplerWaveLoc, 0);//GL_TEXTURE1を適用
	GLint texWidthLoc = glGetUniformLocation(shader_prog3, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint texHeightLoc = glGetUniformLocation(shader_prog3, "texHeight");
	glUniform1i(texHeightLoc, texHeight);
	GLint dummyLoc = glGetUniformLocation(shader_prog3, "nDummy");
	glUniform1i(dummyLoc, nDummy);

	GLint meshXLoc = glGetUniformLocation(shader_prog3, "meshX");
	glUniform1i(meshXLoc, rect.meshX);
	GLint meshYLoc = glGetUniformLocation(shader_prog3, "meshY");
	glUniform1i(meshYLoc, rect.meshY);
	GLint sizeXLoc = glGetUniformLocation(shader_prog3, "sizeX");
	glUniform1f(sizeXLoc, rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader_prog3, "sizeY");
	glUniform1f(sizeYLoc, rect.size.y);

	static float diffuse[] = { 0.4, 0.6, 0.9, 1.0};
	static float ambient[] = { 0.1, 0.2, 0.3, 1.0};
	static float specular[]= { 0.5, 0.5, 0.5, 1.0};

	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf(GL_FRONT,GL_SHININESS,100.0);
	if(flagWireframe == 0)
		drawGridPlate((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);
	else
		drawGridLines((float)rect.meshX, (float)rect.meshY, rect.meshX, rect.meshY);

	glUseProgram(0);
}

void decideRegion()
{
	int i, j, k;
	float x, y;
	float dx = rect.size.x / (float)rect.meshX;
	float dy = rect.size.y / (float)rect.meshY;

	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//格子点の座標
			x = (float)(i - texWidth / 2) * dx;
			y = (float)(j - texHeight / 2) * dy;
			wave[k * 4 + 3] = regionFlag(x, y);
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
			 && vPoint.y >= - rigid[k].vSize.y / 2.0 && vPoint.y <= rigid[k].vSize.y / 2.0) //return 0.0;//障害物内部
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
	//if(flagObsStop) { rigid[0].vVel = Vector3D();  return; }

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
	left.x = rigid[0].vPos.x + rect.size.x/2.0;
	left.y = rigid[0].vPos.y + rect.size.y/2.0;
}
