//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Texture
//------------------------------------------------------------------------------
//---------------------------------------------------------------------

void drawTexPlate(float size, int nRepeatS, int nRepeatT)//x-y平面
{
	float sz = 0.5 * size;
	static float p[4][3] = //z:上方向
	{ 
		{ sz,-sz, 0.0}, { sz, sz, 0.0}, 
		{-sz, sz, 0.0}, {-sz,-sz, 0.0}
	};

	float s = (float)nRepeatS;
	float t = (float)nRepeatT;
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0); //z方向の法線
		//ﾃｸｽﾁｬｰ座標と頂点番号との対応付け
		glTexCoord2f(0.0, 0.0); glVertex3fv(p[3]);
		glTexCoord2f( s , 0.0); glVertex3fv(p[0]);
		glTexCoord2f( s ,  t ); glVertex3fv(p[1]);
		glTexCoord2f(0.0,  t ); glVertex3fv(p[2]);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

//-----------------------------------------------------------------------
//正面だけﾏｯﾋﾟﾝｸﾞ(z軸が上方向）
void drawTexCube1(float size, int nRepeatS, int nRepeatT)
{
	float sz = 0.5 * size;
	float p[8][3] = 
	{ {sz,sz,sz}, {-sz,sz,sz}, {-sz,-sz,sz}, 
	  {sz,-sz,sz},{sz,sz,-sz}, {-sz,sz,-sz},
	  {-sz,-sz,-sz}, {sz,-sz,-sz}
	};

	float s = (float)nRepeatS;
	float t = (float)nRepeatT;

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f); //z方向
		glVertex3fv(p[0]); glVertex3fv(p[1]);
		glVertex3fv(p[2]); glVertex3fv(p[3]);
	glEnd();

	glEnable(GL_TEXTURE_2D);//正面だけマッピング
	glBegin(GL_QUADS);
		glNormal3f(1.0,0.0,0.0); //x方向(正面）
		glTexCoord2f( s ,  t ); glVertex3fv(p[0]);
		glTexCoord2f(0.0,  t ); glVertex3fv(p[3]);
		glTexCoord2f(0.0, 0.0); glVertex3fv(p[7]);
		glTexCoord2f( s , 0.0); glVertex3fv(p[4]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f); //y方向
		glVertex3fv(p[0]); glVertex3fv(p[4]);
		glVertex3fv(p[5]); glVertex3fv(p[1]);

		glNormal3f(-1.0f,0.0f,0.0f); //-x方向
		glVertex3fv(p[1]); glVertex3fv(p[5]);
		glVertex3fv(p[6]); glVertex3fv(p[2]);

		glNormal3f(0.0f,-1.0f,0.0f); //-y方向
		glVertex3fv(p[2]); glVertex3fv(p[6]);
		glVertex3fv(p[7]); glVertex3fv(p[3]);

		glNormal3f(0.0f,0.0f,-1.0f); //-z方向
		glVertex3fv(p[4]); glVertex3fv(p[7]);
		glVertex3fv(p[6]); glVertex3fv(p[5]);
	glEnd();
}
//-----------------------------------------------------------------------
//６面に同じ模様をﾏｯﾋﾟﾝｸﾞ
void drawTexCube2(float size, int nRepeatS, int nRepeatT)
{
	float sz = 0.5 * size;
	float p[8][3] =
	{ { sz, sz, sz}, {-sz, sz, sz}, {-sz,-sz, sz}, 
	  { sz,-sz, sz}, { sz, sz,-sz}, {-sz, sz,-sz},
	  {-sz,-sz,-sz}, { sz,-sz,-sz}
	};

	float s = (float)nRepeatS;
	float t = (float)nRepeatT;

	glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
	//top(z軸が鉛直軸のときz方向）
	glNormal3f(0.0f,0.0f,1.0f); 
	glTexCoord2f( s , 0.0); glVertex3fv(p[0]);
	glTexCoord2f( s ,  t ); glVertex3fv(p[1]);
	glTexCoord2f(0.0,  t ); glVertex3fv(p[2]);
	glTexCoord2f(0.0, 0.0); glVertex3fv(p[3]);
	//x方向(正面）
	glNormal3f(1.0f,0.0f,0.0f); 
	glTexCoord2f( s ,  t ); glVertex3fv(p[0]);
	glTexCoord2f(0.0,  t ); glVertex3fv(p[3]);
	glTexCoord2f(0.0, 0.0); glVertex3fv(p[7]);
	glTexCoord2f( s , 0.0); glVertex3fv(p[4]);
	//y方向
	glNormal3f(0.0f,1.0f,0.0f);
	glTexCoord2f(0.0,  t ); glVertex3fv(p[0]);
	glTexCoord2f(0.0, 0.0); glVertex3fv(p[4]);
	glTexCoord2f( s , 0.0); glVertex3fv(p[5]);
	glTexCoord2f( s ,  t ); glVertex3fv(p[1]);
	//-x方向
	glNormal3f(-1.0f,0.0f,0.0f); 
	glTexCoord2f(0.0,  t ); glVertex3fv(p[6]);
	glTexCoord2f(0.0, 0.0); glVertex3fv(p[2]);
	glTexCoord2f( s , 0.0); glVertex3fv(p[1]);
	glTexCoord2f( s ,  t ); glVertex3fv(p[5]);
	//-y方向
	glNormal3f(0.0f,-1.0f,0.0f); 
	glTexCoord2f(0.0,  t ); glVertex3fv(p[2]);
	glTexCoord2f(0.0, 0.0); glVertex3fv(p[6]);
	glTexCoord2f( s , 0.0); glVertex3fv(p[7]);
	glTexCoord2f( s ,  t ); glVertex3fv(p[3]);
	//-z方向
	glNormal3f(0.0f,0.0f,-1.0f); 
	glTexCoord2f(0.0, 0.0); glVertex3fv(p[6]);
	glTexCoord2f( s , 0.0); glVertex3fv(p[5]);
	glTexCoord2f( s ,  t ); glVertex3fv(p[4]);
	glTexCoord2f(0.0,  t ); glVertex3fv(p[7]);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

//--------------------------------------------------------------------------
//正面側(x>0)に平行投影
void drawTexSphere1(double radius, int nSlice, int nStack)
{
	int i, j;
	double r0, r1, th0, th1, phi;
	double p[2][3];

	//前面にだけ平行投影
	glEnable(GL_TEXTURE_2D);
	for(j = 0; j < nStack; j++)
	{
		//j=0は北極点(x=0, y=0, z=radius)
		//これらの天頂角
		th0 = M_PI * (double)j / (double)nStack;
		th1 = M_PI * (double)(j+1) / (double)nStack;
		//x-y平面に投影した半径
		r0 = radius * sin(th0);
		r1 = radius * sin(th1);
		//頂点z座標
		p[0][2] = radius * cos(th0);
		p[1][2] = radius * cos(th1);

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSlice/2; i++)
		{
			phi = 2.0 * M_PI * (double)i / (double)nSlice;
			//頂点のxy座標(i=0をobjectからみて右端) 
			p[0][0] =   r0 * sin(phi);//x座標
			p[0][1] = - r0 * cos(phi);//y座標
			p[1][0] =   r1 * sin(phi);//x座標
			p[1][1] = - r1 * cos(phi);//y座標

			glTexCoord2d(0.5 + p[0][1], 0.5 + p[0][2]);//テクスチャ座標
			glNormal3dv(p[0]);//法線ベクトル
			glVertex3dv(p[0]);//頂点座標

			glTexCoord2d(0.5 + p[1][1], 0.5 + p[1][2]);//テクスチャ座標
			glNormal3dv(p[1]);//法線ベクトル
			glVertex3dv(p[1]);//頂点座標			
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

	//後面
	for(j = 0; j < nStack; j++)
	{
		//j=0は北極点(x=0, y=0, z=radius)
		//これらの天頂角
		th0 = M_PI * (double)j / (double)nStack;
		th1 = M_PI * (double)(j+1) / (double)nStack;
		//x-y平面に投影した半径
		r0 = radius * sin(th0);
		r1 = radius * sin(th1);
		//頂点z座標
		p[0][2] = radius * cos(th0);
		p[1][2] = radius * cos(th1);

		glBegin(GL_QUAD_STRIP);
		for(i = nSlice/2; i <= nSlice; i++)
		{
			phi = 2.0 * M_PI * (double)i / (double)nSlice;
			//頂点のxy座標(i=0をobjectからみて右端) 
			p[0][0] =   r0 * sin(phi);//x座標
			p[0][1] = - r0 * cos(phi);//y座標
			p[1][0] =   r1 * sin(phi);//x座標
			p[1][1] = - r1 * cos(phi);//y座標

			glNormal3dv(p[0]);//法線ベクトル
			glVertex3dv(p[0]);//頂点座標

			glNormal3dv(p[1]);//法線ベクトル
			glVertex3dv(p[1]);//頂点座標			
		}
		glEnd();
	}
}
//--------------------------------------------------------------------------
//球投影
void drawTexSphere2(double radius, int nSlice, int nStack, int nRepeatS, int nRepeatT)
{
	int i, j;
	double s, t0, t1, r0, r1, th0, th1, phi;
	double p[2][3];

	for(j = 0; j < nStack; j++)
	{
		//j=0は北極点(x=0, y=0, z=radius)
		//2つのt座標
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//これらの天頂角
		th0 = M_PI * t0;
		th1 = M_PI * t1;
		//x-y平面に投影した半径
		r0 = radius * sin(th0);
		r1 = radius * sin(th1);
		//頂点z座標
		p[0][2] = radius * cos(th0);
		p[1][2] = radius * cos(th1);

		//北極点を1とするt座標
		t0 = (1.0 - t0) * nRepeatT;
		t1 = (1.0 - t1) * nRepeatT;

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s;
			//頂点のxy座標(i=0を真後ろ)
			p[0][0] = - r0 * cos(phi);//x座標
			p[0][1] = - r0 * sin(phi);//y座標
			p[1][0] = - r1 * cos(phi);//x座標
			p[1][1] = - r1 * sin(phi);//y座標

			s *= nRepeatS;

			glTexCoord2d(s, t0);//テクスチャ座標
			glNormal3dv(p[0]);//法線ベクトル,正規化すれば頂点座標に同じ
			glVertex3dv(p[0]);//頂点座標

			glTexCoord2d(s, t1);//テクスチャ座標
			glNormal3dv(p[1]);//法線ベクトル,正規化すれば頂点座標に同じ
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}

//----------------------------------------------------------------------
void drawTexCylinder1(double rBottom, double rTop, double height, 
							int nSlice, int nStack, int nRepeatS, int nRepeatT)
{

	//側面全体に円筒投影
	//円柱(rBottom=rTop))、円錐台、円錐(rTop = 0.0)
	//rBottom:下底半径, rTop:上底半径
	//nSlice--xy断面分割数
	//nStack---ｚ方向分割数
	//物体の中心は下底と上底の中間

	int i, j;
	double x, y, z, z0, z1;
	double theta;
	double theta0 = 2.0*M_PI/(double)nSlice;

	//上底(Top)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, height/2.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		x = rTop * cos(theta); //x成分
		y = rTop * sin(theta); //ｙ成分
		z = height/2.0;
		glVertex3f(x, y, z);
	}
	glEnd();

	//下底(Bottom)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0, 0.0, -height/2.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)(nSlice - i);
		x = rBottom * cos(theta); //x成分
		y = rBottom * sin(theta); //ｙ成分
		z = - height/2.0;
		glVertex3f(x, y, z);
	}
	glEnd();

	double s, t0, t1, r0, r1, phi;
	double p[2][3], n[3];
	double rr = rBottom - rTop;
	double nz = rr / sqrt(rr*rr + height*height);
  double nxy = sqrt(1.0 - nz * nz);

	//側面にテクスチャ
	for(j = 0; j < nStack; j++)
	{
		//j=0は上底(x=0, y=0, z=height/2)
		//2つのt座標
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//底面からの高さ
		z0 = height * (1.0 - t0);
		z1 = height * (1.0 - t1);
		//半径
		r0 = rBottom + (rTop - rBottom) * z0 / height;
		r1 = rBottom + (rTop - rBottom) * z1 / height;

		//頂点z座標
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;

		//Topを1とするt座標
		t0 = (1.0 - t0) * nRepeatT;
		t1 = (1.0 - t1) * nRepeatT;

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s;
			//頂点のxy座標(i=0を真後ろ)
			p[0][0] = - r0 * cos(phi);//x座標
			p[0][1] = - r0 * sin(phi);//y座標
			p[1][0] = - r1 * cos(phi);//x座標
			p[1][1] = - r1 * sin(phi);//y座標
			//法線ベクトル
			n[0] = -nxy * cos(phi);
			n[1] = -nxy * sin(phi);
			n[2] =  nz;

			s *= nRepeatS;

			glTexCoord2d(s, t0);//テクスチャ座標
			glNormal3dv(n);     //法線ベクトル
			glVertex3dv(p[0]);  //頂点座標

			glTexCoord2d(s, t1);//テクスチャ座標
			glVertex3dv(p[1]);  //頂点座標
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

}

//-------------------------------------------------------------------------------------
void drawTexCylinder2(double rBottom, double rTop, double height, double thick, 
											int nSlice, int nStack, int nRepeatS, int nRepeatT)
{
	//厚みのある中空円柱(外側側面に円筒投影)
	//円柱(rBottom=rTop))、円錐台、円錐(rTop = 0.0)
	//rBottom:下底半径, rTop:上底半径, thick:厚さ
	//nSlice--xy断面分割数
	//nStack---ｚ方向分割数
	//物体の中心は下底と上底の中間

	int i, j;
	float x, y, z, z0, z1;
	double theta;
	double theta0 = 2.0*M_PI/(double)nSlice;
	//上底(Top)
	glBegin(GL_QUAD_STRIP);
	glNormal3f(0.0, 0.0, 1.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		z = height/2.0;
		//内側座標
		x = (rTop - thick) * cos(theta); //x成分
		y = (rTop - thick) * sin(theta); //y成分
		glVertex3f(x, y, z);

		//外側座標
		x = rTop * cos(theta); //x成分
		y = rTop * sin(theta); //y成分
		glVertex3f(x, y, z);
	}
	glEnd();

	//下底(Bottom)
	glBegin(GL_QUAD_STRIP);
	glNormal3f(0.0, 0.0, -1.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		z = -height/2.0;
		//外側座標
		x = (float)(rBottom * cos(theta)); //x成分
		y = (float)(rBottom * sin(theta)); //y成分
		glVertex3f(x, y, z);
		//内側座標
		x = (float)((rBottom - thick) * cos(theta)); //x成分
		y = (float)((rBottom - thick) * sin(theta)); //y成分
		glVertex3f(x, y, z);
	}
	glEnd();

	double s, t0, t1, r0, r1, phi;
	double p[2][3], n[3];

	double rr = rBottom - rTop;
	double nz = rr / sqrt(rr*rr + height*height);
  double nxy = sqrt(1.0 - nz * nz);

	//外側側面にだけテクスチャ
	for(j = 0; j < nStack; j++)
	{
		//j=0は上底(x=0, y=0, z=height/2)
		//2つのt座標
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//底面からの高さ
		z0 = height * (1.0 - t0);
		z1 = height * (1.0 - t1);
		//半径
		r0 = rBottom + (rTop - rBottom) * z0 / height;
		r1 = rBottom + (rTop - rBottom) * z1 / height;

		//頂点z座標
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;
		
		//Topを1とするt座標
		t0 = (1.0 - t0) * nRepeatT;
		t1 = (1.0 - t1) * nRepeatT;

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s;
			//頂点のxy座標(i=0を真後ろ)
			p[0][0] = - r0 * cos(phi);//x座標
			p[0][1] = - r0 * sin(phi);//y座標
			p[1][0] = - r1 * cos(phi);//x座標
			p[1][1] = - r1 * sin(phi);//y座標
			//法線ベクトル
			n[0] = -nxy * cos(phi);
			n[1] = -nxy * sin(phi);
			n[2] =  nz;

			s *= nRepeatS;

			glTexCoord2d(s, t0);//テクスチャ座標
			glNormal3dv(n);//法線ベクトル
			glVertex3dv(p[0]);//頂点座標

			glTexCoord2d(s, t1);//テクスチャ座標
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	//内側側面
	for(j = 0; j < nStack; j++)
	{
		//j=0は上底(x=0, y=0, z=height/2)
		//2つのt座標
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//底面からの高さ
		z0 = height * (1.0 - t0);
		z1 = height * (1.0 - t1);
		//半径
		r0 = rBottom - thick + (rTop - rBottom) * z0 / height;
		r1 = rBottom - thick + (rTop - rBottom) * z1 / height;

		//頂点z座標
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = nSlice; i >= 0; i--)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s;
			//頂点のxy座標(i=0を真後ろ)
			p[0][0] = - r0 * cos(phi);//x座標
			p[0][1] = - r0 * sin(phi);//y座標
			p[1][0] = - r1 * cos(phi);//x座標
			p[1][1] = - r1 * sin(phi);//y座標

			//法線ベクトル
			n[0] = nxy * cos(phi);
			n[1] = nxy * sin(phi);
			n[2] = - nz;
			
			glNormal3dv(n);//法線ベクトル
			glVertex3dv(p[0]);//頂点座標
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
	}
}

//----------------------------------------------------------------------
void drawTexPrism1(float rBottom, float rTop, float height, int nSlice, int nStack)
{
	//側面全体に円筒投影
	//円柱(rBottom=rTop))、円錐台、円錐(rTop = 0.0)
	//rBottom:下底半径, rTop:上底半径
	//nSlice--xy断面分割数
	//nStack---ｚ方向分割数
	//物体の中心は下底と上底の中間
	float p[31][31][3];
	float n[30][3];
	double theta0, theta, z, r;
	int i, ii, j;

	if(nSlice > 30) nSlice = 30;
	if(nStack > 30) nStack = 30;
	
	theta0 = 2*M_PI/(double)nSlice;
	for(j = 0; j <= nStack; j++)
	{
		z = height * (1.0 - (double)j/(double)nStack);//底面からの高さ
		r = rBottom + (rTop - rBottom) * z / height;
		for(i=0;i<nSlice;i++)
		{
			theta = theta0*(double)i;
			p[i][j][0] = (float)(r * cos(theta)); //x成分
			p[i][j][1] = (float)(r * sin(theta)); //ｙ成分
			p[i][j][2] = (float)z-0.5f*height;    //ｚ成分(高さ)
		}
	}

		//上底(Top)
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f,0.0f,1.0f);
		for(i = 0; i < nSlice; i++) 
		{ 
			ii = i + 1;
			if(ii == nSlice) ii = 0;
			glVertex3f(0.0f, 0.0f, height/2.0f);
			glVertex3fv(p[i][0]);
			glVertex3fv(p[ii][0]);
		}
	glEnd();
	//下底(Bottom)
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f,0.0f,-1.0f); 
		for(i = nSlice-1; i >= 0; i--) 
		{
			ii = i - 1;
			if(i == 0) ii = nSlice - 1;
			glVertex3f(0.0f, 0.0f, -height/2.0f);
			glVertex3fv(p[i][nStack]);
			glVertex3fv(p[ii][nStack]);
		}
	glEnd();

	//側面
	//方向余弦
	double nz, nxy, rr;
	rr = rBottom - rTop;
	nz = rr / sqrt(rr*rr + height * height);
	nxy = sqrt(1.0 - nz*nz);
	for(i=0; i<nSlice;i++)
	{//一周分だけでよい
		n[i][0] = (float)(nxy * cos(theta0 * (double)i));
		n[i][1] = (float)(nxy * sin(theta0 * (double)i));
		n[i][2] = (float)nz;
	}
	for(i=0; i < nSlice;i++)
	{//隣との和の平均
		ii = i+1;
		if(ii == nSlice) ii = 0;
		n[i][0] = (n[i][0] + n[ii][0]) / 2.0f;
		n[i][1] = (n[i][1] + n[ii][1]) / 2.0f;
	}

	//円筒投影
	//側面一周にﾏｯﾋﾟﾝｸﾞ
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	float s1, t1, s2, t2;//ﾃｸｽﾁｬ座標
	for(j=0; j<nStack; j++)
		for(i=0;i<nSlice;i++)
		{
			ii = i+1;
			if(ii == nSlice) ii = 0;
			s1 = (float)i / (float)nSlice; s2 = (float)(i+1) / (float)nSlice;
			t1 = 1.0f -(float)j / (float)nStack; t2 = 1.0f -(float)(j+1) / (float)nStack;
			glNormal3fv(n[i]);
			glTexCoord2f(s1, t1); glVertex3fv(p[i][j]);
			glTexCoord2f(s1, t2); glVertex3fv(p[i][j+1]);
			glTexCoord2f(s2, t2); glVertex3fv(p[ii][j+1]); 
			glTexCoord2f(s2, t1); glVertex3fv(p[ii][j]);
		}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

//-------------------------------------------------------------------------------------
void drawTexPrism2(float rBottom, float rTop, float height, float thick, int nSlice, int nStack)
{
	//厚みのある中空円柱(外側側面に円筒投影)
	//円柱(rBottom=rTop))、円錐台、円錐(rTop = 0.0)
	//rBottom:下底半径, rTop:上底半径, thick:厚さ
	//nSlice--xy断面分割数
	//nStack---ｚ方向分割数
	//物体の中心は下底と上底の中間
	float p[961*2][3];
	float n[30][3];
	double theta0,theta, z, r1, r2;
	double nz, nxy, rr;
	int i, ii, j, N;

	if(nSlice > 30) nSlice = 30;
	if(nStack > 30) nStack = 30;
	
	N = nSlice * (nStack+1);

	theta0 = 2*M_PI/(double)nSlice;
	for(j=0; j<=nStack; j++)
	{
		z = height * ( 1.0 - (double)j/(double)nStack );//底面からの高さ
		r1 = rBottom + (rTop - rBottom) * z / height;//外側半径
		r2 = r1 - thick;                       //内側半径
		for(i=0;i<nSlice;i++)
		{
			theta = theta0*(double)i;
			p[j*nSlice+i][0] = (float)(r1 * cos(theta)); //x成分
			p[j*nSlice+i][1] = (float)(r1 * sin(theta)); //ｙ成分
			p[j*nSlice+i][2] = (float)z-0.5f * height;           //ｚ成分(高さ)
		}
		//内側
		for(i=0;i<nSlice;i++)
		{
			theta = theta0*(double)i;
			p[j*nSlice+i+N][0] = (float)(r2 * cos(theta)); //x成分
			p[j*nSlice+i+N][1] = (float)(r2 * sin(theta)); //ｙ成分
			p[j*nSlice+i+N][2] = (float)z-0.5f*height;     //ｚ成分(高さ)
		}

	}

	//内側側面（内側から描画）
	//方向余弦
	rr = rBottom - rTop;
	nz = rr / sqrt(rr*rr + height*height);
	nxy = sqrt(1.0 - nz * nz);
	for(i=0; i<nSlice;i++)
	{//一周分だけでよい
		n[i][0] = -(float)(nxy * cos(theta0 * (double)i));
		n[i][1] = -(float)(nxy * sin(theta0 * (double)i));
		n[i][2] = -(float)nz;
	}
	for(i=0; i < nSlice;i++)
	{//隣との和の平均
		ii = i+1;
		if(ii == nSlice) ii = 0;
		n[i][0] = (n[i][0] + n[ii][0]) / 2.0f;
		n[i][1] = (n[i][1] + n[ii][1]) / 2.0f;
	}


	//内側側面の描画
	glBegin(GL_QUADS);
	for(j=0; j<nStack; j++)
		for(i=0;i<nSlice;i++)
		{
			ii = i+1;
			if(ii == nSlice) ii = 0;
			glNormal3fv(n[i]);
			glVertex3fv(p[j*nSlice+i+N]);
			glVertex3fv(p[j*nSlice+ii+N]);
			glVertex3fv(p[j*nSlice+ii+nSlice+N]); 
			glVertex3fv(p[j*nSlice+i+nSlice+N]);
		}
	glEnd();

	//外側側面
	//方向余弦
	for(i=0; i<nSlice;i++)
	{//内側法線方向の逆方向
		n[i][0] = -n[i][0];
		n[i][1] = -n[i][1];
		n[i][2] = -n[i][2];
	}
	//円筒投影
	//側面一周にﾏｯﾋﾟﾝｸﾞ
	float s1, t1, s2, t2;
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for(j=0; j<nStack; j++)
	{
		for(i=0;i<nSlice;i++)
		{
			ii = i+1;
			if(ii == nSlice) ii = 0;
			s1 = (float)i / (float)nSlice; s2 = (float)(i+1) / (float)nSlice;
			t1 = 1.0f -(float)j / (float)nStack; t2 = 1.0f -(float)(j+1) / (float)nStack;
			glNormal3fv(n[i]);
			glTexCoord2f(s1, t1); glVertex3fv(p[j*nSlice+i]);
			glTexCoord2f(s1, t2); glVertex3fv(p[j*nSlice+i+nSlice]);
			glTexCoord2f(s2, t2); glVertex3fv(p[j*nSlice+ii+nSlice]); 
			glTexCoord2f(s2, t1); glVertex3fv(p[j*nSlice+ii]);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	//上底
	for(i=0;i<nSlice;i++)
	{
		ii = i+1;
		if(ii == nSlice) ii = 0;
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3fv(p[i]); glVertex3fv(p[ii]);
		glVertex3fv(p[N+ii]); glVertex3fv(p[N+i]);
	}
	//下底
	for(i=0;i<nSlice;i++)
	{
		ii = i+1;
		if(ii == nSlice) ii = 0;
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3fv(p[nSlice*nStack+i]); glVertex3fv(p[nSlice*nStack+N+i]);
		glVertex3fv(p[nSlice*nStack+N+ii]); glVertex3fv(p[nSlice*nStack+ii]);
	}
	glEnd();
}

//-----------------------------------------------------------------------------------------

void drawTexTorus(double radius1, double radius2, 
			int nSide, int nRing, int nRepeatS, int nRepeatT)
{	
	//radius1:円筒断面半径
	//radius2:円筒の中心軸半径
	//nSide:円筒断面における表面分割点数
	//nRing:円筒中心軸に沿った分割点数

	int i, j;
	double rr, zz;
	double s0, s1, t, phi0, phi1, theta;
	double p[2][3];

	for(i = 0; i < nRing; i++)
	{
		//i=0は基本断面(x=radius2を中心とする円, y=0）
		//2つのｓ座標
		s0 = (double)i / (double)nRing;
		s1 = (double)(i+1) / (double)nRing;
		phi0 = 2.0 * M_PI * s0;
		phi1 = 2.0 * M_PI * s1;

		//s座標
		s0 = s0 * (double)nRepeatS;
		s1 = s1 * (double)nRepeatS;

		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= nSide; j++)
		{
			//t座標
			t = (double)j / (double)nSide;
			theta = -M_PI + 2.0 * M_PI * t;
			rr = radius2 + radius1 * cos(theta);//z軸からの距離
			zz = radius1 * sin(theta);
			//頂点のxyz座標(j=0を内側xy平面)
			p[0][0] = rr * cos(phi0);//x座標
			p[0][1] = rr * sin(phi0);//y
			p[0][2] = zz;            //z
			p[1][0] = rr * cos(phi1);//x座標
			p[1][1] = rr * sin(phi1);//y
			p[1][2] = zz;            //z      

		  t = (1.0 - t) * (double)nRepeatT;

			glTexCoord2d(s0, t);//テクスチャ座標
			glNormal3d(cos(theta)*cos(phi0),cos(theta)*sin(phi0),sin(theta));
			glVertex3dv(p[0]);//頂点座標

			glTexCoord2d(s1, t);//テクスチャ座標
			glNormal3d(cos(theta)*cos(phi1),cos(theta)*sin(phi1),sin(theta));
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}
//---------------------------------------------------------------------------------------
void drawTexSpring(float radius1, float radius2, float len, 
					int nSide, int nRing, int nPitch, int nRepeatS, int nRepeatT)
{
	//radius1:円環断面半径
	//radius2:円環の中心軸半径
	//len;ばね長
	//nSide:円環断面における表面分割点数
	//nRing:円環の分割数
	//nPitch:ピッチ数

	if(radius2 < radius1) { 
		printf("drawSpringにおいて radius2 > radius1 としてください \n");
	}

	int i, j, k;
	double rr, zz0, zz1;
	double s0, s1, t, phi0, phi1, theta;
	double p[2][3];
	float pitch, dp;

	pitch = len / (float)nPitch;
	if(pitch < 2.0 * radius1) pitch = radius1 * 2.0;
	dp = (float)pitch / (float)nRing;

	for(k = 0; k < nPitch; k++)
	{
		for(i = 0; i < nRing; i++)
		{
			//i=0は基本断面(x=radius2を中心とする円, y=0）
			//2つのｓ座標
			s0 = (double)i / (double)nRing;
			s1 = (double)(i+1) / (double)nRing;
			phi0 = 2.0 * M_PI * s0;
			phi1 = 2.0 * M_PI * s1;

			//s座標
			s0 = s0 * (double)nRepeatS;
			s1 = s1 * (double)nRepeatS;

			phi0 = 2.0 * M_PI * (double)i / (double)nRing;
			phi1 = 2.0 * M_PI * (double)(i+1) / (double)nRing;

			glBegin(GL_QUAD_STRIP);
			for(j = 0; j <= nSide; j++)
			{
				//t座標
				t = (double)j / (double)nSide;
				theta = -M_PI + 2.0 * M_PI * t;
				rr = radius2 + radius1 * cos(theta);//z軸からの距離
				zz0 = radius1 * sin(theta) + dp * (double)i + pitch * (double)k;
				zz1 = radius1 * sin(theta) + dp * (double)(i+1) + pitch * (double)k;
				//頂点のxyz座標(j=0を内側xy平面)
				p[0][0] = rr * cos(phi0);//x座標
				p[0][1] = rr * sin(phi0);//y
				p[0][2] = zz0;           //z
				p[1][0] = rr * cos(phi1);//x座標
				p[1][1] = rr * sin(phi1);//y
				p[1][2] = zz1;           //z      

				t = (1.0 - t) * (double)nRepeatT;
				glTexCoord2d(s0, t);//テクスチャ座標
				glNormal3d(cos(theta)*cos(phi0),cos(theta)*sin(phi0),sin(theta));
				glVertex3dv(p[0]);//頂点座標

				glTexCoord2d(s1, t);//テクスチャ座標
				glNormal3d(cos(theta)*cos(phi1),cos(theta)*sin(phi1),sin(theta));
				glVertex3dv(p[1]);//頂点座標
			}
			glEnd();
		}
	}
	//始端
	glBegin(GL_POLYGON);
	glNormal3f(0.0, -1.0, 0.0);
	for(j = 0; j < nSide; j++)
	{
		theta = - M_PI + 2.0 * M_PI * (double)j / (double)nSide;
		rr = radius2 + radius1 * cos(theta);//z軸からの距離
		zz0 = radius1 * sin(theta);
		glVertex3f(rr, 0.0, zz0);
	}
	glEnd();
	//終端
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	for(j = 0; j < nSide; j++)
	{
		theta = - M_PI - 2.0 * M_PI * (double)j / (double)nSide;
		rr = radius2 + radius1 * cos(theta) ;//z軸からの距離
		zz0 = radius1 * sin(theta) + pitch * (double)nPitch;
		glVertex3f(rr, 0.0, zz0);
	}
	glEnd();
}

//-----------------------------------------------------------------
//超2次関数
void drawTexSuper(float r, int nSlice, int nStack, double eps1, double eps2)
{
	//上下の中心が原点
	int i,j,ip,im,np,npL,npR,npU,npD,k1,k2;
	float s1,t1,s2,t2;
	double ct,phi,theta,z,cc;
	float a[31][31], b[31][31], c[31][31];
	float n1[3], n2[3], n3[3], n4[3];
	float pd[31*31][3];

	if(nSlice > 30) nSlice = 30;
	if(nStack > 30) nStack = 30;

	for(j = 0 ;j <= nStack;j++)
	{
		phi = (M_PI/(double)nStack) * ((double)nStack / 2.0 - (double)j);
		if(phi >= 0.0) //上半分
		{
			if(phi == 0.0) z = 0.0;//pow(a,b)のaがa<=0.0でエラー
			else z = pow(sin(fabs(phi)),eps1);//z
		}
		else  //下半分        
		{
			z = - pow(sin(fabs(phi)), eps1);
		}
		for (i = 0 ;i <= nSlice / 2;i++)
		{
			k1 = nSlice * j + i;//自分から見て左側
			k2 = nSlice * j + nSlice - i;//右側
			theta = 2.0 * M_PI * (double)i/(double)nSlice;
			ct = cos(theta);
			if( ct == 0.0 ) cc = 0.0;
			else if (ct > 0) { cc = pow(ct, eps2);}
			else         { cc = -pow(fabs(ct),eps2); }
			if(j == 0 || j == nStack) 
			{
				pd[k1][0] = 0.0f;
				pd[k1][1] = 0.0f;
			}

			else 
			{
				pd[k1][0] = r * (float)(pow(cos(phi),eps1)*cc);
				if(sin(theta) == 0.0) pd[k1][1] = 0.0f;
				else pd[k1][1] = r * (float)(pow(cos(phi),eps1)*pow(fabs(sin(theta)),eps2));
			}
			if(i == 0) k2 = k1;
			pd[k2][0] = pd[k1][0];
			pd[k2][1] = -pd[k1][1];
			pd[k1][2] = r * (float)z;
			pd[k2][2] = r * (float)z;
		}
	}

	//側面の法線成分
	for(i = 0;i < nSlice;i++)
	{
		ip = i+1;
		if(ip == nSlice) ip = 0;
		im = i-1;
		if(i == 0) im = nSlice-1;

		//真上(Top)
		a[i][0] = 0.0f; b[i][0] = 0.0f; c[i][0] = 1.0f;
		//真下（Bottom)
		a[i][nStack] = 0.0f; b[i][nStack] = 0.0f; c[i][nStack] = -1.0f;

		for(j=1;j<nStack;j++)//隣り合う4個の三角形の法線ベクトルを平均化
		{
			np = j*nSlice+i;//注目点
			npL = j*nSlice+im;//左側
			npR = j*nSlice+ip;//右側
			npU = np-nSlice;//上
			npD = np+nSlice;//下
			if(j == 1) 
			{
				n1[0]=0.0f; n1[1]=0.0f; n1[2]=1.0f;//Top
				n2[0]=0.0f; n2[1]=0.0f; n2[2]=1.0f;//Top
				calcNormal(pd[np],pd[npL],pd[npD],n3);//外から見て左下
				calcNormal(pd[np],pd[npD],pd[npR],n4);//右下
			}
			if(j == nStack-1)
			{
				calcNormal(pd[np],pd[npU],pd[npL],n1);//外から見て左上
				calcNormal(pd[np],pd[npR],pd[npU],n2);//右上
				n3[0]=0.0f; n3[1]=0.0f; n3[2]=-1.0f;//Bottom
				n4[0]=0.0f; n4[1]=0.0f; n4[2]=-1.0f;//Bottom
			}
			else 
			{
				calcNormal(pd[np],pd[npU],pd[npL],n1);//外から見て左上
				calcNormal(pd[np],pd[npR],pd[npU],n2);//右上
				calcNormal(pd[np],pd[npL],pd[npD],n3);//外から見て左下
				calcNormal(pd[np],pd[npD],pd[npR],n4);//右下
			}
			a[i][j] = (float)((n1[0]+n2[0]+n3[0]+n4[0])/4.0f);//ｘ方向
			b[i][j] = (float)((n1[1]+n2[1]+n3[1]+n4[1])/4.0f);//ｙ
			c[i][j] = (float)((n1[2]+n2[2]+n3[2]+n4[2])/4.0f);//ｚ
		}
	}
	//全体にﾏｯﾋﾟﾝｸﾞ
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for(i = 0;i < nSlice;i++)
	{
		ip = i + 1;
		if(ip == nSlice) ip = 0;
		s1 = (float)i / (float)nSlice;//2πで正規化した角度
		s2 = (float)(i+1) / (float)nSlice;
		//if(i == nSlice-1) s2 = 1.0f;
		for(j = 0;j < nStack; j++)
		{
			t1 = 1.0f - (float)j / (float)nStack;//j=0で1となるように
			t2 = 1.0f - (float)(j+1) / (float)nStack;

			//左端(p1[1]=-0.5)でﾃｸｽﾁｬｰ座標が0となるようにする
			glNormal3d(a[i][j],b[i][j],c[i][j]);
			glTexCoord2f(s1, t1);glVertex3fv(pd[i+j*nSlice]);
			glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
			glTexCoord2f(s1, t2);glVertex3fv(pd[i+(j+1)*nSlice]);
			glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
			glTexCoord2f(s2, t2);glVertex3fv(pd[ip+(j+1)*nSlice]);
			glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
			glTexCoord2f(s2, t1);glVertex3fv(pd[ip +j*nSlice]);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//---------------------------------------------------------------------------
//上半分の超2次関数
void drawTexHemiSuper(float r, int nSlice, int nStack, double eps1, double eps2)
{
	//底の中心が原点
	int i,j,ip,im,np,npL,npR,npU,npD,k1,k2;
	float s1, t1, s2, t2;
	double ct,phi,theta,z,cc;
	double e1, e2;
	float a[31][31], b[31][31], c[31][31];
	float n1[3], n2[3], n3[3], n4[3];
	float pd[31*31][3];

	if(nSlice > 30) nSlice = 30;
	if(nStack > 30) nStack = 30;

	//上半分だけ
	for(j = 0 ;j <= nStack ;j++)
	{
		phi = (M_PI/(double)nStack) * ((double)nStack  - (double)j)/2.0;
		e1 = fabs(sin(phi));
		if( e1 == 0.0) z = 0.0;
		else z = (float)(pow(e1, eps1));//z
		for (i = 0 ;i<= nSlice / 2;i++)
		{
			k1 = nSlice * j + i;//外から見て右側
			k2 = nSlice * j + nSlice - i;//左側
			theta = 2.0*M_PI * (double)i / (double)nSlice;
			ct = cos(theta);
			if( ct == 0.0) cc = 0.0;
			else if (ct > 0) { cc = pow(ct, eps2); }
			else         { cc = -pow(fabs(ct),eps2); }
			if(j == 0) 
			{
				pd[k1][0] = 0.0f;
				pd[k1][1] = 0.0f;
			}
			else
			{
				e1 = fabs(cos(phi));
				if( e1 == 0.0) 
				{
					pd[k1][0] = 0.0f;
					pd[k1][1] = 0.0f;
				}
				else
				{
					pd[k1][0] = r * (float)(pow(e1,eps1) * cc);
					e2 = fabs(sin(theta));
					if( e2 == 0.0) pd[k1][1] = 0.0f;
					else
						pd[k1][1] = r * (float)(pow(e1,eps1) * pow(e2,eps2));
				}
			}
			pd[k2][0] = pd[k1][0];
			pd[k2][1] = -pd[k1][1];
			pd[k1][2] = r * (float)z;
			pd[k2][2] = r * (float)z;
		}
	}

	//側面の法線成分
	for(i = 0;i < nSlice; i++)
	{
		ip = i + 1;
		if(ip==nSlice) ip = 0;
		im=i-1;
		if(i==0) im = nSlice - 1;

		//真上(Top)
		a[i][0] = 0.0f; b[i][0] = 0.0f; c[i][0] = 1.0f;
		np = 0; npU = 0; npL = 0; npR = 0; npU = 0;

		for(j=1;j<nStack;j++)
		{
			np = j*nSlice+i;//注目点
			npL = j*nSlice+im;//左側
			npR = j*nSlice+ip;//右側
			npU = np-nSlice;//上
			npD = np+nSlice;//下
			if(j==1) 
			{
				n1[0]=0.0f; n1[1]=0.0f; n1[2]=1.0f;
				n2[0]=0.0f; n2[1]=0.0f; n2[2]=1.0f;
				calcNormal(pd[np],pd[npL],pd[npD],n3);//外から見て左下
				calcNormal(pd[np],pd[npD],pd[npR],n4);//右下
			}
			else 
			{
				calcNormal(pd[np],pd[npU],pd[npL],n1);//外から見て左上
				calcNormal(pd[np],pd[npR],pd[npU],n2);//右上
				calcNormal(pd[np],pd[npL],pd[npD],n3);//外から見て左下
				calcNormal(pd[np],pd[npD],pd[npR],n4);//右下
			}
			a[i][j] = (float)((n1[0]+n2[0]+n3[0]+n4[0])/4.0f);//ｘ方向
			b[i][j] = (float)((n1[1]+n2[1]+n3[1]+n4[1])/4.0f);//ｙ
			c[i][j] = (float)((n1[2]+n2[2]+n3[2]+n4[2])/4.0f);//ｚ
		}
		j = nStack;//一番下の側面(上の2個の三角形の平均）
		calcNormal(pd[np],pd[npU],pd[npL],n1);//外から見て左上
		calcNormal(pd[np],pd[npR],pd[npU],n2);//右上
		a[i][j] = (float)((n1[0]+n2[0])/2.0f);
		b[i][j] = (float)((n1[1]+n2[1])/2.0f);
		c[i][j] = (float)((n1[2]+n2[2])/2.0f);
	}

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for(i = 0;i < nSlice;i++)
	{
		ip = i + 1; 
		if(ip == nSlice) ip = 0;
		s1 = (float)i / (float)nSlice;//ﾃｸｽﾁｬ座標
		s2 = (float)(i+1) / (float)nSlice;
		//if(i == nSlice-1) s2 = 1.0f;
		for(j = 0;j < nStack; j++)
		{
			t1 = 1.0f - (float)j / (float)nStack;//j=0で1となるように
			t2 = 1.0f - (float)(j+1) / (float)nStack;
			//左端(p1[1]=-0.5)でﾃｸｽﾁｬｰ座標が0となるようにする
			glNormal3d(a[i][j],b[i][j],c[i][j]);
			glTexCoord2f(s1, t1);glVertex3fv(pd[i+j*nSlice]);
			glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
			glTexCoord2f(s1, t2);glVertex3fv(pd[i+(j+1)*nSlice]);
			glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
			glTexCoord2f(s2, t2);glVertex3fv(pd[ip+(j+1)*nSlice]);
			glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
			glTexCoord2f(s2, t1);glVertex3fv(pd[ip +j*nSlice]);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//底
	j = nStack;
	glBegin(GL_POLYGON);
	glNormal3f(0.0f,0.0f,-1.0f);//-z方向
	for(i = nSlice-1;i >= 0;i--) glVertex3fv(pd[i+j*nSlice]);
	glEnd();
}

//----------------------------------------------------------------------
//２次元格子状オブジェクトに対するテクスチャマッピング用サブルーチン
void drawTexGridSquare(int numRow, int numCol, float pd[][3])
{	//裏面も定義
	int i, j, np;
	float n1[3], n2[3], n3[3], n4[3];
	float a[101][101], b[101][101], c[101][101];

	//法線成分
	for(i = 0;i < numRow;i++) //下方向
	  for(j = 0;j < numCol;j++)//右方向
	  {
		//np = j * (N1+1) + i;
        np = i * numCol + j;
		if(i == 0 )
		{
			if(j == 0) {
				calcNormal(pd[0],pd[numCol],pd[1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else if(j == numCol-1) {
				calcNormal(pd[numCol-2],pd[2*numCol-1],pd[numCol-1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else {
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n1);//左側
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//右側
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
		}
		else if(i == numRow-1)
		{
			if(j == 0) {
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else if(j == numCol-1) {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//左側
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//右側
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
		}
		else
		{
			if(j == 0) {
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n1);//上
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//下
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else if(j == numCol-1) {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//上
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n2);//下
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else {//上下左右４個の三角形の平均
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//左上
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//右上
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n3);//左下
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n4);//右下
				a[i][j] = (n1[0]+n2[0]+n3[0]+n4[0])/4.0f;
				b[i][j] = (n1[1]+n2[1]+n3[1]+n4[1])/4.0f;
				c[i][j] = (n1[2]+n2[2]+n3[2]+n4[2])/4.0f; }
		}
	}

	//Texture描画
	glEnable(GL_TEXTURE_2D);
	for(i = 0;i < numRow-1;i++)
	  for(j = 0;j < numCol-1;j++){
		//np = i + (N1+1) * j;
        np = i * numCol + j;
		glBegin(GL_TRIANGLES);
			//左下の三角形
			//各頂点の法線方向,ﾃｸｽﾁｬｰ座標,頂点座標を与える。
			glNormal3d(a[i][j],b[i][j],c[i][j]);//法線方向
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));//ﾃｸｽﾁｬｰ座標
			glVertex3fv(pd[np]);//ﾎﾟﾘｺﾞﾝの頂点座標（以下これらを繰り返す）
			glNormal3d(a[i+1][j],b[i+1][j],c[i+1][j]);
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol]);
			glNormal3d(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol+1]);
			//右上の三角形
			glNormal3d(a[i][j],b[i][j],c[i][j]);
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));
			glVertex3fv(pd[np]);
			glNormal3d(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol+1]);
			glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));
			glVertex3fv(pd[np+1]);
		glEnd();
	}
	//裏面も表面に描いたﾃｸｽﾁｬｰを描画
	for(i = 0;i < numRow-1;i++)
	  for(j = 0;j < numCol-1;j++){
        np = i * numCol + j;
		glBegin(GL_TRIANGLES);
			//左下の三角形
			//各頂点の法線方向に負号,ﾃｸｽﾁｬｰ座標,頂点座標を与える（表とは逆の頂点列,2番目と3番目を交換）。
			glNormal3d(-a[i][j],-b[i][j],-c[i][j]);//法線方向
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));//ﾃｸｽﾁｬｰ座標
			glVertex3fv(pd[np]);//ﾎﾟﾘｺﾞﾝの頂点座標（以下これらを繰り返す）
			glNormal3d(-a[i+1][j+1],-b[i+1][j+1],-c[i+1][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol+1]);
			glNormal3d(-a[i+1][j],-b[i+1][j],-c[i+1][j]);
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol]);
			//右上の三角形
			glNormal3d(-a[i][j],-b[i][j],-c[i][j]);
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));
			glVertex3fv(pd[np]);
			glNormal3d(-a[i][j+1],-b[i][j+1],-c[i][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));
			glVertex3fv(pd[np+1]);
			glNormal3d(-a[i+1][j+1],-b[i+1][j+1],-c[i+1][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol+1]);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}
