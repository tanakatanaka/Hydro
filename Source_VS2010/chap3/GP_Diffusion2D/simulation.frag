#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPhi;
uniform int texWidth;//X方向格子数
uniform float size0;//実際の1辺の長さ(ダミーを含まず）
uniform float deltaT;
uniform float diff_num;
uniform int boundary;

varying vec2 texPos;
int texHeight = texWidth;
vec2 delta;

void main(void)
{
	int nMesh = texWidth - 1;//領域の分割数（格子数）

	delta.x = size0 / float(nMesh);
	delta.y = delta.x;

	vec4 phi = texture2DRect(samplerPhi, texPos);//注目点の物理量
	
	int im, ip, jm, jp;
	float phi_mi, phi_pi, phi_mj, phi_pj;//隣接格子点の物理量

    int i = int(texPos.x);
    int j = int(texPos.y);

	phi_mi = texture2DRect(samplerPhi, texPos + vec2(-1.0, 0.0)).r;	
	phi_pi = texture2DRect(samplerPhi, texPos + vec2( 1.0, 0.0)).r;
	phi_mj = texture2DRect(samplerPhi, texPos + vec2( 0.0,-1.0)).r;
	phi_pj = texture2DRect(samplerPhi, texPos + vec2( 0.0, 1.0)).r;
	
	im = i-1; ip = i+1; jm = j-1; jp = j+1;//注目点の上下左右の格子点
	if(i > 0 && i < texWidth-1 && j > 0 && j < texHeight-1)
	{//領域内部
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
		
		//拡散
		phi.r += diff_num * (phi_mi + phi_pi + phi_mj + phi_pj - 4.0 * phi.r);
	}
	else//注目点が境界
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
