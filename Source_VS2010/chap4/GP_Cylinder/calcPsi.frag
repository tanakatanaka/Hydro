
//const float pi = 3.14159265;
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
 
	int NX = texWidth - 1;//“®Œa•ûŒü•ªŠ„”iŠiŽq”j
	int NY = texHeight -1;//•ûˆÊŠp•ûŒü•ªŠ„”

	vec4 omg = texture2DRect(samplerOmg, texPos);//’–Ú“_‚Ì‰Q“x
	vec4 psi = texture2DRect(samplerPsi, texPos);//’–Ú“_‚Ì—¬‚êŠÖ”‚Æ‘¬“x

	float psi_im, psi_ip, psi_jm, psi_jp;//—×ÚŠiŽq“_‚Ì—¬‚êŠÖ”‚Æ‘¬“x

    int i = int(texPos.x);
    int j = int(texPos.y);

	psi_im = texture2DRect(samplerPsi, texPos + vec2(-1.0, 0.0)).r;	
	psi_ip = texture2DRect(samplerPsi, texPos + vec2( 1.0, 0.0)).r;
	psi_jm = texture2DRect(samplerPsi, texPos + vec2( 0.0,-1.0)).r;
	psi_jp = texture2DRect(samplerPsi, texPos + vec2( 0.0, 1.0)).r;
	if(j == 0) psi_jm = texture2DRect(samplerPsi, vec2(texPos.x, float(NY)-1.0)).r;
	if(j == NY-1) psi_jp = texture2DRect(samplerPsi, vec2(texPos.x, 0.0)).r;

	float ex;
	//ŠÈ—ª‰»‚µ‚½ƒ|ƒAƒ\ƒ“‚Ì•û’öŽ®‚Ì‰ð
	if(i > 0 && i < NX && j >= 0 && j < NY)
	{
		psi.r = ( ( psi_im + psi_ip) * DY2
				     + (psi_jm + psi_jp) * DX2
				     + omg.r * exp(2.0 * float(i) * DX) * DX2 * DY2 ) * fct ;
		//‘¬“x
		ex = exp(- float(i) * DX);
		psi.g =  ex * (psi_jp - psi_jm) / (DY * 2.0);//“®Œa•ûŒü
		psi.b =  ex * (psi_im - psi_ip) / (DX * 2.0);//•ûˆÊŠp•ûŒü
		
	}
	gl_FragColor = psi;
}

