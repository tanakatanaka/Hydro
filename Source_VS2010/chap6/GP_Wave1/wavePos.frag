
#extension GL_ARB_texture_rectangle: enable

uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//ダミーを含む全格子数
uniform float dt;
uniform int boundary;

varying vec2 texPos;

void main(void)
{
	vec4 wave = texture2DRect(samplerWave, texPos);
	wave.g += wave.r * dt;//wave.rはｚ軸方向速度，wave.gは位置（高さ）

	if(boundary == 1)//自由境界条件
	{
		if(texPos.x == 0.0) wave.g = texture2DRect(samplerWave, texPos + vec2(1.0, 0.0)).g;
		if(texPos.x == float(texWidth-1)) wave.g = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0)).g;
		if(texPos.y == 0.0) wave.g = texture2DRect(samplerWave, texPos + vec2(0.0, 1.0)).g;
		if(texPos.y == float(texHeight-1)) wave.g = texture2DRect(samplerWave, texPos + vec2(0.0, -1.0)).g;
	}
	else if(boundary == 2)//固定境界
	{
		if(texPos.x <= 0.0 || texPos.x >= float(texWidth-1)) wave.g = 0.0;
		if(texPos.y <= 0.0 || texPos.y >= float(texHeight-1)) wave.g = 0.0;
	}
		
	gl_FragColor = wave;		
}
