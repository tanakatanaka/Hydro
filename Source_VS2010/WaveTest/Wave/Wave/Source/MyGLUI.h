#pragma once
#include <GL/glew.h>
#include <GL/glui.h>
#include <memory>
#include "Assist3D.h"

#define NUM_MAX_X  210//�ő啪����
#define NUM_MAX_Y  210

//USER ID
#define START_ID  110
#define FREEZE_ID 120
#define STEP_ID 130
#define RESET_ID 140
#define PARAMETER_ID 150
#define OBS_STOP_ID 160
#define INIT_LENGTH_ID 170
//�����ƃJ����
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

Assist3D assist3d;
void control_cb(int control);
GLUI *glui, *glui2;
void initData();
void initData();


int flagObsStop = 0;
int flagResetting = 0;
int flagParamShow = 1;

class MyGLUI
{
	public:

	//��͗̈��`�\����
		struct Rect
		{
			Vector2D size; //��`�̈�̃T�C�Y(�Œ�j
			float obs_left;//���[�����Q�����S�܂ł̋���
			float obs_radius; //��Q���̔��a
			float obs_posY;//��Q���̂������ʒu
			float obs_length;//��Q���̑S�̂̒����i�������T�C�Y�j
			int meshX;     //������(X)
			int meshY;     //������(Y)
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

		int main_window;//GLUT�ō��Window��ID

	private:

		int flagWireframe = 0;
		int flagGrid = 0;
		int flagIdeal = 1;
		int flagShowParticle = 0;
		int flagCoordShow = 0;
		int flagRotation = 0;//��]�^��
		int flagResetting = 0;
		int flagShadowShow = 0;

		
		float deltaT = 0.01;
		float Re = 500.0;//���C�m���Y��
		float obsSpeed = 1.0;
		float maxPrs = 5.0;
		float maxOmg = 5.0;
		float adjustH = 1.0;//�\����̉Q�̍�������
		float adjustC = 2.0;//�R�[�X�e�B�N�X�̋�������
		float waveSpeed = 2.0;//�g�̓`�����x
		float waveFreq = 1.0;
		float waveAmp = 10.0;//�g�̐U��
		float waveDrag = 0.1;
		float waveHeight = 8.0;//���ʂ̍���
		int sizeParticle = 3;
		int numParticle = 1000;
		float transparency = 0.8;//�����x
		float nRatio = 1.0;//����ܗ�

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

		//����
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
		//�J��������
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