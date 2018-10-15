
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPsi;
uniform sampler2DRect samplerOmg;
uniform int texWidth, texHeight;
uniform float sizeX, sizeY;
uniform int kind;
uniform float adjustH;
uniform float maxOmg, maxPsi, maxVel;

varying vec3 P;
varying vec3 N;
varying vec3 T;//�ڐ��x�N�g��
varying vec3 B;//�]�@���x�N�g��
varying vec2 grad;
varying float pp;

//�e�N�X�`�����W��Ԃɂ������f�ʒu�̒P��
float delta = 1.0;//�����Ԋu

void main(void)
{	
	//�e�N�X�`�����W�̒��S���ړ�
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	
	//�ӂ̒�����sizeX,sizeY�ɕϊ�
 	gl_Vertex.x *= sizeX /  float(texWidth-1);
	gl_Vertex.y *= sizeY /  float(texHeight-1);

	//�ȉ��̓V�F�[�f�B���O�ɕK�v	
	vec2 vel;
	float p1, p2;
	if(kind == 0)//Omega
	{ 
		pp = texture2DRect(samplerOmg, texPos).r / maxOmg + 0.5;
		//������(�\����̍����𒲐��j
		gl_Vertex.z = (pp - 0.5) * adjustH;
	
		p1 = texture2DRect(samplerOmg, texPos + vec2(-delta, 0.0)).r;
		p2 = texture2DRect(samplerOmg, texPos + vec2( delta, 0.0)).r;
		grad.x = (p2 - p1) / (2.0 * delta);//x��������
		p1 = texture2DRect(samplerOmg, texPos + vec2(0.0, -delta)).r;
		p2 = texture2DRect(samplerOmg, texPos + vec2(0.0,  delta)).r;
		grad.y = (p2 - p1) / (2.0 * delta);//y��������
	}
	if(kind == 1)//Psi
	{
		pp = texture2DRect(samplerPsi, texPos).r / maxPsi;
		//������(�\����̍����𒲐��j
		gl_Vertex.z = pp * adjustH;
		
		p1 = texture2DRect(samplerPsi, texPos + vec2(-delta, 0.0)).r;
		p2 = texture2DRect(samplerPsi, texPos + vec2( delta, 0.0)).r;
		grad.x = (p2 - p1) / (2.0 * delta);//x��������
		p1 = texture2DRect(samplerPsi, texPos + vec2(0.0, -delta)).r;
		p2 = texture2DRect(samplerPsi, texPos + vec2(0.0,  delta)).r;
		grad.y = (p2 - p1) / (2.0 * delta);//y��������
	}
	if(kind == 2)//Velocity
	{
		vel.x = texture2DRect(samplerPsi, texPos).g;
		vel.y = texture2DRect(samplerPsi, texPos).b;
		pp = length(vel) / maxVel;
		//������(�\����̍����𒲐��j
		gl_Vertex.z = pp * adjustH;

		vel.x = texture2DRect(samplerPsi, texPos + vec2(-delta, 0.0)).g;
		vel.y = texture2DRect(samplerPsi, texPos + vec2(-delta, 0.0)).b;
		p1 = length(vel) / maxVel;
		vel.x = texture2DRect(samplerPsi, texPos + vec2( delta, 0.0)).g;
		vel.y = texture2DRect(samplerPsi, texPos + vec2( delta, 0.0)).b;
		p2 = length(vel) / maxVel;
		grad.x = (abs(p2) - abs(p1)) / (2.0 * delta);//x��������;
		vel.x = texture2DRect(samplerPsi, texPos + vec2( 0.0, -delta)).g;
		vel.y = texture2DRect(samplerPsi, texPos + vec2( 0.0, -delta)).b;
		p1 = length(vel) / maxVel;
		vel.x = texture2DRect(samplerPsi, texPos + vec2( 0.0, delta)).g;
		vel.y = texture2DRect(samplerPsi, texPos + vec2( 0.0, delta)).b;
		p2 = length(vel) / maxVel;
		grad.y = (abs(p2) - abs(p1)) / (2.0 * delta);//y��������;
	}


	P = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//�������ڐ��x�N�g��
	T = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)).xyz;
	//�]�@���x�N�g��
	B = normalize(gl_NormalMatrix * vec3(0.0, 1.0, 0.0)).xyz;
		
	gl_Position = ftransform();
}

