#pragma once
#include "../SubGLSL.h"
#include "SceneBase.h"

class Wave : public SceneBase
{
public:
	int Initialize(int argc, char** argv);
	void Update();
	void Finalize();
private:
	SubGLSL subGlsl;



};