//------------------------------------------------------------------------
void drawTexGrid(int numRow, int numCol, float pd[][3])//, bool flagShadow)
{
	int i, j, np;
	float n1[3], n2[3], n3[3], n4[3];
	float a[21][21], b[21][21], c[21][21];
//    float shadowDiffuse[] = {0.2f,0.2f,0.2f,0.3f};//影の拡散光

	//法線成分
	for(i = 0;i < numRow;i++) //下方向
	  for(j = 0;j < numCol;j++)//右方向
	  {
        np = i * numCol + j;
		if(i == 0 )
		{
			if(j == 0) {
				calcNormal(pd[0],pd[numCol],pd[1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else if(j == numCol-1) {
				calcNormal(pd[numCol-2],pd[2*numCol-1],pd[numCol-1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else {
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n1);//左側
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//右側
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
		}
		else if(i == numRow-1)
		{
			if(j == 0) {
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else if(j == numCol-1) {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//左側
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//右側
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
		}
		else
		{
			if(j == 0) {
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n1);//上
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//下
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else if(j == numCol-1) {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//上
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n2);//下
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else {//上下左右４個の三角形の平均
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//左上
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//右上
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n3);//左下
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n4);//右下
				a[i][j] = (n1[0]+n2[0]+n3[0]+n4[0])/4.0f;
				b[i][j] = (n1[1]+n2[1]+n3[1]+n4[1])/4.0f;
				c[i][j] = (n1[2]+n2[2]+n3[2]+n4[2])/4.0f; }
		}
	}
	//Texture描画
//    if(flagShadow == true) goto SHADOW_DISP;
	glEnable(GL_TEXTURE_2D);
	for(i = 0;i < numRow-1;i++)
	  for(j = 0;j < numCol-1;j++){
        np = i * numCol + j;
		glBegin(GL_TRIANGLES);
			//左下の三角形
			//各頂点の法線方向,ﾃｸｽﾁｬｰ座標,頂点座標を与える。
			glNormal3d(a[i][j],b[i][j],c[i][j]);//法線方向
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));//ﾃｸｽﾁｬｰ座標
			glVertex3fv(pd[np]);//ﾎﾟﾘｺﾞﾝの頂点座標（以下これらを繰り返す）
			glNormal3d(a[i+1][j],b[i+1][j],c[i+1][j]);
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol]);
			glNormal3d(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol+1]);
			//右上の三角形
			glNormal3d(a[i][j],b[i][j],c[i][j]);
			glTexCoord2f((float)j/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));
			glVertex3fv(pd[np]);
			glNormal3d(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
    		glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)(i+1)/(float)(numRow-1)));
			glVertex3fv(pd[np+numCol+1]);
			glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
			glTexCoord2f((float)(j+1)/(float)(numCol-1),(1.0f-(float)i/(float)(numRow-1)));
			glVertex3fv(pd[np+1]);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

