
#define NUM_MAX_X  210
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
	Vector2D pos0; //���[�J���E�B���h�E�̃f�J���g���W�ɂ������`�̈�̒��S
	Vector2D left0;//��`�̈�̍����ʒu
	Vector2D size; //��`�̈�̃T�C�Y
	float obs_left;//��Q�����[�܂ł̋���
	float obs_thickness; //��Q���̌���
	float obs_width;//��Q���̍���
	int nMeshX;     //������
	int nMeshY;
	Vector2D delta;
};
Rect rect = {
	1.0,//scale
	Vector2D(0.0, 0.0), //pos0
	Vector2D(-0.5, 0.5),//left0(���̒l)
	Vector2D(2.0, 1.0), //size
	0.5, 0.5, 0.5,      //obs_left, thickness, width
	100, 50             //nMeshX, nMeshY
};

int flagExecute = 0; //�X�^�[�g�t���O
int flagPotential = 1;
int flagVelocity = 0;
int flagGrid = 0;
int flagParameter = 1;

int main_window;//GLUT�ō��Window��ID

int iteration = 10000;//�ő�J��Ԃ���
float tolerance = 0.000001;//���e�덷
int nLine = 10;//�����\���{��
float scaleArrow = 0.02;
float widthArrow = 0.0;
int arrow = 0;

//Pointers
GLUI *glui;
GLUI_Panel *shapePanel;
GLUI_EditText *sizeX_edit;
GLUI_EditText *sizeY_edit;
GLUI_EditText *left_edit;
GLUI_EditText *width_edit;
GLUI_EditText *height_edit;
GLUI_EditText *nMeshX_edit;
GLUI_EditText *nMeshY_edit;

GLUI_RadioGroup *radio;

GLUI_Panel *calcPanel;
GLUI_EditText *repeat_edit;
GLUI_EditText *tolerance_edit;

GLUI_Panel *dispPanel;

GLUI_Panel *partPanel;

GLUI_Button *executeBtn;
GLUI_Button *initBtn;
GLUI_Button *paraBtn;

//USER ID
#define INIT_ID 50
#define EXECUTE_ID  100
#define PARTICLE_ID 200
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
	else if(control == PARAMETER_ID) flagParameter = !flagParameter;
}

void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	shapePanel = new GLUI_Panel(glui, "Shape" );
	sizeX_edit = new GLUI_EditText(shapePanel,"size.x", &rect.size.x);
	sizeX_edit->set_float_limits( 0.8, 2.0);
	sizeY_edit = new GLUI_EditText(shapePanel,"size.y", &rect.size.y);
	sizeY_edit->set_float_limits( 0.5, 1.2);
	left_edit = new GLUI_EditText(shapePanel,"obs_left", &rect.obs_left);
	left_edit->set_float_limits( 0.0, 1.5);
	width_edit = new GLUI_EditText(shapePanel,"obs_thickness", &rect.obs_thickness);
	width_edit->set_float_limits( 0.0, 0.8);
	height_edit = new GLUI_EditText(shapePanel,"obs_width", &rect.obs_width);
	height_edit->set_float_limits( 0.0, 0.8);
	nMeshX_edit = new GLUI_EditText(shapePanel,"nMeshX", &rect.nMeshX);
	nMeshX_edit->set_int_limits( 10, 200);
	nMeshY_edit = new GLUI_EditText(shapePanel,"nMeshY", &rect.nMeshY);
	nMeshY_edit->set_int_limits( 10, 100);

	calcPanel = new GLUI_Panel(glui, "Calculation" );
	repeat_edit = new GLUI_EditText(calcPanel,"iteration", &iteration);
	//repeat_edit->set_int_limits( 100, 500);
	tolerance_edit = new GLUI_EditText(calcPanel,"tolerance", &tolerance);
	tolerance_edit -> set_w( 150 );
	executeBtn = new GLUI_Button(calcPanel, "Execute", EXECUTE_ID, control_cb);
	initBtn = new GLUI_Button(calcPanel, "Initialize", INIT_ID, control_cb);
//	new GLUI_StaticText( glui, "" );//��
	paraBtn = new GLUI_Button(glui, "Parameter Show", PARAMETER_ID, control_cb);

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
	glui->add_separator_to_panel(dispPanel);

	new GLUI_Checkbox(dispPanel, "grid", &flagGrid);

	new GLUI_StaticText( glui, "" );//��
	// �I�� 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

}
