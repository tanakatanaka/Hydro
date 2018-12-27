#include"SubGLSL.h"
#include "../../myGlsl.h"
//#include"../../Assist3D.h"


void SubGLSL::InitGlsl(GLuint *program, std::string vertexFile, std::string fragmentFile)
{
	std::string vertexPass = "shader/" + vertexFile;

	if (fragmentFile != "")
	{
		std::string fragmentPass = "shader/" + fragmentFile;
		initGlsl(program, vertexPass.c_str(), fragmentPass.c_str());
	}
	else
	{
		initGlsl(program, vertexPass.c_str());
	}

	InitWindow();
}

void SubGLSL::InitWindow()
{
	//Assist3D::GetInstance().SetLight();

	/*
	//背景色
	glClearColor(0.2, 0.5, 0.9, 1.0);
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
	lightPos[2] = 20.0;
	lightPos0[2] = 20.0;

	//時間関係
	memset(&freq, 0x00, sizeof freq);
	memset(&lastTime, 0x00, sizeof lastTime);
	memset(&currentTime, 0x00, sizeof currentTime);
	QueryPerformanceCounter(&lastTime);
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
	*/
}

