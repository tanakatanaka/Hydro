
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerOmg;
uniform sampler2DRect samplerPsi;
uniform int texWidth;
uniform int texHeight;
uniform float DX, DY;

varying vec2 texPos;

void main(void)
{

	float DX2 = DX * DX;
	float DY2 = DY * DY;
	float fct = 1.0 / (2.0 * DX2 + 2.0 * DY2);
 
	int NX = texWidth - 1;//x����������
	int NY = texHeight -1;//y����������

	vec4 omg = texture2DRect(samplerOmg, texPos);//���ړ_�̉Q�x
	vec4 psi = texture2DRect(samplerPsi, texPos);//���ړ_�̗���֐��Ƒ��x

	float psi_im, psi_ip, psi_jm, psi_jp;//�אڊi�q�_�̗���֐��Ƒ��x

    int i = int(texPos.x);
    int j = int(texPos.y);

	psi_im = texture2DRect(samplerPsi, texPos + vec2(-1.0, 0.0)).r;	
	psi_ip = texture2DRect(samplerPsi, texPos + vec2( 1.0, 0.0)).r;
	psi_jm = texture2DRect(samplerPsi, texPos + vec2( 0.0,-1.0)).r;
	psi_jp = texture2DRect(samplerPsi, texPos + vec2( 0.0, 1.0)).r;
	
	//�ȗ��������|�A�\���̕������̉�
	if(i > 0 && i < NX && j > 0 && j < NY)
	{
		psi.r = ( ( psi_im + psi_ip) * DY2 + (psi_jm + psi_jp) * DX2
				     + omg.r * DX2 * DY2 ) * fct ;
	}
	
	//���x
	psi.g =  (psi_jp - psi_jm) / (DY * 2.0);//x����
	psi.b =  (psi_im - psi_ip) / (DX * 2.0);//y����

	gl_FragColor = psi;
}

