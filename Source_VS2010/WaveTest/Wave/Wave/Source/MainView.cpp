#pragma once
#include <iostream>
#include <windows.h>
#include "MainView.h"
#include "Assist3D.h"
#include "../../../../rigid.h"
#include"Scene/CausticsScene/CausticsScene.h"
#include "../../../../myPrimitive6.h"
using std::cout;
using std::endl;
#include <cstdio>
class MyGLUI;
class Assist3D;

static void drawNumberingPoints();
static void calcObsPos();
static void makeTextureOfCubeMapping();
static void Rendering();
static void renderingP();
static void drawParticles();
static void drawUpperObject();
static void drawLowerObject();
static void drawShadow();
static void drawGridPlate(float sizeX, float sizeY, int nSliceX, int nSliceY);

void  MainView::InitializeGL(int argc, char** argv)
{
	LARGE_INTEGER freq;

	glutInit(&argc, argv);
	//表示モード
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//表示ウィンドウのサイズ
	glutInitWindowSize(myGLUI->assist3d.win_width, myGLUI->assist3d.win_height);
	//左上の位置
	glutInitWindowPosition(myGLUI->assist3d.win_x0, myGLUI->assist3d.win_y0);
	//ウィンドウ作成
	myGLUI->main_window = glutCreateWindow("GP_Caustics2");
	//ウィンドウのサイズ変更
	glutReshapeFunc(myGLUI->assist3d.Resize);
	//表示
	glutDisplayFunc(Display);
	
	scene = std::make_unique<CausticsScene>();
	scene->InitScene();
}


void MainView::paintGL() 
{

	//scene->update(0.0f);
    //GLUtils::checkForOpenGLError(__FILE__,__LINE__);
	//scene->render();
}

void MainView::resizeGL(int w, int h )
{
    //scene->resize(w,h);
}

