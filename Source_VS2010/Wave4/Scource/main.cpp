#pragma once
#include "stdafx.h"
#include "Scene/SceneBase.h"
#include"Scene/Wave.h"

//シェーダーについて考えろ
int main(int argc, char** argv)
{
	std::unique_ptr<SceneBase> scene = std::make_unique<Wave>();
	//リンクとかを頑張って外ですます
	//棒の操作いれる
	scene->Initialize(argc, argv);

	scene->Finalize();
	return 0;
}
