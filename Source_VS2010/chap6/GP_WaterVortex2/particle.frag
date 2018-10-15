#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform sampler2DRect samplerPosP;
uniform int meshX, meshY;
uniform float adjustH;
uniform float maxOmg, maxPrs;
uniform float sizeX, sizeY;
uniform int numParticle;
uniform float dt;
varying vec2 texPos;

void main(void)
{
	//�t�ʂ̍��������߂�
	//Omega(���͐�����������j
	float pp = -abs(texture2DRect(samplerVelY, texPos).a) / maxOmg;
	pp += texture2DRect(samplerVelX, texPos).a / maxPrs ;//���͐�����������
	float hh = pp * adjustH;//������(�\����̍����𒲐��j
	
	//���q�̈ʒu���W�v�Z
	vec3 posP;
	float velX, velY;
	float deltaX = sizeX / float(meshX);
	float deltaY = sizeY / float(meshY);

	posP = texture2DRect(samplerPosP, texPos).xyz;//���q�̈ʒu
	//���̈ʒu�ɍł��߂��i�q�_
	float I = (posP.x + sizeX/2.0) / deltaX;
	float J = (posP.y + sizeY/2.0) / deltaY;
	//�i�q�_�̗��̑��x
	velX = texture2DRect(samplerVelX, vec2(I, J)).r;	
	velY = texture2DRect(samplerVelY, vec2(I, J)).r;
	vec3 velP = vec3(velX, velY, 0.0);

	//update position
	posP += velP * dt;
	posP.z = texture2DRect(samplerPosP, vec2(I, J)).a;

	gl_FragColor = vec4(posP, hh);
}


