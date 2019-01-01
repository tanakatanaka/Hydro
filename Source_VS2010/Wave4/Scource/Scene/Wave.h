#pragma once
#include "../SubGLSL.h"
#include "SceneBase.h"

void ControlObjPos();
void CalcLine();
static float direction = 1.0;//áŠQ•¨’¼üˆÚ“®•ûŒü

class Wave : public SceneBase
{
public:
	int Initialize(int argc, char** argv);
	void Update();
	void Finalize();
private:
	SubGLSL subGlsl;



};