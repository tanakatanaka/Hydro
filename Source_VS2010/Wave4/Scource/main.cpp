#pragma once
#include "stdafx.h"
#include "Scene/SceneBase.h"
#include"Scene/Wave.h"

//�V�F�[�_�[�ɂ��čl����
int main(int argc, char** argv)
{
	std::unique_ptr<SceneBase> scene = std::make_unique<Wave>();
	//�����N�Ƃ����撣���ĊO�ł��܂�
	//�_�̑��삢���
	scene->Initialize(argc, argv);

	scene->Finalize();
	return 0;
}
