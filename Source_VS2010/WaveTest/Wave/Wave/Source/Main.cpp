// WaveMain.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "../Source/stdafx.h"
#include <windows.h>
#include"MainView.h"


int main(int argc, char** argv)
{
	MainView mainView;
	mainView.InitializeGL(argc, argv);


    return 0;
}