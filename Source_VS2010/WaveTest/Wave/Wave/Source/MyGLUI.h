#pragma once
#include <GL/glew.h>
#include <GL/glui.h>
#include <memory>
#include "Assist3D.h"

#define NUM_MAX_X  210//Ååª
#define NUM_MAX_Y  210

//USER ID
#define START_ID  110
#define FREEZE_ID 120
#define STEP_ID 130
#define RESET_ID 140
#define PARAMETER_ID 150
#define OBS_STOP_ID 160
#define INIT_LENGTH_ID 170
//õ¹ÆJ
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

//ðÍpf[^ÌTCY
int texWidth;
int texHeight;//Siq

//ðÍpf[^zñ
float *velX;//¬xVelXÆ³Í
float *velY;//¬xVelYÆQx
float *wave;//gÌ³Ì¬xCÊu
GLubyte *caus;
float *particle;//±qÊu
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
Type g_type[NUM_MAX_X][NUM_MAX_Y]; //iq_Ì^Cv

int NX, NY, NX1, NY1, NX2, NY2;
float DX, DY;
float direction = 1.0;//áQ¨¼üÚ®ûü


class MyGLUI
{
	public:

	//ðÍÌæé`\¢Ì
		struct Rect
		{
			Vector2D size; //é`ÌæÌTCY(Åèj
			float obs_left;//¶[©çáQ¨SÜÅÌ£
			float obs_radius; //áQ¨Ì¼a
			float obs_posY;//áQ¨ÌûüÊu
			float obs_length;//áQ¨ÌSÌÌ·³iûüTCYj
			int meshX;     //ª(X)
			int meshY;     //ª(Y)
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

		int main_window;//GLUTÅìéWindowÌID
		int numParticle = 1000;
		Assist3D assist3d;
		float deltaT = 0.01;
		int flagWireframe = 0;
		int flagShowParticle = 0;
		float Re = 500.0;//CmY
		int flagShadowShow = 0;
		int flagCoordShow = 0;
		float transparency = 0.8;//§¾x
		float nRatio = 1.0;//äüÜ¦
		float waveHeight = 8.0;//ÊÌ³
		float waveAmp = 10.0;//gÌU
		float waveSpeed = 2.0;//gÌ`À¬x
		float waveFreq = 1.0;
		float waveDrag = 0.1;
		float maxOmg = 5.0;
		int flagIdeal = 1;
		int flagRotation = 0;//ñ]^®
		int flagResetting = 0;
		float obsSpeed = 1.0;
		float maxPrs = 5.0;
		float adjustH = 1.0;//\¦ãÌQÌ³²®
		float adjustC = 2.0;//R[XeBNXÌ­³²®
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

		//õ¹
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
		//Jì
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