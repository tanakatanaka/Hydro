
#define NUM_MAX_X  210//�ő啪����
#define NUM_MAX_Y  210

//��͗̈��`�\����
struct Rect
{
	Vector2D size; //��`�̈�̃T�C�Y(�Œ�j
	float obs_left;//���[�����Q�����S�܂ł̋���
	float obs_thick; //��Q���̌���
	float obs_width; //��Q���̕�
	float obs_posY;//��Q���̍���
	int meshX;     //������(X)
	int meshY;     //������(Y)
};
Rect rect = {
	Vector2D(20.0, 20.0), //size
	3.0, 0.8, 0.8, 10.0,  //obs_left, thick, width, height
	200,//meshX
	200 //meshY
};

//void init();
void initData();
void setGLUI();

int flagWireframe = 0;
int flagGrid = 0;
int flagParamShow = 1;
int flagIdeal = 1;
int flagShowParticle = 1;
int flagCoordShow = 0;
int flagObsStop = 0;
int flagRotation = 0;//��]�^��

int main_window;//GLUT�ō��Window��ID
float deltaT = 0.01;
float Re = 500.0;//���C�m���Y��
float obsSpeed = 1.0;
float maxPrs = 5.0;
float maxOmg =  10.0;
float adjustH = 0.5;//�\����̍�������
int dispSpeed = 1;//�\���X�s�[�h�i�Ԉ����\���j
int sizeParticle = 3;
int numParticle = 1000;

//Pointers
GLUI *glui, *glui2;
GLUI_Panel *paraPanel;
GLUI_EditText *sizeX_edit;
GLUI_EditText *sizeY_edit;
GLUI_EditText *left_edit;
GLUI_EditText *thick_edit;
GLUI_EditText *width_edit;
GLUI_EditText *height_edit;
GLUI_EditText *meshX_edit;
GLUI_EditText *meshY_edit;
GLUI_EditText *obsSpeed_edit;
GLUI_RadioGroup *radio;

GLUI_Panel *calcPanel;
GLUI_EditText *repeat_edit;
GLUI_EditText *tolerance_edit;
GLUI_EditText *deltaT_edit;
GLUI_EditText *Re_edit;

GLUI_Panel *dispPanel;
GLUI_Button *startBtn;
GLUI_Button *resetBtn;
GLUI_Button *stepBtn;
GLUI_Button *freezeBtn;
GLUI_Button *obsStopBtn;
GLUI_Button *paraBtn;
GLUI_RadioGroup *radioC;
GLUI_RadioGroup *radioK;

GLUI_Panel *partPanel;
GLUI_EditText *sizeP_edit;
GLUI_EditText *numP_edit;

