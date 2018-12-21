#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform sampler2DRect samplerWave;
uniform int meshX, meshY;
uniform float maxOmg, maxPrs;
uniform float adjustH;
uniform float adjustC;
uniform float deltaT;

varying vec2 texPos;

void main(void)
{
	vec4 wave = texture2DRect(samplerWave, texPos) ;

	//���E����
	//wave.r�͂����������x�Cwave.g�͕ψʗ�

	wave.g += wave.r * deltaT;//wave.r�͂����������x�Cwave.g�͈ʒu�i�ψʗʁj
	
	//���R���E����
	if(texPos.x == 0.0) wave.g = texture2DRect(samplerWave, texPos + vec2(1.0, 0.0)).g;
	if(texPos.x == float(meshX)) wave.g = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0)).g;
	if(texPos.y == 0.0) wave.g = texture2DRect(samplerWave, texPos + vec2(0.0, 1.0)).g;
	if(texPos.y == float(meshY)) wave.g = texture2DRect(samplerWave, texPos + vec2(0.0, -1.0)).g;

	//����
	wave.b = -abs(texture2DRect(samplerVelY, texPos).a) / maxOmg;
	wave.b += texture2DRect(samplerVelX, texPos).a / maxPrs ;//���͐�����������
	wave.b += wave.g;
	wave.b *= adjustH;
	
	//���z
	float delta, p1, p2;
	vec2 grad;
	delta = 0.5;
	p1 = texture2DRect(samplerWave, texPos + vec2(-delta, 0.0)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2( delta, 0.0)).b;
	grad.x = (p2 - p1) / delta;//x��������
	p1 = texture2DRect(samplerWave, texPos + vec2(0.0, -delta)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2(0.0,  delta)).b;
	grad.y = (p2 - p1) / delta;//y��������
	float c = length(grad);
	c *= c * c;
	wave.a = 0.5 + c * adjustC;//0.5�͏W���͗l���Ȃ��Ƃ��̖��邳
	wave.a = clamp(wave.a, 0.0, 1.0);

	gl_FragColor = wave;		
}
