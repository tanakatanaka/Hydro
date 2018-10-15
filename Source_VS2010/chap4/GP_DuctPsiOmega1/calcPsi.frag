
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
 
	int NX = texWidth - 1;//x方向分割数
	int NY = texHeight -1;//y方向分割数

	vec4 omg = texture2DRect(samplerOmg, texPos);//注目点の渦度
	vec4 psi = texture2DRect(samplerPsi, texPos);//注目点の流れ関数と速度

	float psi_im, psi_ip, psi_jm, psi_jp;//隣接格子点の流れ関数と速度

    int i = int(texPos.x);
    int j = int(texPos.y);

	psi_im = texture2DRect(samplerPsi, texPos + vec2(-1.0, 0.0)).r;	
	psi_ip = texture2DRect(samplerPsi, texPos + vec2( 1.0, 0.0)).r;
	psi_jm = texture2DRect(samplerPsi, texPos + vec2( 0.0,-1.0)).r;
	psi_jp = texture2DRect(samplerPsi, texPos + vec2( 0.0, 1.0)).r;
	
	//簡略化したポアソンの方程式の解
	if(i > 0 && i < NX && j > 0 && j < NY)
	{
		psi.r = ( ( psi_im + psi_ip) * DY2 + (psi_jm + psi_jp) * DX2
				     + omg.r * DX2 * DY2 ) * fct ;
	}
	
	//速度
	psi.g =  (psi_jp - psi_jm) / (DY * 2.0);//x方向
	psi.b =  (psi_im - psi_ip) / (DX * 2.0);//y方向

	gl_FragColor = psi;
}

