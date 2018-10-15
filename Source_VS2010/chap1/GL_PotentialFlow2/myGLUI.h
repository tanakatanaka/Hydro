
//Windowのサイズ,位置
int win_width  = 600;
int win_height = 500;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 300;
int win_y0 = 100;

#define NUM_MAX  201//最大分割数（最大NUM_MAX*NUM_MAXの格子数）

//解析領域矩形構造体
struct Rect
{
	float scale;//表示倍率
	Vector2D pos0;//ローカルウィンドウのデカルト座標における矩形領域の中心
	Vector2D left0;//その左下位置
	Vector2D size;//矩形領域のサイズ
	int nMesh;//全体の分割数(X,Y共通）
	Vector2D delta;
};
Rect rect = {
	1.0,//scale
	Vector2D(0.0, 0.0),//pos0
	Vector2D(-1.0, 1.0),//left0(仮の値)
	Vector2D(2.0, 1.6),//size
	100//nMesh
};

void init();
void setGLUI();
int flagExecute = 0;
int flagMouse = 0;
int flagStart = 0; 
int flagFreeze = 0;
int flagStream = 1;
int flagPotential = 1;
int flagVelocity = 0;
int flagGrid = 0;
int flagUniform = 1;
int flagSource = 1;
int flagVortex = 0;
int flagParameter = 1;

int main_window;//GLUTで作るWindowのID

int nLine = 40;//ポテンシャル，流線表示本数
float range = 1.0;//その表示範囲調整
float scaleArrow = 0.02;
float widthArrow = 0.0;
int arrow = 0;//矢印の太さ
int sizeParticle = 5;
float speedCoef = 0.1;//速度倍率
float intervalP = 0.1;

float flowVelocity = 1.0;//一様流れの流速
float alpha = 0.0;//x軸となす角(傾斜角)
float alpha_int = 0;
float Q_Value = 1.0;//湧き出しの単位厚さあたりの流量
float distA = 0.1;//原点から湧出し，吸込みまでの距離
float Gamma = 1.0;//渦の強さ（循環）

//Pointers
GLUI *glui;
GLUI_Panel *rectPanel;
GLUI_EditText *sizeX_edit;
GLUI_EditText *sizeY_edit;
GLUI_EditText *nMesh_edit;
GLUI_RadioGroup *radio;

GLUI_Panel *kindPanel;

GLUI_Panel *dispPanel;
GLUI_EditText *nLine_edit;

GLUI_Panel *partPanel;
GLUI_EditText *numP_edit;
GLUI_EditText *sizeP_edit;
GLUI_EditText *speedC_edit;
GLUI_EditText *interval_edit;

GLUI_Button *executeBtn;
GLUI_Button *initBtn;
GLUI_Button *startBtn;
GLUI_Button *freezeBtn;
GLUI_Button *resetBtn;
GLUI_Button *paraBtn;

//USER ID
#define EXECUTE_ID  100
#define INIT_ID 105
#define START_ID  110
#define FREEZE_ID 120
#define RESET_ID 150
#define STOP_ID 300
#define PARAMETER_ID 400

