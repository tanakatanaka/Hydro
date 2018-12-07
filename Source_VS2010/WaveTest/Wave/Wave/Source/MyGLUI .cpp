#include"MyGLUI.h"

void MyGLUI::Initialize(std::unique_ptr<Assist3D>&& arg_assist3d)
{
	assist3d = std::move(arg_assist3d);
}

void MyGLUI::setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, assist3d->win_x0 + assist3d->win_width + 10, assist3d->win_y0);

	paraPanel = new GLUI_Panel(glui, "Parameters");
	//left_edit = new GLUI_EditText(paraPanel,"obs_left", &rect.obs_left);
	radius_edit = new GLUI_EditText(paraPanel, "obs_radius", &rect.obs_radius);
	radius_edit->set_float_limits(0.0, 10.0);
	//posY_edit = new GLUI_EditText(paraPanel,"obs_posY", &rect.obs_posY);
	length_edit = new GLUI_EditText(paraPanel, "obs_length", &rect.obs_length);
	Re_edit = new GLUI_EditText(paraPanel, "Reynolds", &Re);
	deltaT_edit = new GLUI_EditText(paraPanel, "deltaT", &deltaT);
	obsSpeed_edit = new GLUI_EditText(paraPanel, "obsSpeed", &obsSpeed);

	calcPanel = new GLUI_Panel(glui, "Calculation");
	new GLUI_Checkbox(calcPanel, "Rotation", &flagRotation);
	startBtn = new GLUI_Button(calcPanel, "Start", START_ID, control_cb);
	freezeBtn = new GLUI_Button(calcPanel, "Freeze", FREEZE_ID, control_cb);
	stepBtn = new GLUI_Button(calcPanel, "Step", STEP_ID, control_cb);
	resetBtn = new GLUI_Button(calcPanel, "Reset", RESET_ID, control_cb);
	obsStopBtn = new GLUI_Button(calcPanel, "ObsStop", OBS_STOP_ID, control_cb);

	partPanel = new GLUI_Panel(glui, "Particle");
	new GLUI_Checkbox(partPanel, "show", &flagShowParticle);
	sizeP_edit = new GLUI_EditText(partPanel, "size", &sizeParticle);
	numP_edit = new GLUI_EditText(partPanel, "num", &numParticle);

	//光源
	GLUI_Rollout *light = new GLUI_Rollout(glui, "Light", false);
	lightXP_btn = new GLUI_Button(light, "LightX+", LIGHTX_P_ID, control_cb);
	lightXM_btn = new GLUI_Button(light, "LightX-", LIGHTX_M_ID, control_cb);
	lightYP_btn = new GLUI_Button(light, "LightY+", LIGHTY_P_ID, control_cb);
	lightYM_btn = new GLUI_Button(light, "LightY-", LIGHTY_M_ID, control_cb);
	lightZP_btn = new GLUI_Button(light, "LightZ+", LIGHTZ_P_ID, control_cb);
	lightZM_btn = new GLUI_Button(light, "LightZ-", LIGHTZ_M_ID, control_cb);
	resetLight_btn = new GLUI_Button(light, "ResetLight", LIGHT_RESET_ID, control_cb);


	glui->add_column(false);

	dispPanel = new GLUI_Panel(glui, "Display");
	GLUI_Spinner *spinner_maxOmg;
	spinner_maxOmg = new GLUI_Spinner(dispPanel, "maxOmg", &maxOmg);
	spinner_maxOmg->set_float_limits(0.0, 25.0);
	spinner_maxOmg->set_speed(0.1);

	GLUI_Spinner *spinner_maxPrs;
	spinner_maxPrs = new GLUI_Spinner(dispPanel, "maxPrs", &maxPrs);
	spinner_maxPrs->set_float_limits(0.0, 25.0);
	spinner_maxPrs->set_speed(0.1);
	glui->add_separator_to_panel(dispPanel);

	GLUI_Spinner *spinner_adjustH;
	spinner_adjustH = new GLUI_Spinner(dispPanel, "adjustH", &adjustH);
	spinner_adjustH->set_float_limits(0.0, 10.0);
	spinner_adjustH->set_speed(0.2);

	// 	new GLUI_Checkbox(dispPanel, "OrthProjection", &flagOrtho);

	new GLUI_Checkbox(dispPanel, "Wireframe", &flagWireframe);
	new GLUI_Checkbox(dispPanel, "Parameter", &flagParamShow);
	new GLUI_Checkbox(dispPanel, "CoordShow", &flagCoordShow);
	new GLUI_Checkbox(dispPanel, "ShadowShow", &flagShadowShow);

	wavePanel = new GLUI_Panel(glui, "Wave");

	GLUI_Spinner *spinner_height;
	spinner_height = new GLUI_Spinner(wavePanel, "height", &waveHeight, INIT_LENGTH_ID, control_cb);
	spinner_height->set_speed(0.1);

	GLUI_Spinner *spinner_amp;
	spinner_amp = new GLUI_Spinner(wavePanel, "amp", &waveAmp);
	spinner_amp->set_speed(0.2);

	GLUI_Spinner *spinner_freq;
	spinner_freq = new GLUI_Spinner(wavePanel, "freq", &waveFreq);
	spinner_freq->set_speed(0.2);

	GLUI_Spinner *spinner_speed;
	spinner_speed = new GLUI_Spinner(wavePanel, "speed", &waveSpeed);
	spinner_speed->set_speed(0.2);

	GLUI_Spinner *spinner_drag;
	spinner_drag = new GLUI_Spinner(wavePanel, "drag", &waveDrag);
	spinner_drag->set_speed(0.2);
	spinner_drag->set_float_limits(0.0, 10.0);

	glui->add_separator_to_panel(wavePanel);

	GLUI_Spinner *spinner_nRatio;
	spinner_nRatio = new GLUI_Spinner(wavePanel, "nRatio", &nRatio);
	spinner_nRatio->set_speed(0.1);
	spinner_nRatio->set_float_limits(1.0, 1.5);
	GLUI_Spinner *spinner_transparency;
	spinner_transparency = new GLUI_Spinner(wavePanel, "transparency", &transparency);
	spinner_transparency->set_speed(0.2);
	spinner_transparency->set_float_limits(0.0, 1.0);

	causPanel = new GLUI_Panel(glui, "Caustics");
	GLUI_Spinner *spinner_adjustC;
	spinner_adjustC = new GLUI_Spinner(causPanel, "adjustC", &adjustC);
	spinner_adjustC->set_float_limits(0.0, 10.0);
	spinner_adjustC->set_speed(0.2);

	//new GLUI_StaticText( glui, "" );//空白
	// 終了 
	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);

	//********　　glui2　　****************
	glui2 = GLUI_Master.create_glui("GLUI-CAMERA", 0, win_x0, win_y0 + win_height + 30);
	//カメラ操作
	dollyP_btn = new GLUI_Button(glui2, "Dolly+", DOLLY_P_ID, control_cb);
	dollyP_btn->set_w(60);
	dollyM_btn = new GLUI_Button(glui2, "Dolly-", DOLLY_M_ID, control_cb);
	dollyM_btn->set_w(60);
	glui2->add_column(false);
	panP_btn = new GLUI_Button(glui2, "Pan+", PAN_P_ID, control_cb);
	panP_btn->set_w(60);
	panM_btn = new GLUI_Button(glui2, "Pan-", PAN_M_ID, control_cb);
	panM_btn->set_w(60);
	glui2->add_column(false);
	tiltP_btn = new GLUI_Button(glui2, "Tilt+", TILT_P_ID, control_cb);
	tiltP_btn->set_w(60);
	tiltM_btn = new GLUI_Button(glui2, "Tilt-", TILT_M_ID, control_cb);
	tiltM_btn->set_w(60);
	glui2->add_column(false);
	tumbleP_btn = new GLUI_Button(glui2, "Tumble+", TUMBLE_P_ID, control_cb);
	tumbleP_btn->set_w(60);
	tumbleM_btn = new GLUI_Button(glui2, "Tumble-", TUMBLE_M_ID, control_cb);
	tumbleM_btn->set_w(60);
	glui2->add_column(false);
	craneP_btn = new GLUI_Button(glui2, "Crane+", CRANE_P_ID, control_cb);
	craneP_btn->set_w(60);
	craneP_btn->set_w(60);
	craneM_btn = new GLUI_Button(glui2, "Crane-", CRANE_M_ID, control_cb);
	craneM_btn->set_w(60);
	glui2->add_column(false);
	zoomP_btn = new GLUI_Button(glui2, "Zoom+", ZOOM_P_ID, control_cb);
	zoomP_btn->set_w(60);
	zoomM_btn = new GLUI_Button(glui2, "Zoom-", ZOOM_M_ID, control_cb);
	zoomM_btn->set_w(60);
	glui2->add_column(false);
	resetCamera_btn = new GLUI_Button(glui2, "ResetCamera", CAMERA_RESET_ID, control_cb);

	glui2->set_main_gfx_window(main_window);
}

