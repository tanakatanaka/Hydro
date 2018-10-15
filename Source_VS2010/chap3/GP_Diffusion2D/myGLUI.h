
//glui関連のパラメータ，関数など

#define MAX_SLICE 560//最大分割数（両辺同じ）

struct Rect
{
	Vector2D size;//矩形領域のサイズ
	int nMesh;//全体の分割数(X,Y共通）
	Vector2D delta;
};
Rect rect = {
	Vector2D(10.0, 10.0),//size(固定）
	100//nMesh
};


//プロジェクトのメインファイルに実装されている関数
void initData();

int main_window;//GLUTで作るWindowのID
int flagWireframe = 0;
int flagParamShow = 1;
int flagConstant = 0;
int flagScaleShow = 0;
int flagCoordShow = 0;
int flagFloor = 0;

int color = 1;
float adjustH = 5.0;//表示上の高さ調整
float diff_coef = 0.1;//拡散係数
float deltaT0 = 0.01;
float radiusInit = 2.0;//初期中心温度（濃度）半径
int boundary = 0;
int thinningN = 1;
int profile = 0;//円柱

//Pointers
GLUI *glui, *glui2;
GLUI_Panel  *animePanel;
GLUI_Panel *paraPanel;
GLUI_Panel *boundaryPanel;
GLUI_Panel *dispPanel;
GLUI_Panel *profilePanel;
GLUI_Button *startBtn;
GLUI_Button *freezeBtn;
GLUI_Button *stepBtn;
GLUI_Button *resetBtn;
GLUI_EditText *deltaT_edit;
GLUI_EditText *thinning_edit;
GLUI_EditText *radius_edit;
GLUI_EditText *diff_edit;
GLUI_EditText *nMesh_edit;
GLUI_RadioGroup *radioB;
GLUI_RadioGroup *radioC;
GLUI_RadioGroup *radioD;

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
	double v2 = 0.5;//カメラ操作

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

	//オブジェクト初期設定
	paraPanel = new GLUI_Panel(glui, "Parameters" );
	
	nMesh_edit  = new GLUI_EditText(paraPanel, "nMesh", &rect.nMesh);
	nMesh_edit->set_int_limits(50, MAX_SLICE);
	nMesh_edit ->set_w(80);
	radius_edit  = new GLUI_EditText(paraPanel, "radiusInt", &radiusInit);
	radius_edit ->set_w(80);
	diff_edit  = new GLUI_EditText(paraPanel, "diff_coef", &diff_coef);
	diff_edit ->set_w(80);
	deltaT_edit = new GLUI_EditText(paraPanel,"deltaT0[s]", &deltaT0);
	deltaT_edit ->set_w(80);
	thinning_edit = new GLUI_EditText(paraPanel,"thinningN", &thinningN);
	thinning_edit->set_int_limits( 1, 200 );
	thinning_edit ->set_w(60);

	profilePanel = new GLUI_Panel(glui, "Profile" );
	radioD = new GLUI_RadioGroup(profilePanel, &profile, 3);
	new GLUI_RadioButton(radioD, "Cylinder");
	new GLUI_RadioButton(radioD, "Cube");

	new GLUI_Checkbox(glui, "Constant", &flagConstant );
	animePanel = new GLUI_Panel(glui, "Animation" );
	startBtn = new GLUI_Button(animePanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(animePanel, "Freeze", FREEZE_ID, control_cb);
	stepBtn = new GLUI_Button(animePanel, "Step", STEP_ID, control_cb);
	resetBtn = new GLUI_Button(animePanel, "Reset", RESET_ID, control_cb);
	new GLUI_StaticText( glui, "" );//空白

	glui->add_column(true);

	dispPanel = new GLUI_Panel(glui, "Display" );
	GLUI_Spinner *spinner_adjustH;
	spinner_adjustH = new GLUI_Spinner( dispPanel, "adjustH", &adjustH);
	spinner_adjustH -> set_float_limits(0.01, 5.0 );
	spinner_adjustH -> set_speed( 0.1 );
	radioC = new GLUI_RadioGroup(dispPanel, &color, 3);
	new GLUI_RadioButton(radioC, "Color1");
	new GLUI_RadioButton(radioC, "Color2");
	new GLUI_Checkbox(dispPanel, "Wireframe", &flagWireframe);
 	new GLUI_Checkbox(dispPanel, "Parameter", &flagParamShow);
 	new GLUI_Checkbox(dispPanel, "Scales", &flagScaleShow);
 	new GLUI_Checkbox(dispPanel, "CoordShow", &flagCoordShow);
 	new GLUI_Checkbox(dispPanel, "FloorShow", &flagFloor);
 	new GLUI_Checkbox(dispPanel, "OrthoProjection", &flagOrtho);

	boundaryPanel = new GLUI_Panel(glui, "Boundary Condition" );
	radioB = new GLUI_RadioGroup(boundaryPanel, &boundary, 2);
	new GLUI_RadioButton(radioB, "Dirichlet");
	new GLUI_RadioButton(radioB, "Neumann");

	cameraOpe_panel = new GLUI_Panel(glui, "CameraOperation");
	hide_btn = new GLUI_Button(cameraOpe_panel, "Hide", HIDE_ID, control_cb);
	show_btn = new GLUI_Button(cameraOpe_panel, "Show", SHOW_ID, control_cb);
	
	glui->set_main_gfx_window( main_window );

	//new GLUI_StaticText( glui, "" );//空白

	//****** 終了 *****
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );


	//********glui2****************
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

