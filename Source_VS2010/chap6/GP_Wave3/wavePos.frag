#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform float dt;

varying vec2 texPos;

void main(void)
{
	vec4 wave = texture2DRect(samplerWave, texPos);

	wave.g += wave.r * dt;
	//wave.rÍ²ûü¬xCwave.gÍÏÊÊ

	//ÅèáQ¨Ì«E(©R«Eð)
	vec4 cA, cB, cC, cD;
	cA = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0));
	cB = texture2DRect(samplerWave, texPos + vec2( 1.0, 0.0));
	cC = texture2DRect(samplerWave, texPos + vec2( 0.0,-1.0));
	cD = texture2DRect(samplerWave, texPos + vec2( 0.0, 1.0));
	
	if(wave.a == 0.0)//Ú_ªáQ¨
	{ 
		//×Ú_Ì1ÂÅàÊÈçÚ_ÍáQ¨\Ê
		if(cA.a == 1.0 || cB.a == 1.0 || cC.a == 1.0 || cD.a == 1.0)
		{
			if(cA.a == 1.0) wave.g = cA.g;
			if(cB.a == 1.0) wave.g = cB.g;
			if(cC.a == 1.0) wave.g = cC.g;
			if(cD.a == 1.0) wave.g = cD.g;
		}
	}

	gl_FragColor = wave;		
}
