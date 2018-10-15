
#define NUM_MAX_X 210
#define NUM_MAX_Y 110

void init();
void setupGLUI();

//Window�̃T�C�Y,�ʒu
int win_width  = 800;
int win_height = 500;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 300;
int win_y0 = 100;

struct Rect
{
	float scale;
	Vector2D pos0;//���[�J���E�B���h�E�̃f�J���g���W�ɂ������`�̈�̒��S
	Vector2D left0;//��`�̈�̍����ʒu
	Vector2D size;//��`�̈�̃T�C�Y
	int nMeshX;    //������
	int nMeshY;
	Vector2D delta;
};
Rect rect = {
	1.0,//scale
	Vector2D(0.0, 0.0),//pos0
	Vector2D(-0.5, 0.5),//left0(���̒l)
	Vector2D(2.0, 1.0),//size
	100, 50            //nMeshX, nMeshY
};

int flagExecute = 0; //�X�^�[�g�t���O
int flagPotential = 1;
int flagStream = 1;
int flagVelocity = 0;
int flagGrid = 0;
int flagParameter = 1;
int flagStart = 0; 
int flagMouse = 0;
int flagFreeze = 0;

int main_window;//GLUT�ō��Window��ID

int iteration = 5000;//�ő�J��Ԃ���
float tolerance = 0.000001;//���e�덷
int nLine = 10;//�����\���{��
float scaleArrow = 0.02;
float widthArrow = 0.0;
int arrow = 0;
int sizeParticle = 5;
float speedCoef = 0.1;//���x�{��
float intervalP = 0.1;
int thinningV = 2;//���x�x�N�g���\���Ԋu
float Q_value = 100.0;//���o��
float Radius = 0.05;    //�\�[�X�̔��a

//Pointers
GLUI *glui;
GLUI_Panel *shapePanel;
GLUI_EditText *sizeX_edit;
GLUI_EditText *sizeY_edit;
GLUI_EditText *nMeshX_edit;
GLUI_EditText *nMeshY_edit;
GLUI_EditText *source_edit;
GLUI_EditText *radius_edit;

GLUI_RadioGroup *radio;

GLUI_Panel *calcPanel;
GLUI_EditText *repeat_edit;
GLUI_EditText *tolerance_edit;

GLUI_Panel *dispPanel;

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
#define INIT_ID 50
#define EXECUTE_ID  100
#define START_ID  110
#define FREEZE_ID 120
#define RESET_ID 130
//#define PARTICLE_ID 200
#define PARAMETER_ID 300

void control_cb(int control)
{

	//�v�Z���s
	if(control == EXECUTE_ID) {
		flagExecute = 1;
		init();
	}
	else if(control == INIT_ID)
	{
		flagExecute = 0;
		init();
	}
	//���摀��
	if(control == START_ID) {
		flagStart = 1;
		flagMouse = 0;
		flagExecute = 0;
		flagFreeze = 0;
	}
	else if(control == FREEZE_ID) {
		if(flagFreeze == 0) flagFreeze = 1;
		else {flagFreeze = 0; }
	}
	else if(control == RESET_ID)
	{
		flagStart = 0;
		flagFreeze= 0;
	}

	else if(control == PARAMETER_ID) flagParameter = !flagParameter;
}

void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	shapePanel = new GLUI_Panel(glui, "Shape & Source" );
	sizeX_edit = new GLUI_EditText(shapePanel,"size.x", &rect.size.x);
	sizeX_edit->set_float_limits( 0.8, 2.0);
	sizeY_edit = new GLUI_EditText(shapePanel,"size.y", &rect.size.y);
	sizeY_edit->set_float_limits( 0.5, 1.2);
	nMeshX_edit = new GLUI_EditText(shapePanel,"nMeshX", &rect.nMeshX);
	nMeshX_edit->set_int_limits( 10, 200);
	nMeshY_edit = new GLUI_EditText(shapePanel,"nMeshY", &rect.nMeshY);
	nMeshY_edit->set_int_limits( 10, 100);

	glui->add_separator_to_panel(shapePanel);

	source_edit = new GLUI_EditText(shapePanel,"Q_value", &Q_value);
	radius_edit = new GLUI_EditText(shapePanel,"Radius", &Radius);

	calcPanel = new GLUI_Panel(glui, "Calculation" );
	repeat_edit = new GLUI_EditText(calcPanel,"iteration", &iteration);
	//repeat_edit->set_int_limits( 100, 500);
	tolerance_edit = new GLUI_EditText(calcPanel,"tolerance", &tolerance);
	tolerance_edit -> set_w( 150 );
	executeBtn = new GLUI_Button(calcPanel, "Execute", EXECUTE_ID, control_cb);
	initBtn = new GLUI_Button(calcPanel, "Initialize", INIT_ID, control_cb);
