#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;//�_�~�[���܂ޑS�i�q��
uniform int nDummy;
uniform int meshX, meshY;
uniform float sizeX, sizeY;
varying vec3 P;
varying vec3 N;
varying vec3 T;//�ڐ��x�N�g��
varying vec3 B;//�]�@���x�N�g��
varying vec2 grad;

//�e�N�X�`�����W��Ԃɂ������f�ʒu�̒P��
float delta = 0.1;//�����Ԋu

void main(void)
{
	//�e�N�X�`�����W�̒��S���ړ�
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));

	//�ʒu���W
	vec4 wave = texture2DRect(samplerWave, texPos);
	//x��������
	vec4 wave1 =  texture2DRect(samplerWave, texPos+vec2(-delta, 0.0));
	vec4 wave2 =  texture2DRect(samplerWave, texPos+vec2( delta, 0.0));
	grad.x = (wave2.g - wave1.g) / (2.0 * delta);
	//y��������
	wave1 =  texture2DRect(samplerWave, texPos+vec2(0.0,-delta));
	wave2 =  texture2DRect(samplerWave, texPos+vec2(0.0, delta));
	grad.y =  (wave2.g - wave1.g) / (2.0 * delta);
	//1�ӂ̒�����size0�ɕϊ�
 	gl_Vertex.x *= sizeX /  float(meshX);
	gl_Vertex.y *= sizeY /  float(meshY);
	gl_Vertex.z = wave.g;//�ψʗ�

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//�������ڐ��x�N�g��
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//�]�@���x�N�g��
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}