//----------------------------------------------------------------------
//立方体格子状オブジェクトに対するテクスチャマッピング用サブルーチン
void drawTexGridCube(int numRow, int numCol, int numStk, float p1[][3],
        float p2[][3], float p3[][3], float p4[][3], float p5[][3],
        float p6[][3])
{
    //すべて同じテクスチャー
    //上側
    drawTexGrid(numRow, numCol, p1);
    //正面
    drawTexGrid(numStk, numCol, p2);
    //右側
    drawTexGrid(numStk, numRow, p3);
    //左側
    drawTexGrid(numStk, numRow, p4);
    //裏側
    drawTexGrid(numStk, numCol, p5);
    //下側
    drawTexGrid(numRow, numCol, p6);
}
//----------------------------------------------------------------------------
//球格子状オブジェクトに対するテクスチャマッピング用サブルーチン
//2004.07.13改良
//2011.07.14改良
void drawTexGridSphere(int Nxy, int Nz, float pd[][3])
{
	int i,j,ip,im,np,npL,npR,npU,npD;
	float a[11][11],b[11][11],c[11][11];//頂点の法線成分
	float n[11][3];//頂点の法線成分

    if(Nxy > 11) Nxy = 11;
    if(Nz > 11) Nz = 11;
    //Top
    //TopのnumCol個の三角形の法線ベクトルを平均化
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
        calcNormal(pd[i],pd[i+Nxy],pd[ip+Nxy],n[i]);
        a[0][0] += n[i][0];
        b[0][0] += n[i][1];
        c[0][0] += n[i][2];
    }
    a[0][0] = a[0][0] / (float)Nxy;
    b[0][0] = b[0][0] / (float)Nxy;
    c[0][0] = c[0][0] / (float)Nxy;
    for(i = 1;i < Nxy; i++){ //Top頂点はすべて同じ
        a[i][0] = a[0][0]; b[i][0] = b[0][0]; c[i][0] = c[0][0];
    }

	//側面の法線成分
    //隣り合う4個の三角形の法線ベクトルを平均化
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
		for(j = 1;j < Nz-1;j++)
		{
			np =  j * Nxy + i;//注目点
			npL = j * Nxy + im;//左側
			npR = j * Nxy + ip;//右側
			npU = np - Nxy;//上
			npD = np + Nxy;//下
            calcNormal(pd[np],pd[npU],pd[npL],n[0]);//外から見て左上
            calcNormal(pd[np],pd[npR],pd[npU],n[1]);//右上
            calcNormal(pd[np],pd[npL],pd[npD],n[2]);//外から見て左下
            calcNormal(pd[np],pd[npD],pd[npR],n[3]);//右下

			a[i][j] = (float)((n[0][0]+n[1][0]+n[2][0]+n[3][0])/4.0f);//ｘ方向
			b[i][j] = (float)((n[0][1]+n[1][1]+n[2][1]+n[3][1])/4.0f);//ｙ
			c[i][j] = (float)((n[0][2]+n[1][2]+n[2][2]+n[3][2])/4.0f);//ｚ
		}
	}
    //Bottom
    //BottomのnumCol個の三角形の法線ベクトルを平均化
    a[0][Nz-1] = 0.0; b[0][Nz-1] = 0.0; c[0][Nz-1] = 0.0;
	for(i = 0;i < Nxy;i++){
        np = (Nz-1) * Nxy +i;
 		ip = i+1;
		if(ip == Nxy) ip = 0;
        calcNormal(pd[np],pd[np-i+ip-Nxy],pd[np-Nxy],n[i]);
        a[0][Nz-1] += n[i][0];
        b[0][Nz-1] += n[i][1];
        c[0][Nz-1] += n[i][2];
    }
    a[0][Nz-1] = a[0][Nz-1] / (float)Nxy;
    b[0][Nz-1] = b[0][Nz-1] / (float)Nxy;
    c[0][Nz-1] = c[0][Nz-1] / (float)Nxy;
    for(i = 1;i < Nxy; i++){ //Top頂点はすべて同じ
        a[i][Nz-1] = a[0][Nz-1];
        b[i][Nz-1] = b[0][Nz-1];
        c[i][Nz-1] = c[0][Nz-1];
    }

	//表示
	//側面(4角形パッチ）
	float th1, th2, ph1, ph2;
	glEnable(GL_TEXTURE_2D);
	for(j = 0;j < Nz-1;j++){
		ph1 = 1.0f - (float)j / (float)(Nz-1);
		ph2 = 1.0f - (float)(j + 1) / (float)(Nz-1);
		for(i = 0;i < Nxy;i++)
		{	ip = i+1;
			if(ip == Nxy) ip = 0;
			th1 = (float)i / (float)Nxy;
			th2 = (float)(i+1) / (float)Nxy;
			glBegin(GL_QUADS);
				glNormal3d(a[i][j],b[i][j],c[i][j]);
				glTexCoord2f(th1, ph1);glVertex3fv(pd[j*Nxy+i]);
				glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
				glTexCoord2f(th1, ph2);glVertex3fv(pd[(j+1)*Nxy+i]);
				glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
				glTexCoord2f(th2, ph2);glVertex3fv(pd[(j+1)*Nxy+ip]);
				glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
				glTexCoord2f(th2, ph1);glVertex3fv(pd[j*Nxy+ip]);
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
}

//----------------------------------------------------------------------
//側面だけに円筒投影
void drawTexGridCylinder(int Nxy, int Nz, float p[][3])
{
    //p[0]は上底の中心，最後のp[]は下底の中心
	int i,j,ip,im,np,npL,npR,npU,npD;
	float a[11][11],b[11][11],c[11][11];//頂点の法線成分

	float n[11][3];//頂点の法線成分

    if(Nxy > 10) Nxy = 10;
    if(Nz > 10) Nz = 10;

    //全頂点数
    int num = Nxy * Nz + 2;

    //Top
    //上底中心はTopのnumCol個の三角形の法線ベクトルを平均化
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
    //上底の周囲の頂点は隣り合う2つの三角形の法線ベクトルを平均化
    for(i = 1;i <= Nxy; i++){
        a[i][0] = 0.0; b[i][0] = 0.0; c[i][0] = 0.0;
    }
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
        calcNormal(p[0],p[i+1],p[ip+1],n[i]);
        a[0][0] += n[i][0];
        b[0][0] += n[i][1];
        c[0][0] += n[i][2];
        a[i+1][0] += n[i][0];
        b[i+1][0] += n[i][1];
        c[i+1][0] += n[i][2];
        a[ip+1][0] += n[i][0];
        b[ip+1][0] += n[i][1];
        c[ip+1][0] += n[i][2];
    }
    a[0][0] = a[0][0] / (float)Nxy;
    b[0][0] = b[0][0] / (float)Nxy;
    c[0][0] = c[0][0] / (float)Nxy;
    for(i = 1; i < Nxy; i++){
        a[i][0] = a[i][0] / 2.0f;
        b[i][0] = b[i][0] / 2.0f;
        c[i][0] = c[i][0] / 2.0f;
    }
    //Topの表示
    for(i = 1; i <= Nxy; i++){
        ip = i + 1;
        if(ip == Nxy + 1) ip = 1;
		glBegin(GL_TRIANGLES);
			glNormal3f(a[0][0],b[0][0],c[0][0]);glVertex3fv(p[0]);
			glNormal3f(a[i][0],b[i][0],c[i][0]);glVertex3fv(p[i]);
			glNormal3f(a[ip][0],b[ip][0],c[ip][0]);glVertex3fv(p[ip]);
		glEnd();
	}

    //Bottom
    //下底中心はBottomのnumCol個の三角形の法線ベクトルを平均化
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
    //下底の周囲の頂点は隣り合う2つの三角形の法線ベクトルを平均化
    for(i = 1;i <= Nxy; i++){
        a[i][0] = 0.0; b[i][0] = 0.0; c[i][0] = 0.0;
    }
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
        calcNormal(p[num-1],p[num-2-i],p[num-2-ip],n[i]);
        a[0][0] += n[i][0];
        b[0][0] += n[i][1];
        c[0][0] += n[i][2];
        a[i+1][0] += n[i][0];
        b[i+1][0] += n[i][1];
        c[i+1][0] += n[i][2];
        a[ip+1][0] += n[i][0];
        b[ip+1][0] += n[i][1];
        c[ip+1][0] += n[i][2];
    }
    a[0][0] = a[0][0] / (float)Nxy;
    b[0][0] = b[0][0] / (float)Nxy;
    c[0][0] = c[0][0] / (float)Nxy;
    for(i = 1; i < Nxy; i++){
        a[i][0] = a[i][0] / 2.0f;
        b[i][0] = b[i][0] / 2.0f;
        c[i][0] = c[i][0] / 2.0f;
    }
    //Bottomの表示
    for(i = 1; i <= Nxy; i++){
        ip = i + 1;
        if(ip == Nxy + 1) ip = 1;
		glBegin(GL_TRIANGLES);
			glNormal3f(a[0][0],b[0][0],c[0][0]);glVertex3fv(p[num-1]);
			glNormal3f(a[i][0],b[i][0],c[i][0]);glVertex3fv(p[num-1-i]);
			glNormal3f(a[ip][0],b[ip][0],c[ip][0]); glVertex3fv(p[num-1-ip]);
		glEnd();
	}

	//側面の法線成分
    //上底の周囲頂点の側面に対する法線成分は
    //隣り合う2個の三角形の法線ベクトルを平均化
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
        //j = 0;
        np = i+1;//注目点
        npL = im+1;//左側
        npR = ip+1;//右側
        npD = np + Nxy;//下
        calcNormal(p[np],p[npL],p[npD],n[0]);//外から見て左下
        calcNormal(p[np],p[npD],p[npR],n[1]);//右下

        a[i][0] = (float)((n[0][0]+n[1][0])/2.0f);//ｘ方向
        b[i][0] = (float)((n[0][1]+n[1][1])/2.0f);//ｙ
        c[i][0] = (float)((n[0][2]+n[1][2])/2.0f);//ｚ
	}

    //同じように下底の頂点に対して
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
        j = Nz - 1;
        np = j*Nxy+i+1;//注目点
        npL = j*Nxy+im+1;//左側
        npR = j*Nxy+ip+1;//右側
        npU = np - Nxy;//上
        calcNormal(p[np],p[npU],p[npL],n[0]);//外から見て左上
        calcNormal(p[np],p[npR],p[npU],n[1]);//右上

        a[i][j] = (float)((n[0][0]+n[1][0])/2.0f);//ｘ方向
        b[i][j] = (float)((n[0][1]+n[1][1])/2.0f);//ｙ
        c[i][j] = (float)((n[0][2]+n[1][2])/2.0f);//ｚ
	}

    //他の側面の頂点に対しては隣り合う4個の三角形の法線ベクトルを平均化
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
		for(j = 1;j < Nz-1;j++)
		{
			np =  j * Nxy + i + 1;//注目点
			npL = j * Nxy + im + 1;//左側
			npR = j * Nxy + ip + 1;//右側
			npU = np - Nxy;//上
			npD = np + Nxy;//下
            calcNormal(p[np],p[npU],p[npL],n[0]);//外から見て左上
            calcNormal(p[np],p[npR],p[npU],n[1]);//右上
            calcNormal(p[np],p[npL],p[npD],n[2]);//外から見て左下
            calcNormal(p[np],p[npD],p[npR],n[3]);//右下

			a[i][j] = (float)((n[0][0]+n[1][0]+n[2][0]+n[3][0])/4.0f);//ｘ方向
			b[i][j] = (float)((n[0][1]+n[1][1]+n[2][1]+n[3][1])/4.0f);//ｙ
			c[i][j] = (float)((n[0][2]+n[1][2]+n[2][2]+n[3][2])/4.0f);//ｚ
		}
	}

	//側面(4角形パッチ）
	float th1, th2, ph1, ph2;
	glEnable(GL_TEXTURE_2D);
	for(j = 0;j < Nz-1;j++){
		ph1 = 1.0f - (float)j / (float)(Nz-1);
		ph2 = 1.0f - (float)(j + 1) / (float)(Nz-1);
		for(i = 0;i < Nxy;i++)
		{	ip = i+1;
			if(ip == Nxy) ip = 0;
			th1 = (float)i / (float)Nxy;
			th2 = (float)(i+1) / (float)Nxy;
			glBegin(GL_QUADS);
				glNormal3d(a[i][j],b[i][j],c[i][j]);
				glTexCoord2f(th1, ph1);glVertex3fv(p[j*Nxy+i+1]);
				glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
				glTexCoord2f(th1, ph2);glVertex3fv(p[(j+1)*Nxy+i+1]);
				glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
				glTexCoord2f(th2, ph2);glVertex3fv(p[(j+1)*Nxy+ip+1]);
				glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
				glTexCoord2f(th2, ph1);glVertex3fv(p[j*Nxy+ip+1]);
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
}
//------------------------------------------------------------------------
//2次元格子状オブジェクトに格子間隔に従って赤,シアンのチェック模様を直接描画
void drawCheckSquare(int numRow, int numCol, float pd[][3], bool flagShadow)
{
	int i, j, np;
	float n1[3], n2[3], n3[3], n4[3];
	float a[21][21], b[21][21], c[21][21];
    float shadowDiffuse[] = {0.0f,0.0f,0.0f,0.3f};//影の拡散光

	//法線成分
	for(i = 0;i < numRow;i++) //下方向
	  for(j = 0;j < numCol;j++)//右方向
	  {
        np = i * numCol + j;
		if(i == 0 )
		{
			if(j == 0) {
				calcNormal(pd[0],pd[numCol],pd[1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else if(j == numCol-1) {
				calcNormal(pd[numCol-2],pd[2*numCol-1],pd[numCol-1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else {
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n1);//左側
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//右側
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
		}
		else if(i == numRow-1)
		{
			if(j == 0) {
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else if(j == numCol-1) {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);
				a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
			else {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//左側
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//右側
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
		}
		else
		{
			if(j == 0) {
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n1);//上
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//下
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else if(j == numCol-1) {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//上
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n2);//下
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else {//上下左右４個の三角形の平均
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//左上
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//右上
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n3);//左下
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n4);//右下
				a[i][j] = (n1[0]+n2[0]+n3[0]+n4[0])/4.0f;
				b[i][j] = (n1[1]+n2[1]+n3[1]+n4[1])/4.0f;
				c[i][j] = (n1[2]+n2[2]+n3[2]+n4[2])/4.0f; }
		}
	}

    float diffuse[4] ;
    int k;
	//チェック模様描画
	for(i = 0;i < numRow-1;i++)
	  for(j = 0;j < numCol-1;j++){
        if(fmod(i+j, 2.0) == 0.0)
            { diffuse[0] = 1.0f; diffuse[1] = 0.0f; diffuse[2] = 0.0f; }
        else
            { diffuse[0] = 0.0f; diffuse[1] = 1.0f; diffuse[2] = 1.0f; }
        diffuse[3] = 1.0f;
        if(flagShadow == true){
            for(k = 0; k < 4; k++) diffuse[k] = shadowDiffuse[k];
        }
        glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuse);

        np = i * numCol + j;
		glBegin(GL_TRIANGLES);
			//左下の三角形
			//各頂点の法線方向,ﾃｸｽﾁｬｰ座標,頂点座標を与える。
			glNormal3d(a[i][j],b[i][j],c[i][j]);//法線方向
			glVertex3fv(pd[np]);//ﾎﾟﾘｺﾞﾝの頂点座標（以下これらを繰り返す）
			glNormal3d(a[i+1][j],b[i+1][j],c[i+1][j]);
			glVertex3fv(pd[np+numCol]);
			glNormal3d(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glVertex3fv(pd[np+numCol+1]);
			//右上の三角形
			glNormal3d(a[i][j],b[i][j],c[i][j]);
			glVertex3fv(pd[np]);
			glNormal3d(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glVertex3fv(pd[np+numCol+1]);
			glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
			glVertex3fv(pd[np+1]);
		glEnd();
	}
}
//----------------------------------------------------------------------
void drawCheckCube(int numRow, int numCol, int numStk, float p1[][3],
        float p2[][3], float p3[][3], float p4[][3], float p5[][3], float p6[][3], bool flagShadow)
{
    //すべて同じテクスチャー
    //上側
    drawCheckSquare(numRow, numCol, p1, flagShadow);
    //正面
    drawCheckSquare(numStk, numCol, p2, flagShadow);
    //右側
    drawCheckSquare(numStk, numRow, p3, flagShadow);
    //左側
    drawCheckSquare(numStk, numRow, p4, flagShadow);
    //裏側
    drawCheckSquare(numStk, numCol, p5, flagShadow);
    //下側
    drawCheckSquare(numRow, numCol, p6, flagShadow);
}
//------------------------------------------------------------------------------
//球格子状オブジェクトに対するサブルーチン
//格子間隔に従って赤，シアンのチェック模様を直接描画
void drawCheckSphere(int Nxy, int Nz, float pd[][3], bool flagShadow)
{
	int i,j,ip,im,n0,np,npL,npR,npU,npD;
	float a[11][11],b[11][11],c[11][11];//頂点の法線成分
    float shadowDiffuse[] = {0.0f,0.0f,0.0f,0.3f};//影の拡散光

	float n[11][3];//頂点の法線成分

    if(Nxy > 11) Nxy = 11;
    if(Nz > 11) Nz = 11;

    //Top
    //TopのnumCol個の三角形の法線ベクトルを平均化
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
        calcNormal(pd[i],pd[i+Nxy],pd[ip+Nxy],n[i]);
        //calcNormal(pd[0],pd[i],pd[ip],n[i]);
        a[0][0] += n[i][0];
        b[0][0] += n[i][1];
        c[0][0] += n[i][2];
    }
    a[0][0] = a[0][0] / (float)Nxy;
    b[0][0] = b[0][0] / (float)Nxy;
    c[0][0] = c[0][0] / (float)Nxy;
    for(i = 1;i < Nxy; i++){ //Top頂点はすべて同じ
        a[i][0] = a[0][0]; b[i][0] = b[0][0]; c[i][0] = c[0][0];
    }

	//側面の法線成分
    //隣り合う4個の三角形の法線ベクトルを平均化
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
		for(j = 1;j < Nz-1;j++)
		{
			np =  j * Nxy + i;//注目点
			npL = j * Nxy + im;//左側
			npR = j * Nxy + ip;//右側
			npU = np - Nxy;//上
			npD = np + Nxy;//下
            calcNormal(pd[np],pd[npU],pd[npL],n[0]);//外から見て左上
            calcNormal(pd[np],pd[npR],pd[npU],n[1]);//右上
            calcNormal(pd[np],pd[npL],pd[npD],n[2]);//外から見て左下
            calcNormal(pd[np],pd[npD],pd[npR],n[3]);//右下

			a[i][j] = (float)((n[0][0]+n[1][0]+n[2][0]+n[3][0])/4.0f);//ｘ方向
			b[i][j] = (float)((n[0][1]+n[1][1]+n[2][1]+n[3][1])/4.0f);//ｙ
			c[i][j] = (float)((n[0][2]+n[1][2]+n[2][2]+n[3][2])/4.0f);//ｚ
		}
	}

    //Bottom
    //BottomのnumCol個の三角形の法線ベクトルを平均化(2011.7.14修正)
    a[0][Nz-1] = 0.0; b[0][Nz-1] = 0.0; c[0][Nz-1] = 0.0;
	for(i = 0;i < Nxy;i++){
		n0 = (Nz-1) * Nxy + i;
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		np = (Nz-1) * Nxy + ip;
        calcNormal(pd[n0],pd[np-Nxy],pd[n0-Nxy],n[i]);
        a[0][Nz-1] += n[i][0];
        b[0][Nz-1] += n[i][1];
        c[0][Nz-1] += n[i][2];
    }
    a[0][Nz-1] = a[0][Nz-1] / (float)Nxy;
    b[0][Nz-1] = b[0][Nz-1] / (float)Nxy;
    c[0][Nz-1] = c[0][Nz-1] / (float)Nxy;
    for(i = 1;i < Nxy; i++)
	{   //Bottom頂点はすべて同じ
        a[i][Nz-1] = a[0][Nz-1];
        b[i][Nz-1] = b[0][Nz-1];
        c[i][Nz-1] = c[0][Nz-1];
    }

    if(flagShadow == true) goto SHADOW_DISP;

	//表示
    float diffuse[4];
	//側面(4角形パッチ）
	for(j = 0;j < Nz-1;j++){
	//for(j = 0;j < Nz;j++){
		for(i = 0;i < Nxy;i++)
		{
            if(fmod(i+j, 2.0) == 0.0)
                { diffuse[0] = 1.0f; diffuse[1] = 0.0f; diffuse[2] = 0.0f; }
            else
                { diffuse[0] = 0.0f; diffuse[1] = 1.0f; diffuse[2] = 1.0f; }
            diffuse[3] = 1.0f;
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuse);

        	ip = i+1;
			if(ip == Nxy) ip = 0;
			glBegin(GL_QUADS);
				glNormal3d(a[i][j],b[i][j],c[i][j]);
				glVertex3fv(pd[j*Nxy+i]);
				glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
				glVertex3fv(pd[(j+1)*Nxy+i]);
				glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
				glVertex3fv(pd[(j+1)*Nxy+ip]);
				glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
				glVertex3fv(pd[j*Nxy+ip]);
			glEnd();
		}
	}
    return;

SHADOW_DISP:;
	for(j = 0;j < Nz-1;j++){
		for(i = 0;i < Nxy;i++)
		{	ip = i+1;
			if(ip == Nxy) ip = 0;
            glMaterialfv(GL_FRONT,GL_DIFFUSE,shadowDiffuse);
			glBegin(GL_QUADS);
				glNormal3d(a[i][j],b[i][j],c[i][j]);
                glVertex3fv(pd[j*Nxy+i]);
				glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
				glVertex3fv(pd[(j+1)*Nxy+i]);
				glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
				glVertex3fv(pd[(j+1)*Nxy+ip]);
				glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
				glVertex3fv(pd[j*Nxy+ip]);
			glEnd();
		}
	}
}
//----------------------------------------------------------------------------
//円柱格子状オブジェクトに対するサブルーチン
//格子間隔に従って赤，シアンのチェック模様を直接描画
void drawCheckCylinder(int Nxy, int Nz, float p[][3], bool flagShadow)
{
	int i,j,ip,im,np,npL,npR,npU,npD;
	float a[11][11],b[11][11],c[11][11];//頂点の法線成分
	float n[11][3];//頂点の法線成分
    float diffuse[4];
    float shadowDiffuse[] = {0.0f,0.0f,0.0f,0.2f};//影の拡散光

    if(Nxy > 11) Nxy = 11;
    if(Nz > 11) Nz = 11;

    //全頂点数
    int num = Nxy * Nz + 2;

    //Top
    //上底中心はTopのnumCol個の三角形の法線ベクトルを平均化
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
    //上底の周囲の頂点は隣り合う2つの三角形の法線ベクトルを平均化
    for(i = 1;i <= Nxy; i++){
        a[i][0] = 0.0; b[i][0] = 0.0; c[i][0] = 0.0;
    }
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
        calcNormal(p[0],p[i+1],p[ip+1],n[i]);
        a[0][0] += n[i][0];
        b[0][0] += n[i][1];
        c[0][0] += n[i][2];
        a[i+1][0] += n[i][0];
        b[i+1][0] += n[i][1];
        c[i+1][0] += n[i][2];
        a[ip+1][0] += n[i][0];
        b[ip+1][0] += n[i][1];
        c[ip+1][0] += n[i][2];
    }
    a[0][0] = a[0][0] / (float)Nxy;
    b[0][0] = b[0][0] / (float)Nxy;
    c[0][0] = c[0][0] / (float)Nxy;
    for(i = 1; i < Nxy; i++){
        a[i][0] = a[i][0] / 2.0f;
        b[i][0] = b[i][0] / 2.0f;
        c[i][0] = c[i][0] / 2.0f;
    }
    //Topの表示
    for(i = 1; i <= Nxy; i++){
        ip = i + 1;
        if(ip == Nxy + 1) ip = 1;
        if(fmod(i, 2.0) == 0.0)
        { diffuse[0] = 1.0f; diffuse[1] = 0.0f; diffuse[2] = 0.0f; }
        else
        { diffuse[0] = 0.0f; diffuse[1] = 1.0f; diffuse[2] = 1.0f; }
        diffuse[3] = 1.0f;
        if(flagShadow == true) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,shadowDiffuse);
        else glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuse);
		glBegin(GL_TRIANGLES);
			glNormal3f(a[0][0],b[0][0],c[0][0]);glVertex3fv(p[0]);
			glNormal3f(a[i][0],b[i][0],c[i][0]);glVertex3fv(p[i]);
			glNormal3f(a[ip][0],b[ip][0],c[ip][0]);glVertex3fv(p[ip]);
		glEnd();
	}

    //Bottom
    //下底中心はBottomのnumCol個の三角形の法線ベクトルを平均化
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
    //下底の周囲の頂点は隣り合う2つの三角形の法線ベクトルを平均化
    for(i = 1;i <= Nxy; i++){
        a[i][0] = 0.0; b[i][0] = 0.0; c[i][0] = 0.0;
    }
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
        calcNormal(p[num-1],p[num-2-i],p[num-2-ip],n[i]);
        a[0][0] += n[i][0];
        b[0][0] += n[i][1];
        c[0][0] += n[i][2];
        a[i+1][0] += n[i][0];
        b[i+1][0] += n[i][1];
        c[i+1][0] += n[i][2];
        a[ip+1][0] += n[i][0];
        b[ip+1][0] += n[i][1];
        c[ip+1][0] += n[i][2];
    }
    a[0][0] = a[0][0] / (float)Nxy;
    b[0][0] = b[0][0] / (float)Nxy;
    c[0][0] = c[0][0] / (float)Nxy;
    for(i = 1; i < Nxy; i++){
        a[i][0] = a[i][0] / 2.0f;
        b[i][0] = b[i][0] / 2.0f;
        c[i][0] = c[i][0] / 2.0f;
    }
    //Bottomの表示
    for(i = 1; i <= Nxy; i++){
        ip = i + 1;
        if(ip == Nxy + 1) ip = 1;
        if(fmod(i, 2.0) == 0.0)
        { diffuse[0] = 1.0f; diffuse[1] = 0.0f; diffuse[2] = 0.0f; }
        else
        { diffuse[0] = 0.0f; diffuse[1] = 1.0f; diffuse[2] = 1.0f; }
        diffuse[3] = 1.0f;
        if(flagShadow == true) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,shadowDiffuse);
        else glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuse);
		glBegin(GL_TRIANGLES);
			glNormal3f(a[0][0],b[0][0],c[0][0]);glVertex3fv(p[num-1]);
			glNormal3f(a[i][0],b[i][0],c[i][0]);glVertex3fv(p[num-1-i]);
			glNormal3f(a[ip][0],b[ip][0],c[ip][0]); glVertex3fv(p[num-1-ip]);
		glEnd();
	}

	//側面の法線成分
    //上底の周囲頂点の側面に対する法線成分は
    //隣り合う2個の三角形の法線ベクトルを平均化
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
        //j = 0;
        np = i+1;//注目点
        npL = im+1;//左側
        npR = ip+1;//右側
        npD = np + Nxy;//下
        calcNormal(p[np],p[npL],p[npD],n[0]);//外から見て左下
        calcNormal(p[np],p[npD],p[npR],n[1]);//右下

        a[i][0] = (float)((n[0][0]+n[1][0])/2.0f);//ｘ方向
        b[i][0] = (float)((n[0][1]+n[1][1])/2.0f);//ｙ
        c[i][0] = (float)((n[0][2]+n[1][2])/2.0f);//ｚ
	}

    //同じように下底の頂点に対して
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
        j = Nz - 1;
        np = j*Nxy+i+1;//注目点
        npL = j*Nxy+im+1;//左側
        npR = j*Nxy+ip+1;//右側
        npU = np - Nxy;//上
        calcNormal(p[np],p[npU],p[npL],n[0]);//外から見て左上
        calcNormal(p[np],p[npR],p[npU],n[1]);//右上

        a[i][j] = (float)((n[0][0]+n[1][0])/2.0f);//ｘ方向
        b[i][j] = (float)((n[0][1]+n[1][1])/2.0f);//ｙ
        c[i][j] = (float)((n[0][2]+n[1][2])/2.0f);//ｚ
	}

    //他の側面の頂点に対しては隣り合う4個の三角形の法線ベクトルを平均化
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
		for(j = 1;j < Nz-1;j++)
		{
			np =  j * Nxy + i + 1;//注目点
			npL = j * Nxy + im + 1;//左側
			npR = j * Nxy + ip + 1;//右側
			npU = np - Nxy;//上
			npD = np + Nxy;//下
            calcNormal(p[np],p[npU],p[npL],n[0]);//外から見て左上
            calcNormal(p[np],p[npR],p[npU],n[1]);//右上
            calcNormal(p[np],p[npL],p[npD],n[2]);//外から見て左下
            calcNormal(p[np],p[npD],p[npR],n[3]);//右下

			a[i][j] = (float)((n[0][0]+n[1][0]+n[2][0]+n[3][0])/4.0f);//ｘ方向
			b[i][j] = (float)((n[0][1]+n[1][1]+n[2][1]+n[3][1])/4.0f);//ｙ
			c[i][j] = (float)((n[0][2]+n[1][2]+n[2][2]+n[3][2])/4.0f);//ｚ
		}
	}

    //表示
    if(flagShadow == true) goto SHADOW_DISP;
	//側面(4角形パッチ）
	for(j = 0;j < Nz-1;j++){
		for(i = 0;i < Nxy;i++)
		{	ip = i+1;
			if(ip == Nxy) ip = 0;
            if(fmod(i+j, 2.0) == 0.0)
                { diffuse[0] = 1.0f; diffuse[1] = 0.0f; diffuse[2] = 0.0f; }
            else
                { diffuse[0] = 0.0f; diffuse[1] = 1.0f; diffuse[2] = 1.0f; }
            diffuse[3] = 1.0f;
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,diffuse);
			glBegin(GL_QUADS);
				glNormal3d(a[i][j],b[i][j],c[i][j]);
				glVertex3fv(p[j*Nxy+i+1]);
				glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
				glVertex3fv(p[(j+1)*Nxy+i+1]);
				glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
				glVertex3fv(p[(j+1)*Nxy+ip+1]);
				glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
				glVertex3fv(p[j*Nxy+ip+1]);
			glEnd();
		}
	}
    return;

SHADOW_DISP:;
	for(j = 0;j < Nz-1;j++){
		for(i = 0;i < Nxy;i++)
		{	ip = i+1;
			if(ip == Nxy) ip = 0;
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,shadowDiffuse);
			glBegin(GL_QUADS);
				glNormal3d(a[i][j],b[i][j],c[i][j]);
				glVertex3fv(p[j*Nxy+i+1]);
				glNormal3d(a[i][j+1],b[i][j+1],c[i][j+1]);
				glVertex3fv(p[(j+1)*Nxy+i+1]);
				glNormal3d(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
				glVertex3fv(p[(j+1)*Nxy+ip+1]);
				glNormal3d(a[ip][j],b[ip][j],c[ip][j]);
				glVertex3fv(p[j*Nxy+ip+1]);
			glEnd();
		}
	}
}
