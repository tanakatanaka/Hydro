#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//�_�~�[���܂�X�����S�i�q��
uniform int nDummy;
uniform float sizeX, sizeY;//���ۂ�1�ӂ̒���(�_�~�[���܂܂��j
uniform int meshX, meshY;
uniform int nSource;
uniform int sourceX;
uniform int interval;
uniform float amp;
uniform float freq;
uniform float waveSpeed;
uniform float drag0;
uniform float dt;
uniform float time;

varying vec2 texPos;

float dx, dy;
float makeWave(float v0);

void main(void)
{
	//�i�q�Ԋu
	dx = sizeX / float(meshX);	
	dy = sizeY / float(meshY);	
	
	vec4 wave = texture2DRect(samplerWave, texPos);
	wave.r = makeWave(wave.r);//�g���̔g���쐬(�����������x�j

	float drag = drag0;
	int nm = nDummy;      //�����˂̂Ƃ��̗L���̈拫�E�ԍ�	
	int npX = meshX + nm;//�����˂̂Ƃ��̗L���̈拫�E�ԍ�
	int npY = meshY + nm;//�����˂̂Ƃ��̗L���̈拫�E�ԍ�

    int i = int(texPos.x);
    int j = int(texPos.y);

	//�L���̈拫�E�ł͖����ˋ��E
	float dragMax = 5.0;
	if(j < nm) drag = drag0 + dragMax * float(nm - j) / float(nm);
	if(j > npY)drag = drag0 + dragMax * float(j - npY) / float(nm);
	if(i < nm) drag = drag0 + dragMax * float(nm - i) / float(nm);
	if(i > npX)drag = drag0 + dragMax * float(i - npX) / float(nm);
	
	//�����x�v�Z�ɕK�v�ȌW�������߂�
	vec4 cA, cB, cC, cD;//���ʂł����1.0�C��Q���ł����0.0
	cA = texture2DRect(samplerWave, texPos + vec2(-1.0, 0.0));
	cB = texture2DRect(samplerWave, texPos + vec2( 1.0, 0.0));
	cC = texture2DRect(samplerWave, texPos + vec2( 0.0,-1.0));
	cD = texture2DRect(samplerWave, texPos + vec2( 0.0, 1.0));
	
	float c1, c2;

	float accel = 0.0;
	if(wave.a == 1.0 ||       //���ړ_������
	  (wave.a == 0.0 && (cA.a == 1.0 || cB.a == 1.0 
		                || cC.a == 1.0 || cD.a == 1.0)))//�\��
	{
		if(cA.a * cB.a == 1.0) c1 = 2.0;//�ǂ��������
		else c1 = 1.5;                  //�ǂ��炩����Q���\��
		if(cC.a * cD.a == 1.0) c2 = 2.0;//�ǂ��������
		else c2 = 1.5;                  //�ǂ��炩����Q���\��
	  
		accel = (waveSpeed * waveSpeed) * 
		 ((cA.a * cA.g + cB.a * cB.g - c1 * wave.g) / (dx*dx) 
		+ (cC.a * cC.g + cD.a * cD.g - c2 * wave.g) / (dy*dy));
	}

	//�S����R
	accel -= drag * wave.r;
	//���x�̍X�V
	wave.r += accel * dt;

	gl_FragColor = wave;
}
//----------------------------------------------------------
float makeWave(float v0)
{
	float pi = radians(180.0);

	int k;
	//�̈�̒��S�����_�ɕύX
	vec2 pos = texPos.xy
	         - vec2(float((texWidth)/2),float((texHeight)/2));
	vec2 source;//�g���̈ʒu

	float value = v0;
	for(k = 0; k < nSource; k++)
	{
		source.x = float(sourceX) ;
		source.y = float(interval) *
		           (-0.5 * float(nSource-1) + float(k));

		if(pos.x == source.x && pos.y == source.y)
		{//�A���g����
			value = amp * sin(2.0 * pi * freq * time);
		}
	}
	return value;
}
