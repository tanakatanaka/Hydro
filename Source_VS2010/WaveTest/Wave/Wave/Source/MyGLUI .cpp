#include"MyGLUI.h"
#include"../../../../rigid.h"
#include"Scene/CausticsScene/CausticsScene.h"
//同ファイル内でのみ使う方法
extern std::unique_ptr<Assist3D> _assist3d;
extern std::unique_ptr<GLUI> _glui2;
extern std::unique_ptr<MyGLUI::Rect> _rect;

static void setTextureVelX();


void MyGLUI::Initialize()
{
	int a = 0;
	_assist3d = std::make_unique<Assist3D>(assist3d);
	_glui2 = std::make_unique<GLUI>(glui2);
}

void MyGLUI::setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI-MAIN", 0, assist3d.win_x0 + assist3d.win_width + 10, assist3d.win_y0);

	paraPanel = new GLUI_Panel(glui, "Parameters");
	//left_edit = new GLUI_EditText(paraPanel,"obs_left", &_rect->obs_left);
	radius_edit = new GLUI_EditText(paraPanel, "obs_radius", &_rect->obs_radius);
	radius_edit->set_float_limits(0.0, 10.0);
	//posY_edit = new GLUI_EditText(paraPanel,"obs_posY", &_rect->obs_posY);
	length_edit = new GLUI_EditText(paraPanel, "obs_length", &_rect->obs_length);
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
	glui2 = GLUI_Master.create_glui("GLUI-CAMERA", 0, assist3d.win_x0, assist3d.win_y0 + assist3d.win_height + 30);
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

void control_cb(int control)
{
	double v1 = 2.0;//光源
	double v2 = 1.0;//カメラ操作

	Assist3D::View &view = _assist3d->view;

					//計算
	if (control == START_ID) 
	{
		_assist3d->flagStart = 1;
		_assist3d->flagFreeze = 0;
		_assist3d->flagStep = 0;
	}
	else if (control == FREEZE_ID) 
	{
		if (_assist3d->flagFreeze == 0)
		{
			_assist3d->flagFreeze = 1;
		}
		else 
		{ 
			_assist3d->flagFreeze = 0;
			_assist3d->flagStep = 0;
		}
	}
	else if (control == STEP_ID) 
	{
		_assist3d->flagStep = 1;
		_assist3d->flagFreeze = 0;
	}
	else if (control == RESET_ID)
	{
		_assist3d->flagStart = 0;
		_assist3d->flagFreeze = 0;
		flagObsStop = 0;
		
		initData();
	}
	else if (control == OBS_STOP_ID)
	{
		flagObsStop = !flagObsStop;
	}
	else if (control == INIT_LENGTH_ID)
	{
		initObjectLength();
	}
	else if (control == PARAMETER_ID)
	{
		flagParamShow = !flagParamShow;
	}

	//光源操作
	else if (control == LIGHTX_P_ID) { _assist3d->lightPos[0] += v1; }
	else if (control == LIGHTX_M_ID) { _assist3d->lightPos[0] -= v1; }
	else if (control == LIGHTY_P_ID) { _assist3d->lightPos[1] += v1; }
	else if (control == LIGHTY_M_ID) { _assist3d->lightPos[1] -= v1; }
	else if (control == LIGHTZ_P_ID) { _assist3d->lightPos[2] += v1; }
	else if (control == LIGHTZ_M_ID) { _assist3d->lightPos[2] -= v1; }
	else if (control == LIGHT_RESET_ID)
	{
		for (int i = 0; i < 3; i++)
		{
			_assist3d->lightPos[i] = _assist3d->lightPos0[i];
		}
	}

	else if (control == SHOW_ID)
	{
		_glui2->show();
	}
	else if (control == HIDE_ID)
	{
		_glui2->hide();
	}

	//カメラ操作
	else if (control == DOLLY_P_ID || control == DOLLY_M_ID)//dolly
	{
		if (control == DOLLY_P_ID)
		{ //近づく
			if (!_assist3d->flagOrtho)
			{
				view.dist -= v2;
			}
			else
			{
				_assist3d->ortho_scale -= 0.5;
			}
		}
		else
		{//遠ざかる
			if (!_assist3d->flagOrtho)
			{
				view.dist += v2;
			}
			else
			{
				_assist3d->ortho_scale += 0.5;
			}
		}
		_assist3d->SetCamera();
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
		if (control == ZOOM_P_ID)
		{
			view.fovY -= v2;//zoom in
		}
		else
		{
			view.fovY += v2;//zoom out	
		}
		_assist3d->SetCamera();
	}
	else if (control == TUMBLE_P_ID || control == TUMBLE_M_ID)//tumble
	{
		if (control == TUMBLE_P_ID)
		{
			view.phi += v2;
		}
		else
		{
			view.phi -= v2;
		}
		_assist3d->SetCamera();
	}
	else if (control == CRANE_P_ID || control == CRANE_M_ID)//crane
	{
		if (control == CRANE_P_ID)
		{
			view.theta += v2;
		}
		else
		{
			view.theta -= v2;
		}
		_assist3d->SetCamera();
	}
	else if (control == CAMERA_RESET_ID) view = _assist3d->view0;
}

