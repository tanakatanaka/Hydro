const float pi = 3.14159265;
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform int texWidth, texHeight;
uniform float deltaT, DX, DY, Re;

varying vec2 texPos;

void main(void)
{
	//CIP法で速度velXを計算
	int NX = texWidth - 1;//x方向分割数
	int NY = texHeight -1;//y方向分割数

	float velX = texture2DRect(samplerVelX, texPos).r;
	float velY = texture2DRect(samplerVelY, texPos).r;
	float f = velX;
	float gx  = texture2DRect(samplerVelX, texPos).g;//注目点の速度のx微分
	float gy  = texture2DRect(samplerVelX, texPos).b;//注目点の速度のy微分
	
	float c11, c12, c21, c02, c30, c20, c03, a, b, sx, sy, x, y, dx, dy,
	      dx2, dy2, dx3, dy3;
	float f_iup, f_jup, f_iup_jup, gx_iup, gx_jup, gy_iup, gy_jup; 
	int i, j, im, ip, jm, jp;
	
    i = int(texPos.x);
    j = int(texPos.y);	

	if(i > 0 && i < NX && j > 0 && j < NY)
	{
		if(velX >= 0.0) sx = 1.0; else sx = -1.0;
		if(velY >= 0.0) sy = 1.0; else sy = -1.0;

		x = - velX * deltaT;
		y = - velY * deltaT;
		f_iup = texture2DRect(samplerVelX, texPos + vec2(-sx, 0.0)).r;//x方向風上点の速度	
		f_jup = texture2DRect(samplerVelX, texPos + vec2( 0.0,-sy)).r;//y方向風上点の速度	
		f_iup_jup = texture2DRect(samplerVelX, texPos + vec2(-sx, -sy)).r;//xy方向風上点の速度	
		gx_iup = texture2DRect(samplerVelX, texPos + vec2(-sx, 0.0)).g;
		gx_jup = texture2DRect(samplerVelX, texPos + vec2( 0.0,-sy)).g;
		gy_iup = texture2DRect(samplerVelX, texPos + vec2(-sx, 0.0)).b;
		gy_jup = texture2DRect(samplerVelX, texPos + vec2( 0.0,-sy)).b;

		dx = sx * DX;
		dy = sy * DY;
		dx2 = dx * dx;
		dy2 = dy * dy;
		dx3 = dx2 * dx;
		dy3 = dy2 * dy;
		
		c30 = ((gx_iup + gx) * dx - 2.0 * (f - f_iup) ) / dx3;
		c20 = (3.0 * (f_iup - f)  + (gx_iup + 2.0 * gx) * dx) / dx2;
		c03 = ((gy_jup + gy) * dy - 2.0 * (f - f_jup) ) / dy3;
		c02 = (3.0 * (f_jup - f) + (gy_jup + 2.0 * gy) * dy) / dy2;
		a = ( f - f_jup - f_iup + f_iup_jup) ;
		b = gy_iup - gy;
		c12 = (- a - b * dy) / (dx * dy2);
		c21 = (- a - (gx_jup - gx) * dx) / (dx2*dy);
		c11 = -b / dx + c21 * dx;

		f += ( ((c30 * x + c21 * y + c20) * x + c11 * y + gx) * x
			       + ((c03 * y + c12 * x + c02) * y + gy) * y ) ;

		gx += ((3.0 * c30 * x + 2.0 * (c21 * y + c20)) * x + (c12 * y + c11) * y);
		gy += ((3.0 * c03 * y + 2.0 * (c12 * x + c02)) * y + (c21 * x + c11) * x);

		//粘性項に中央差分
		int ip = i+1, im = i-1, jp = j+1, jm = j-1;
		float f_ip, f_im, f_jp, f_jm;
		f_im = texture2DRect(samplerVelX, texPos + vec2(-1.0, 0.0)).r; 
		f_ip = texture2DRect(samplerVelX, texPos + vec2(1.0, 0.0)).r; 
		f_jm = texture2DRect(samplerVelX, texPos + vec2(0.0, -1.0)).r;
		f_jp = texture2DRect(samplerVelX, texPos + vec2(0.0, 1.0)).r;
		
		f += deltaT * ( (f_im + f_ip - 2.0 * f) / dx2 
			                    + (f_jm + f_jp - 2.0 * f) / dy2 ) / Re;
	}
		
//	vec4 vel = vec4(f, gx, gy, 0.0);
	
//----------------------------------------------------------------------
	//圧力をポアソン方程式で計算
	float prs = texture2DRect(samplerVelX, texPos).a;//圧力
	float A1 = 0.5 * DY*DY / (DX*DX + DY*DY);
	float A2 = 0.5 * DX*DX / (DX*DX + DY*DY);
	float A3 = 0.25 * DY*DY / (1.0 + DY*DY / (DX*DX));

	float prs_im, prs_ip, prs_jm, prs_jp;//隣接格子点の圧力
	float vx_im, vx_ip, vy_jm, vy_jp;//速度

	prs_im = texture2DRect(samplerVelX, texPos + vec2(-1.0, 0.0)).a;	
	prs_ip = texture2DRect(samplerVelX, texPos + vec2( 1.0, 0.0)).a;
	prs_jm = texture2DRect(samplerVelX, texPos + vec2( 0.0,-1.0)).a;
	prs_jp = texture2DRect(samplerVelX, texPos + vec2( 0.0, 1.0)).a;
	vx_im = texture2DRect(samplerVelX, texPos + vec2(-1.0, 0.0)).r;
	vx_ip = texture2DRect(samplerVelX, texPos + vec2( 1.0, 0.0)).r;
	vy_jm = texture2DRect(samplerVelY, texPos + vec2( 0.0,-1.0)).r;
	vy_jp = texture2DRect(samplerVelY, texPos + vec2( 0.0, 1.0)).r;
	
	float d;
	//簡略化したポアソンの方程式の解
	if(i > 0 && i < NX && j > 0 && j < NY)
	{
		d = A3 * ( (vx_ip - vx_im)/DX + (vy_jp - vy_jm)/DY ) / deltaT;
		prs = A1 * ( prs_im + prs_ip) + A2 * (prs_jm + prs_jp) - d;		   
	}
	if(i <= 1 || i >= NX-1 || j <= 1 || j >= NY-1) prs = 0.0;//発散防止
	
	//速度ベクトルの更新
	velX = f - 0.5 * deltaT * (prs_ip - prs_im) / DX;

	vec4 vel = vec4(velX, gx, gy, prs);

	gl_FragColor = vel;
}

