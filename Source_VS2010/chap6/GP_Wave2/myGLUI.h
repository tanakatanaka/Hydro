//glui関連のパラメータ，関数など

#define MAX_SOURCE  50//全波源数

enum WaveMode { SINGLE, CONTINUOUS};
WaveMode mode = SINGLE;
enum Boundary {B_NON, B_FREE, B_FIXED};
Boundary boundary = B_FREE;

//解析領域矩形構造体
struct Rect
{
	Vector2D size; //矩形領域のサイズ(固定）
	int meshX;    //分割数(X)
	int meshY;    //分割数(Y)
	float obs_thick;//障害物の厚さ
	float obs_width;//障害物の幅

};
Rect rect = {
	Vector2D(20.0, 10.0),//size
	200,//meshX
	100, //meshY
	1.0, 2.0//obs_thick, width
};

//プロジェクトのメインファイルに実装されている関数
void initData();

int main_window;//GLUTで作るWindowのID
int flagWireframe = 0;
int flagParamShow = 1;
int flagCoordShow = 0;
int nSource0 = 1;//波源個数
int sourceX0 = -50.0;//波源のx座標(格子数）
int interval0 = 1.0;//波源間隔(格子数）
float amp0 = 10.0;//振幅
float freq0 = 1.0;//振動数[Hz]
float waveSpeed = 2.0;//伝搬速度
float drag0 = 0.1;    //減衰係数
float deltaT = 0.01;
int pattern = 1;//障害物配置のパターン

//Pointers
GLUI *glui, *glui2;

GLUI_Panel *paraPanel;
GLUI_EditText *deltaT_edit;
GLUI_EditText *pattern_edit;
GLUI_EditText *sizeY_edit;
GLUI_EditText *meshY_edit;
GLUI_EditText *thick_edit;
GLUI_EditText *width_edit;

GLUI_Panel  *calcPanel;
GLUI_Button *startBtn;
GLUI_Button *freezeBtn;
GLUI_Button *stepBtn;
GLUI_Button *resetBtn;
GLUI_Panel *dispPanel;

GLUI_Panel *wavePanel;
GLUI_EditText *nSource_edit;
GLUI_EditText *sourceX_edit;
GLUI_EditText *interval_edit;
GLUI_EditText *amp_edit;
GLUI_EditText *freq_edit;
GLUI_EditText *drag_edit;
GLUI_EditText *waveSpeed_edit;

GLUI_RadioGroup *radioMode;
GLUI_RadioGroup *radioBoundary;

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

//USER ID
#define START_ID  100
#define FREEZE_ID 110
#define STEP_ID   120
#define RESET_ID  130
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