void control_cb(int control)
{

	//計算実行
	if(control == EXECUTE_ID) {
		flagExecute = 1;
		init();
	}
	else if(control == INIT_ID) {
		flagExecute = 0;
		init();
	}
	//動画操作
	else if(control == START_ID) {
		flagStart = 1;
		flagMouse = 0;
		flagExecute = 0;
		flagFreeze = 0;
		init();
	}
	else if(control == FREEZE_ID) {
		if(flagFreeze == 0) flagFreeze = 1;
		else {flagFreeze = 0; }
	}
	else if(control == RESET_ID)
	{
		flagStart = 0;
		flagFreeze= 0;
		init();
	}
	else if(control == STOP_ID)
	{
		flagExecute = 0;
	}
	else if(control == PARAMETER_ID) flagParameter = !flagParameter;
}
void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	rectPanel = new GLUI_Panel(glui, "Rect" );
	sizeX_edit = new GLUI_EditText(rectPanel,"size.x", &rect.size.x, STOP_ID, control_cb);
	sizeX_edit -> set_float_limits( 0.5, 5.0);
	sizeX_edit -> set_w( 80 );
	sizeY_edit = new GLUI_EditText(rectPanel,"size.y", &rect.size.y, STOP_ID, control_cb);
	sizeY_edit -> set_float_limits( 0.5, 5.0);
	sizeY_edit -> set_w( 80 );
	nMesh_edit = new GLUI_EditText(rectPanel,"nMesh", &rect.nMesh, STOP_ID, control_cb);
	nMesh_edit -> set_int_limits( 10, 200);
	nMesh_edit -> set_w( 80 );

	kindPanel = new GLUI_Panel(glui, "Kind");
	new GLUI_Checkbox(kindPanel, "Uniform", &flagUniform );
	GLUI_Spinner *spinner_Vel;
	spinner_Vel = new GLUI_Spinner( kindPanel, "Velocity", &flowVelocity);
	spinner_Vel -> set_float_limits(0.0, 5.0 );
	spinner_Vel -> set_speed( 1.0 );
	GLUI_Spinner *spinner_Alpha;
	spinner_Alpha = new GLUI_Spinner( kindPanel, "alpha(deg)", &alpha_int);
	spinner_Alpha -> set_int_limits(0, 90 );
	spinner_Alpha -> set_speed( 1.0 );

	glui->add_separator_to_panel(kindPanel);
	new GLUI_Checkbox(kindPanel, "Source&Sink", &flagSource );
	GLUI_Spinner *spinner_Q_Value;
	spinner_Q_Value = new GLUI_Spinner( kindPanel, "Q_Value", &Q_Value);
	spinner_Q_Value -> set_float_limits(-5.0, 5.0);
	spinner_Q_Value -> set_speed( 1.0 );
	GLUI_Spinner *spinner_Q_DistA;
	spinner_Q_DistA = new GLUI_Spinner( kindPanel, "distA", &distA);
	spinner_Q_DistA -> set_float_limits(0.1, 1.0);
	spinner_Q_DistA -> set_speed( 1.0 );

	glui->add_separator_to_panel(kindPanel);
	new GLUI_Checkbox(kindPanel, "Vortex", &flagVortex );
	GLUI_Spinner *spinner_Gamma;
	spinner_Gamma = new GLUI_Spinner( kindPanel, "Gamma", &Gamma);
	spinner_Gamma -> set_int_limits(-5.0, 5.0 );
	spinner_Gamma -> set_speed( 1.0 );

	partPanel = new GLUI_Panel(glui, "Particle" );
	numP_edit = new GLUI_EditText(partPanel,"numP0", &numP0);
	numP_edit->set_int_limits( 10, MAX_PARTICLE);
	sizeP_edit = new GLUI_EditText(partPanel,"size", &sizeParticle);
	speedC_edit = new GLUI_EditText(partPanel,"speedC", &speedCoef);
	interval_edit = new GLUI_EditText(partPanel,"interval", &intervalP);
	startBtn = new GLUI_Button(partPanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(partPanel, "Freeze", FREEZE_ID, control_cb);
	resetBtn = new GLUI_Button(partPanel, "Reset", RESET_ID, control_cb);

	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display" );
	GLUI_Spinner *spinner_scale;
	spinner_scale = new GLUI_Spinner( dispPanel, "scale", &rect.scale);
	spinner_scale -> set_float_limits(0.1, 10.0 );
	spinner_scale -> set_speed( 0.5 );
	GLUI_Spinner *spinner_posY;
	spinner_posY = new GLUI_Spinner( dispPanel, "pos0.y", &rect.pos0.y);
	spinner_posY -> set_float_limits(-2.0, 2.0 );
	spinner_posY -> set_speed( 0.2 );
	GLUI_Spinner *spinner_posX;
	spinner_posX = new GLUI_Spinner( dispPanel, "pos0.x", &rect.pos0.x);
	spinner_posX -> set_float_limits(-2.0, 2.0 );
	spinner_posX -> set_speed( 0.2 );

	glui->add_separator_to_panel(dispPanel);
	new GLUI_Checkbox(dispPanel, "potential", &flagPotential);
	new GLUI_Checkbox(dispPanel, "stream", &flagStream);
	GLUI_Spinner *spinner_nLine;
	spinner_nLine = new GLUI_Spinner( dispPanel, "nLine", &nLine);
	spinner_nLine -> set_int_limits(1, 100 );
	spinner_nLine -> set_speed( 0.2 );

	GLUI_Spinner *spinner_range;
	spinner_range = new GLUI_Spinner( dispPanel, "range", &range);
	spinner_range -> set_float_limits(-2.0, 2.0 );
	spinner_range -> set_speed( 0.2 );

	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "velocity", &flagVelocity );
	GLUI_Spinner *spinner_scaleArrow;
	spinner_scaleArrow = new GLUI_Spinner( dispPanel, "scaleArrow", &scaleArrow);
	spinner_scaleArrow -> set_float_limits(0.0, 1.0 );
	spinner_scaleArrow -> set_speed( 0.1 );
	radio = new GLUI_RadioGroup(dispPanel, &arrow);
	new GLUI_RadioButton(radio, "Thin");
	new GLUI_RadioButton(radio, "Thick");
	glui->add_separator_to_panel(dispPanel);
	new GLUI_Checkbox(dispPanel, "grid", &flagGrid );

	paraBtn = new GLUI_Button(dispPanel, "Parameter Show", PARAMETER_ID, control_cb);

	executeBtn = new GLUI_Button(glui, "Execute", EXECUTE_ID, control_cb);
	initBtn = new GLUI_Button(glui, "Initialize", INIT_ID, control_cb);


	new GLUI_StaticText( glui, "" );//空白
	// 終了 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

}


