
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPhi;
uniform int texWidth;//�_�~�[���܂ޑS�i�q��
uniform int nMesh;
uniform float size0;
uniform float adjustH;
varying vec3 P;
varying vec3 N;
varying vec3 T;//�ڐ��x�N�g��
varying vec3 B;//�]�@���x�N�g��
varying vec2 grad;
varying float pp;

int texHeight = texWidth;

void main(void)
{
	//�e�N�X�`�����W�̒��S���ړ�
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	//�ʒu���W
	vec4 phi = texture2DRect(samplerPhi, texPos);
	pp = phi.r;
	//1�ӂ̒�����size0�ɕϊ�
 	gl_Vertex.x *= size0 /  float(nMesh);
	gl_Vertex.y *= size0 /  float(nMesh);
	gl_Vertex.z = phi.r * adjustH;//������(���������j
	gl_Position = ftransform();
}

