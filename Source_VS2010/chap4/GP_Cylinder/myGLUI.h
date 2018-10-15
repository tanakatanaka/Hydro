/*
//Windowのサイズ,位置
int win_width  = 800;
int win_height = 500;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 300;
int win_y0 = 100;
*/
#define NUM_MAX  110//最大分割数（最大NUM_MAX*NUM_MAXの格子数）

//解析領域構造体
struct Region
{
	float Radius;//外側領域の半径
	int nMesh;//動径方向分割数
};
Region region = {
	30.0,//Radius(外側半径）
	100//nMesh(動径方向分割数)
};

void initData();
void setGLUI();

int flagWireframe = 0;
int flagStream = 1;
int flagOmega = 0.0;
int flagVelocity = 0;
int flagGrid = 0;
int flagParamShow = 1;
int flagShading;

int main_window;//GLUTで作るWindowのID
//int maxRepeat = 500;//最大繰り返し回数(Poisson方程式）
//float tolerance = 0.00001;//許容誤差
float deltaT = 0.002;
float Re = 500.0;//レイノルズ数
//int minPsi = -20;
//float minOmega = -15.0;
float maxOmega =  2.0;
float maxPsi = 10;
float maxVel = 5.0;
float adjustH = 0.0;//表示上の高さ調整
int dispSpeed = 10.0;//表示スピード（間引き表示）
int color = 0;
int kind = 0;//表示項目（Psi, Omg, Vel)

//Pointers
GLUI *glui, *glui2;
GLUI_Panel *regionPanel;
GLUI_EditText *nMesh_edit;
GLUI_EditText *Radius_edit;
GLUI_RadioGroup *radio;

GLUI_Panel *calcPanel;
GLUI_EditText *repeat_edit;
GLUI_EditText *tolerance_edit;
GLUI_EditText *deltaT_edit;
GLUI_EditText *Re_edit;

GLUI_Panel *dispPanel;
GLUI_Button *startBtn;
GLUI_Button *stepBtn;
GLUI_Button *resetBtn;
GLUI_Button *freezeBtn;
GLUI_Button *paraBtn;
GLUI_RadioGroup *radioC;
GLUI_RadioGroup *radioK;

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
#define START_ID  110
#define FREEZE_ID 120
#define STEP_ID 130
#define RESET_ID 140
#define PARAMETER_ID 150
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