void initData()
{
	NX = _rect->meshX;
	NY = _rect->meshY;

	DX = _rect->size.x / (float)NX;
	DY = _rect->size.y / (float)NY;

	//格子点のタイプ
	int i, j;
	float eps = DX / 2.0;
	float x, y, x1, x2, y1, y2, x0, y0, r, r0;
	x0 = _rect->obs_left;//円柱の中心
	y0 = _rect->size.y / 2.0;
	r0 = _rect->obs_radius;
	for (i = 0; i <= NX; i++)
		for (j = 0; j <= NY; j++)
		{

			g_type[i][j] = INSIDE;//内点
			if (j == 0) g_type[i][j] = BOTTOM;//下側(-y)
			if (j == NY) g_type[i][j] = TOP;  //上側(+y)
			if (i == 0) g_type[i][j] = LEFT;  //左端(-x)
			if (i == NX) g_type[i][j] = RIGHT;//右端(+x)
			x = (float)i * DX;
			y = (float)j * DY;
			if (y < y0 - r0 - DY) continue;
			if (y > y0 + r0 + DY) continue;
			if (x < x0 - r0 - DX) continue;
			if (x > x0 + r0 + DX) continue;

			r = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
			if (r > r0 + eps) continue;
			if (r <= r0) g_type[i][j] = OBSTACLE;//ここでは境界を含む

			if (fabs(y - y0 - r0) < eps) g_type[i][j] = OBS_TOP;
			else if (fabs(y - y0 + r0) < eps) g_type[i][j] = OBS_BOTTOM;
			else if (fabs(x - x0 + r0) < eps) g_type[i][j] = OBS_LEFT;
			else if (fabs(x - x0 - r0) < eps) g_type[i][j] = OBS_RIGHT;
			else
			{
				x1 = x0 - sqrt(r0 * r0 - (y - y0) * (y - y0));//左側交点
				x2 = x0 + sqrt(r0 * r0 - (y - y0) * (y - y0));//右側交点
				y1 = y0 - sqrt(r0 * r0 - (x - x0) * (x - x0));//下側交点
				y2 = y0 + sqrt(r0 * r0 - (x - x0) * (x - x0));//上側交点
				if (i == int(x1 / DX + 0.5) && y > y0) g_type[i][j] = OBS_UL;//左上
				if (j == int(y2 / DY + 0.5) && x < x0) g_type[i][j] = OBS_UL;//左上
				if (i == int(x1 / DX + 0.5) && y < y0) g_type[i][j] = OBS_LL;//左下
				if (j == int(y1 / DY + 0.5) && x < x0) g_type[i][j] = OBS_LL;//左下
				if (i == int(x2 / DX + 0.5) && y > y0) g_type[i][j] = OBS_UR;//右上
				if (j == int(y2 / DY + 0.5) && x > x0) g_type[i][j] = OBS_UR;//右上
				if (i == int(x2 / DX + 0.5) && y < y0) g_type[i][j] = OBS_LR;//右下
				if (j == int(y1 / DY + 0.5) && x > x0) g_type[i][j] = OBS_LR;//右下
			}
		}

	//物理量テクセルサイズ
	texWidth = NX + 1;//座標テクスチャの横サイズ
	texHeight = NY + 1;//座標テクスチャの縦サイズ

					   //物理量配列
	velX = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//X速度その微分および圧力(A成分)
	velY = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//Y速度とその微分(A成分は表示用渦度)
	wave = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//水面のｚ軸方向速度と変位
	caus = (GLubyte*)malloc(3 * texWidth * texHeight * sizeof(GLubyte));//caustics

																		//initialize
	int k;
	for (j = 0; j < texHeight; j++)
		for (i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;

			//速度X
			velX[4 * k + 0] = 0.0;
			velX[4 * k + 1] = 0.0;//x微分
			velX[4 * k + 2] = 0.0;//y微分
			velX[4 * k + 3] = 0.0;//圧力
								  //速度Y
			velY[4 * k + 0] = 0.0;
			velY[4 * k + 1] = 0.0;//x微分
			velY[4 * k + 2] = 0.0;//y微分
			velY[4 * k + 3] = 0.0;//表示用渦度（GPU側だけで利用)
								  //波
			wave[4 * k + 0] = 0.0;//波のｚ軸方向速度
			wave[4 * k + 1] = 0.0;//水面の変位
			wave[4 * k + 2] = 0.0;//渦＋波
			wave[4 * k + 3] = 0.0;//未使用
								  //caustics
			caus[3 * k + 0] = 127.0;//R
			caus[3 * k + 1] = 127.0;//G
			caus[3 * k + 2] = 127.0;//B
		}

	setTextureVelX();
	setFramebufferVelX();
	setTextureVelY();
	setFramebufferVelY();
	setTextureWave();
	setFramebufferWave();
	setFramebufferCube();
	setCubeMap();
	setTextureCaus();

	numRigid = 11;
	for (i = 0; i < numRigid; i++)
	{
		rigid[i] = CRigid();
	}
	//障害物（円柱，球）
	rigid[0].kind = CYLINDER;
	rigid[0].color = RED;
	rigid[0].vPos = Vector3D(-_rect->size.x / 2.0 + _rect->obs_left, -_rect->size.y / 2.0 + _rect->obs_posY, waveHeight + _rect->obs_length / 4.0);
	rigid[0].vSize = Vector3D(_rect->obs_radius*2.0, _rect->obs_radius*2.0, _rect->obs_length / 2.0);

	//下半分(水面下の障害物）
	rigid[1].kind = CYLINDER;
	rigid[1].color = BLUE;
	rigid[1].vPos = Vector3D(-_rect->size.x / 2.0 + _rect->obs_left, -_rect->size.y / 2.0 + _rect->obs_posY, waveHeight - _rect->obs_length / 4.0);
	rigid[1].vSize = Vector3D(_rect->obs_radius*2.0, _rect->obs_radius*2.0, _rect->obs_length / 2.0);
	makeLowerTank();
	makeUpperTank();

	elapseTime1 = 0.0;//1sec間以内の経過時間
	elapseTime2 = 0.0;//start後の総経過時間
	elapseTime3 = 0.0;//start後の総経過時間(無次元)

					  //粒子
	particle = (float*)malloc(4 * texWidth * texHeight * sizeof(float));//粒子の位置

	for (j = 0; j < texHeight; j++)
		for (i = 0; i < texWidth; i++)
		{
			k = i + j * texWidth;
			if (k > numParticle) break;
			particle[4 * k + 0] = getRandom(-_rect->size.x / 2.0, _rect->size.x / 2.0);
			particle[4 * k + 1] = getRandom(-_rect->size.y / 2.0, _rect->size.y / 2.0);
			particle[4 * k + 2] = 0.0;
			particle[4 * k + 3] = 0.0;//未使用
		}
	setTextureParticle();
	setFramebufferParticle();

}

