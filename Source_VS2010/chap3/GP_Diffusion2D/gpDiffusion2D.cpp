/*
Microsoft Visual Studio .NET2005 Standard Edition
プロジェクトの種類：VisualC++プロジェクト
テンプレート：Win32コンソールプロジェクト
アプリケーションの種類：コンソールアプリケーション
追加のオプション：空のプロジェクト

2次元拡散方程式
3D表示
GPGPU(数値計算をGPU側で実行）

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
#include "../../support3D.h"
#include "myGLUI.h"

//関数のプロトタイプ宣言
void init();
void idle();
void display();
void rendering();
void setTexturePhi();
void setFramebufferPhi();
void drawNumberingPoints();
void renewPhi(float d);
void createFloor();

//解析用データのサイズ
int texWidth, texHeight;//全格子数

//解析用データ配列
float *phi;
GLuint texID[1];//texture object
GLuint fbo[1];//frame buffer object

GLenum target = GL_TEXTURE_RECTANGLE_ARB;
GLenum internalFormat = GL_RGBA32F_ARB;
GLenum format = GL_RED;//RGBA;
GLenum type = GL_FLOAT;
//シェーダプログラム名
GLuint shader1, shader2;

int floor0;//floorのdisplay list

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
	main_window = glutCreateWindow("GP_Diffusion2D");
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
	initGlsl(&shader1, "simulation.vert", "simulation.frag");
	initGlsl(&shader2, "rendering.vert", "rendering.frag");

	setupGLUI();//myGLUI.hに実装
	GLUI_Master.set_glutIdleFunc( idle );

	//イベント処理ループに入る
	glutMainLoop();
	//リソースの解放
 	free(phi);
	glDeleteTextures(1, texID);
	glDeleteFramebuffersEXT(1, fbo);
	glDeleteProgram(shader1);
	glDeleteProgram(shader2);
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
	view.vCnt = Vector3D(0.0, 0.0, 2.0);
	view.theta = 10.0;
	view.phi = -90.0;//-y軸が手前，右が +x軸
	view.fovY = 20.0;
	view.nearZ =  0.1;
	view.farZ = 200.0;    	
	setCamera();
	view0 = view;

	//時間関係
	memset(&freq,  0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter( &lastTime);

	glGenTextures(1, texID);//テクスチャオブジェクトの名前付け
	glGenFramebuffersEXT(1, fbo);//フレームバッファオブジェクトの生成

	//表示ウィンドウに文字をレンダリングする準備
	setFontBitmaps();

	initData();
	createFloor();

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
	flagFreeze = 0;
	flagStep = false;

	rect.delta.x = rect.size.x / (float)rect.nMesh;
	rect.delta.y = rect.size.y / (float)rect.nMesh;

	//物理量テクセルサイズ
	texWidth  = rect.nMesh + 1;//座標テクスチャの横サイズ
	texHeight = texWidth;      //座標テクスチャの縦サイズ
	//物理量配列の宣言
	phi = (float*)malloc(texWidth * texHeight * sizeof(float));
	
	int i, j, k;
	float x, y, r;

	for(j = 0; j < texHeight; j++)
		for(i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			//矩形中心からの距離
			x = (float)(i - texWidth / 2) * rect.delta.x ;
			y = (float)(j - texHeight / 2) * rect.delta.y;
			if(profile == 0)//Cylinder
			{
				r = sqrt(x * x + y * y);
				if(r < radiusInit) phi[k] = 1.0;//物理量
				else phi[k] = 0.0;
			}
			else//Cube
			{
				if(fabs(x) < radiusInit && fabs(y) < radiusInit) phi[k] = 1.0;
				else phi[k] = 0.0;
			}
		}
	setTexturePhi();
	setFramebufferPhi();

	elapseTime1 = 0.0;//1sec間以内の経過時間
	elapseTime2 = 0.0;//start後の総経過時間
}

void setTexturePhi()
{
	glBindTexture(target, texID[0]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, phi);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferPhi()
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

	float deltaT = deltaT0 / (float)thinningN;
	float diff_num = diff_coef * deltaT / (rect.delta.x * rect.delta.x);//拡散数
	if(flagStart==true && flagFreeze == false)
	{
		//物理量の更新
		for(int i = 0; i < thinningN; i++) renewPhi( diff_num ); 

		if(flagStep) flagFreeze = true;
		elapseTime2 += deltaT;//dt;
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

	if(flagWireframe == 1)
	{
	  glPolygonMode(GL_FRONT,GL_LINE);
	  glPolygonMode(GL_BACK,GL_POINT);
	}
	else 
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	rendering();
	
	glCallList(floor0);//床・地面
	if(flagScaleShow) drawScale(10.0, 10.0, 5.0);
	if(flagCoordShow) drawWorldCoordinates(3.0);

	if(flagParamShow) 
	{
		drawParam("framerate=%d[fps]", fps0, 10, 30);
		drawParam("timestep=%1.5f[s]", dt, 200, 30);
		drawParam("elapsetime=%3.3f[s]", elapseTime2, 400, 30);
		drawParam("deltaX=%f", rect.delta.x, 10, 60);
		drawParam("deltaT=%1.4f[s]", deltaT, 200, 60);
		drawParam("diff_num=%1.4f", diff_num, 400, 60);
	}
	//バッファの切り替え
	glutSwapBuffers();	
}

void renewPhi(float diff_num)
{
	//framebuffer object0を有効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo[0] );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//phi[]
	//シェーダプログラムを有効
	glUseProgram(shader1);
	GLint samplerPhiLoc = glGetUniformLocation(shader1, "samplerPhi");
	glUniform1i(samplerPhiLoc, 0);//GL_TEXTURE0を適用
	GLint texWidthLoc = glGetUniformLocation(shader1, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint diffLoc = glGetUniformLocation(shader1, "diff_num");
	glUniform1f(diffLoc, diff_num);
	GLint sizeLoc = glGetUniformLocation(shader1, "size0");
	glUniform1f(sizeLoc, rect.size.x);
	GLint boundaryLoc = glGetUniformLocation(shader1, "boundary");
	glUniform1i(boundaryLoc, boundary);

	drawNumberingPoints();//数値解析用テクセルを貼り付けるオブジェクト

	// シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, phi);
	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	if(flagConstant)
	{
		int i, j, k;
		float x, y, r;
		for(j = 0; j < texHeight; j++)
			for(i = 0; i < texWidth; i++)
			{
				k = i + j * texWidth;
				//矩形中心からの距離
				x = (float)(i - texWidth / 2) * rect.delta.x ;
				y = (float)(j - texHeight / 2) * rect.delta.y;
				if(profile == 0)//Cylinder
				{
					r = sqrt(x * x + y * y);
					if(r < radiusInit) phi[k] = 1.0;//物理量（温度，濃度etc.)
				}
				else//Cube
				{
					if(fabs(x) < radiusInit && fabs(y) < radiusInit) phi[k] = 1.0;//物理量（温度，濃度etc.)
				}
			}
		setTexturePhi();
		setFramebufferPhi();
	}
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
	glUseProgram(shader2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[0]);//phi[]

	GLint samplerPhiLoc = glGetUniformLocation(shader2, "samplerPhi");
	glUniform1i(samplerPhiLoc, 0);//GL_TEXTURE0を適用
	GLint texWidthLoc = glGetUniformLocation(shader2, "texWidth");
	glUniform1i(texWidthLoc, texWidth);
	GLint nMeshLoc = glGetUniformLocation(shader2, "nMesh");
	glUniform1i(nMeshLoc, rect.nMesh);
	GLint sizeLoc = glGetUniformLocation(shader2, "size0");
	glUniform1f(sizeLoc, rect.size.x);
	GLint adjustHLoc = glGetUniformLocation(shader2, "adjustH");
	glUniform1f(adjustHLoc, adjustH);
	GLint colorLoc = glGetUniformLocation(shader2, "color");
	glUniform1i(colorLoc, color);

	if(flagWireframe == 0)
		drawGridPlate((float)rect.nMesh, (float)rect.nMesh, rect.nMesh, rect.nMesh);
	else
		drawGridLines((float)rect.nMesh, (float)rect.nMesh, rect.nMesh, rect.nMesh);

	glUseProgram(0);
}

void createFloor()
{
	floor0 = glGenLists(1);
	if(floor0 == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(floor0, GL_COMPILE);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -0.001);
	drawFloorZ(rect.size.x, rect.size.y, 2, 2);
	glPopMatrix();
	glEndList();
}
