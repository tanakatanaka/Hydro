
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//�_�~�[���܂�X�����S�i�q��
uniform int nDummy;
uniform float sizeX, sizeY;//���ۂ�1�ӂ̒���(�_�~�[���܂܂��j
uniform int meshX, meshY;
uniform float amp;
uniform float freq;
uniform float waveSpeed;
uniform float drag0;
uniform float dt;
uniform float time;
uniform vec3 posObs; //�������̂̈ʒu
uniform vec3 velObs; //���̑��x
uniform vec3 sizeObs;//���̃T�C�Y

varying vec2 texPos;

float dx, dy;
float makeWave2(float v0);

void main(void)
{
	//�i�q�Ԋu
	dx = sizeX / float(meshX);	
	dy = sizeY / float(meshY);	
	
	vec4 wave = texture2DRect(samplerWave, texPos);
	wave.r = makeWave2(wave.r);//�ړ���Q���ɂ��g���쐬(�����������x�j

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
	vec4 cA, cB, cC, cD;//cA.a�Ȃǂ͐��ʂł����1.0�C��Q���ł����0.0
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
float makeWave2(float v0)
{
	const float pi = 3.14159265;
	int k;
	//�̈�̒��S�����_�ɕύX
	vec2 pos = texPos.xy
	         - vec2(float((texWidth)/2),float((texHeight)/2));
	pos.x *= dx;//���ۂ̃T�C�Y�ɕύX
	pos.y *= dy;

	float value = v0;
	
	float R =  (sizeObs.x + sizeObs.y) / 4.0;
	//�O������
	vec2 dir = normalize(vec2(velObs.x, velObs.y));
	vec2 posW = posObs.xy + dir * R ;//�U�������ړ����̂̑O���ɒu��

	if(pos.x > posW.x-dx/2.0 &&  pos.x < posW.x+dx/2.0 
	   && pos.y > posW.y-dy/2.0 && pos.y < posW.y+dy/2.0)
//	if(pos.x == posW.x && pos.y == posW.y)
	{
		value = amp * sin(2.0 * pi * freq * time);		
	}
//	if(value >  sizeObs.z/2.0) value =  sizeObs.z/2.0;
//	if(value < -sizeObs.z/2.0) value = -sizeObs.z/2.0;
	
	return value;
}
