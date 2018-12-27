#pragma once
#include<windows.h>
#include "myMath6.h"


class Assist3D
{
public:

	//Windowのサイズ,位置
	int win_width = 600;
	int win_height = 500;
	int win_x0 = 500;
	int win_y0 = 100;

	//経過時間
	LARGE_INTEGER freq, lastTime, currentTime;
	double pTime;
	double elapseTime1, elapseTime2, elapseTime3;

	//マウス操作
	int xStart, yStart;
	bool flagMouse = false;
	bool flagSlide = false;
	bool flagStart = false; //アニメーションスタートフラグ
	bool flagFreeze = false;//一時停止フラグ
	bool flagFreeze0 = false;
	bool flagStep = false;

	//タイムステップの急激な変化に対応させるためのﾊﾟﾗﾒｰﾀ
	float timeStep, timeStep0, timeStep1;
	//正投影
	int flagOrtho = 0;
	float ortho_scale = 5.0;//正投影のときのサイズ比例係数 

	//光源
	float lightPos[4] = { 0.0, 0.0, 100.0, 1.0 };//光源位置
	float lightPos0[4] = { 0.0, 0.0, 100.0, 1.0 };//光源位置

	void SetLight()
	{
		float lightAmbient0[] = { 0.7, 0.7, 0.7, 1.0 }; //環境光
		float lightDiffuse0[] = { 0.9, 0.9, 0.9, 1.0 }; //拡散光
		float lightSpecular0[] = { 0.9, 0.9, 0.9, 1.0 };//鏡面光
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}

	//カメラと視体積
	struct View
	{
		//カメラ
		Vector3D vPos;//位置（視点）
		Vector3D vCnt;//注視点
		float dist;  //注視点から視点までの距離(R)
		float theta; //仰角（水平面との偏角）
		float phi;   //方位角
					 //視体積
		float fovY;  //視野角
		float nearZ; //前方クリップ面(近平面)
		float farZ;  //後方クリップ面(遠平面)
	};
	View view =
	{
		Vector3D(),             //pos(視点,仮設定）
		Vector3D(0.0, 0.0, 7.0),//cnt(注視点） 
		100.0, 15.0, 0.0,       //dist, theta, phi
								//40.0, 0.1, 20.0       //fovY,nearZ,farZ
								30.0, 1.0, 1000.0       //fovY,nearZ,farZ
	};
	View view0 = view;



	void resize(int w, int h)
	{
		//ビューポート変換
		glViewport(0, 0, w, h);
		//プロジェクション行列の指定
		glMatrixMode(GL_PROJECTION);
		//プロジェクション行列の初期化
		glLoadIdentity();
		//透視投影行列の設定（投影変換）
		float aspect = (double)w / (double)h;
		if (flagOrtho) glOrtho(-ortho_scale * aspect, ortho_scale*aspect, -ortho_scale, ortho_scale, view.nearZ, view.farZ);
		else gluPerspective(view.fovY, aspect, view.nearZ, view.farZ);
		glMatrixMode(GL_MODELVIEW);
		//モデルビュー行列の初期化
		glLoadIdentity();
		//表示ウィンドウのサイズ
		win_width = w;
		win_height = h;
	}





	
	
	
	
	//インスタンスを取得するためのstaticメンバ関数
	static Assist3D& GetInstance()
	{
		if (!Instance)
		{
			Instance = new Assist3D;
		}
		return *Instance;
	}

	static void Destroy()
	{
		delete Instance;
		Instance = nullptr;
	}
	

private:
	Assist3D() {}
	//コピー禁止しておく
	Assist3D(const Assist3D& other);
	Assist3D& operator = (const Assist3D& other);
	Assist3D(Assist3D&&) = delete;

	static Assist3D* Instance;
};

//グローバルに宣言
Assist3D* Assist3D::Instance = nullptr;