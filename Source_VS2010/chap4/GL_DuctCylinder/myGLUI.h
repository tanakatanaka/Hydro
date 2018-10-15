
#define NUM_MAX_X 210
#define NUM_MAX_Y 110

//Windowのサイズ,位置
int win_width  = 800;
int win_height = 500;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 200;
int win_y0 = 100;

//解析領域矩形構造体
struct Rect
{
	float scale;
	Vector2D pos0; //ローカルウィンドウのデカルト座標における矩形領域の中心
	Vector2D left0;//その左下位置
	Vector2D size; //矩形領域のサイズ(固定）
	float obs_left;//左端から障害物中心までの距離
	float obs_radius; //障害物の半径
	int nMeshX;     //分割数(X)
	int nMeshY;     //分割数(Y)
	Vector2D delta;//格子間隔
};
Rect rect = {
	1.0,//scale
	Vector2D(0.0, -0.05),//pos0(仮の値)
	Vector2D(-0.5, 0.5), //left0(仮の値)
	Vector2D(2.0, 1.0),  //size
	0.3, 0.1,      //obs_left, radius
	100,//nMeshX
	50 //nMeshY

};

void init();
void setGLUI();

int flagExecute = 0;
int flagMouse = 0;
int flagStart = 0; 
int flagP_Start = 0;
int flagFreeze = 0;
int flagStep = 0;
int flagStream = 0;
int flagVorticity = 1;
int flagVelocity = 0;
int flagGrid = 0;
int flagParameter = 1;
int flagResetting = 0;

int main_window;//GLUTで作るWindowのID
int iteration = 200;//最大繰り返し回数(Poisson方程式）
float tolerance = 0.00001;//許容誤差
float deltaT = 0.002;
float Re = 5000.0;//レイノルズ数
int sizeParticle = 5;
float speedCoef = 1.0;//速度倍率
float intervalP = 0.05;
int thinningV = 1;

float minPsi = 0.0;
float maxPsi = 1.0;
float minOmg = -100.0;
float maxOmg = 100.0;

int nLine = 20;//等高線表示本数
float scaleArrow = 0.02;
float widthArrow = 0.0;
int arrow = 0;

//Pointers
GLUI *glui;
GLUI_Panel *paraPanel;
GLUI_EditText *left_edit;
GLUI_EditText *radius_edit;
GLUI_EditText *nMeshX_edit;
GLUI_EditText *nMeshY_edit;
GLUI_EditText *Re_edit;
GLUI_EditText *deltaT_edit;
GLUI_EditText *thinningN_edit;

GLUI_Panel *calcPanel;
GLUI_EditText *repeat_edit;
GLUI_EditText *tolerance_edit;

GLUI_Panel *dispPanel;
GLUI_EditText *nLine_edit;
GLUI_RadioGroup *radio;

GLUI_Panel *partPanel;
GLUI_EditText *numP_edit;
GLUI_EditText *sizeP_edit;
GLUI_EditText *speedC_edit;
GLUI_EditText *interval_edit;

GLUI_Button *startBtn;
GLUI_Button *p_startBtn;
GLUI_Button *resetBtn;
GLUI_Button *stepBtn;
GLUI_Button *freezeBtn;
GLUI_Button *paraBtn;

//USER ID
#define START_ID  110
#define FREEZE_ID 120
#define STEP_ID 130
#define RESET_ID 140
#define pSTART_ID 200
#define PARAMETER_ID 400

void control_cb(int control)
{
	//計算
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
		flagStep = 0;
		flagResetting = 0;
		init();
	}
	else if(control == PARAMETER_ID) flagParameter = !flagParameter;
}

