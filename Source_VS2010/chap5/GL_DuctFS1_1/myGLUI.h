
#define NUM_MAX_X 120
#define NUM_MAX_Y 60

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
	float obs_thick; //障害物の厚さ
	float obs_width; //障害物の幅
	float obs_posY;//高さ(下の壁面からの距離)
	int meshX;     //分割数(X)
	int meshY;     //分割数(Y)
	Vector2D delta;//格子間隔
};
Rect rect = {

	1.0,//scale
	Vector2D(0.0, -0.05),//pos0(仮の値)
	Vector2D(-0.5, 0.5), //left0(仮の値)
	Vector2D(2.0, 1.0),  //size
	0.3, 0.12, 0.12, 0.5, //obs_left, thick, width, height
	100,//meshX
	50 //meshY

};

void init();
void setGLUI();

int flagExecute = 0;
int flagMouse = 0;
int flagStart = 0; 
int flagP_Start = 0;
int flagFreeze = 0;
int flagStep = 0;
int flagPressureColor = 1;
int flagPressureIsobar = 0;
int flagStream = 0;
int flagOmega = 1;
int flagOmegaColor = 0;
int flagVelocity = 0;
int flagGrid = 0;
int flagParameter = 1;

int main_window;//GLUTで作るWindowのID
int iteration = 100;//最大繰り返し回数(Poisson方程式）
float tolerance = 0.001;//許容誤差
float deltaT = 0.002;
float Re = 1000.0;//レイノルズ数
int sizeParticle = 5;
float speedCoef = 1.0;//速度倍率
float intervalP = 0.05;
int thinningV = 1;

float minPrs = -0.5;
float maxPrs = 0.5;
float minOmg = -10.0;
float maxOmg = 10.0;

int nLine = 20;//等高線表示本数
float scaleArrow = 0.02;
float widthArrow = 0.0;
int arrow = 0;

//Pointers
GLUI *glui;
GLUI_Panel *paraPanel;
GLUI_EditText *left_edit;
GLUI_EditText *thick_edit;
GLUI_EditText *width_edit;
GLUI_EditText *height_edit;
GLUI_EditText *meshX_edit;
GLUI_EditText *meshY_edit;
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
	thick_edit = new GLUI_EditText(paraPanel,"obs_thick", &rect.obs_thick);
	//thick_edit->set_float_limits( 0.0, 0.8);
	width_edit = new GLUI_EditText(paraPanel,"obs_width", &rect.obs_width);
	//width_edit->set_float_limits( 0.0, 0.8);
	height_edit = new GLUI_EditText(paraPanel,"obs_posY", &rect.obs_posY);
	meshX_edit = new GLUI_EditText(paraPanel,"meshX", &rect.meshX);
	meshX_edit->set_int_limits( 20, 100);
	meshY_edit = new GLUI_EditText(paraPanel,"meshY", &rect.meshY);
	meshY_edit->set_int_limits( 5, 50);
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
	
	partPanel = new GLUI_Panel(glui, "Particle" );
	new GLUI_Checkbox(partPanel, "particle", &flagP_Start);
	numP_edit = new GLUI_EditText(partPanel,"numP0", &numP0);
	numP_edit->set_int_limits( 10, MAX_PARTICLE);
	sizeP_edit = new GLUI_EditText(partPanel,"size", &sizeParticle);
	speedC_edit = new GLUI_EditText(partPanel,"speedC", &speedCoef);
	interval_edit = new GLUI_EditText(partPanel,"interval", &intervalP);

	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display" );
	GLUI_Spinner *spinner_scale;
	spinner_scale = new GLUI_Spinner( dispPanel, "scale", &rect.scale);
	//spinner_scale->set_float_limits(0.5, 5.0 );
	spinner_scale->set_speed( 0.1 );
	//spinner_scale->set_alignment( GLUI_ALIGN_RIGHT );
	GLUI_Spinner *spinner_posY;
	spinner_posY = new GLUI_Spinner( dispPanel, "pos0.y", &rect.pos0.y);
	//spinner_posY->set_float_limits(-2.0, 2.0 );
	spinner_posY->set_speed( 0.2 );
	GLUI_Spinner *spinner_posX;
	spinner_posX = new GLUI_Spinner( dispPanel, "pos0.x", &rect.pos0.x);
	//spinner_posX->set_float_limits(-2.0, 2.0 );
	spinner_posX->set_speed( 0.2 );

	glui->add_separator_to_panel(dispPanel);
	GLUI_Spinner *spinner_nLine;
	spinner_nLine = new GLUI_Spinner( dispPanel, "nLine", &nLine);
	spinner_nLine->set_int_limits( 10, 50 );
	spinner_nLine->set_speed( 0.5 );
	glui->add_separator_to_panel(dispPanel);
	glui->add_statictext_to_panel(dispPanel, "pressure" );
	new GLUI_Checkbox(dispPanel, "color", &flagPressureColor );
	new GLUI_Checkbox(dispPanel, "isobar", &flagPressureIsobar );
	GLUI_Spinner *spinner_maxPrs;
	spinner_maxPrs = new GLUI_Spinner( dispPanel, "maxPrs", &maxPrs);
	spinner_maxPrs->set_float_limits( 0.0, 50.0 );
	spinner_maxPrs->set_speed( 0.1 );
	GLUI_Spinner *spinner_minPrs;
	spinner_minPrs = new GLUI_Spinner( dispPanel, "minPrs", &minPrs);
	spinner_minPrs->set_float_limits( -50.0, 0.0 );
	spinner_minPrs->set_speed( 0.1 );
	glui->add_separator_to_panel(dispPanel);

	glui->add_statictext_to_panel(dispPanel, "Vorticity(omega)" );
	new GLUI_Checkbox(dispPanel, "color", &flagOmegaColor );
	new GLUI_Checkbox(dispPanel, "Vorticity(omega)", &flagOmega);
	GLUI_Spinner *spinner_maxOmg;
	spinner_maxOmg = new GLUI_Spinner( dispPanel, "maxOmg", &maxOmg);
	spinner_maxOmg->set_float_limits( 0.0, 50.0 );
	spinner_maxOmg->set_speed( 0.1 );
	GLUI_Spinner *spinner_minOmg;
	spinner_minOmg = new GLUI_Spinner( dispPanel, "minOmg", &minOmg);
	spinner_minOmg->set_float_limits( -50.0, 0.0 );
	spinner_minOmg->set_speed( 0.1 );

	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "velocity", &flagVelocity );
	GLUI_Spinner *spinner_scaleArrow;
	spinner_scaleArrow = new GLUI_Spinner( dispPanel, "scaleArrow", &scaleArrow);
	spinner_scaleArrow->set_float_limits(0.0, 1.0 );
	spinner_scaleArrow->set_speed( 0.1 );
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

	//new GLUI_StaticText( glui, "" );//空白
	// 終了 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

}