void Display(void)
{
	//時間計測
	QueryPerformanceFrequency(&myGLUI->assist3d.freq);
	QueryPerformanceCounter(&myGLUI->assist3d.currentTime);
	myGLUI->assist3d.pTime = (double)((myGLUI->assist3d.currentTime.QuadPart - myGLUI->assist3d.lastTime.QuadPart) * 1000000 / myGLUI->assist3d.freq.QuadPart);
	double dt = myGLUI->assist3d.pTime / 1000000.0;//[sec]
	myGLUI->assist3d.elapseTime1 += dt;
	static int fps = 0;
	static int fps0 = 0;
	fps++;

	if (myGLUI->assist3d.elapseTime1 >= 1.0)
	{
		myGLUI->assist3d.elapseTime1 = 0.0;
		fps0 = fps;
		fps = 0;
	}
	QueryPerformanceCounter(&myGLUI->assist3d.lastTime);

	int i, j, k;

	if (myGLUI->assist3d.flagStart == true && myGLUI->assist3d.flagFreeze == false)
	{
		//移動障害物表面の速度境界条件
		for (i = 1; i < NX; i++)
			for (j = 0; j <= NY; j++)
			{
				if (g_type[i][j] < OBS_LEFT) continue;
				k = 4 * (i + j * texWidth);
				velX[k] = rigid[0].vVel.x;
				velY[k] = rigid[0].vVel.y;
			}

		//圧力境界条件
		for (i = 0; i <= NX; i++)
			for (j = 0; j <= NY; j++)
			{
				k = 4 * (i + j * texWidth) + 3;//圧力
				if (g_type[i][j] == INSIDE) continue;
				else if (g_type[i][j] == LEFT)  velX[k] = 0.0;//-x
				else if (g_type[i][j] == RIGHT) velX[k] = 0.0;//+x
				else if (g_type[i][j] == TOP) velX[k] = 0.0;//+y
				else if (g_type[i][j] == BOTTOM) velX[k] = 0.0;//-y
				else if (g_type[i][j] == OBS_LEFT) velX[k] = velX[4 * (i - 1 + j * texWidth) + 3];
				else if (g_type[i][j] == OBS_RIGHT) velX[k] = velX[4 * (i + 1 + j * texWidth) + 3];
				else if (g_type[i][j] == OBS_TOP) velX[k] = velX[4 * (i + (j + 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_BOTTOM) velX[k] = velX[4 * (i + (j - 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_UL) velX[k] = velX[4 * (i - 1 + (j + 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_UR) velX[k] = velX[4 * (i + 1 + (j + 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_LL) velX[k] = velX[4 * (i - 1 + (j - 1) * texWidth) + 3];
				else if (g_type[i][j] == OBS_LR) velX[k] = velX[4 * (i + 1 + (j - 1) * texWidth) + 3];
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
					   int k3 = 3*(i + texWidth * texHeight/2);
					   float aa = (float)caus[k3];
					   if(aa > 200.0) printf("i=%d caus=%f \n", i, aa);
					   }*/
					   //particle
		for (j = 0; j < texHeight; j++)
			for (i = 0; i < texWidth; i++)
			{
				k = i + j * texWidth;
				if (k > myGLUI->numParticle) break;
				//速度X
				if (particle[4 * k + 0] > myGLUI->rect.size.x / 2.0 || particle[4 * k + 0] < - myGLUI->rect.size.x / 2.0 ||
					particle[4 * k + 1] > myGLUI->rect.size.y / 2.0 || particle[4 * k + 1] < - myGLUI->rect.size.y / 2.0)
				{
					particle[4 * k + 0] = getRandom(-myGLUI->rect.size.x / 2.0, myGLUI->rect.size.x / 2.0);
					particle[4 * k + 1] = getRandom(-myGLUI->rect.size.y / 2.0, myGLUI->rect.size.y / 2.0);
				}
			}
		setTextureParticle();
		setFramebufferParticle();

		renewParticle();//粒子位置の更新

		if (myGLUI->assist3d.flagStep)
		{
			myGLUI->assist3d.flagFreeze = true;
		}

		myGLUI->assist3d.elapseTime2 += dt;
		myGLUI->assist3d.elapseTime3 += myGLUI->deltaT;

		calcObsPos();

	}

	//環境マップのテクスチャ作成
	makeTextureOfCubeMapping();

	//カラーバッファ,デプスバッファのクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myGLUI->assist3d.Resize(myGLUI->assist3d.win_width, myGLUI->assist3d.win_height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//視点を変えるときはこの位置に必要

	Assist3D::View &view = myGLUI->assist3d.view;

	if (cos(M_PI * view.theta / 180.0) >= 0.0)//カメラ仰角90度でﾋﾞｭｰｱｯﾌﾟﾍﾞｸﾄﾙ切替
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, 1.0);
	else
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vCnt.x, view.vCnt.y, view.vCnt.z, 0.0, 0.0, -1.0);

	//光源設定
	glLightfv(GL_LIGHT0, GL_POSITION, myGLUI->assist3d.lightPos);

	if (myGLUI->flagWireframe == 1)//'w'でwireframeとsolid model切り替え
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_POINT);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Rendering();//水面

	if (myGLUI->flagShowParticle)
	{
		renderingP();
	}

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);//背面は描画しない
	drawUpperObject();

	//投影マッピング
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, texID[5]);//caus[]
									// シェーダプログラムの適用 
	glUseProgram(shader8);
	GLint texLoc = glGetUniformLocation(shader8, "samplerCaus");
	glUniform1i(texLoc, 0);//GL_TEXTURE0を適用

	drawLowerObject();
	//fish1.motion(elapseTime2);
	//fish2.motion(elapseTime2);
	if (myGLUI->flagShadowShow)
	{
		drawShadow();
	}
	//glDisable(GL_CULL_FACE);

	// シェーダプログラムの適用を解除	
	glUseProgram(0);

	//発散しないための目安を知るため
	float Courant, diff_num;
	if (DX < DY)
	{
		Courant = myGLUI->deltaT / DX;
		diff_num = (1.0 / myGLUI->Re) * myGLUI->deltaT / (DX * DX);//拡散数
	}
	else
	{
		Courant = myGLUI->deltaT / DY;
		diff_num = (1.0 / myGLUI->Re) * myGLUI->deltaT / (DY * DY);//拡散数
	}

	if (flagParamShow)
	{
		myGLUI->assist3d.drawParam("f_rate=%d[fps]", fps0, 10, 30);
		myGLUI->assist3d.drawParam("t_step=%1.5f[s]", dt, 150, 30);
		myGLUI->assist3d.drawParam("e_time=%3.3f[s]", myGLUI->assist3d.elapseTime2, 300, 30);
		myGLUI->assist3d.drawParam("n_time=%3.3f", myGLUI->assist3d.elapseTime3, 460, 30);
		myGLUI->assist3d.drawParam("Courant=%1.4f", Courant, 10, 50);
		myGLUI->assist3d.drawParam("diff_num=%1.4f", diff_num, 150, 50);
		myGLUI->assist3d.drawParam("Re=%3.1f", myGLUI->Re, 300, 50);
	}

	if (myGLUI->flagCoordShow)
	{
		myGLUI->assist3d.drawWorldCoordinates(5.0);
	}
	//バッファの切り替え
	glutSwapBuffers();
}

void Rendering()
{
	glUseProgram(shader5);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(target, texID[2]);//wave[]

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[4]);

	GLint texLoc = glGetUniformLocation(shader5, "smplRefract");
	glUniform1i(texLoc, 3);//GL_TEXTURE3を適用
	GLint tranLoc = glGetUniformLocation(shader5, "transparency");
	glUniform1f(tranLoc, myGLUI->transparency);
	GLint ratioLoc = glGetUniformLocation(shader5, "nRatio");
	glUniform1f(ratioLoc, myGLUI->nRatio);
	GLint inverseLoc = glGetUniformLocation(shader5, "flagInverse");
	int flagInverse = -1;
	if (myGLUI->assist3d.view.vPos.z < myGLUI->waveHeight)
	{
		flagInverse = 1;//視点が水面以下
	}
	glUniform1i(inverseLoc, flagInverse);
	float m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	GLint ViewTransLoc = glGetUniformLocation(shader5, "ViewTranspose");
	glUniformMatrix4fv(ViewTransLoc, 1, GL_TRUE, m);//GL_TRUEで転置行列
	GLint samplerWaveLoc = glGetUniformLocation(shader5, "samplerWave");
	glUniform1i(samplerWaveLoc, 2);//GL_TEXTURE0を適用
	GLint texWLoc = glGetUniformLocation(shader5, "texWidth");
	glUniform1i(texWLoc, texWidth);
	GLint texHLoc = glGetUniformLocation(shader5, "texHeight");
	glUniform1i(texHLoc, texHeight);
	GLint heightLoc = glGetUniformLocation(shader5, "height");
	glUniform1f(heightLoc, myGLUI->waveHeight);

	GLint sizeXLoc = glGetUniformLocation(shader5, "sizeX");
	glUniform1f(sizeXLoc, myGLUI->rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader5, "sizeY");
	glUniform1f(sizeYLoc, myGLUI->rect.size.y);

	//質点のマテリアル
	static float diffuse[] = { 0.6, 0.6, 0.9, 1.0 };
	static float ambient[] = { 0.1, 0.1, 0.2, 1.0 };
	//static float specular[]= { 0.9, 0.9, 0.9, 1.0};
	static float specular[] = { 0.5, 0.5, 0.5, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0);

	if (myGLUI->flagWireframe == 0)
	{
		drawGridPlate((float)NX, (float)NY, NX, NY);
	}
	else
	{
		drawGridLines((float)NX, (float)NY, NX, NY);
	}
	glUseProgram(0);
}

void renewVelX()
{
	//framebuffer object1を有効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[0]);
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
	glUniform1f(deltaTLoc, myGLUI->deltaT);
	GLint DXLoc = glGetUniformLocation(shader1, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader1, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader1, "Re");
	glUniform1f(ReLoc, myGLUI->Re);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

						  // シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, velX);//データをCPU側へ

	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void renewVelY()
{
	//framebuffer object1を有効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[1]);
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
	glUniform1f(deltaTLoc, myGLUI->deltaT);
	GLint DXLoc = glGetUniformLocation(shader2, "DX");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader2, "DY");
	glUniform1f(DYLoc, DY);
	GLint ReLoc = glGetUniformLocation(shader2, "Re");
	glUniform1f(ReLoc, myGLUI->Re);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

						  // シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, velY);//データをCPU側へ

	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void renewWaveVel()
{
	//framebuffer object2を有効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[2]);
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
	glUniform1f(deltaTLoc, myGLUI->deltaT);
	GLint timeLoc = glGetUniformLocation(shader3, "time");
	glUniform1f(timeLoc, myGLUI->assist3d.elapseTime3);
	GLint DXLoc = glGetUniformLocation(shader3, "dx");
	glUniform1f(DXLoc, DX);
	GLint DYLoc = glGetUniformLocation(shader3, "dy");
	glUniform1f(DYLoc, DY);
	GLint ampLoc = glGetUniformLocation(shader3, "amp");
	glUniform1f(ampLoc, myGLUI->waveAmp);
	GLint freqLoc = glGetUniformLocation(shader3, "freq");
	glUniform1f(freqLoc, myGLUI->waveFreq);
	GLint speedLoc = glGetUniformLocation(shader3, "speed");
	glUniform1f(speedLoc, myGLUI->waveSpeed);
	GLint dragLoc = glGetUniformLocation(shader3, "drag");
	glUniform1f(dragLoc, myGLUI->waveDrag);

	GLint posObsLoc = glGetUniformLocation(shader3, "posObs");
	float pos[] = { rigid[0].vPos.x, rigid[0].vPos.y, rigid[0].vPos.z };
	glUniform3fv(posObsLoc, 1, pos);
	GLint sizeObsLoc = glGetUniformLocation(shader3, "sizeObs");
	float size[] = { rigid[0].vSize.x, rigid[0].vSize.y, rigid[0].vSize.z };
	glUniform3fv(sizeObsLoc, 1, size);
	GLint velObsLoc = glGetUniformLocation(shader3, "velObs");
	float vel[] = { rigid[0].vVel.x, rigid[0].vVel.y, rigid[0].vVel.z };
	glUniform3fv(velObsLoc, 1, vel);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

						  // シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, wave);//データをCPU側へ

	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void renewWavePos()
{
	//framebuffer object2を有効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[2]);
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
	GLint texWLoc = glGetUniformLocation(shader4, "meshX");
	glUniform1i(texWLoc, myGLUI->rect.meshX);
	GLint texHLoc = glGetUniformLocation(shader4, "meshY");
	glUniform1i(texHLoc, myGLUI->rect.meshY);
	GLint maxOmgLoc = glGetUniformLocation(shader4, "maxOmg");
	glUniform1f(maxOmgLoc, myGLUI->maxOmg);
	GLint maxPrsLoc = glGetUniformLocation(shader4, "maxPrs");
	glUniform1f(maxPrsLoc, myGLUI->maxPrs);
	GLint adjustHLoc = glGetUniformLocation(shader4, "adjustH");
	glUniform1f(adjustHLoc, myGLUI->adjustH);
	GLint adjustCLoc = glGetUniformLocation(shader4, "adjustC");
	glUniform1f(adjustCLoc, myGLUI->adjustC);
	GLint deltaTLoc = glGetUniformLocation(shader4, "deltaT");
	glUniform1f(deltaTLoc, myGLUI->deltaT);

	drawNumberingPoints();//数値解析用のテクスチャを貼り付けるオブジェクト

						  // シェーダプログラムの解除 
	glUseProgram(0);
	glReadPixels(0, 0, texWidth, texHeight, format, type, wave);//データをCPU側へ
																//コースティクスのテクスチャ作成
	int i, j, k, k3;
	for (j = 0; j < texHeight; j++)
		for (i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			k3 = 3 * k;
			caus[k3] = caus[k3 + 1] = caus[k3 + 2] = 255.0 * wave[k * 4 + 3];
		}
	setTextureCaus();

	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setTextureCaus()
{
	glBindTexture(target, texID[5]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, caus);//texCaus);
																								   //テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(target, 0);
}


void renewParticle()
{
	//framebuffer objectを有効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[3]);
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
	glUniform1f(sizeXLoc, myGLUI->rect.size.x);
	GLint sizeYLoc = glGetUniformLocation(shader6, "sizeY");
	glUniform1f(sizeYLoc, myGLUI->rect.size.y);
	GLint dtLoc = glGetUniformLocation(shader6, "dt");
	glUniform1f(dtLoc, myGLUI->deltaT);

	drawNumberingPoints();
	// シェーダプログラムの解除 
	glUseProgram(0);
	//更新データ（速度と位置）をテクスチャメモリにコピー
	glReadPixels(0, 0, texWidth, texHeight, format, type, particle);//データをCPU側へ
	glBindTexture(target, 0);
	//framebuffer objectの無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

static void drawNumberingPoints()
{
	//ビューポートのサイズ設定
	glViewport(0, 0, texWidth, texHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//正投影行列の設定
	gluOrtho2D(0.0, texWidth, 0.0, texHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//物理量をテクスチャとして貼り付けるための仮想オブジェクト

	glBegin(GL_POINTS);
	for (int j = 0; j < texHeight; j++)
	{
		for (int i = 0; i < texWidth; i++)
		{
			glVertex2f((float)i, (float)j);//点座標が2次元格子頂点番号
		}
	}
	glEnd();
}

static void calcObsPos()
{
	if (flagObsStop) 
	{ 
		rigid[0].vVel = Vector3D();  
		return; 
	}

	Vector3D left;//ダクトの左端および下端からの距離
				  //	float direction = 1.0;//障害物直線移動方向

	MyGLUI::Rect &_rect = myGLUI->rect;

	float R = _rect.size.x / 2.0 - _rect.obs_left;//回転半径

	if (myGLUI->flagRotation)
	{//初期位置が左側のとき右回り，右側のとき左回り
		rigid[0].vVel.x = myGLUI->obsSpeed * rigid[0].vPos.y / R;
		rigid[0].vVel.y = - myGLUI->obsSpeed * rigid[0].vPos.x / R;
	}
	else//直進
	{
		if (rigid[0].vPos.x >= _rect.size.x / 2.0 - _rect.obs_left) direction = -1.0;
		if (rigid[0].vPos.x <= - _rect.size.x / 2.0 + _rect.obs_left) direction = 1.0;
		rigid[0].vVel.x = direction * myGLUI->obsSpeed;
	}
	rigid[0].vPos += rigid[0].vVel * myGLUI->deltaT;
	//水面下の障害物の像
	rigid[1].vPos = rigid[0].vPos;
	rigid[0].vPos.z = myGLUI->waveHeight + rigid[1].vSize.z / 2.0;
	rigid[1].vPos.z = myGLUI->waveHeight - rigid[1].vSize.z / 2.0;
	left.x = rigid[0].vPos.x + _rect.size.x / 2.0;
	left.y = rigid[0].vPos.y + _rect.size.y / 2.0;

	//格子点のタイプ
	int i, j;
	float eps = DX / 2.0;
	float x, y, x1, x2, y1, y2, x0, y0, r, r0;
	x0 = left.x;//rect.obs_left;//円柱の中心
	y0 = left.y;//rect.size.y/2.0;
	r0 = _rect.obs_radius;

	for (i = 1; i < NX; i++)
	{
		for (j = 1; j < NY; j++)
		{
			g_type[i][j] = INSIDE;//内点
			x = (float)i * DX;
			y = (float)j * DY;
			if (y < y0 - r0 - DY) continue;
			if (y > y0 + r0 + DY) continue;
			if (x < x0 - r0 - DX) continue;
			if (x > x0 + r0 + DX) continue;

			r = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
			if (r > r0 + eps) continue;
			if (r <= r0) g_type[i][j] = OBSTACLE;//ここでは境界を含む

			if (fabs(y - y0 - r0) < eps) g_type[i][j] = OBS_TOP;
			else if (fabs(y - y0 + r0) < eps) g_type[i][j] = OBS_BOTTOM;
			else if (fabs(x - x0 + r0) < eps) g_type[i][j] = OBS_LEFT;
			else if (fabs(x - x0 - r0) < eps) g_type[i][j] = OBS_RIGHT;
			else
			{
				x1 = x0 - sqrt(r0 * r0 - (y - y0) * (y - y0));//左側交点
				x2 = x0 + sqrt(r0 * r0 - (y - y0) * (y - y0));//右側交点
				y1 = y0 - sqrt(r0 * r0 - (x - x0) * (x - x0));//下側交点
				y2 = y0 + sqrt(r0 * r0 - (x - x0) * (x - x0));//上側交点
				if (i == int(x1 / DX + 0.5) && y > y0) g_type[i][j] = OBS_UL;//左上
				if (j == int(y2 / DY + 0.5) && x < x0) g_type[i][j] = OBS_UL;//左上
				if (i == int(x1 / DX + 0.5) && y < y0) g_type[i][j] = OBS_LL;//左下
				if (j == int(y1 / DY + 0.5) && x < x0) g_type[i][j] = OBS_LL;//左下
				if (i == int(x2 / DX + 0.5) && y > y0) g_type[i][j] = OBS_UR;//右上
				if (j == int(y2 / DY + 0.5) && x > x0) g_type[i][j] = OBS_UR;//右上
				if (i == int(x2 / DX + 0.5) && y < y0) g_type[i][j] = OBS_LR;//右下
				if (j == int(y1 / DY + 0.5) && x > x0) g_type[i][j] = OBS_LR;//右下
			}
		}
	}
}

static void makeTextureOfCubeMapping()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[4]);

	// 透視変換行列の設定 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.1, 100.0);//視野角を90°
										  // モデルビュー変換行列の設定 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Assist3D::View &view = myGLUI->assist3d.view;

	//視点から見たシーンを屈折マッピングに利用 
	for (int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, c_target[i].name, texID[4], 0);
		glViewport(0, 0, CUBE_WIDTH, CUBE_HEIGHT);
		//カラーバッファ,デプスバッファのクリア
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		//視点から見えるものをレンダリング 
		gluLookAt(view.vPos.x, view.vPos.y, view.vPos.z, view.vPos.x + c_target[i].cx, view.vPos.y + c_target[i].cy, view.vPos.z + c_target[i].cz,
			c_target[i].ux, c_target[i].uy, c_target[i].uz);

		glLightfv(GL_LIGHT0, GL_POSITION, myGLUI->assist3d.lightPos);

		if (view.vPos.z < myGLUI->waveHeight)//視点が水面より下
		{
			drawUpperObject();//水面より上のオブジェクト
		}

		//投影マッピング
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(target, texID[5]);//caus[]
										// シェーダプログラムの適用 

		glUseProgram(shader8);
		GLint texLoc = glGetUniformLocation(shader8, "samplerCaus");
		glUniform1i(texLoc, 0);

		drawLowerObject();
		//fish1.motion(elapseTime2);
		//fish2.motion(elapseTime2);
		if (myGLUI->flagShadowShow)
		{
			drawShadow();
		}

		// シェーダプログラムの適用を解除	
		glUseProgram(0);

		glPopMatrix();
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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
	glUniform1f(heightLoc, myGLUI->waveHeight);

	drawParticles();
	glUseProgram(0);

	glBindTexture(target, 0);
}
void drawParticles()
{
	glDisable(GL_LIGHTING);
	glPointSize(myGLUI->sizeParticle);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_POINTS);
	for (int j = 0; j < texHeight; j++)
		for (int i = 0; i < texWidth; i++) glVertex3f((float)i, (float)j, 0.0);
	glEnd();
	glPointSize(1);
	glEnable(GL_LIGHTING);
}

void drawLowerObject()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);//背面は描画しない
	for (int i = 1; i <= 6; i++)
	{
		//テクスチャに対するモデリング変換
		setTextureMatrix();
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
	rigid[11].vPos = Vector3D(myGLUI->assist3d.lightPos[0], myGLUI->assist3d.lightPos[1], myGLUI->assist3d.lightPos[2] + 1.0);
	for (int i = 7; i < 12; i++) rigid[i].draw();
	glDisable(GL_CULL_FACE);
}

void drawShadow()
{
	//計算は平行光線に対する影
	//ｵﾌﾞｼﾞｪｸﾄごとに光源の方向を変え、疑似的に点光源に対する影を作る
	float mat[16]; //影行列の要素
	float pos[3];

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_FALSE);
	//移動物体の影の描画
	for (int i = 0; i <= 1; i++)
	{
		pos[0] = rigid[i].vPos.x;
		pos[1] = rigid[i].vPos.y;
		pos[2] = rigid[i].vPos.z;

		rigid[i].flagShadow = true;
		CalcShadowMat(pos, mat);
		glPushMatrix();
		glMultMatrixf(mat);//マトリクスの乗算
		rigid[i].draw();//ｵﾌﾞｼﾞｪｸﾄの描画
		glPopMatrix();
		rigid[i].flagShadow = false;
	}
	//魚1
	//pos[0] = fish1.pos[0];
	//pos[1] = fish1.pos[1];
	//pos[2] = fish1.pos[2];
	//fish1.flagShadow = true;
	CalcShadowMat(pos, mat);
	glPushMatrix();
	glMultMatrixf(mat);
	//fish1.draw();
	glPopMatrix();
	//fish1.flagShadow = false;
	//魚2
	//pos[0] = fish2.pos[0];
	//pos[1] = fish2.pos[1];
	//pos[2] = fish2.pos[2];
	//fish2.flagShadow = true;
	CalcShadowMat(pos, mat);
	glPushMatrix();
	glMultMatrixf(mat);
	//fish2.draw();
	glPopMatrix();
	//fish2.flagShadow = false;

	//ボール
	pos[0] = rigid[6].vPos.x;
	pos[1] = rigid[6].vPos.y;
	pos[2] = rigid[6].vPos.z;
	rigid[6].flagShadow = true;
	CalcShadowMat(pos, mat);
	glPushMatrix();
	glMultMatrixf(mat);
	rigid[6].draw();
	glPopMatrix();
	rigid[6].flagShadow = false;

	//影の描画終了
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void drawGridPlate(float sizeX, float sizeY, int nSliceX, int nSliceY)
{//xy平面，中心は原点
	int i, j;
	//double ;
	float p[2][3];
	float pitchX = sizeX / (float)nSliceX;
	float pitchY = sizeY / (float)nSliceY;

	for (j = 0; j < nSliceY; j++)
	{
		//頂点z座標
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_TRIANGLE_STRIP);
		for (i = 0; i <= nSliceX; i++)
		{

			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[0][1] = (float)(j + 1) * pitchY - sizeY / 2.0;//y座標
			p[1][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[1][1] = (float)j * pitchY - sizeY / 2.0;//y座標

			glNormal3f(0.0, 0.0, 1.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[1]);//頂点座標			
		}
		glEnd();
	}
}

void setTextureMatrix()
{
	//テクスチャ変換行列を設定する 
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	//正規化座標の [-0.5,0.5] の範囲をテクスチャ座標の範囲 [0,1] に変換 
	glTranslatef(0.5, 0.5, 0.0);
	float fovC = 2.0 * RAD_TO_DEG * atan2(myGLUI->rect.size.x, (myGLUI->assist3d.lightPos[2] - myGLUI->waveHeight));
	gluPerspective(fovC, 1.0, 0.1, 20.0);
	//投影中心=光源位置
	gluLookAt(myGLUI->assist3d.lightPos[0], myGLUI->assist3d.lightPos[1], myGLUI->assist3d.lightPos[2], 0.0, 0.0, myGLUI->waveHeight, 0.0, 1.0, 0.0);
}

void CalcShadowMat(float* pos, float* mat)
{
	float ex, ey, ez;//光源の方向
	float a, b, c, d;//床の面のパラメータ
	float s; //object中心から光源までの距離
	float x, y, z;
	x = myGLUI->assist3d.lightPos[0] - pos[0];
	y = myGLUI->assist3d.lightPos[1] - pos[1];
	z = myGLUI->assist3d.lightPos[2] - pos[2];

	//光源の方向ベクトル
	s = sqrt(x * x + y * y + z * z);
	ex = x / s;
	ey = y / s;
	ez = z / s;

	//フロアの方向ベクトル(ｙ方向）
	a = 0.0;
	b = 0.0;
	c = 1.0;
	d = -0.05;
	//shadow matrix
	mat[0] = b * ey + c * ez;
	mat[1] = -a * ey;
	mat[2] = -a * ez;
	mat[3] = 0.0;
	mat[4] = -b * ex;
	mat[5] = a * ex + c * ez;
	mat[6] = -b * ez;
	mat[7] = 0.0;
	mat[8] = -c * ex;
	mat[9] = -c * ey;
	mat[10] = a * ex + b * ey;
	mat[11] = 0.0;
	mat[12] = -d * ex;
	mat[13] = -d * ey;
	mat[14] = -d * ez;
	mat[15] = a * ex + b * ey + c * ez;
}