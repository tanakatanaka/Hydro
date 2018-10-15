
//Windowのサイズ,位置
int win_width  = 520;
int win_height = 520;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 300;
int win_y0 = 100;

#define NUM_MAX  110//最大分割数（最大NUM_MAX*NUM_MAXの格子数）

//解析領域矩形構造体
struct Rect
{
	float scale;
	Vector2D pos0; //ローカルウィンドウのデカルト座標における矩形領域の中心
	Vector2D left0;//その左下位置
	Vector2D size; //矩形領域のサイズ(固定）
	int mesh;      //全体の分割数(X,Y共通）
	Vector2D delta;//格子間隔
};
Rect rect = {
	1.0,//scale
	Vector2D(0.0, -0.1),//pos0(仮の値)
	Vector2D(-0.5, 0.5),//left0(仮の値)
	Vector2D(1.0, 1.0), //size
	50//mesh
};

void init();
void setGLUI();

int flagExecute = 0;
int flagMouse = 0;
int flagStart = 0; 
int flagP_Start = 0;
int flagFreeze = 0;
int flagStep;
int flagPressureColor = 0;
int flagPressureIsobar = 1;
int flagStream = 0;
int flagVorticity = 0;
int flagVelocity = 0;
int flagGrid = 0;
int flagParameter = 1;

int main_window;//GLUTで作るWindowのID
int iteration = 100;//最大繰り返し回数(Poisson方程式）
float tolerance = 0.00001;//許容誤差
//float radius = 0.2;//初期分布の半径
int thinningN = 1;
int thinningV = 1;
float deltaT = 0.001;
float Re = 50.0;//レイノルズ数
int sizeParticle = 5;

float minPsi = -0.1;
float maxPsi = 0.0;
float minPrs = -0.2;
float maxPrs = 0.2;
float minOmg = -30.0;
float maxOmg = 10.0;


int nLine = 10;//等高線表示本数
float scaleArrow = 0.2;
float widthArrow = 0.0;
int arrow = 0;

//Pointers
GLUI *glui;
GLUI_Panel *paraPanel;
GLUI_EditText *mesh_edit;
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

GLUI_Button *startBtn;
GLUI_Button *resetBtn;
GLUI_Button *freezeBtn;
GLUI_Button *stepBtn;
GLUI_Button *paraBtn;

//USER ID
//#define EXECUTE_ID  100
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
		init();
	}
	else if(control == PARAMETER_ID) flagParameter = !flagParameter;
}

void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	paraPanel = new GLUI_Panel(glui, "Parameters" );
	mesh_edit = new GLUI_EditText(paraPanel,"mesh", &rect.mesh);
	mesh_edit->set_int_limits( 20, 100);
	Re_edit = new GLUI_EditText(paraPanel,"Reynolds", &Re);
	Re_edit ->set_w(60);
	deltaT_edit = new GLUI_EditText(paraPanel,"deltaT", &deltaT);
	deltaT_edit ->set_w(60);

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
	sizeP_edit = new GLUI_EditText(partPanel,"size", &sizeParticle);

	new GLUI_StaticText( glui, "" );//空白
	// 終了 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display" );
	GLUI_Spinner *spinner_scale;
	spinner_scale = new GLUI_Spinner( dispPanel, "scale", &rect.scale);
	spinner_scale->set_float_limits(0.5, 1.5 );
	spinner_scale->set_speed( 0.5 );
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

	new GLUI_Checkbox(dispPanel, "Stream(psi)", &flagStream);
	GLUI_Spinner *spinner_maxPsi;
	spinner_maxPsi = new GLUI_Spinner( dispPanel, "maxPsi", &maxPsi);
	spinner_maxPsi->set_float_limits( 0.0, 5.0 );
	spinner_maxPsi->set_speed( 0.1 );
	GLUI_Spinner *spinner_minPsi;
	spinner_minPsi = new GLUI_Spinner( dispPanel, "minPsi", &minPsi);
	spinner_minPsi->set_float_limits( -5.0, 0.0 );
	spinner_minPsi->set_speed( 0.1 );
	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "Vorticity(omega)", &flagVorticity);
	GLUI_Spinner *spinner_maxOmg;
	spinner_maxOmg = new GLUI_Spinner( dispPanel, "maxOmg", &maxOmg);
	spinner_maxOmg->set_float_limits( 0.0, 50.0 );
	GLUI_Spinner *spinner_minOmg;
	spinner_minOmg = new GLUI_Spinner( dispPanel, "minOmg", &minOmg);
	spinner_minOmg->set_float_limits( -50.0, 0.0 );
	spinner_minOmg->set_speed( 0.1 );
	spinner_maxOmg->set_speed( 0.1 );

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