void initObjectLength()
{
	//GLUIウィンドウで水面の高さを変えたとき障害物のｚ軸方向長さを変える
	rigid[0].vPos.z = waveHeight + rect.obs_length / 4.0;
	rigid[1].vPos.z = waveHeight - rect.obs_length / 4.0;
	makeLowerTank();
	makeUpperTank();
}

void setTextureVelX()
{
	glBindTexture(target, texID[0]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, velX);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureVelY()
{
	glBindTexture(target, texID[1]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, velY);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setTextureWave()
{
	glBindTexture(target, texID[2]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, type, wave);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferVelX()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[0]);
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[0], 0);
	//framebuffer object の無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
void setFramebufferVelY()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[1]);
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[1], 0);
	//framebuffer object の無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
void setFramebufferWave()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[2]);
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[2], 0);
	//framebuffer object の無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setTextureParticle()
{
	glBindTexture(target, texID[3]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(target, 0, internalFormat, texWidth, texHeight, 0, format, GL_FLOAT, particle);
	//テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(target, 0);
}
void setFramebufferParticle()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[3]);
	//textureをframebuffer objectに結びつける
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texID[3], 0);
	//framebuffer object の無効化
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setFramebufferCube()
{
	//renderbuffer
	glGenRenderbuffersEXT(1, &rbo);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rbo);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, CUBE_WIDTH, CUBE_HEIGHT);
	//framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo[4]);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbo);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setCubeMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[4]);
	for (int i = 0; i < 6; i++)
		glTexImage2D(c_target[i].name, 0, GL_RGB, CUBE_WIDTH, CUBE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void setTextureCaus()
{
	glBindTexture(target, texID[5]);
	//ピクセル格納モード
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//テクスチャの割り当て 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, caus);//texCaus);
																								   //テクスチャを拡大・縮小する方法の指定 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(target, 0);
}

