#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform sampler2DRect samplerWave;
uniform int meshX, meshY;
uniform float maxOmg, maxPrs;
uniform float adjustH;
uniform float deltaT;

varying vec2 texPos;

void main(void)
{
	vec4 wave = texture2DRect(samplerWave, texPos) ;

	//境界条件
	//wave.rはｚ軸方向速度，wave.gは変位量

	wave.g += wave.r * deltaT;//wave.rはｚ軸方向速度，wave.gは位置（変位量）
	
	//自由境界条件
	if(texPos.x == 0.0) wave.g = texture2DRect(samplerWave, texPos + vec2(1.0, 0.0)).g;
	if(texPos.x == float(meshX)) wave.g = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0)).g;
	if(texPos.y == 0.0) wave.g = texture2DRect(samplerWave, texPos + vec2(0.0, 1.0)).g;
	if(texPos.y == float(meshY)) wave.g = texture2DRect(samplerWave, texPos + vec2(0.0, -1.0)).g;

	//合成
	wave.b = -abs(texture2DRect(samplerVelY, texPos).a) / maxOmg;
	wave.b += texture2DRect(samplerVelX, texPos).a / maxPrs ;//圧力成分も加える
	wave.b += wave.g;
	wave.b *= adjustH;		

	gl_FragColor = wave;		
}