void control_cb(int control)
{
	double v1 = 10.0;//光源
	double v2 = 1.0; //カメラ操作

	//動画操作
	if(control == START_ID) {
		flagStart = true;
		flagFreeze = false;
		initData();
	}
	else if(control == FREEZE_ID) {
		if(flagFreeze == false) flagFreeze = true;
		else {flagFreeze = false; flagStep = false;}
	}
	else if(control == STEP_ID) {
		flagStep = true;
		flagFreeze = false;
	}
	else if(control == RESET_ID) {
		flagStart = false;
		flagStep = false;
		flagFreeze = false;
		initData();
	}
	//光源操作
	else if(control == LIGHTX_P_ID) { lightPos[0] += v1;}
	else if(control == LIGHTX_M_ID) { lightPos[0] -= v1;}
	else if(control == LIGHTY_P_ID) { lightPos[1] += v1;}
	else if(control == LIGHTY_M_ID) { lightPos[1] -= v1;}
	else if(control == LIGHTZ_P_ID) { lightPos[2] += v1;}
	else if(control == LIGHTZ_M_ID) { lightPos[2] -= v1;}
	else if(control == LIGHT_RESET_ID) 
	{
		for(int i = 0; i < 3; i++) lightPos[i] = lightPos0[i];
	}
	
	else if ( control == SHOW_ID )
	{
		glui2->show();
	}
	else if ( control == HIDE_ID )
	{
		glui2->hide();
	}

	//カメラ操作
	else if(control == DOLLY_P_ID || control == DOLLY_M_ID)//dolly
	{
		if(control == DOLLY_P_ID)  view.dist -= v2; //近づく
		else  view.dist += v2; //遠ざかる
		setCamera();
	}
	else if( control == PAN_P_ID || control == PAN_M_ID)//pan
	{
		if(control == PAN_P_ID) view.phi += v2;
		else view.phi -= v2;
		view.vCnt.x = view.vPos.x - view.dist * cos(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
		view.vCnt.y = view.vPos.y - view.dist * sin(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
	}
	else if(control == TILT_P_ID || control == TILT_M_ID)//tilt
	{
	  if(control == TILT_P_ID)
		  view.theta += v2; 
	  else
		  view.theta -= v2;
	  
	  view.vCnt.x = view.vPos.x - view.dist * (float)(cos(DEG_TO_RAD * view.theta) * cos(DEG_TO_RAD * view.phi));
	  view.vCnt.y = view.vPos.y - view.dist * (float)(cos(DEG_TO_RAD * view.theta) * sin(DEG_TO_RAD * view.phi));
	  view.vCnt.z = view.vPos.z - view.dist * (float)sin(DEG_TO_RAD * view.theta);
	}

	else if(control == ZOOM_P_ID || control == ZOOM_M_ID)//zoom
	{
		if(control == ZOOM_P_ID) view.fovY -= v2;//zoom in
		else view.fovY += v2;//zoom out	
		setCamera();
	}
	else if(control == TUMBLE_P_ID || control == TUMBLE_M_ID)//tumble
	{
		if( control == TUMBLE_P_ID) view.phi += v2;
		else                        view.phi -= v2;
		setCamera();
	}
	else if(control == CRANE_P_ID || control == CRANE_M_ID)//crane
	{
		if( control == CRANE_P_ID) view.theta += v2;
		else                        view.theta -= v2;
		setCamera();
	}
	else if(control == CAMERA_RESET_ID) view = view0;
}

void setupGLUI()
{
	//*******glui***********
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	paraPanel = new GLUI_Panel(glui, "Parameters" );
	sizeY_edit =  new GLUI_EditText(paraPanel,"sizeY", &rect.size.y);
	meshY_edit =  new GLUI_EditText(paraPanel,"meshY", &rect.meshY);
	thick_edit = new GLUI_EditText(paraPanel,"obs_thick", &rect.obs_thick);
	width_edit = new GLUI_EditText(paraPanel,"obs_width", &rect.obs_width);

	deltaT_edit = new GLUI_EditText(paraPanel,"deltaT", &deltaT);
	pattern_edit = new GLUI_EditText(paraPanel,"pattern", &pattern);
	pattern_edit->set_int_limits( 0, 1 );

	calcPanel = new GLUI_Panel(glui, "Calculation" );
	startBtn = new GLUI_Button(calcPanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(calcPanel, "Freeze", FREEZE_ID, control_cb);
	stepBtn = new GLUI_Button(calcPanel, "Step", STEP_ID, control_cb);
	resetBtn = new GLUI_Button(calcPanel, "Reset", RESET_ID, control_cb);

	//光源
	GLUI_Rollout *light = new GLUI_Rollout(glui, "Light", false );
	lightXP_btn = new GLUI_Button(light, "LightX+", LIGHTX_P_ID, control_cb);
	lightXM_btn = new GLUI_Button(light, "LightX-", LIGHTX_M_ID, control_cb);
	lightYP_btn = new GLUI_Button(light, "LightY+", LIGHTY_P_ID, control_cb);
	lightYM_btn = new GLUI_Button(light, "LightY-", LIGHTY_M_ID, control_cb);
	lightZP_btn = new GLUI_Button(light, "LightZ+", LIGHTZ_P_ID, control_cb);
	lightZM_btn = new GLUI_Button(light, "LightZ-", LIGHTZ_M_ID, control_cb);
	resetLight_btn = new GLUI_Button(light, "ResetLight", LIGHT_RESET_ID, control_cb);

	new GLUI_StaticText( glui, "" );//空白
	//****** 終了 *****
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

	glui->add_column(true);
	
	dispPanel = new GLUI_Panel(glui, "Display" );
	new GLUI_Checkbox(dispPanel, "Wireframe", &flagWireframe);
 	new GLUI_Checkbox(dispPanel, "Parameter", &flagParamShow);
 	new GLUI_Checkbox(dispPanel, "CoordShow", &flagCoordShow);

	wavePanel = new GLUI_Panel(glui, "Wave" );
	nSource_edit = new GLUI_EditText(wavePanel, "nSource", &nSource0);
	nSource_edit->set_int_limits(1, MAX_SOURCE);
	sourceX_edit  = new GLUI_EditText(wavePanel, "sourceX", &sourceX0);
	interval_edit  = new GLUI_EditText(wavePanel, "interval", &interval0);
	amp_edit = new GLUI_EditText(wavePanel, "amp", &amp0);
	freq_edit = new GLUI_EditText(wavePanel, "freq", &freq0);
	waveSpeed_edit = new GLUI_EditText(wavePanel, "waveSpeed", &waveSpeed);
	drag_edit = new GLUI_EditText(wavePanel, "drag", &drag0);

	cameraOpe_panel = new GLUI_Panel(glui, "CameraOperation");
	hide_btn = new GLUI_Button(cameraOpe_panel, "Hide", HIDE_ID, control_cb);
	show_btn = new GLUI_Button(cameraOpe_panel, "Show", SHOW_ID, control_cb);
	
	glui->set_main_gfx_window( main_window );

	//********glui2****************
	glui2 = GLUI_Master.create_glui("GLUI-CAMERA", 0, win_x0 - 50, win_y0 + win_height + 30);
	//カメラ操作
	dollyP_btn = new GLUI_Button(glui2, "Dolly+", DOLLY_P_ID, control_cb);
	dollyM_btn = new GLUI_Button(glui2, "Dolly-", DOLLY_M_ID, control_cb);
	glui2->add_column(false);
	panP_btn = new GLUI_Button(glui2, "Pan+", PAN_P_ID, control_cb);
	panM_btn = new GLUI_Button(glui2, "Pan-", PAN_M_ID, control_cb);
	glui2->add_column(false);
	tiltP_btn = new GLUI_Button(glui2, "Tilt+", TILT_P_ID, control_cb);
	tiltM_btn = new GLUI_Button(glui2, "Tilt-", TILT_M_ID, control_cb);
	glui2->add_column(false);
	tumbleP_btn = new GLUI_Button(glui2, "Tumble+", TUMBLE_P_ID, control_cb);
	tumbleM_btn = new GLUI_Button(glui2, "Tumble-", TUMBLE_M_ID, control_cb);
	glui2->add_column(false);
	craneP_btn = new GLUI_Button(glui2, "Crane+", CRANE_P_ID, control_cb);
	craneM_btn = new GLUI_Button(glui2, "Crane-", CRANE_M_ID, control_cb);
	glui2->add_column(false);
	zoomP_btn = new GLUI_Button(glui2, "Zoom+", ZOOM_P_ID, control_cb);
	zoomM_btn = new GLUI_Button(glui2, "Zoom-", ZOOM_M_ID, control_cb);
	glui2->add_column(false);
	resetCamera_btn = new GLUI_Button(glui2, "ResetCamera", CAMERA_RESET_ID, control_cb);

	glui2->set_main_gfx_window( main_window );

}