void MyGLUI::control_cb(int control)
{
	double v1 = 2.0;//光源
	double v2 = 1.0;//カメラ操作

	Assist3D::View &view = assist3d->view;

					//計算
	if (control == START_ID) 
	{
		assist3d->flagStart = 1;
		assist3d->flagFreeze = 0;
		assist3d->flagStep = 0;
	}
	else if (control == FREEZE_ID) 
	{
		if (assist3d->flagFreeze == 0)
		{
			assist3d->flagFreeze = 1;
		}
		else 
		{ 
			assist3d->flagFreeze = 0;
			assist3d->flagStep = 0;
		}
	}
	else if (control == STEP_ID) {
		assist3d->flagStep = 1;
		assist3d->flagFreeze = 0;
	}
	else if (control == RESET_ID)
	{
		assist3d->flagStart = 0;
		assist3d->flagFreeze = 0;
		flagObsStop = 0;
		flagResetting = 0;
		initData();
	}
	else if (control == OBS_STOP_ID)
	{
		flagObsStop = !flagObsStop;
	}
	else if (control == INIT_LENGTH_ID) initObjectLength();
	else if (control == PARAMETER_ID) flagParamShow = !flagParamShow;

	//光源操作
	else if (control == LIGHTX_P_ID) { assist3d->lightPos[0] += v1; }
	else if (control == LIGHTX_M_ID) { assist3d->lightPos[0] -= v1; }
	else if (control == LIGHTY_P_ID) { assist3d->lightPos[1] += v1; }
	else if (control == LIGHTY_M_ID) { assist3d->lightPos[1] -= v1; }
	else if (control == LIGHTZ_P_ID) { assist3d->lightPos[2] += v1; }
	else if (control == LIGHTZ_M_ID) { assist3d->lightPos[2] -= v1; }
	else if (control == LIGHT_RESET_ID)
	{
		for (int i = 0; i < 3; i++)
		{
			assist3d->lightPos[i] = assist3d->lightPos0[i];
		}
	}

	else if (control == SHOW_ID)
	{
		glui2->show();
	}
	else if (control == HIDE_ID)
	{
		glui2->hide();
	}

	//カメラ操作
	else if (control == DOLLY_P_ID || control == DOLLY_M_ID)//dolly
	{
		if (control == DOLLY_P_ID)
		{ //近づく
			if (!assist3d->flagOrtho)
			{
				view.dist -= v2;
			}
			else
			{
				assist3d->ortho_scale -= 0.5;
			}
		}
		else
		{//遠ざかる
			if (!assist3d->flagOrtho)
			{
				view.dist += v2;
			}
			else
			{
				assist3d->ortho_scale += 0.5;
			}
		}
		assist3d->SetCamera();
	}
	else if (control == PAN_P_ID || control == PAN_M_ID)//pan
	{
		if (control == PAN_P_ID)
		{
			view.phi += v2;
		}
		else 
		{
			view.phi -= v2;
		}
		view.vCnt.x = view.vPos.x - view.dist * cos(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
		view.vCnt.y = view.vPos.y - view.dist * sin(DEG_TO_RAD * view.phi) * cos(DEG_TO_RAD * view.theta);
	}
	else if (control == TILT_P_ID || control == TILT_M_ID)//tilt
	{
		if (control == TILT_P_ID)
		{
			view.theta += v2;
		}
		else
		{
			view.theta -= v2;
		}

		view.vCnt.x = view.vPos.x - view.dist * (float)(cos(DEG_TO_RAD * view.theta) * cos(DEG_TO_RAD * view.phi));
		view.vCnt.y = view.vPos.y - view.dist * (float)(cos(DEG_TO_RAD * view.theta) * sin(DEG_TO_RAD * view.phi));
		view.vCnt.z = view.vPos.z - view.dist * (float)sin(DEG_TO_RAD * view.theta);
	}

	else if (control == ZOOM_P_ID || control == ZOOM_M_ID)//zoom
	{
		if (control == ZOOM_P_ID) view.fovY -= v2;//zoom in
		else view.fovY += v2;//zoom out	
		setCamera();
	}
	else if (control == TUMBLE_P_ID || control == TUMBLE_M_ID)//tumble
	{
		if (control == TUMBLE_P_ID) view.phi += v2;
		else                        view.phi -= v2;
		setCamera();
	}
	else if (control == CRANE_P_ID || control == CRANE_M_ID)//crane
	{
		if (control == CRANE_P_ID) view.theta += v2;
		else                       view.theta -= v2;
		setCamera();
	}
	else if (control == CAMERA_RESET_ID) view = view0;
}