//����
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
//�J��������
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
#define OBS_STOP_ID 160
//�����ƃJ����
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
	double v1 = 10.0;//����
	double v2 = 1.0;//�J��������

	//�v�Z
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
		flagObsStop = 0;
		initData();
	}
	else if(control == OBS_STOP_ID)
	{
		flagObsStop = !flagObsStop;
	}
	else if(control == PARAMETER_ID) flagParamShow = !flagParamShow;

	//��������
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

	//�J��������
	else if(control == DOLLY_P_ID || control == DOLLY_M_ID)//dolly
	{
		if(control == DOLLY_P_ID){ //�߂Â�
			if(!flagOrtho) view.dist -= v2; 
			else ortho_scale -= 0.5;
		}
		else  {//��������
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

	paraPanel = new GLUI_Panel(glui, "Parameters" );
	left_edit = new GLUI_EditText(paraPanel,"obs_left", &rect.obs_left);
	thick_edit = new GLUI_EditText(paraPanel,"obs_thick", &rect.obs_thick);
	thick_edit->set_float_limits( 0.0, 10.0);
	width_edit = new GLUI_EditText(paraPanel,"obs_width", &rect.obs_width);
	width_edit->set_float_limits( 0.0, 8.0);
	height_edit = new GLUI_EditText(paraPanel,"obs_posY", &rect.obs_posY);
/*
	meshX_edit = new GLUI_EditText(paraPanel,"meshX", &rect.meshX);
	meshX_edit->set_int_limits( 50, 200);
	meshY_edit = new GLUI_EditText(paraPanel,"meshY", &rect.meshY);
	meshY_edit->set_int_limits( 20, 200);*/
	Re_edit = new GLUI_EditText(paraPanel,"Reynolds", &Re);
	deltaT_edit = new GLUI_EditText(paraPanel,"deltaT", &deltaT);
//	deltaT_edit ->set_w(120);
	obsSpeed_edit = new GLUI_EditText(paraPanel,"obsSpeed", &obsSpeed);

	calcPanel = new GLUI_Panel(glui, "Calculation" );
 //	new GLUI_Checkbox(calcPanel, "ideal at wall", &flagIdeal);
//	new GLUI_StaticText( calcPanel, "" );//��
 	new GLUI_Checkbox(calcPanel, "Rotation", &flagRotation);
	startBtn = new GLUI_Button(calcPanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(calcPanel, "Freeze", FREEZE_ID, control_cb);
	stepBtn = new GLUI_Button(calcPanel, "Step", STEP_ID, control_cb);
	resetBtn = new GLUI_Button(calcPanel, "Reset", RESET_ID, control_cb);
	obsStopBtn = new GLUI_Button(calcPanel, "ObsStop", OBS_STOP_ID, control_cb);

	//����
	GLUI_Rollout *light = new GLUI_Rollout(glui, "Light", false );
	lightXP_btn = new GLUI_Button(light, "LightX+", LIGHTX_P_ID, control_cb);
	lightXM_btn = new GLUI_Button(light, "LightX-", LIGHTX_M_ID, control_cb);
	lightYP_btn = new GLUI_Button(light, "LightY+", LIGHTY_P_ID, control_cb);
	lightYM_btn = new GLUI_Button(light, "LightY-", LIGHTY_M_ID, control_cb);
	lightZP_btn = new GLUI_Button(light, "LightZ+", LIGHTZ_P_ID, control_cb);
	lightZM_btn = new GLUI_Button(light, "LightZ-", LIGHTZ_M_ID, control_cb);
	resetLight_btn = new GLUI_Button(light, "ResetLight", LIGHT_RESET_ID, control_cb);

	new GLUI_StaticText( glui, "" );//��
	// �I�� 
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display" );

	glui->add_separator_to_panel(dispPanel);
	GLUI_Spinner *spinner_maxOmg;
	spinner_maxOmg = new GLUI_Spinner( dispPanel, "maxOmg", &maxOmg);
	spinner_maxOmg->set_float_limits( 0.0, 25.0 );
	spinner_maxOmg->set_speed( 0.1 );

	GLUI_Spinner *spinner_maxPrs;
	spinner_maxPrs = new GLUI_Spinner( dispPanel, "maxPrs", &maxPrs);
	spinner_maxPrs->set_float_limits( 0.0, 25.0 );
	spinner_maxPrs->set_speed( 0.1 );
	glui->add_separator_to_panel(dispPanel);

	GLUI_Spinner *spinner_adjustH;
	spinner_adjustH = new GLUI_Spinner( dispPanel, "adjustH", &adjustH);
	spinner_adjustH -> set_float_limits( 0.0, 50.0 );
	spinner_adjustH -> set_speed( 0.01 );

 	new GLUI_Checkbox(dispPanel, "OrthProjection", &flagOrtho);

	new GLUI_Checkbox(dispPanel, "Wireframe", &flagWireframe);
 	new GLUI_Checkbox(dispPanel, "Parameter", &flagParamShow);
 	new GLUI_Checkbox(dispPanel, "CoordShow", &flagCoordShow);

	partPanel = new GLUI_Panel(glui, "Particle" );
	new GLUI_Checkbox(partPanel, "show", &flagShowParticle);
	sizeP_edit = new GLUI_EditText(partPanel,"size", &sizeParticle);
	numP_edit = new GLUI_EditText(partPanel,"num", &numParticle);
	new GLUI_StaticText( glui, "" );//��

	glui->set_main_gfx_window( main_window );

	//********�@�@glui2�@�@****************
	glui2 = GLUI_Master.create_glui("GLUI-CAMERA", 0, win_x0, win_y0 + win_height + 30);
	//�J��������
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

