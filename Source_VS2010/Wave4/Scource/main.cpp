#pragma once
#include "stdafx.h"
#include "Scene/SceneBase.h"
#include"Scene/Wave.h"


int main(int argc, char** argv)
{
	std::unique_ptr<SceneBase> scene = std::make_unique<Wave>();
	//�����N�Ƃ����撣���ĊO�ł��܂�
	scene->Initialize(argc, argv);

	return 0;
}