void control_cb(int control)
{
	double v1 = 10.0;//光源
	double v2 = 1.0;//カメラ操作

	//動画操作
	if(control == START_ID) {
		flagStart = 1;
		flagFreeze = 0;
		flagStep = 0;
		//init();
	}
	else if(control == FREEZE_ID) {
		if(flagFreeze == 0) flagFreeze = 1;
		else {flagFreeze = 0; flagStep = 0;}
	}
	else if(control == STEP_ID) {
		flagStep = 1;
		flagFreeze = 0;
	}
	else if(control == RESET_ID)
	{
		flagStart = 0;
		flagFreeze = 0;
		initData();
	}
	else if(control == PARAMETER_ID) flagParamShow = !flagParamShow;

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
		if(control == DOLLY_P_ID){ //近づく
			if(!flagOrtho) view.dist -= v2; 
			else ortho_scale -= 0.5;
		}
		else  {//遠ざかる
			if(!flagOrtho) view.dist += v2; 
			else ortho_scale += 0.5;
		}
		setCamera();
	}
	else if( control == PAN_P_ID || control == PAN_M_ID)//pan
	{
		if(control == PAN_P_ID) view.phi += v2 ;
		else view.phi -= v2 ;
		view.vCnt.x = view.vPos.x - view.dist * cos(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
		view.vCnt.y = view.vPos.y - view.dist * sin(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
	}
	else if(control == TILT_P_ID || control == TILT_M_ID)//tilt
	{
	  if(control == TILT_P_ID)
		  view.theta += v2 ; 
	  else
		  view.theta -= v2 ;
	  
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
		if( control == TUMBLE_P_ID) view.phi += v2 ;
		else                        view.phi -= v2 ;
		setCamera();
	}
	else if(control == CRANE_P_ID || control == CRANE_M_ID)//crane
	{
		if( control == CRANE_P_ID) view.theta += v2;
		else                       view.theta -= v2;
		setCamera();
	}
	else if(control == CAMERA_RESET_ID) view = view0;

}

void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	regionPanel = new GLUI_Panel(glui, "Region & Property" );
	nMesh_edit = new GLUI_EditText(regionPanel,"nMesh", &region.nMesh);
	nMesh_edit->set_int_limits( 10, 200);
	Radius_edit = new GLUI_EditText(regionPanel,"Radius", &region.Radius);
	glui->add_separator_to_panel(regionPanel);
	Re_edit = new GLUI_EditText(regionPanel,"Reynolds", &Re);
	Re_edit ->set_w(120);
	deltaT_edit = new GLUI_EditText(regionPanel,"deltaT", &deltaT);
	deltaT_edit ->set_w(120);

	calcPanel = new GLUI_Panel(glui, "Calculation" );
/*
	repeat_edit = new GLUI_EditText(calcPanel,"maxRepeat", &maxRepeat);
	repeat_edit->set_int_limits( 100, 50000);
	tolerance_edit = new GLUI_EditText(calcPanel,"tolerance", &tolerance);
	tolerance_edit -> set_w( 150 );
*/
	new GLUI_StaticText( calcPanel, "" );//空白
	startBtn = new GLUI_Button(calcPanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(calcPanel, "Freeze", FREEZE_ID, control_cb);
	stepBtn = new GLUI_Button(calcPanel, "Step", STEP_ID, control_cb);
	resetBtn = new GLUI_Button(calcPanel, "Reset", RESET_ID, control_cb);

	//paraBtn = new GLUI_Button(glui, "Parameter Show", PARAMETER_ID, control_cb);
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
	// 終了 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display" );
	GLUI_Spinner *spinner_speed;
	spinner_speed = new GLUI_Spinner( dispPanel, "speed", &dispSpeed);
	spinner_speed -> set_int_limits( 1, 50 );
	spinner_speed -> set_speed( 0.1 );

	//new GLUI_Checkbox(dispPanel, "Stream", &flagStream);
//	nLine_edit = new GLUI_EditText(dispPanel,"nLine", &nLine);
//	GLUI_Spinner *spinner_nLine;
//	spinner_nLine = new GLUI_Spinner( dispPanel, "nLine", &nLine);
//	spinner_nLine->set_int_limits( 10, 50 );
//	spinner_nLine->set_speed( 0.5 );
	radioK = new GLUI_RadioGroup(dispPanel, &kind, 3);
	new GLUI_RadioButton(radioK, "Omg");
	new GLUI_RadioButton(radioK, "Psi");
	new GLUI_RadioButton(radioK, "Vel");
	glui->add_separator_to_panel(dispPanel);

	GLUI_Spinner *spinner_maxOmg;
	spinner_maxOmg = new GLUI_Spinner( dispPanel, "maxOmega", &maxOmega);
	spinner_maxOmg->set_float_limits( 0.0, 25.0 );
	spinner_maxOmg->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);

	GLUI_Spinner *spinner_maxPsi;
	spinner_maxPsi = new GLUI_Spinner( dispPanel, "maxPsi", &maxPsi);
	spinner_maxPsi->set_float_limits( 0.0, 25.0 );
	spinner_maxPsi->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);

	GLUI_Spinner *spinner_maxVel;
	spinner_maxVel = new GLUI_Spinner( dispPanel, "maxVel", &maxVel);
	spinner_maxVel->set_float_limits( 0.0, 25.0 );
	spinner_maxVel->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);

 	new GLUI_Checkbox(dispPanel, "shading", &flagShading);

	GLUI_Spinner *spinner_adjustH;
	spinner_adjustH = new GLUI_Spinner( dispPanel, "adjustH", &adjustH);
	spinner_adjustH -> set_float_limits( 0.0, 50.0 );
	spinner_adjustH -> set_speed( 0.01 );

 	new GLUI_Checkbox(dispPanel, "OrthProjection", &flagOrtho);
	radioC = new GLUI_RadioGroup(dispPanel, &color, 3);
	new GLUI_RadioButton(radioC, "Color1");
	new GLUI_RadioButton(radioC, "Color2");
	new GLUI_RadioButton(radioC, "Monochrome");
	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "Wireframe", &flagWireframe);
 	new GLUI_Checkbox(dispPanel, "Parameter", &flagParamShow);
 	//new GLUI_Checkbox(dispPanel, "Scales", &flagScaleShow);
 	//new GLUI_Checkbox(dispPanel, "CoordShow", &flagCoordShow);
 	//new GLUI_Checkbox(dispPanel, "FloorShow", &flagFloor);

	//new GLUI_Checkbox(dispPanel, "grid", &flagGrid );

	glui->set_main_gfx_window( main_window );

	//********　　glui2　　****************
	glui2 = GLUI_Master.create_glui("GLUI-CAMERA", 0, win_x0, win_y0 + win_height + 30);
	//カメラ操作
	dollyP_btn = new GLUI_Button(glui2, "Dolly+", DOLLY_P_ID, control_cb);
	dollyP_btn ->set_w(60);
	dollyM_btn = new GLUI_Button(glui2, "Dolly-", DOLLY_M_ID, control_cb);
	dollyM_btn ->set_w(60);
	glui2->add_column(false);
	panP_btn = new GLUI_Button(glui2, "Pan+", PAN_P_ID, control_cb);
	panP_btn ->set_w(60);
	panM_btn = new GLUI_Button(glui2, "Pan-", PAN_M_ID, control_cb);
	panM_btn ->set_w(60);
	glui2->add_column(false);
	tiltP_btn = new GLUI_Button(glui2, "Tilt+", TILT_P_ID, control_cb);
	tiltP_btn ->set_w(60);
	tiltM_btn = new GLUI_Button(glui2, "Tilt-", TILT_M_ID, control_cb);
	tiltM_btn ->set_w(60);
	glui2->add_column(false);
	tumbleP_btn = new GLUI_Button(glui2, "Tumble+", TUMBLE_P_ID, control_cb);
	tumbleP_btn ->set_w(60);
	tumbleM_btn = new GLUI_Button(glui2, "Tumble-", TUMBLE_M_ID, control_cb);
	tumbleM_btn ->set_w(60);
	glui2->add_column(false);
	craneP_btn = new GLUI_Button(glui2, "Crane+", CRANE_P_ID, control_cb);
	craneP_btn ->set_w(60);
	craneP_btn ->set_w(60);
	craneM_btn = new GLUI_Button(glui2, "Crane-", CRANE_M_ID, control_cb);
	craneM_btn ->set_w(60);
	glui2->add_column(false);
	zoomP_btn = new GLUI_Button(glui2, "Zoom+", ZOOM_P_ID, control_cb);
	zoomP_btn ->set_w(60);
	zoomM_btn = new GLUI_Button(glui2, "Zoom-", ZOOM_M_ID, control_cb);
	zoomM_btn ->set_w(60);
	glui2->add_column(false);
	resetCamera_btn = new GLUI_Button(glui2, "ResetCamera", CAMERA_RESET_ID, control_cb);

	glui2->set_main_gfx_window( main_window );

}


