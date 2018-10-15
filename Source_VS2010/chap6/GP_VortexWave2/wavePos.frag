#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//ダミーを含む全格子数
uniform float maxOmg, maxPrs;
uniform float dt;

varying vec2 texPos;

void main(void)
{
	vec4 wave = texture2DRect(samplerWave, texPos);

	wave.g += wave.r * dt;
	//wave.rはｚ軸方向速度，wave.gは変位量

	//固定障害物の境界(自由境界条件)
	vec4 cA, cB, cC, cD;
	cA = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0));
	cB = texture2DRect(samplerWave, texPos + vec2( 1.0, 0.0));
	cC = texture2DRect(samplerWave, texPos + vec2( 0.0,-1.0));
	cD = texture2DRect(samplerWave, texPos + vec2( 0.0, 1.0));
	
	if(wave.a == 0.0)//注目点が障害物
	{ 
		//隣接点の1つでも水面なら注目点は障害物表面
		if(cA.a == 1.0 || cB.a == 1.0 || cC.a == 1.0 || cD.a == 1.0)
		{
			if(cA.a == 1.0) wave.g = cA.g;
			if(cB.a == 1.0) wave.g = cB.g;
			if(cC.a == 1.0) wave.g = cC.g;
			if(cD.a == 1.0) wave.g = cD.g;
		}
	}
	//合成
	wave.b = -abs(texture2DRect(samplerVelY, texPos).a) / maxOmg;
	wave.b += texture2DRect(samplerVelX, texPos).a / maxPrs ;//圧力成分も加える
	wave.b += wave.g;		

	gl_FragColor = wave;		
}
