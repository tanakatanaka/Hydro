#pragma once

//Scene�̒��ۃN���X
class SceneBase
{
	public:
		virtual int Initialize(int argc, char** argv) = 0;
		virtual void Update() = 0;
		virtual void Finalize() = 0;
};