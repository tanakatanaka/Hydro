#pragma once
#include <GL/glew.h>
#include <GL/glui.h>
#include <memory>
#include "Assist3D.h"

#define NUM_MAX_X  210//最大分割数
#define NUM_MAX_Y  210

//USER ID
#define START_ID  110
#define FREEZE_ID 120
#define STEP_ID 130
#define RESET_ID 140
#define PARAMETER_ID 150
#define OBS_STOP_ID 160
#define INIT_LENGTH_ID 170
//光源とカメラ
#define LIGHTX_P_ID 210
#define LIGHTX_M_ID 220
#define LIGHTY_P_ID 230
#define LIGHTY_M_ID 240
#define LIGHTZ_P_ID 250
#define LIGHTZ_M_ID 260
#define LIGHT_RESET_ID 270
#define HIDE_ID 280
#define SHOW_ID 290
#define DOLLY_P_ID 310
#define DOLLY_M_ID 320
#define PAN_P_ID 330
#define PAN_M_ID 340
#define TILT_P_ID 350
#define TILT_M_ID 360
#define TUMBLE_P_ID 370
#define TUMBLE_M_ID 380
#define CRANE_P_ID 390
#define CRANE_M_ID 400
#define ZOOM_P_ID 410
#define ZOOM_M_ID 420
#define CAMERA_RESET_ID 430

int flagObsStop = 0;
int flagResetting = 0;
int flagParamShow = 1;

void control_cb(int control);

//解析用データのサイズ
int texWidth;
int texHeight;//全格子数

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


enum Type
{
	INSIDE, LEFT, RIGHT, TOP, BOTTOM, OBS_LEFT, OBS_TOP, OBS_BOTTOM,
	OBS_RIGHT, OBSTACLE, OBS_UL, OBS_UR, OBS_LL, OBS_LR
};
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //格子点のタイプ

int NX, NY, NX1, NY1, NX2, NY2;
float DX, DY;
float direction = 1.0;//障害物直線移動方向


class MyGLUI
{
	public:

	//解析領域矩形構造体
		struct Rect
		{
			Vector2D size; //矩形領域のサイズ(固定）
			float obs_left;//左端から障害物中心までの距離
			float obs_radius; //障害物の半径
			float obs_posY;//障害物のｙ方向位置
			float obs_length;//障害物の全体の長さ（ｚ方向サイズ）
			int meshX;     //分割数(X)
			int meshY;     //分割数(Y)
		};

		Rect rect =
		{
			Vector2D(20.0, 20.0),//size
			3.0, 0.5, 10.0, 2.0,  //obs_left, radius, posY, length
			200,//meshX
			200 //meshY
		};

		void Initialize();
		void initObjectLength();
		void setGLUI();
		void GetAssist3d();
		void initData();

		int main_window;//GLUTで作るWindowのID
		int numParticle = 1000;
		Assist3D assist3d;
		float deltaT = 0.01;
		int flagWireframe = 0;
		int flagShowParticle = 0;
		float Re = 500.0;//レイノルズ数
		int flagShadowShow = 0;
		int flagCoordShow = 0;
		float transparency = 0.8;//透明度
		float nRatio = 1.0;//比屈折率
		float waveHeight = 8.0;//水面の高さ
		float waveAmp = 10.0;//波の振幅
		float waveSpeed = 2.0;//波の伝搬速度
		float waveFreq = 1.0;
		float waveDrag = 0.1;
		float maxOmg = 5.0;
		int flagIdeal = 1;
		int flagRotation = 0;//回転運動
		int flagResetting = 0;
		float obsSpeed = 1.0;
		float maxPrs = 5.0;
		float adjustH = 1.0;//表示上の渦の高さ調整
		float adjustC = 2.0;//コースティクスの強さ調整
		int sizeParticle = 3;

		GLUI *glui, *glui2;

	private:
		//Pointers
		GLUI_Panel *paraPanel;
		GLUI_EditText *sizeX_edit;
		GLUI_EditText *sizeY_edit;
		GLUI_EditText *left_edit;
		GLUI_EditText *radius_edit;
		GLUI_EditText *posY_edit;
		GLUI_EditText *length_edit;
		GLUI_EditText *meshX_edit;
		GLUI_EditText *meshY_edit;
		GLUI_EditText *obsSpeed_edit;
		GLUI_RadioGroup *radio;

		GLUI_Panel *calcPanel;
		GLUI_EditText *repeat_edit;
		GLUI_EditText *tolerance_edit;
		GLUI_EditText *deltaT_edit;
		GLUI_EditText *Re_edit;

		GLUI_Panel *dispPanel;
		GLUI_Button *startBtn;
		GLUI_Button *resetBtn;
		GLUI_Button *stepBtn;
		GLUI_Button *freezeBtn;
		GLUI_Button *obsStopBtn;
		GLUI_Button *paraBtn;

		GLUI_Panel *wavePanel;
		GLUI_Panel *causPanel;

		GLUI_Panel *partPanel;
		GLUI_EditText *sizeP_edit;
		GLUI_EditText *numP_edit;

		//光源
		GLUI_Button *lightXP_btn;
		GLUI_Button *lightXM_btn;
		GLUI_Button *lightYP_btn;
		GLUI_Button *lightYM_btn;
		GLUI_Button *lightZP_btn;
		GLUI_Button *lightZM_btn;
		GLUI_Button *resetLight_btn;
		GLUI_Panel *cameraOpe_panel;
		GLUI_Button *hide_btn;
		GLUI_Button *show_btn;
		//カメラ操作
		GLUI_Button *dollyP_btn;
		GLUI_Button *dollyM_btn;
		GLUI_Button *panP_btn;
		GLUI_Button *panM_btn;
		GLUI_Button *tiltP_btn;
		GLUI_Button *tiltM_btn;
		GLUI_Button *tumbleP_btn;
		GLUI_Button *tumbleM_btn;
		GLUI_Button *craneP_btn;
		GLUI_Button *craneM_btn;
		GLUI_Button *zoomP_btn;
		GLUI_Button *zoomM_btn;
		GLUI_Button *resetCamera_btn;
		void setupGLUI();
};