
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerPhi;
uniform sampler2DRect samplerVel;
uniform int texWidth;//全格子数
uniform float size0;//実際の1辺の長さ(ダミーを含まず）
uniform float deltaT;
uniform float diff_num;
uniform int boundary;

varying vec2 texPos;
int texHeight = texWidth;
vec2 delta;
void methodCIP(inout vec4 phi);

void main(void)
{

	int nMesh = texWidth - 1;//有効領域の分割数（格子数）

	delta.x = size0 / float(nMesh);
	delta.y = delta.x;

	vec4 phi = texture2DRect(samplerPhi, texPos);//注目点の物理量
	
	int im, ip, jm, jp;
	float phi_mi, phi_pi, phi_mj, phi_pj;//隣接格子点の物理量

    int i = int(texPos.x);
    int j = int(texPos.y);
	im = i-1; ip = i+1; jm = j-1; jp = j+1;//注目点の上下左右の格子点
	
	phi_mi = texture2DRect(samplerPhi, texPos + vec2(-1.0, 0.0)).r;	
	phi_pi = texture2DRect(samplerPhi, texPos + vec2( 1.0, 0.0)).r;
	phi_mj = texture2DRect(samplerPhi, texPos + vec2( 0.0,-1.0)).r;
	phi_pj = texture2DRect(samplerPhi, texPos + vec2( 0.0, 1.0)).r;
	
	if(i > 0 && i < texWidth-1 && j > 0 && j < texHeight-1)
	{
		if(boundary == 0)//Direchlet
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
		
		methodCIP(phi);
				
		//拡散
		phi.r += diff_num * (phi_mi + phi_pi + phi_mj + phi_pj - 4.0 * phi.r);
	}

	else//注目点が境界
	{
		if(boundary == 0)//Dirichlet
			phi.r = 0.0;
		else if(boundary == 1)//Neumann
		{
			if(i == 0) phi.r = texture2DRect(samplerPhi, texPos + vec2( 1.0, 0.0)).r;
			if(i == texWidth-1) phi.r = texture2DRect(samplerPhi, texPos + vec2(-1.0, 0.0)).r;
			if(j == 0) phi.r = texture2DRect(samplerPhi, texPos + vec2(0.0, 1.0)).r;
			if(j == texHeight-1) phi.r = texture2DRect(samplerPhi, texPos + vec2(0.0,-1.0)).r;
		}
	}

	gl_FragColor = phi;
}

void methodCIP(inout vec4 phi)
{
	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dy;
	float f, gx, gy, fip, fjp, gxip, gxjp, gyip, gyjp, fpp; 
	float ip, jp;
	

	vec2 velocity = texture2DRect( samplerVel, texPos ).rg;
 
	f = phi.r; gx = phi.g; gy = phi.b;

	if(velocity.r > 0.0) sx = 1.0; else sx = -1.0;
	if(velocity.g > 0.0) sy = 1.0; else sy = -1.0;
	
	ip = - sx;
	jp = - sy;
	//微小移動分
	x = - velocity.r * deltaT;
	y = - velocity.g * deltaT;
	dx =  sx * delta.x;
	dy =  sy * delta.y;
	//上流点の物理量
	vec4 phi_ip = texture2DRect( samplerPhi, texPos + vec2(ip, 0.0) );
	vec4 phi_jp = texture2DRect( samplerPhi, texPos + vec2(0.0, jp) );
	vec4 phi_pp = texture2DRect( samplerPhi, texPos + vec2(ip, jp) ); 	
    fip = phi_ip.r; fjp = phi_jp.r; fpp = phi_pp.r;
    gxip = phi_ip.g; gxjp = phi_jp.g; 
    gyip = phi_ip.b; gyjp = phi_jp.b;
    
	//係数
	c30 = ((gxip + gx) * dx - 2.0 * (f - fip)) / (dx*dx*dx);
	c20 = (3.0 * (fip - f) + (gxip + 2.0 * gx) * dx) / (dx * dx);
	c03 = ((gyjp + gy) * dy - 2.0 * (f - fjp)) / (dy*dy*dy);
	c02 = (3.0 * (fjp - f) + (gyjp + 2.0 * gy) * dy) / (dy * dy);
	a = f - fjp - fip + fpp;
	b = gyip - gy;
	c12 = (-a - b * dy) / (dx * dy * dy);
	c21 = (-a - (gxjp - gx) * dx) / (dx*dx*dy);
	c11 = - b / dx + c21 * dx;

	//更新
	phi.r += ((c30 * x + c21 * y + c20) * x + c11 * y + gx) * x
		        + ((c03 * y + c12 * x + c02) * y + gy) * y;
	phi.g += (3.0 * c30 * x + 2.0 * (c21 * y + c20)) * x + (c12 * y + c11) * y;
	phi.b += (3.0 * c03 * y + 2.0 * (c12 * x + c02)) * y + (c21 * x + c11) * x;
	
	//非移流項
	float velocityPX = texture2DRect( samplerVel, texPos + vec2( 1.0, 0.0) ).r;
	float velocityMX = texture2DRect( samplerVel, texPos + vec2(-1.0, 0.0) ).r;
	float velocityPY = texture2DRect( samplerVel, texPos + vec2( 0.0, 1.0) ).g;
	float velocityMY = texture2DRect( samplerVel, texPos + vec2( 0.0,-1.0) ).g;
	phi.r += -f * deltaT * ((velocityPX - velocityMX) / abs(dx) + (velocityPY - velocityMY) / abs(dy));

	if(phi.r > 1.0) phi.r = 1.0;
	if(phi.r < 0.0) phi.r = 0.0;

}
 