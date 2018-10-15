

void init();
void setGLUI();

//Window�̃T�C�Y,�ʒu
int win_width  = 600;
int win_height = 400;
float win_ratio = (float)win_height / (float)win_width;
int win_x0 = 300;
int win_y0 = 100;

struct Rect
{
	float scale;
	Vector2D pos0; //���[�J���E�B���h�E�̃f�J���g���W�ɂ������`�̈�̒��S
	Vector2D left0;//��`�̈�̍����ʒu
	Vector2D size; //��`�̈�̃T�C�Y
	int nMesh;      //������
	float deltaX;  //�i�q�Ԋu
};
Rect rect = {
	1.0,//scale
	Vector2D(0.0, -0.1),//pos0
	Vector2D(-0.5, 0.5),//left0(���̒l)
	Vector2D(1.0, 0.4), //size
	100//nMeshX
};

int flagStart = 0; //�X�^�[�g�t���O
int flagReset = 1;
int flagFreeze = 0;//�ꎞ��~
int flagTime = 0;  //���n��f�[�^�\���t���O
int flagParameter = 1;
int flagConstant = 0;

int main_window;//GLUT�ō��Window��ID

//������
int nMesh0 = 100;
float widthInit = 0.1;//�p���X�g�`�̕�
float centerInit = 0.05;//���̒��S�_

float diff_coef = 0.001;
float deltaT0 = 0.01;
int thinningN = 1;//�Ԉ�����
int method = 0;//�z��@
int boundary = 0;//Dirichlet
float interval = 2.0;//���n��f�[�^�̊Ԋu
//Pointers
GLUI *glui;
GLUI_Panel *animePanel;
GLUI_Panel *paraPanel;
GLUI_EditText *nMesh_edit;
GLUI_EditText *diffusion_edit;
GLUI_EditText *widthInit_edit;
GLUI_EditText *centerInit_edit;
GLUI_EditText *thinning_edit;
GLUI_EditText *deltaT_edit;
GLUI_EditText *interval_edit;

GLUI_Button *startBtn;
GLUI_Button *freezeBtn;
GLUI_Button *resetBtn;
GLUI_Button *timeBtn;
GLUI_Button *paraBtn;
GLUI_Panel *methodPanel;
GLUI_Panel *dispPanel;
GLUI_RadioGroup *radioM;
GLUI_Panel *boundaryPanel;
GLUI_RadioGroup *radioB;

//USER ID
#define START_ID  100
#define FREEZE_ID  110
#define RESET_ID 120
#define TIME_ID 200
#define PARAMETER_ID 210

void control_cb(int control)
{
	//�v�Z���s
	if(control == START_ID) {
		flagStart = 1;
		flagFreeze = 0;
		//flagTime = 0;
		init();
	}
	else if(control == FREEZE_ID) {
		flagFreeze = !flagFreeze;
	}
	else if(control == RESET_ID) {
		flagStart = 0;
		flagReset = 1;
		flagFreeze = 0;
		flagTime = 0;
		init();
	}
	else if(control == TIME_ID) {
		flagTime = 1;
		flagStart = 0;
		flagFreeze = 0;
	}
	else if(control == PARAMETER_ID) {
		flagParameter = !flagParameter;
	}

}

void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, win_x0 + win_width + 10, win_y0);

	paraPanel = new GLUI_Panel(glui, "Parameter" );
	nMesh_edit = new GLUI_EditText(paraPanel,"nMesh", &rect.nMesh);
	nMesh_edit -> set_int_limits( 50, 500);
	widthInit_edit = new GLUI_EditText(paraPanel,"widthInit", &widthInit);
	widthInit_edit -> set_float_limits( 0.01, 0.5);
	centerInit_edit = new GLUI_EditText(paraPanel,"centerInit", &centerInit);
	diffusion_edit = new GLUI_EditText(paraPanel,"diff_coef", &diff_coef);
	deltaT_edit = new GLUI_EditText(paraPanel,"deltaT0[s]", &deltaT0);
	thinning_edit = new GLUI_EditText(paraPanel,"thinningN", &thinningN);
	thinning_edit->set_int_limits( 1, 100 );
	thinning_edit ->set_w(80);
	new GLUI_Checkbox(glui, "Constant", &flagConstant );

	animePanel = new GLUI_Panel(glui, "Animation" );
	startBtn = new GLUI_Button(animePanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(animePanel, "Freeze", FREEZE_ID, control_cb);
	resetBtn = new GLUI_Button(animePanel, "Reset", RESET_ID, control_cb);
	new GLUI_StaticText( animePanel, "" );//��
	timeBtn = new GLUI_Button(animePanel, "Time Series", TIME_ID, control_cb);
	interval_edit = new GLUI_EditText(animePanel,"interval[s]", &interval);
	interval_edit ->set_w(80);
//	new GLUI_StaticText( glui, "" );//��

	glui->add_column(true);

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

	methodPanel = new GLUI_Panel(glui, "Method" );
	radioM = new GLUI_RadioGroup(methodPanel, &method, 2);
	new GLUI_RadioButton(radioM, "explicit");
	new GLUI_RadioButton(radioM, "implicit");
	
	boundaryPanel = new GLUI_Panel(glui, "Boundary Condition" );
	radioB = new GLUI_RadioGroup(boundaryPanel, &boundary, 2);
	new GLUI_RadioButton(radioB, "Dirichlet");
	new GLUI_RadioButton(radioB, "Neumann");

	new GLUI_StaticText( glui, "" );//��
	paraBtn = new GLUI_Button(glui, "Parameters Show", PARAMETER_ID, control_cb);

	new GLUI_StaticText( glui, "" );//��
	// �I�� 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

}