//	new GLUI_StaticText( glui, "" );//��
	paraBtn = new GLUI_Button(glui, "Parameter Show", PARAMETER_ID, control_cb);
	new GLUI_StaticText( glui, "" );//��
	// �I�� 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display" );
	GLUI_Spinner *spinner_scale;
	spinner_scale = new GLUI_Spinner( dispPanel, "scale", &rect.scale);
	spinner_scale->set_float_limits(0.5, 1.5 );
	spinner_scale->set_speed( 0.1 );
	GLUI_Spinner *spinner_posY;
	spinner_posY = new GLUI_Spinner( dispPanel, "pos0.y", &rect.pos0.y);
	spinner_posY->set_float_limits(-2.0, 2.0 );
	spinner_posY->set_speed( 0.2 );
	GLUI_Spinner *spinner_posX;
	spinner_posX = new GLUI_Spinner( dispPanel, "pos0.x", &rect.pos0.x);
	spinner_posX->set_float_limits(-2.0, 2.0 );
	spinner_posX->set_speed( 0.2 );

	glui->add_separator_to_panel(dispPanel);
	new GLUI_Checkbox(dispPanel, "potential", &flagPotential);
	new GLUI_Checkbox(dispPanel, "stream", &flagStream);
	GLUI_Spinner *spinner_nLine;
	spinner_nLine = new GLUI_Spinner( dispPanel, "nLine", &nLine);
	spinner_nLine -> set_speed( 0.2 );

	new GLUI_Checkbox(dispPanel, "velocity", &flagVelocity);
	GLUI_Spinner *spinner_scaleArrow;
	spinner_scaleArrow = new GLUI_Spinner( dispPanel, "scaleArrow", &scaleArrow);
	spinner_scaleArrow->set_float_limits(0.0, 1.0 );
	spinner_scaleArrow->set_speed( 0.1 );
	spinner_scaleArrow->set_alignment( GLUI_ALIGN_RIGHT );
	radio = new GLUI_RadioGroup(dispPanel, &arrow);
	new GLUI_RadioButton(radio, "Thin");
	//glui->add_column_to_panel(dispPanel, false);
	new GLUI_RadioButton(radio, "Thick");
	GLUI_Spinner *spinner_thinningV;
	spinner_thinningV = new GLUI_Spinner( dispPanel, "thinningV", &thinningV);
	spinner_thinningV->set_int_limits(1, 5 );
	spinner_thinningV->set_speed( 0.1 );

	glui->add_separator_to_panel(dispPanel);


	new GLUI_Checkbox(dispPanel, "grid", &flagGrid);
	partPanel = new GLUI_Panel(glui, "Particle" );
	numP_edit = new GLUI_EditText(partPanel,"numP0", &numP0);
	numP_edit->set_int_limits( 10, MAX_PARTICLE);
	sizeP_edit = new GLUI_EditText(partPanel,"size", &sizeParticle);
	speedC_edit = new GLUI_EditText(partPanel,"speedC", &speedCoef);
	interval_edit = new GLUI_EditText(partPanel,"interval", &intervalP);
	startBtn = new GLUI_Button(partPanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(partPanel, "Freeze", FREEZE_ID, control_cb);
	resetBtn = new GLUI_Button(partPanel, "Reset", RESET_ID, control_cb);

}
