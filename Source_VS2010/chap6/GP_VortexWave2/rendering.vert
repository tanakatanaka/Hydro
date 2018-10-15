
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform sampler2DRect samplerWave;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform int meshX, meshY;
uniform float adjustH;
uniform float  maxOmg, maxPrs;

varying vec3 P;
varying vec3 N;
varying vec3 T;//�ڐ��x�N�g��
varying vec3 B;//�]�@���x�N�g��
varying vec2 grad;

void main(void)
{	
	//�e�N�X�`�����W�̒��S���ړ�
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));

	float p1, p2;
	float delta = 0.1;//�����Ԋu
	float pp = texture2DRect(samplerWave, texPos).b;//�������ꂽ�ψʗ�

	//���z
	p1 = texture2DRect(samplerWave, texPos + vec2(-delta, 0.0)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2( delta, 0.0)).b;
	grad.x = (p2 - p1) * adjustH / (2.0 * delta);//x��������
	p1 = texture2DRect(samplerWave, texPos + vec2(0.0, -delta)).b;
	p2 = texture2DRect(samplerWave, texPos + vec2(0.0,  delta)).b;
	grad.y = (p2 - p1) * adjustH / (2.0 * delta);//y��������

	//�ӂ̒�����sizeX,sizeY�ɕϊ�
 	gl_Vertex.x *= sizeX /  float(meshX);
	gl_Vertex.y *= sizeY /  float(meshY);

	gl_Vertex.z = pp * adjustH;//������(�\����̍����𒲐��j

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//�������ڐ��x�N�g��
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//�]�@���x�N�g��
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}

