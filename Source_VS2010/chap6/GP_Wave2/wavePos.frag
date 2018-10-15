#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform float dt;

varying vec2 texPos;

void main(void)
{
	vec4 wave0 = texture2DRect(samplerWave, texPos);

	wave0.g += wave0.r * dt;
	//wave0.r�͂����������x�Cwave0.g�͕ψʗ�

	//�Œ��Q���̋��E(���R���E����)
	vec4 cA, cB, cC, cD;
	cA = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0));
	cB = texture2DRect(samplerWave, texPos + vec2( 1.0, 0.0));
	cC = texture2DRect(samplerWave, texPos + vec2( 0.0,-1.0));
	cD = texture2DRect(samplerWave, texPos + vec2( 0.0, 1.0));
	
	if(wave0.a == 0.0)//���ړ_����Q��
	{ 
		//�אړ_��1�ł����ʂȂ璍�ړ_�͏�Q���\��
		if(cA.a == 1.0 || cB.a == 1.0 || cC.a == 1.0 || cD.a == 1.0)
		{
			if(cA.a == 1.0) wave0.g = cA.g;
			if(cB.a == 1.0) wave0.g = cB.g;
			if(cC.a == 1.0) wave0.g = cC.g;
			if(cD.a == 1.0) wave0.g = cD.g;
		}
	}

	gl_FragColor = wave0;		
}
