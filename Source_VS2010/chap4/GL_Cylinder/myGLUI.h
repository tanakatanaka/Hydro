
//Windowのサイズ,位置
int win_width  = 800;
int win_height = 500;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 300;
int win_y0 = 100;

#define NUM_MAX  110//最大分割数（最大NUM_MAX*NUM_MAXの格子数）

//解析領域構造体
struct Region
{
	float scale;
	Vector2D pos0;//ローカルウィンドウのデカルト座標における円柱の中心
	float Radius;//外側領域の半径
	int nMesh;//分割数(動径方向，方位角方向同じ)
	//Vector2D delta;
};
Region region = {
	1.0,//scale
	Vector2D(0.0, 0.0),//pos0
	30.0,//Radius
	40//nMesh(分割数）
};

void init();
void setGLUI();

int flagExecute = 0;
int flagMouse = 0;
int flagStart = 0; 
int flagFreeze = 0;
int flagStep = 0;
int flagStream = 1;
int flagOmega = 0.0;
int flagVelocity = 0;
int flagGrid = 0;
int flagParameter = 1;

int main_window;//GLUTで作るWindowのID
int iteration = 300;//最大繰り返し回数(Poisson方程式）
float tolerance = 0.00001;//許容誤差
float deltaT = 0.02;
float Re = 500.0;//レイノルズ数
float minPsi = -4.0;
float maxPsi = 4.0;
float minOmega = -10.0;
float maxOmega =  10.0;
int thinningV = 1;

int nLine = 30;//等高線表示本数
float scaleArrow = 0.02;
float widthArrow = 0.0;
int arrow = 0;

//Pointers
GLUI *glui;
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
GLUI_EditText *nLine_edit;
GLUI_EditText *minPsi_edit;
GLUI_EditText *maxPsi_edit;

GLUI_Button *startBtn;
GLUI_Button *resetBtn;
GLUI_Button *freezeBtn;
GLUI_Button *stepBtn;
GLUI_Button *paraBtn;

//USER ID
#define START_ID  110
#define FREEZE_ID 120
#define STEP_ID 130
#define RESET_ID 140
#define PARAMETER_ID 400

void control_cb(int control)
{

	//動画操作
	if(control == START_ID) {
		flagStart = 1;
		flagFreeze = 0;
		flagStep = 0;
//		init();
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
		flagStep = 0;
		init();
	}
	else if(control == PARAMETER_ID) flagParameter = !flagParameter;
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
	deltaT_edit ->set_w(100);

	calcPanel = new GLUI_Panel(glui, "Calculation" );
	repeat_edit = new GLUI_EditText(calcPanel,"iteration", &iteration);
	repeat_edit->set_int_limits( 1, 500);
	tolerance_edit = new GLUI_EditText(calcPanel,"tolerance", &tolerance);
	tolerance_edit -> set_w( 150 );
	deltaT_edit = new GLUI_EditText(calcPanel,"deltaT", &deltaT);
	deltaT_edit ->set_w(60);
	new GLUI_StaticText( calcPanel, "" );//空白
	startBtn = new GLUI_Button(calcPanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(calcPanel, "Freeze", FREEZE_ID, control_cb);
	stepBtn = new GLUI_Button(calcPanel, "Step", STEP_ID, control_cb);
	resetBtn = new GLUI_Button(calcPanel, "Reset", RESET_ID, control_cb);

	paraBtn = new GLUI_Button(glui, "Parameter Show", PARAMETER_ID, control_cb);
	
	new GLUI_StaticText( glui, "" );//空白
	// 終了 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display" );
	GLUI_Spinner *spinner_scale;
	spinner_scale = new GLUI_Spinner( dispPanel, "scale", &region.scale);
	spinner_scale->set_float_limits(0.5, 50.0 );
	spinner_scale->set_speed( 0.1 );
	//spinner_scale->set_alignment( GLUI_ALIGN_RIGHT );
	GLUI_Spinner *spinner_posY;
	spinner_posY = new GLUI_Spinner( dispPanel, "pos0.y", &region.pos0.y);
	spinner_posY->set_float_limits(-2.0, 2.0 );
	spinner_posY->set_speed( 0.1 );
	GLUI_Spinner *spinner_posX;
	spinner_posX = new GLUI_Spinner( dispPanel, "pos0.x", &region.pos0.x);
	spinner_posX->set_float_limits(-2.0, 2.0 );
	spinner_posX->set_speed( 0.1 );

	glui->add_separator_to_panel(dispPanel);
//	nLine_edit = new GLUI_EditText(dispPanel,"nLine", &nLine);
	GLUI_Spinner *spinner_nLine;
	spinner_nLine = new GLUI_Spinner( dispPanel, "nLine", &nLine);
	spinner_nLine->set_int_limits( 10, 50 );
	spinner_nLine->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "Vorticity(omega)", &flagOmega);
	GLUI_Spinner *spinner_minOmg;
	spinner_minOmg = new GLUI_Spinner( dispPanel, "minOmega", &minOmega);
	spinner_minOmg->set_float_limits( -250.0, 0.0 );
	spinner_minOmg->set_speed( 0.5 );
	GLUI_Spinner *spinner_maxOmg;
	spinner_maxOmg = new GLUI_Spinner( dispPanel, "maxOmega", &maxOmega);
	spinner_maxOmg->set_float_limits( 0.0, 250.0 );
	spinner_maxOmg->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "Stream(psi)", &flagStream);
	GLUI_Spinner *spinner_minPsi;
	spinner_minPsi = new GLUI_Spinner( dispPanel, "minPsi", &minPsi);
	spinner_minPsi->set_float_limits( -50.0, 0 );
	spinner_minPsi->set_speed( 0.5 );
	GLUI_Spinner *spinner_maxPsi;
	spinner_maxPsi = new GLUI_Spinner( dispPanel, "maxPsi", &maxPsi);
	spinner_maxPsi->set_float_limits( 0, 50.0 );
	spinner_maxPsi->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);



	new GLUI_Checkbox(dispPanel, "velocity", &flagVelocity );
	GLUI_Spinner *spinner_scaleArrow;
	spinner_scaleArrow = new GLUI_Spinner( dispPanel, "scaleArrow", &scaleArrow);
	spinner_scaleArrow->set_float_limits(0.0, 1.0 );
	spinner_scaleArrow->set_speed( 0.1 );
	//spinner_scaleArrow->set_alignment( GLUI_ALIGN_RIGHT );
	radio = new GLUI_RadioGroup(dispPanel, &arrow);
	new GLUI_RadioButton(radio, "Thin");
	//glui->add_column_to_panel(dispPanel, false);
	new GLUI_RadioButton(radio, "Thick");
	GLUI_Spinner *spinner_thinningV;
	spinner_thinningV = new GLUI_Spinner( dispPanel, "thinningV", &thinningV);
	spinner_thinningV->set_int_limits(1, 5 );
	spinner_thinningV->set_speed( 0.1 );
	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "grid", &flagGrid );
}


