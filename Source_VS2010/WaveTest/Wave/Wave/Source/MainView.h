#pragma once
#include <memory>
#include "Scene/scene.h"
#include "myGlsl.h"
#include "myGLUI.h"

class MainView
{
public:
    void InitializeGL(int argc, char** argv);
    void paintGL();
    void resizeGL(int w, int h);
    void toggleAnimation();
    void takeScreenShot();

private:
	std::unique_ptr<Scene> scene;
	//MyGLUI Myglui;
};

