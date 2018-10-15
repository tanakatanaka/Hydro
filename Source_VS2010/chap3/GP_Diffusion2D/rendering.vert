
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPhi;
uniform int texWidth;//�S�i�q��
uniform int nMesh;
uniform float size0;
uniform float adjustH;

varying float pp;

void main(void)
{
	int texHeight = texWidth;
	//�e�N�X�`�����W�̒��S���ړ�
	vec2 texPos = gl_Vertex.xy + vec2(float(texWidth/2), float(texHeight/2));
	
	vec4 phi = texture2DRect(samplerPhi, texPos);
	pp = phi.r;//������
	
	//1�ӂ̒�����size0�ɕϊ�
 	gl_Vertex.x *= size0 /  float(nMesh);
	gl_Vertex.y *= size0 /  float(nMesh);
	gl_Vertex.z = phi.r * adjustH;//������
	
	gl_Position = ftransform();
}

