
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform float adjustH;
uniform float  maxOmg, maxPrs;

varying vec3 P;
varying vec3 N;
varying vec3 T;//�ڐ��x�N�g��
varying vec3 B;//�]�@���x�N�g��
varying vec2 grad;
varying float pp;

void main(void)
{	
	//�e�N�X�`�����W�̒��S���ړ�
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	
	vec2 vel;
	float p1, p2;
	float delta = 0.1;//�����Ԋu

	//�t�ʕϓ�
	pp = -abs(texture2DRect(samplerVelY, texPos).a) / maxOmg;//Omega
	pp += texture2DRect(samplerVelX, texPos).a / maxPrs ;//���͐�����������
		
	p1 = -abs(texture2DRect(samplerVelY, texPos + vec2(-delta, 0.0)).a);
	p2 = -abs(texture2DRect(samplerVelY, texPos + vec2( delta, 0.0)).a);
	grad.x = (p2 - p1) * adjustH / (2.0 * delta);//x��������
	p1 = -abs(texture2DRect(samplerVelY, texPos + vec2(0.0, -delta)).a);
	p2 = -abs(texture2DRect(samplerVelY, texPos + vec2(0.0,  delta)).a);
	grad.y = (p2 - p1) * adjustH / (2.0 * delta);//y��������

	//�ӂ̒�����sizeX,sizeY�ɕϊ�
 	gl_Vertex.x *= sizeX /  float(texWidth-1);
	gl_Vertex.y *= sizeY /  float(texHeight-1);

	gl_Vertex.z = pp * adjustH;//������(�\����̍����𒲐��j

	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//�������ڐ��x�N�g��
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//�]�@���x�N�g��
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}

