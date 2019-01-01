//OpenGLで利用するキーボード
#pragma once
#include "Wave.h"
//宣言
void Keyboard(unsigned char key, int x, int y);



void Keyboard(unsigned char key, int x, int y)
{
	if ( false == flagStart && false == flagObsControl)
	{
		return;
	}

	float R = rect.size.x / 2.0 - rect.obs_left;//回転半径

	switch (key)
	{
	case 'w':
		if (rigid[0].vPos.x >= rect.size.x / 2.0 - rect.obs_left)
		{
			direction = -1.0;
		}
		
		break;
	case 's':
		if (rigid[0].vPos.x <= -rect.size.x / 2.0 + rect.obs_left)
		{
			direction = 1.0;
		}
		
		break;
	case 'a':
		rigid[0].vVel.x = obsSpeed * rigid[0].vPos.y / R;

		break;
	case 'd':
		rigid[0].vVel.y = -obsSpeed * rigid[0].vPos.x / R;

		break;

	default:
		break;
	}

	rigid[0].vVel.x = direction * obsSpeed;
	rigid[0].vPos += rigid[0].vVel * deltaT;
	CalcLine();
}