void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	paraPanel = new GLUI_Panel(glui, "Parameters" );
	left_edit = new GLUI_EditText(paraPanel,"obs_left", &rect.obs_left);
	//left_edit->set_float_limits( 0.0, 1.5);
	radius_edit = new GLUI_EditText(paraPanel,"obs_radius", &rect.obs_radius);
	radius_edit->set_float_limits( 0.06, 0.2);
	nMeshX_edit = new GLUI_EditText(paraPanel,"nMeshX", &rect.nMeshX);
	//nMeshX_edit->set_int_limits( 20, 100);
	nMeshY_edit = new GLUI_EditText(paraPanel,"nMeshY", &rect.nMeshY);
	//nMeshY_edit->set_int_limits( 5, 50);
	Re_edit = new GLUI_EditText(paraPanel,"Reynolds", &Re);
	//Re_edit ->set_w(100);
	deltaT_edit = new GLUI_EditText(paraPanel,"deltaT", &deltaT);
	//deltaT_edit ->set_w(100);

	calcPanel = new GLUI_Panel(glui, "Calculation" );
	repeat_edit = new GLUI_EditText(calcPanel,"iteration", &iteration);
	tolerance_edit = new GLUI_EditText(calcPanel,"tolerance", &tolerance);
	tolerance_edit -> set_w( 150 );
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
	spinner_scale = new GLUI_Spinner( dispPanel, "scale", &rect.scale);
	spinner_scale->set_float_limits(0.5, 5.0 );
	spinner_scale->set_speed( 0.5 );
	//spinner_scale->set_alignment( GLUI_ALIGN_RIGHT );
	GLUI_Spinner *spinner_posY;
	spinner_posY = new GLUI_Spinner( dispPanel, "pos0.y", &rect.pos0.y);
	spinner_posY->set_float_limits(-2.0, 2.0 );
	spinner_posY->set_speed( 0.2 );
	GLUI_Spinner *spinner_posX;
	spinner_posX = new GLUI_Spinner( dispPanel, "pos0.x", &rect.pos0.x);
	spinner_posX->set_float_limits(-2.0, 2.0 );
	spinner_posX->set_speed( 0.2 );

	glui->add_separator_to_panel(dispPanel);
	GLUI_Spinner *spinner_nLine;
	spinner_nLine = new GLUI_Spinner( dispPanel, "nLine", &nLine);
	spinner_nLine->set_int_limits( 10, 50 );
	spinner_nLine->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "Vorticity(omega)", &flagVorticity);
	GLUI_Spinner *spinner_minOmg;
	spinner_minOmg = new GLUI_Spinner( dispPanel, "minOmg", &minOmg);
	spinner_minOmg->set_float_limits( -500.0, 0.0 );
	spinner_minOmg->set_speed( 1.0 );
	GLUI_Spinner *spinner_maxOmg;
	spinner_maxOmg = new GLUI_Spinner( dispPanel, "maxOmg", &maxOmg);
	spinner_maxOmg->set_float_limits( 0.0, 500.0 );
	spinner_maxOmg->set_speed( 1.0 );

	glui->add_separator_to_panel(dispPanel);
	new GLUI_Checkbox(dispPanel, "Stream(psi)", &flagStream);
	GLUI_Spinner *spinner_minPsi;
	spinner_minPsi = new GLUI_Spinner( dispPanel, "minPsi", &minPsi);
	spinner_minPsi->set_float_limits( -5.0, 0.0 );
	spinner_minPsi->set_speed( 1.0 );
	GLUI_Spinner *spinner_maxPsi;
	spinner_maxPsi = new GLUI_Spinner( dispPanel, "maxPsi", &maxPsi);
	spinner_maxPsi->set_float_limits( 0.0, 5.0 );
	spinner_maxPsi->set_speed( 1.0 );
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

	partPanel = new GLUI_Panel(glui, "Particle" );
	new GLUI_Checkbox(partPanel, "particle", &flagP_Start);
	numP_edit = new GLUI_EditText(partPanel,"numP0", &numP0);
	numP_edit->set_int_limits( 10, MAX_PARTICLE);
	sizeP_edit = new GLUI_EditText(partPanel,"size", &sizeParticle);
	speedC_edit = new GLUI_EditText(partPanel,"speedC", &speedCoef);
	interval_edit = new GLUI_EditText(partPanel,"interval", &intervalP);
}


