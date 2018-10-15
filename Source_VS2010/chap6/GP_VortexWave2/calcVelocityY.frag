
//const float pi = 3.14159265;
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect samplerVelX;
uniform sampler2DRect samplerVelY;
uniform int texWidth;
uniform int texHeight;
uniform float deltaT, DX, DY, Re;

varying vec2 texPos;

void main(void)
{
	//CIP�@�ő��xvelY���v�Z
	int NX = texWidth - 1;//x����������
	int NY = texHeight -1;//y����������

	float velX = texture2DRect(samplerVelX, texPos).r;
	float velY = texture2DRect(samplerVelY, texPos).r;

	float f = velY;
	float gx  = texture2DRect(samplerVelY, texPos).g;//���ړ_�̑��x��x����
	float gy  = texture2DRect(samplerVelY, texPos).b;//���ړ_�̑��x��y����
	
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
		f_iup = texture2DRect(samplerVelY, texPos + vec2(-sx, 0.0)).r;//x��������_�̑��x	
		f_jup = texture2DRect(samplerVelY, texPos + vec2( 0.0,-sy)).r;//y��������_�̑��x	
		f_iup_jup = texture2DRect(samplerVelY, texPos + vec2(-sx, -sy)).r;//xy��������_�̑��x	
		gx_iup = texture2DRect(samplerVelY, texPos + vec2(-sx, 0.0)).g;
		gx_jup = texture2DRect(samplerVelY, texPos + vec2( 0.0,-sy)).g;
		gy_iup = texture2DRect(samplerVelY, texPos + vec2(-sx, 0.0)).b;
		gy_jup = texture2DRect(samplerVelY, texPos + vec2( 0.0,-sy)).b;

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

		//�S�����ɒ�������
		int ip = i+1, im = i-1, jp = j+1, jm = j-1;
		float f_ip, f_im, f_jp, f_jm;
		f_im = texture2DRect(samplerVelY, texPos + vec2(-1.0, 0.0)).r; 
		f_ip = texture2DRect(samplerVelY, texPos + vec2(1.0, 0.0)).r; 
		f_jm = texture2DRect(samplerVelY, texPos + vec2(0.0, -1.0)).r;
		f_jp = texture2DRect(samplerVelY, texPos + vec2(0.0, 1.0)).r;
		
		f += deltaT * ( (f_im + f_ip - 2.0 * f) / dx2 
			                    + (f_jm + f_jp - 2.0 * f) / dy2 ) / Re;
	}
		
	//���x�x�N�g���̍X�V
	float prs_jp = texture2DRect(samplerVelX, texPos + vec2(0.0, 1.0)).a;
	float prs_jm = texture2DRect(samplerVelX, texPos + vec2(0.0,-1.0)).a;
	velY = f - 0.5 * deltaT * (prs_jp - prs_jm) / DY;

	//�Q�x�̌v�Z�i�\���p�j
	float vy_im = texture2DRect(samplerVelY, texPos + vec2(-1.0, 0.0)).r;
	float vy_ip = texture2DRect(samplerVelY, texPos + vec2( 1.0, 0.0)).r;
	float vx_jm = texture2DRect(samplerVelX, texPos + vec2( 0.0,-1.0)).r;
	float vx_jp = texture2DRect(samplerVelX, texPos + vec2( 0.0, 1.0)).r;
	float omg = 0.5 * ((vy_ip - vy_im) / DX - (vx_jp - vx_jm) / DY);

	vec4 vel = vec4(velY, gx, gy, omg);

	gl_FragColor = vel;
}

