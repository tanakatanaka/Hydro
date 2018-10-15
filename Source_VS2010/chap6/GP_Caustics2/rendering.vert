

#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform float height;//���ʂ̍���
uniform float nRatio;
uniform int flagInverse;

varying vec4 refCoord;
varying vec3 P;
varying vec3 N;

vec3 calcRefract(vec3 I, vec3 N, int flagInverse);

void main(void)
{	
	vec3 T;//�ڐ��x�N�g��
	vec3 B;//�]�@���x�N�g��
	vec2 grad;

	//�e�N�X�`�����W�̒��S���ړ�
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	
	float p1, p2;
	float delta = 0.1;//�����Ԋu
	float pp = texture2DRect(samplerWave, texPos).b;//�������ꂽ�ψʗ�
	pp = clamp(pp, -0.5, 0.5);//�\����̐U������

	//���z
	p1 = texture2DRect(samplerWave, texPos + vec2(-delta, 0.0)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2( delta, 0.0)).b;
	grad.x = (p2 - p1) / (2.0 * delta);//x��������
	p1 = texture2DRect(samplerWave, texPos + vec2(0.0, -delta)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2(0.0,  delta)).b;
	grad.y = (p2 - p1) / (2.0 * delta);//y��������

	//�ӂ̒�����sizeX,sizeY�ɕϊ�
 	gl_Vertex.x *= sizeX /  float(texWidth-1);
	gl_Vertex.y *= sizeY /  float(texHeight-1);

	gl_Vertex.z = height + pp;//������(�\����̍����𒲐��j

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//�������ڐ��x�N�g��
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//�]�@���x�N�g��
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
	//�V�@���x�N�g��
	N = normalize(N - grad.x * T - grad.y * B);

	//���܃x�N�g���̌v�Z
	vec3 incident = normalize(P);//���˃x�N�g��
	vec3 TT = calcRefract(incident, N, flagInverse) ;//���̋��܃x�N�g��
	//vec3 TT;
	//if(flagInverse == 0) TT = refract(incident, N, 1.0/nRatio);
	//else TT = refract(incident, N, nRatio);
//	refCoord = ViewTranspose * vec4(TT, 0.0) ;//���܃x�N�g���ɂ��e�N�X�`�����W
	refCoord = gl_ModelViewMatrixTranspose * vec4(TT, 0.0) ;//���܃x�N�g���ɂ��e�N�X�`�����W
 
	gl_Position = ftransform();
}

//------------------------------------------------------------------------------
vec3 calcRefract(vec3 I, vec3 N, int flagInverse)
{//���܃x�N�g���̌v�Z

	if(flagInverse == 1){//���_�����ʈȉ�
		N = - N;
		nRatio = 1.0 / nRatio;
	}
	float cosIN = dot(-I, N);
	float a = nRatio * nRatio - 1.0 + cosIN * cosIN;
	//if(a < 0.0) return reflect(I, N);//�S����
	if(a < 0.0) return I + 2.0*cosIN * N;
	else 
	return (I - N *(sqrt(a) - cosIN)) / nRatio;
}
