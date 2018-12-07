#pragma once
#include "../scene.h"

class CausticsScene : public Scene
{
	public:
		CausticsScene();
		void InitScene();
		void Idle();
		void Display();

		void Update(float t);
		void Render();
		void Resize(int, int);

	
	private:
		void Rendering();
		void SetTextureVelX();
		void SetFramebufferVelX();
		void SetTextureVelY();
		void SetFramebufferVelY();
		void SetTextureWave();
		void SetFramebufferWave();
		void SetTextureCaus();
		void DrawNumberingPoints();
		void RenewVelX();
		void RenewVelY();
		void RenewWaveVel();
		void RenewWavePos();
		void RenewParticle();
		void RenderingP();
		void DrawPlate();
		void DrawParticles();
		void SetTextureParticle();
		void SetFramebufferParticle();
		void CalcObsPos();
		void SetFramebufferCube();
		void SetCubeMap();
		void MakeTextureOfCubeMapping();
		void RefractMapping();
		void MakeUpperTank();
		void makeLowerTank();
		void setTextureMatrix();
		void drawShadow();

};






