#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPhi;
uniform int texWidth;//X�����i�q��
uniform float size0;//���ۂ�1�ӂ̒���(�_�~�[���܂܂��j
uniform float deltaT;
uniform float diff_num;
uniform int boundary;

varying vec2 texPos;
int texHeight = texWidth;
vec2 delta;

void main(void)
{
	int nMesh = texWidth - 1;//�̈�̕������i�i�q���j

	delta.x = size0 / float(nMesh);
	delta.y = delta.x;

	vec4 phi = texture2DRect(samplerPhi, texPos);//���ړ_�̕�����
	
	int im, ip, jm, jp;
	float phi_mi, phi_pi, phi_mj, phi_pj;//�אڊi�q�_�̕�����

    int i = int(texPos.x);
    int j = int(texPos.y);

	phi_mi = texture2DRect(samplerPhi, texPos + vec2(-1.0, 0.0)).r;	
	phi_pi = texture2DRect(samplerPhi, texPos + vec2( 1.0, 0.0)).r;
	phi_mj = texture2DRect(samplerPhi, texPos + vec2( 0.0,-1.0)).r;
	phi_pj = texture2DRect(samplerPhi, texPos + vec2( 0.0, 1.0)).r;
	
	im = i-1; ip = i+1; jm = j-1; jp = j+1;//���ړ_�̏㉺���E�̊i�q�_
	if(i > 0 && i < texWidth-1 && j > 0 && j < texHeight-1)
	{//�̈����
		if(boundary == 0)//Dirichlet
		{
			if(im == 0) phi_mi = 0.0;
			if(ip == texWidth-1) phi_pi = 0.0;
			if(jm == 0) phi_mj = 0.0;
			if(jp == texHeight-1) phi_pj = 0.0;
		}
		else if(boundary == 1)//Neumann
		{
			if(im == 0) phi_mi = texture2DRect(samplerPhi, texPos).r;
			if(ip == texWidth-1) phi_pi = texture2DRect(samplerPhi, texPos).r;
			if(jm == 0) phi_mj = texture2DRect(samplerPhi, texPos).r;
			if(jp == texHeight-1) phi_pj = texture2DRect(samplerPhi, texPos).r;
		}
		
		//�g�U
		phi.r += diff_num * (phi_mi + phi_pi + phi_mj + phi_pj - 4.0 * phi.r);
	}
	else//���ړ_�����E
	{
		if(boundary == 0)//Dirichlet
			phi.r = 0.0;
		else//Neumann
		{
			if(i == 0) phi.r = texture2DRect(samplerPhi, texPos + vec2( 1.0, 0.0)).r;
			if(i == texWidth-1) phi.r = texture2DRect(samplerPhi, texPos + vec2(-1.0, 0.0)).r;
			if(j == 0) phi.r = texture2DRect(samplerPhi, texPos + vec2(0.0, 1.0)).r;
			if(j == texHeight-1) phi.r = texture2DRect(samplerPhi, texPos + vec2(0.0,-1.0)).r;
		}
	}

	gl_FragColor = phi;
}
