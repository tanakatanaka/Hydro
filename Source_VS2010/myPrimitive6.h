#include <stdio.h>

#define M_PI 3.14159265358979323846

//法線方向計算ルーチン
void calcNormal(float* p1,float* p2,float* p3,float* nn)
{
	Vector3D A = Vector3D(p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]);
	Vector3D B = Vector3D(p3[0] - p2[0], p3[1] - p2[1], p3[2] - p2[2]);
	Vector3D n = vCross(A , B);//外積
	n.norm();
	nn[0] = n.x; nn[1] = n.y; nn[2] = n.z;
}
//------------------------------------------------------------------------------
void drawPlate()//x-y平面
{//面の法線方向がｚ軸
	float p[4][3] = 
	{ { 0.5,-0.5, 0.0}, { 0.5, 0.5, 0.0}, {-0.5, 0.5, 0.0}, {-0.5,-0.5, 0.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0); //z方向の法線
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}

//------------------------------------------------------------------------------
void drawPlateZ(float s)//x-y平面
{//面の法線方向がｚ軸
	float p[4][3] = 
	{ { s/2.0,-s/2.0, 0.0}, { s/2.0, s/2.0, 0.0}, {-s/2.0, s/2.0, 0.0}, 
	  {-s/2.0,-s/2.0, 0.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0); //z方向の法線
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateY(float s)//x-z平面
{//面の法線方向がy軸
	float p[4][3] = 
	{ { s/2.0, 0.0, s/2.0}, { s/2.0, 0.0,-s/2.0}, {-s/2.0, 0.0,-s/2.0}, {-s/2.0, 0.0, s/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0); //y方向の法線
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
/*
//------------------------------------------------------------------------------
void drawPlatePX(float sx, float sy, float sz)//y-z平面
{//面の法線方向が+x軸
	float p[4][3] = 
	{ { sx, sy/2.0, sz/2.0}, { sx, -sy/2.0, sz/2.0}, {sx, -sy/2.0,-sz/2.0}, {sx, sy/2.0, -sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(1.0, 0.0, 0.0); //+x方向の法線
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateMX(float sx, float sy, float sz)//y-z平面
{//面の法線方向が-x軸
	float p[4][3] = 
	{ { sx, sy/2.0, sz/2.0}, { sx, -sy/2.0, sz/2.0}, {sx, -sy/2.0,-sz/2.0}, {sx, sy/2.0, -sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(-1.0, 0.0, 0.0); //-x方向の法線
		glVertex3fv(p[3]);
		glVertex3fv(p[2]);
		glVertex3fv(p[1]);
		glVertex3fv(p[0]);
	glEnd();
}

//------------------------------------------------------------------------------
void drawPlatePY(float sx, float sy, float sz)//x-z平面
{//面の法線方向が+y軸
	float p[4][3] = 
	{ { sx/2.0, sy, sz/2.0}, { sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy, sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0); //+y方向の法線
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateMY(float sx, float sy, float sz)//x-z平面
{//面の法線方向が-y軸
	float p[4][3] = 
	{ { sx/2.0, sy, sz/2.0}, { sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy, sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, -1.0, 0.0); //-y方向の法線
		glVertex3fv(p[3]);
		glVertex3fv(p[2]);
		glVertex3fv(p[1]);
		glVertex3fv(p[0]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlatePZ(float sx, float sy, float sz)//x-y平面
{//面の法線方向が+z軸
	float p[4][3] = 
	{ { sx/2.0, sy/2.0, sz}, {-sx/2.0, sy/2.0, sz}, {-sx/2.0, -sy/2.0,sz}, {sx/2.0, -sy/2.0, sz}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0); //+z方向の法線
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateMZ(float sx, float sy, float sz)//x-y平面
{//面の法線方向が-z軸
	float p[4][3] = 
	{ { sx/2.0, sy/2.0, sz}, {-sx/2.0, sy/2.0, sz}, {-sx/2.0, -sy/2.0,sz}, {sx/2.0, -sy/2.0, sz}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, -1.0); //+z方向の法線
		glVertex3fv(p[3]);
		glVertex3fv(p[2]);
		glVertex3fv(p[1]);
		glVertex3fv(p[0]);
	glEnd();
}
*/
//-----------------------------------------------------------------------

void drawCube(float s)
{
	float p[8][3] = 
	{ {0.5*s,0.5*s,0.5*s}, {-0.5*s,0.5*s,0.5*s}, {-0.5*s,-0.5*s,0.5*s}, 
	  {0.5*s,-0.5*s,0.5*s},{0.5*s,0.5*s,-0.5*s}, {-0.5*s,0.5*s,-0.5*s},
	  {-0.5*s,-0.5*s,0-0.5*s}, {0.5*s,-0.5*s,-0.5*s}
	};

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f); //z方向
		glVertex3fv(p[0]); glVertex3fv(p[1]);
		glVertex3fv(p[2]); glVertex3fv(p[3]);

		glNormal3f(1.0f,0.0f,0.0f); //x方向(正面）
		glVertex3fv(p[0]); glVertex3fv(p[3]);
		glVertex3fv(p[7]); glVertex3fv(p[4]);

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

void drawGridPlate(float sizeX, float sizeY, int nSliceX, int nSliceY)
{//xy平面，中心は原点
	int i, j;
	//double ;
	float p[2][3];
	float pitchX = sizeX / (float)nSliceX;
	float pitchY = sizeY / (float)nSliceY;

	for(j = 0; j < nSliceY; j++)
	{
		//頂点z座標
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSliceX; i++)
		{

			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[0][1] = (float)(j+1) * pitchY - sizeY / 2.0;//y座標
			p[1][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[1][1] = (float)j * pitchY - sizeY / 2.0;//y座標

			glNormal3f(0.0, 0.0, 1.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[1]);//頂点座標			
		}
		glEnd();
	}
}

//-----------------------------------------------------------------
void drawGridLines(float sizeX, float sizeY, int nSliceX, int nSliceY)
{//xy平面，中心は原点(wireframe用）
	int i, j;
	//double ;
	float p[2][3];
	float pitchX = sizeX / (float)nSliceX;
	float pitchY = sizeY / (float)nSliceY;

	for(j = 0; j < nSliceY; j++)
	{
		//頂点z座標
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_LINES);
		for(i = 0; i <= nSliceX; i++)
		{
			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[0][1] = (float)j * pitchY - sizeY / 2.0;//y座標
			p[1][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[1][1] = (float)(j+1) * pitchY - sizeY / 2.0;//y座標

			glNormal3f(0.0, 0.0, 1.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[1]);//頂点座標			
		}
		glEnd();
	}

	for(i = 0; i < nSliceX; i++)
	{
		//頂点z座標
		p[0][2] = 0.0;
		p[1][2] = 0.0;
		glBegin(GL_LINES);
		for(j = 0; j <= nSliceY; j++)
		{
			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[0][1] = (float)j * pitchY - sizeY / 2.0;//y座標
			p[1][0] = (float)(i+1) * pitchX - sizeX / 2.0;//x座標
			p[1][1] = (float)j * pitchY - sizeY / 2.0;//y座標

			glNormal3f(0.0, 0.0, 1.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[1]);//頂点座標			
		}
		glEnd();
	}
}
/*
//---------------------------------------------------------------------------
void drawGridPlateY(float sizeX, float sizeZ, int nSliceX, int nSliceZ)
{//xz平面，中心は原点(ｙ軸が鉛直軸)
	int i, j;
	//double ;
	float p[2][3];
	float pitchX = sizeX / (float)nSliceX;
	float pitchZ = sizeZ / (float)nSliceZ;

	for(j = 0; j < nSliceZ; j++)
	{
		//頂点y座標
		p[0][1] = 0.0;
		p[1][1] = 0.0;

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSliceX; i++)
		{

			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[0][2] = (float)(j+1) * pitchZ - sizeZ / 2.0;//y座標
			p[1][0] = (float)i * pitchX - sizeX / 2.0;//x座標
			p[1][2] = (float)j * pitchZ - sizeZ / 2.0;//y座標

			glNormal3f(0.0, 1.0, 0.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[2]);//頂点座標			
		}
		glEnd();
	}
}
*/

void drawPolarGridPlate(float radius1, float radius2, int nSliceX, int nSliceY)
{
	//極座標の格子状円盤
	//xy平面，中心は原点
	int i, j;
	
	float p[2][3];
	float xiMax = log(radius2 / radius1);//
	float dx = xiMax / (float)nSliceX;//動径方向刻み
	float dy = 2.0 * M_PI / (float)nSliceY;//方位角方向刻み

	float r;
	for(j = 0; j < nSliceY; j++)
	{
		//頂点z座標
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSliceX; i++)
		{
			r = radius1 * exp((float)i * dx);
			p[0][0] = r * cos((float)j * dy);//x座標
			p[0][1] = r * sin((float)j * dy);//y座標
			p[1][0] = r * cos((float)(j+1) * dy);//x座標
			p[1][1] = r * sin((float)(j+1) * dy);;//y座標

			glNormal3f(0.0, 0.0, 1.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[1]);//頂点座標			
		}
		glEnd();
	}
}

//-----------------------------------------------------------------
void drawPolarGridLines(float radius1, float radius2, int nSliceX, int nSliceY)
{//xy平面，中心は原点(wireframe用）
	int i, j;
	//double ;
	float p[2][3];
	float xiMax = log(radius2 / radius1);//
	float dx = xiMax / (float)nSliceX;//動径方向刻み
	float dy = 2.0 * M_PI / (float)nSliceY;//方位角方向刻み

	float r;

	for(j = 0; j < nSliceY; j++)
	{
		//頂点z座標
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_LINES);
		for(i = 0; i <= nSliceX; i++)
		{
			r = radius1 * exp((float)i * dx);
			p[0][0] = r * cos((float)j * dy);//x座標
			p[0][1] = r * sin((float)j * dy);//y座標
			p[1][0] = r * cos((float)(j+1) * dy);//x座標
			p[1][1] = r * sin((float)(j+1) * dy);;//y座標

			glNormal3f(0.0, 0.0, 1.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[1]);//頂点座標			
		}
		glEnd();
	}

	for(i = 0; i < nSliceX; i++)
	{
		//頂点z座標
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_LINES);
		for(j = 0; j < nSliceY; j++)
		{
			r = radius1 * exp((float)i * dx);
			p[0][0] = r * cos((float)j * dy);//x座標
			p[0][1] = r * sin((float)j * dy);//y座標
			r = radius1 * exp((float)(i+1) * dx);
			p[1][0] = r * cos((float)j * dy);//x座標
			p[1][1] = r * sin((float)j * dy);;//y座標

			glNormal3f(0.0, 0.0, 1.0);//法線ベクトル
			glVertex3fv(p[0]);//頂点座標
			glVertex3fv(p[1]);//頂点座標			
		}
		glEnd();
	}
}



//-----------------------------------------------------------------------
void drawSphere(float radius, int nSlice, int nStack)
{
	int i, j;
	double r0, r1, th0, th1, phi;
	double p[2][3];

	for(j = 0; j < nStack; j++)
	{
		//j=0は北極点(x=0, y=0, z=radius)
		//天頂角
		th0 = M_PI * (double)j / (double)nStack;
		th1 = M_PI * (double)(j+1) / (double)nStack;
		//x-y平面に投影した半径
		r0 = radius * sin(th0);
		r1 = radius * sin(th1);
		//頂点z座標
		p[0][2] = radius * cos(th0);
		p[1][2] = radius * cos(th1);

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSlice; i++)
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
//----------------------------------------------------------------------
void drawCylinder(float rBottom, float rTop, float height, int nSlice, int nStack)
{ //上方向はz軸
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
		x = (float)(rTop * cos(theta)); //x成分
		y = (float)(rTop * sin(theta)); //ｙ成分
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
		x = (float)(rBottom * cos(theta)); //x成分
		y = (float)(rBottom * sin(theta)); //ｙ成分
		z = -height/2.0;
		glVertex3f(x, y, z);
	}
	glEnd();

	double s, t0, t1, r0, r1, phi;
	double p[2][3], n[3];

	double rr = rBottom - rTop;
	double nz = rr / sqrt(rr*rr + height*height);
  double nxy = sqrt(1.0 - nz * nz);
	//側面
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

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//頂点のxy座標
			p[0][0] = r0 * cos(phi);//x座標
			p[0][1] = r0 * sin(phi);//y座標
			p[1][0] = r1 * cos(phi);//x座標
			p[1][1] = r1 * sin(phi);//y座標
			//法線ベクトル
			n[0] = nxy * cos(phi);
			n[1] = nxy * sin(phi);
			n[2] = nz;

			glNormal3dv(n);//法線ベクトル
			glVertex3dv(p[0]);//頂点座標
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
	}
}
//----------------------------------------------------------------------
void drawCylinder(float rBottom, float rTop, float height, float thick, int nSlice, int nStack)
{ //厚みのある中空円筒
	//上方向はz軸
	//円柱(rBottom=rTop))、円錐台、円錐(rTop = 0.0)
	//rBottom:下底半径, rTop:上底半径（内側）
	//nSlice--xy断面分割数
	//nStack---ｚ方向分割数
	//物体の中心は下底と上底の中間

	int i, j;
	double x, y, z, z0, x1, y1,z1;
	double theta;
	double theta0 = 2.0*M_PI/(double)nSlice;
	//上底(Top)
	glBegin(GL_QUAD_STRIP);
	glNormal3f(0.0, 0.0, 1.0);
	//glVertex3f(0.0, 0.0, height/2.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		x = (float)(rTop * cos(theta)); //x成分(内側）
		y = (float)(rTop * sin(theta)); //y成分(内側）
		x1 = (float)((rTop + thick) * cos(theta)); //x成分(外側）
		y1 = (float)((rTop + thick) * sin(theta)); //y成分(外側）
		z = height/2.0;
		glVertex3f(x, y, z);
		glVertex3f(x1, y1, z);
	}
	glEnd();

	//下底(Bottom)
	glBegin(GL_QUAD_STRIP);
	glNormal3f(0.0, 0.0, -1.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)(nSlice - i);
		x = (float)(rBottom * cos(theta)); //x成分(内側）
		y = (float)(rBottom * sin(theta)); //ｙ成分(内側）
		x1 = (float)((rBottom + thick) * cos(theta)); //x成分(外側）
		y1 = (float)((rBottom + thick) * sin(theta)); //y成分(外側）
		z = -height/2.0;
		glVertex3f(x1, y1, z);
		glVertex3f(x, y, z);
	}
	glEnd();

	double s, t0, t1, r0, r1, phi;
	double p[2][3], n[3];

	double rr = rBottom - rTop;
	double nz = rr / sqrt(rr*rr + height*height);
  double nxy = sqrt(1.0 - nz * nz);
	//側面(外側）
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
		r0 = rBottom + thick + (rTop - rBottom) * z0 / height;
		r1 = rBottom + thick + (rTop - rBottom) * z1 / height;

		//頂点z座標
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//頂点のxy座標
			p[0][0] = r0 * cos(phi);//x座標
			p[0][1] = r0 * sin(phi);//y座標
			p[1][0] = r1 * cos(phi);//x座標
			p[1][1] = r1 * sin(phi);//y座標
			//法線ベクトル
			n[0] = nxy * cos(phi);
			n[1] = nxy * sin(phi);
			n[2] = nz;

			glNormal3dv(n);//法線ベクトル
			glVertex3dv(p[0]);//頂点座標
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
	}
	//側面(内側）
	for(j = 0; j < nStack; j++)
	{
		//j=0は上底(x=0, y=0, z=height/2)
		//2つのt座標
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//底面からの高さ
		z0 = height * t0;
		z1 = height * t1;
		//半径
		r0 = rBottom + (rTop - rBottom) * z0 / height;
		r1 = rBottom + (rTop - rBottom) * z1 / height;

		//頂点z座標
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//頂点のxy座標
			p[0][0] = r0 * cos(phi);//x座標
			p[0][1] = r0 * sin(phi);//y座標
			p[1][0] = r1 * cos(phi);//x座標
			p[1][1] = r1 * sin(phi);//y座標
			//法線ベクトル
			n[0] = -nxy * cos(phi);
			n[1] = -nxy * sin(phi);
			n[2] = -nz;

			glNormal3dv(n);//法線ベクトル
			glVertex3dv(p[1]);//頂点座標
			glVertex3dv(p[0]);//頂点座標
		}
		glEnd();
	}
}

//----------------------------------------------------------------------
void drawCylinderY(float rBottom, float rTop, float height, int nSlice, int nStack)
{ //上方向はY軸
	//円柱(rBottom=rTop))、円錐台、円錐(rTop = 0.0)
	//rBottom:下底半径, rTop:上底半径
	//nSlice--ZX断面分割数
	//nStack---Y方向分割数
	//物体の中心は下底と上底の中間

	int i, j;
	double Z, X, Y, Y0, Y1;
	double theta;
	double theta0 = 2.0*M_PI/(double)nSlice;
	//上底(Top)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, height/2.0, 0.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		Z = (float)(rTop * cos(theta)); //z成分
		X = (float)(rTop * sin(theta)); //x成分
		Y = height/2.0;
		glVertex3f(X, Y, Z);
	}
	glEnd();

	//下底(Bottom)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, -height/2.0, 0.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)(nSlice - i);
		Z = (float)(rBottom * cos(theta)); //z成分
		X = (float)(rBottom * sin(theta)); //x成分
		Y = -height/2.0;
		glVertex3f(X, Y, Z);
	}
	glEnd();

	double s, t0, t1, r0, r1, phi;
	double p[2][3], n[3];

	double rr = rBottom - rTop;
	double nY = rr / sqrt(rr*rr + height*height);
  double nZX = sqrt(1.0 - nY * nY);
	//側面
	for(j = 0; j < nStack; j++)
	{
		//j=0は上底(Z=0, X=0, Y=height/2)
		//2つのt座標
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//底面からの高さ
		Y0 = height * (1.0 - t0);
		Y1 = height * (1.0 - t1);
		//半径
		r0 = rBottom + (rTop - rBottom) * Y0 / height;
		r1 = rBottom + (rTop - rBottom) * Y1 / height;

		//頂点Y座標
		p[0][1] = Y0 - height / 2.0;
		p[1][1] = Y1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s座標
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//頂点のZX座標
			p[0][2] = r0 * cos(phi);//Z座標
			p[0][0] = r0 * sin(phi);//X座標
			p[1][2] = r1 * cos(phi);//Z座標
			p[1][0] = r1 * sin(phi);//X座標
			//法線ベクトル
			n[2] = nZX * cos(phi);
			n[0] = nZX * sin(phi);
			n[1] = nY;

			glNormal3dv(n);//法線ベクトル
			glVertex3dv(p[0]);//頂点座標
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
	}
}

//-----------------------------------------------------------------------------------------

void drawTorus(float radius1, float radius2, int nSide, int nRing)
{	
	//radius1:円環断面半径
	//radius2:円環の中心軸半径
	//nSide:円環断面における表面分割点数
	//nRing:円環の分割数
	if(radius1 > radius2) { printf("radius1 < radius2としてください \n "); return;}

	int i, j;
	double rr, zz;
	double phi0, phi1, theta;
	double p[2][3];

	for(i = 0; i < nRing; i++)
	{
		//i=0は基本断面(x=radius2を中心とする円, y=0）
		phi0 = 2.0 * M_PI * (double)i / (double)nRing;
		phi1 = 2.0 * M_PI * (double)(i+1) / (double)nRing;

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= nSide; j++)
		{
			theta = M_PI - 2.0 * M_PI * (double)j / (double)nSide;
			rr = radius2 + radius1 * cos(theta);//z軸からの距離
			zz = radius1 * sin(theta);
			//頂点のxyz座標(j=0を内側xy平面)
			p[0][0] = rr * cos(phi0);//x座標
			p[0][1] = rr * sin(phi0);//y
			p[0][2] = zz;            //z
			p[1][0] = rr * cos(phi1);//x座標
			p[1][1] = rr * sin(phi1);//y
			p[1][2] = zz;            //z      

			glNormal3d(cos(theta)*cos(phi0),cos(theta)*sin(phi0),sin(theta));
			glVertex3dv(p[0]);//頂点座標

			glNormal3d(cos(theta)*cos(phi1),cos(theta)*sin(phi1),sin(theta));
			glVertex3dv(p[1]);//頂点座標
		}
		glEnd();
	}
}
//-----------------------------------------------------------------
void drawSuper(float r, int nSlice, int nStack, double eps1, double eps2)
{
	//上下の中心が原点
	int i,j,ip,im,np,npL,npR,npU,npD,k1,k2;
	double ct,theta,phi,z,cc;
	float a[31][31], b[31][31], c[31][31];
	float n1[3], n2[3], n3[3], n4[3];
	float pd[31*31][3];

	if(nSlice > 30) nSlice = 30;
	if(nStack > 30) nStack = 30;

	for(j = 0 ;j <= nStack;j++)
	{
		theta = (M_PI/(double)nStack) * ((double)nStack / 2.0 - (double)j);
		                //thetaはx-y平面からの偏角となっている

		if(theta >= 0.0) //上半分
		{
			if(theta == 0.0) z = 0.0;//pow(a,b)のaがa<=0.0でエラー
			else z = pow(sin(fabs(theta)),eps1);//z
		}
		else  //下半分        
		{
			z = - pow(sin(fabs(theta)), eps1);
		}
		for (i = 0 ;i <= nSlice / 2;i++)
		{
			k1 = nSlice * j + i;// + nSlice / 8.0;//objectから見て左側
			k2 = nSlice * j + nSlice - i;// - nSlice / 8.0;//右側
			phi = 2.0 * M_PI * (double)i/(double)nSlice;
			ct = cos(phi);
			if( ct == 0.0 ) cc = 0.0;
			else if (ct > 0) { cc = pow(ct, eps2);}
			else             { cc = -pow(fabs(ct),eps2); }
			if(j == 0 || j == nStack) 
			{
				pd[k1][0] = 0.0f;
				pd[k1][1] = 0.0f;
			}

			else 
			{
				pd[k1][0] = r * (float)(pow(cos(theta),eps1)*cc);
				if(sin(phi) == 0.0) pd[k1][1] = 0.0f;
				else pd[k1][1] = r * (float)(pow(cos(theta),eps1)*pow(fabs(sin(phi)),eps2));
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

	glBegin(GL_QUADS);
	for(i = 0;i < nSlice;i++)
	{
		ip = i + 1;
		if(ip == nSlice) ip = 0;
		for(j = 0;j < nStack; j++)
		{
			glNormal3f(a[i][j],b[i][j],c[i][j]);
			glVertex3fv(pd[i+j*nSlice]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glVertex3fv(pd[i+(j+1)*nSlice]);
			glNormal3f(a[ip][j+1],b[ip][j+1],c[ip][j+1]);
			glVertex3fv(pd[ip+(j+1)*nSlice]);
			glNormal3f(a[ip][j],b[ip][j],c[ip][j]);
			glVertex3fv(pd[ip +j*nSlice]);
		}
	}
	glEnd();
}
void drawSuper(float* size1, float* size2, int nSlice, int nStack, double eps1, double eps2, 
			   float middle, float angle, int jStart, int type)
{	
	//上下の中心が原点
	int i,j,ip,im,np,npL,npR,npU,npD,k1,k2;
	double ct,phi,theta,z,fz;
	float a[31][31],b[31][31],c[31][31];
	float n1[3],n2[3],n3[3],n4[3];
	double cc, xx, yy, zz, aa, sr, cr;
	float pd[961][3];

	if( nSlice > 30 ) nSlice = 30;
	if( nStack > 30 ) nStack = 30;

	for(j = 0 ;j <= nStack;j++)
	{
		theta = (M_PI/(double)nStack) * ((double)nStack / 2.0 - (double)j);
		if(theta > 0.0) z = (float)(pow(sin(theta),eps1));//z
		else z = - (float)(pow(sin(fabs(theta)), eps1));
		//形状関数
		if(z < 0.0) fz = (middle-1.0)*z + middle;
		else fz = (1.0-middle)*z + middle;

		for (i = 0 ;i <= nSlice/2 ;i++)
		{
			k1 = nSlice * j + i;//object自身から見て左側(x > 0)
			k2 = nSlice * j + nSlice - i;//右側(x < 0)
			phi = 2.0 * M_PI * (double)i/(double)nSlice;
			ct = cos(phi);
			if (ct >= 0) { cc = pow(ct, eps2);}
			else  { cc = -pow(fabs(ct),eps2); }
			if(j == 0 || j == nStack) 
			{
				pd[k1][0] = 0.0f;
				pd[k1][1] = 0.0f;
			}
			else 
			{
				if(j <= nStack/2)
				{
					pd[k1][0] = size1[0] * (float)(pow(cos(theta),eps1)*cc*fz);
					pd[k1][1] = size1[1] * (float)(pow(cos(theta),eps1)*pow(fabs(sin(phi)),eps2)*fz);
				}
				else
				{
					pd[k1][0] = size2[0] * (float)(pow(cos(theta),eps1)*cc*fz);
					pd[k1][1] = size2[1] * (float)(pow(cos(theta),eps1)*pow(fabs(sin(phi)),eps2)*fz);
				}
			}
			if(i == 0) k2 = k1;

			pd[k2][0] = pd[k1][0];
			pd[k2][1] = -pd[k1][1];
			if(j <= nStack/2)
			{
				pd[k1][2] = size1[2] * (float)z;
				pd[k2][2] = size1[2] * (float)z;
			}
			else
			{
				pd[k1][2] = size2[2] * (float)z;
				pd[k2][2] = size2[2] * (float)z;
			}
		}
	}

	//変形
	if(type == 0)
	{
		//前方：z軸
		//z軸回転(x>0なら正のz軸回転，x<0なら負のz軸回転）
		for(j = jStart; j < nStack; j++)
		{
			for(i = 0; i <= nSlice/2; i++)
			{
				k1 = nSlice * j + i;//自分から見て左側(x > 0)
				k2 = nSlice * j + nSlice - i;//右側(x < 0)
				xx = pd[k1][0]; yy = pd[k1][1];
				if(j <= nStack/2)
					aa = M_PI * angle * fabs(xx) / size1[0] / 180.0;
				else
					aa = M_PI * angle * fabs(xx) / size2[0] / 180.0;
				cr = cos(aa);
				sr = sin(aa);
				if(xx > 0.0)
				{
					pd[k1][0] = xx * cr - yy * sr;
					pd[k1][1] = xx * sr + yy * cr;
				}
				else
				{
					pd[k1][0] = xx * cr + yy * sr ;
					pd[k1][1] = -xx * sr + yy * cr;
				}

				xx = pd[k2][0]; yy = pd[k2][1];
				if(j <= nStack/2)
					aa = M_PI * angle * fabs(xx) / size1[0] / 180.0;
				else
					aa = M_PI * angle * fabs(xx) / size2[0] / 180.0;
				
				cr = cos(aa);
				sr = sin(aa);
				if(xx > 0.0)
				{
					pd[k2][0] = (xx * cr - yy * sr) ;
					pd[k2][1] = (xx * sr + yy * cr) ;
				}
				else
				{
					pd[k2][0] = (xx * cr + yy * sr) ;
					pd[k2][1] = (-xx * sr + yy * cr) ;
				}
			}
		}
	}

	else if(type == 1)
	{
		//前方：z軸
		//後半を上下（x軸回転）
		for(j = jStart; j <= nStack; j++)
		{
			for(i = 0; i < nSlice; i++)
			{
				k1 = nSlice * j + i;
				yy = pd[k1][1]; zz = pd[k1][2];
				aa = M_PI * angle / 180.0 * (float)(j-jStart) / (float)nSlice;
				cr = cos(aa);
				sr = sin(aa);
				pd[k1][1] = yy * cr - zz * sr;
				pd[k1][2] = yy * sr + zz * cr;
				
			}
		}
	}

	else if(type == 2)
	{
		//前方：z軸
		//後半を左右（y軸回転）
		for(j = jStart; j <= nStack; j++)
		{
			for(i = 0; i < nSlice; i++)
			{
				k1 = nSlice * j + i;
				xx = pd[k1][0]; zz = pd[k1][2];
				aa = M_PI * angle / 180.0 * (float)(j-jStart) / (float)nSlice;
				cr = cos(aa);
				sr = sin(aa);
				pd[k1][0] = xx * cr + zz * sr;
				pd[k1][2] = -xx * sr + zz * cr;
				
			}
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
		a[i][0] = 0.0; b[i][0] = 0.0; c[i][0] = 1.0;
		//真下（Bottom)
		a[i][nStack] = 0.0; b[i][nStack] = 0.0; c[i][nStack] = -1.0;

		for(j=1;j<nStack;j++)//隣り合う4個の三角形の法線ベクトルを平均化
		{
			np = j*nSlice+i;//注目点
			npL = j*nSlice+im;//左側
			npR = j*nSlice+ip;//右側
			npU = np-nSlice;//上
			npD = np+nSlice;//下
			if(j == 1) {
				n1[0]=0.0; n1[1]=0.0; n1[2]=1.0;//Top
				n2[0]=0.0; n2[1]=0.0; n2[2]=1.0;//Top
				calcNormal(pd[np],pd[npL],pd[npD],n3);//外から見て左下
				calcNormal(pd[np],pd[npD],pd[npR],n4);//右下
			}
			if(j == nStack-1){
				calcNormal(pd[np],pd[npU],pd[npL],n1);//外から見て左上
				calcNormal(pd[np],pd[npR],pd[npU],n2);//右上
				n3[0]=0.0; n3[1]=0.0; n3[2]=-1.0;//Bottom
				n4[0]=0.0; n4[1]=0.0; n4[2]=-1.0;//Bottom
			}
			else {
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
	//表示
	glBegin(GL_TRIANGLES);
	//上底
	for(i = 0;i < nSlice;i++)
	{	ip = i+1;
		if(ip == nSlice) ip = 0;

			glNormal3f(a[i][0],b[i][0],c[i][0]);glVertex3fv(pd[i]);
			glNormal3f(a[i][1],b[i][1],c[i][1]);glVertex3fv(pd[nSlice+i]);
			glNormal3f(a[ip][1],b[ip][1],c[ip][1]);glVertex3fv(pd[nSlice+ip]);
	}
	//下底
	j = nStack - 1;
	for(i = 0;i < nSlice;i++)
	{	ip = i+1;
		if(ip == nSlice) ip = 0;
			glNormal3f(a[i][j],b[i][j],c[i][j]);glVertex3fv(pd[j*nSlice+i]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);glVertex3fv(pd[(j+1)*nSlice+i]);
			glNormal3f(a[ip][j],b[ip][j],c[ip][j]);glVertex3fv(pd[j*nSlice+ip]);
	}
	glEnd();
	//側面(4角形パッチ）
	glBegin(GL_QUADS);
	for(j = 0;j < nStack;j++)
		for(i = 0;i < nSlice;i++)
		{	ip = i+1;
			if(ip == nSlice) ip=0;

				glNormal3f(a[i][j],b[i][j],c[i][j]);glVertex3fv(pd[j*nSlice+i]);
				glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);glVertex3fv(pd[(j+1)*nSlice+i]);
				glNormal3f(a[ip][j+1],b[ip][j+1],c[ip][j+1]);glVertex3fv(pd[(j+1)*nSlice+ip]);
				glNormal3f(a[ip][j],b[ip][j],c[ip][j]);glVertex3fv(pd[j*nSlice+ip]);
		}
	glEnd();
}

/*
//-----------------------------------------------------------------------------
//四角形のメッシュ（x-y平面,中心が原点）
//ｘ軸方向，ｙ軸方向の幅を固定
void drawElevation(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, float* data)
{
	//全体の幅,長さsizeX, sizeY
	//sideFlag = 0:側面表示せず
	//sideFlag = 1:側面表示する

	const int NMAX = 110;
	int i, j;
	float p[NMAX][NMAX][3]; //頂点座標
	float a[NMAX][NMAX], b[NMAX][NMAX], c[NMAX][NMAX];//頂点の法線
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	if(Nx > NMAX) printf("NxがNMAXを超えています(drawElevation1) \n");
	if(Ny > NMAX) printf("NyがNMAXを超えています(drawElevation1) \n");

	//セルのサイズ
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;

	//各頂点の座標
	for(j = 0; j <= Ny; j++){
		for(i = 0; i <= Nx; i++){
			p[i][j][0] = (float)(i - Nx / 2) * pitchX;
			p[i][j][1] = (float)(j - Ny / 2) * pitchY;
			p[i][j][2] = data[j * (Nx+1) + i];
		}
	}
	
	//法線成分
	for(i = 0;i <= Nx;i++)
		for(j = 0;j <= Ny;j++)
		{			
			if(j == 0 )
			{
				if(i == 0) 
				{
					calcNormal(p[0][0],p[1][0],p[0][1],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx-1][0],p[Nx][0],p[Nx][1],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else 
				{
					calcNormal(p[i][0],p[i][1],p[i-1][0],n1);//左側
					calcNormal(p[i][0],p[i+1][0],p[i][1],n2);//右側
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
			}
			else if(j == Ny)
			{
				if(i == 0) 
				{
					calcNormal(p[0][Ny],p[0][Ny-1],p[1][Ny],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx][Ny],p[Nx-1][Ny],p[Nx][Ny-1],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else 
				{
					calcNormal(p[i][Ny],p[i-1][Ny],p[i][Ny-1],n1);//左側
					calcNormal(p[i][Ny],p[i][Ny-1],p[i+1][Ny],n2);//右側
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
			}
			else
			{
				if(i == 0) 
				{
					calcNormal(p[0][j],p[1][j],p[0][j+1],n1);//上
					calcNormal(p[0][j],p[0][j-1],p[0][1],n2);//下
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx][j],p[Nx][j+1],p[Nx-1][j],n1);//上
					calcNormal(p[Nx][j],p[Nx-1][j],p[Nx][j-1],n2);//下
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else 
				{//上下左右４個の三角形の平均
					calcNormal(p[i][j],p[i][j+1],p[i-1][j],n1);//左上
					calcNormal(p[i][j],p[i+1][j],p[i][j+1],n2);//右上
					calcNormal(p[i][j],p[i-1][j],p[i][j-1],n3);//左下
					calcNormal(p[i][j],p[i][j-1],p[i+1][j],n4);//右下
					a[i][j] = (n1[0]+n2[0]+n3[0]+n4[0])/4.0f;
					b[i][j] = (n1[1]+n2[1]+n3[1]+n4[1])/4.0f;
					c[i][j] = (n1[2]+n2[2]+n3[2]+n4[2])/4.0f; }
			}
		}

	//三角形で面を定義
	glBegin(GL_TRIANGLES);
	for(j = 0;j < Ny;j++)
		for(i = 0;i < Nx;i++)
		{
			//左下の三角形
			//各頂点の法線方向,頂点座標を与える。
			glNormal3f(a[i][j],b[i][j],c[i][j]);//法線方向
			glVertex3fv(p[i][j]);//ﾎﾟﾘｺﾞﾝの頂点座標（以下これらを繰り返す）
			glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
			glVertex3fv(p[i+1][j]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glVertex3fv(p[i][j+1]);
			//右上の三角形
			glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
			glVertex3fv(p[i+1][j]);
			glNormal3f(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glVertex3fv(p[i+1][j+1]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glVertex3fv(p[i][j+1]);
		}
	glEnd();

	if(sideFlag == 1)//側面描画
	{
		glBegin(GL_QUADS);
		//+x方向（i=Nx)
		glNormal3f(1.0, 0.0, 0.0);
		for(j = 0; j < Ny; j++)
		{
			glVertex3f(p[Nx][j][0], p[Nx][j][1], 0.0f);
			glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], 0.0f);
			glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], p[Nx][j+1][2]);
			glVertex3f(p[Nx][j][0], p[Nx][j][1], p[Nx][j][2]);
		}
		//-x方向（i=0)
		glNormal3f(-1.0, 0.0, 0.0);
		for(j = 0; j < Ny; j++)
		{
			glVertex3f(p[0][j][0], p[0][j][1], 0.0f);
			glVertex3f(p[0][j][0], p[0][j][1], p[0][j][2]);
			glVertex3f(p[0][j+1][0], p[0][j+1][1], p[0][j+1][2]);
			glVertex3f(p[0][j+1][0], p[0][j+1][1], 0.0f);
		}
		//+y方向（j=Ny)
		glNormal3f(0.0, 1.0, 0.0);
		for(i = 0; i < Nx; i++)
		{
			glVertex3f(p[i][Ny][0], p[i][Ny][1], 0.0f);
			glVertex3f(p[i][Ny][0], p[i][Ny][1], p[i][Ny][2]);
			glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], p[i+1][Ny][2]);
			glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], 0.0f);
		}
		//-y方向（j=0)
		glNormal3f(0.0, -1.0, 0.0);
		for(i = 0; i < Nx; i++)
		{
			glVertex3f(p[i][0][0], p[i][0][1], 0.0f);
			glVertex3f(p[i+1][0][0], p[i+1][0][1], 0.0f);
			glVertex3f(p[i+1][0][0], p[i+1][0][1], p[i+1][0][2]);
			glVertex3f(p[i][0][0], p[i][0][1], p[i][0][2]);
		}
		glEnd();
	}
}
//-----------------------------------------------------------------------------
//四角形のメッシュ（x-y平面,中心が原点）
//ｘ軸方向，ｙ軸方向の幅を固定
void drawElevationC(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, int model, float* data)
{
	//全体の幅,長さsizeX, sizeY
	//sideFlag = 0:側面表示せず
	//sideFlag = 1:側面表示する
	//model = 0:Solid model
	//model = 1:Wireframe model

	const int NMAX = 110;
	int i, j;
	float p[NMAX][NMAX][3]; //頂点座標
	float a[NMAX][NMAX], b[NMAX][NMAX], c[NMAX][NMAX];//頂点の法線
	float rr[NMAX][NMAX], gg[NMAX][NMAX], bb[NMAX][NMAX];
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	if(Nx > NMAX) printf("NxがNMAXを超えています(drawElevation1) \n");
	if(Ny > NMAX) printf("NyがNMAXを超えています(drawElevation1) \n");

	//セルのサイズ
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;

	//各頂点の座標,色
	for(j = 0; j <= Ny; j++){
		for(i = 0; i <= Nx; i++){
			p[i][j][0] = (float)(i - Nx / 2) * pitchX;
			p[i][j][1] = (float)(j - Ny / 2) * pitchY;
			p[i][j][2] = data[j * (Nx+1) + i];
			if(p[i][j][2] < 0.25)
			{
				rr[i][j] = 0.0; gg[i][j] = 4.0 * p[i][j][2]; bb[i][j] = 1.0;
			}
			else if(p[i][j][2] < 0.5)
			{
				rr[i][j] = 0.0; gg[i][j] = 1.0; bb[i][j] = 4.0 * (0.5 - p[i][j][2]);
			}
			else if(p[i][j][2] < 0.75)
			{
				rr[i][j] = 4.0 * (p[i][j][2] - 0.5); gg[i][j] = 1.0; bb[i][j] = 0.0;
			}
			else
			{
				rr[i][j] = 1.0; gg[i][j] = (1.0 - p[i][j][2]) * 4.0; bb[i][j] = 0.0;
			}
		}
	}
	
	//法線成分
	for(i = 0;i <= Nx;i++)
		for(j = 0;j <= Ny;j++)
		{			
			if(j == 0 )
			{
				if(i == 0) 
				{
					calcNormal(p[0][0],p[1][0],p[0][1],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx-1][0],p[Nx][0],p[Nx][1],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else 
				{
					calcNormal(p[i][0],p[i][1],p[i-1][0],n1);//左側
					calcNormal(p[i][0],p[i+1][0],p[i][1],n2);//右側
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
			}
			else if(j == Ny)
			{
				if(i == 0) 
				{
					calcNormal(p[0][Ny],p[0][Ny-1],p[1][Ny],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx][Ny],p[Nx-1][Ny],p[Nx][Ny-1],n1);
					a[i][j] = n1[0]; b[i][j] = n1[1]; c[i][j] = n1[2]; }
				else 
				{
					calcNormal(p[i][Ny],p[i-1][Ny],p[i][Ny-1],n1);//左側
					calcNormal(p[i][Ny],p[i][Ny-1],p[i+1][Ny],n2);//右側
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
			}
			else
			{
				if(i == 0) 
				{
					calcNormal(p[0][j],p[1][j],p[0][j+1],n1);//上
					calcNormal(p[0][j],p[0][j-1],p[0][1],n2);//下
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx][j],p[Nx][j+1],p[Nx-1][j],n1);//上
					calcNormal(p[Nx][j],p[Nx-1][j],p[Nx][j-1],n2);//下
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else 
				{//上下左右４個の三角形の平均
					calcNormal(p[i][j],p[i][j+1],p[i-1][j],n1);//左上
					calcNormal(p[i][j],p[i+1][j],p[i][j+1],n2);//右上
					calcNormal(p[i][j],p[i-1][j],p[i][j-1],n3);//左下
					calcNormal(p[i][j],p[i][j-1],p[i+1][j],n4);//右下
					a[i][j] = (n1[0]+n2[0]+n3[0]+n4[0])/4.0f;
					b[i][j] = (n1[1]+n2[1]+n3[1]+n4[1])/4.0f;
					c[i][j] = (n1[2]+n2[2]+n3[2]+n4[2])/4.0f; }
			}
		}

	static float dif[4];
	if(model == 0)//solid model
	{
		//static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
		//glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
		//glMaterialf(GL_FRONT,GL_SHININESS,100);
		//三角形で面を定義
		glBegin(GL_TRIANGLES);
		for(j = 0;j < Ny;j++)
			for(i = 0;i < Nx;i++)
			{
				//左下の三角形
				//各頂点の法線方向,頂点座標を与える。
				glNormal3f(a[i][j],b[i][j],c[i][j]);//法線方向
				dif[0] = rr[i][j], dif[1] = gg[i][j], dif[2] = bb[i][j], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i][j]);//ﾎﾟﾘｺﾞﾝの頂点座標（以下これらを繰り返す）
				glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
				dif[0] = rr[i+1][j], dif[1] = gg[i+1][j], dif[2] = bb[i+1][j], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i+1][j]);
				glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
				dif[0] = rr[i][j+1], dif[1] = gg[i][j+1], dif[2] = bb[i][j+1], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i][j+1]);
				//右上の三角形
				glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
				dif[0] = rr[i+1][j], dif[1] = gg[i+1][j], dif[2] = bb[i+1][j], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i+1][j]);
				glNormal3f(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
				dif[0] = rr[i+1][j+1], dif[1] = gg[i+1][j+1], dif[2] = bb[i+1][j+1], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i+1][j+1]);
				glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
				dif[0] = rr[i][j+1], dif[1] = gg[i][j+1], dif[2] = bb[i][j+1], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i][j+1]);
			}
		glEnd();

		if(sideFlag == 1)//側面描画
		{
			glBegin(GL_QUADS);
			//+x方向（i=Nx)
			glNormal3f(1.0, 0.0, 0.0);
			for(j = 0; j < Ny; j++)
			{
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[Nx][j][0], p[Nx][j][1], 0.0f);
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], 0.0f);
				dif[0] = rr[Nx][j+1], dif[1] = gg[Nx][j+1], dif[2] = bb[Nx][j+1], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], p[Nx][j+1][2]);
				dif[0] = rr[Nx][j], dif[1] = gg[Nx][j], dif[2] = bb[Nx][j], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[Nx][j][0], p[Nx][j][1], p[Nx][j][2]);
			}
			//-x方向（i=0)
			glNormal3f(-1.0, 0.0, 0.0);
			for(j = 0; j < Ny; j++)
			{
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[0][j][0], p[0][j][1], 0.0f);
				dif[0] = rr[0][j], dif[1] = gg[0][j], dif[2] = bb[0][j], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[0][j][0], p[0][j][1], p[0][j][2]);
				dif[0] = rr[0][j+1], dif[1] = gg[0][j+1], dif[2] = bb[0][j+1], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[0][j+1][0], p[0][j+1][1], p[0][j+1][2]);
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[0][j+1][0], p[0][j+1][1], 0.0f);
			}
			//+y方向（j=Ny)
			glNormal3f(0.0, 1.0, 0.0);
			for(i = 0; i < Nx; i++)
			{
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i][Ny][0], p[i][Ny][1], 0.0f);	
				dif[0] = rr[i][Ny], dif[1] = gg[i][Ny], dif[2] = bb[i][Ny], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i][Ny][0], p[i][Ny][1], p[i][Ny][2]);	
				dif[0] = rr[i+1][Ny], dif[1] = gg[i+1][Ny], dif[2] = bb[i+1][Ny], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], p[i+1][Ny][2]);
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], 0.0f);
			}
			//-y方向（j=0)
			glNormal3f(0.0, -1.0, 0.0);
			for(i = 0; i < Nx; i++)
			{
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i][0][0], p[i][0][1], 0.0f);
				
				dif[0] = 0.0, dif[1] = 0.0, dif[2] = 1.0, dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i+1][0][0], p[i+1][0][1], 0.0f);
				
				dif[0] = rr[i+1][0], dif[1] = gg[i+1][0], dif[2] = bb[i+1][0], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i+1][0][0], p[i+1][0][1], p[i+1][0][2]);
				
				dif[0] = rr[i][0], dif[1] = gg[i][0], dif[2] = bb[i][0], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3f(p[i][0][0], p[i][0][1], p[i][0][2]);
			}
			glEnd();
		}
		return;
	}

	//三角形で面を定義
	glBegin(GL_TRIANGLES);
	for(j = 0;j < Ny;j++)
		for(i = 0;i < Nx;i++)
		{
			//左下の三角形
			//各頂点の法線方向,頂点座標を与える。
			glNormal3f(a[i][j],b[i][j],c[i][j]);//法線方向
			glColor3f(rr[i][j], gg[i][j], bb[i][j]);
			glVertex3fv(p[i][j]);//ﾎﾟﾘｺﾞﾝの頂点座標（以下これらを繰り返す）
			glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
			glColor3f(rr[i+1][j], gg[i+1][j], bb[i+1][j]);
			glVertex3fv(p[i+1][j]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glColor3f(rr[i][j+1], gg[i][j+1], bb[i][j+1]);
			glVertex3fv(p[i][j+1]);
			//右上の三角形
			glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
			glColor3f(rr[i+1][j], gg[i+1][j], bb[i+1][j]);
			glVertex3fv(p[i+1][j]);
			glNormal3f(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glColor3f(rr[i+1][j+1], gg[i+1][j+1], bb[i+1][j+1]);
			glVertex3fv(p[i+1][j+1]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glColor3f(rr[i][j+1], gg[i][j+1], bb[i][j+1]);
			glVertex3fv(p[i][j+1]);
		}
	glEnd();

	if(sideFlag == 1)//側面描画
	{
		glBegin(GL_QUADS);
		//+x方向（i=Nx)
		glNormal3f(1.0, 0.0, 0.0);
		for(j = 0; j < Ny; j++)
		{
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[Nx][j][0], p[Nx][j][1], 0.0f);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], 0.0f);
			glColor3f(rr[Nx][j+1], gg[Nx][j+1], bb[Nx][j+1]);
			glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], p[Nx][j+1][2]);
			glColor3f(rr[Nx][j], gg[Nx][j], bb[Nx][j]);
			glVertex3f(p[Nx][j][0], p[Nx][j][1], p[Nx][j][2]);
		}
		//-x方向（i=0)
		glNormal3f(-1.0, 0.0, 0.0);
		for(j = 0; j < Ny; j++)
		{
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[0][j][0], p[0][j][1], 0.0f);
			glColor3f(rr[0][j], gg[0][j], bb[0][j]);
			glVertex3f(p[0][j][0], p[0][j][1], p[0][j][2]);
			glColor3f(rr[0][j+1], gg[0][j+1], bb[0][j+1]);
			glVertex3f(p[0][j+1][0], p[0][j+1][1], p[0][j+1][2]);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[0][j+1][0], p[0][j+1][1], 0.0f);
		}
		//+y方向（j=Ny)
		glNormal3f(0.0, 1.0, 0.0);
		for(i = 0; i < Nx; i++)
		{
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[i][Ny][0], p[i][Ny][1], 0.0f);
			glColor3f(rr[i][Ny], gg[i][Ny], bb[i][Ny]);
			glVertex3f(p[i][Ny][0], p[i][Ny][1], p[i][Ny][2]);
			glColor3f(rr[i+1][Ny], gg[i+1][Ny], bb[i+1][Ny]);
			glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], p[i+1][Ny][2]);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], 0.0f);
		}
		//-y方向（j=0)
		glNormal3f(0.0, -1.0, 0.0);
		for(i = 0; i < Nx; i++)
		{
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[i][0][0], p[i][0][1], 0.0f);
			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(p[i+1][0][0], p[i+1][0][1], 0.0f);
			glColor3f(rr[i+1][0], gg[i+1][0], bb[i+1][0]);
			glVertex3f(p[i+1][0][0], p[i+1][0][1], p[i+1][0][2]);
			glColor3f(rr[i][0], gg[i][0], bb[i][0]);
			glVertex3f(p[i][0][0], p[i][0][1], p[i][0][2]);
		}
		glEnd();
	}
}
*/



//-----------------------------------------------------------------------------
//四角形のメッシュ（x-y平面,中心が原点）
//ｘ軸方向，ｙ軸方向の幅を固定
void drawElevation(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, float* data)
{
	//全体の幅,長さsizeX, sizeY
	//sideFlag = 0:側面表示せず
	//sideFlag = 1:側面表示する

	int i, j;
	float p[8][3];//頂点座標
	float a[2][3];//頂点の法線
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	//セルのサイズ
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;

	for(j = 0; j < Ny; j++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= Nx; i++)
		{
			//各頂点の座標(i, j)
			p[0][0] = (float)(i - Nx / 2) * pitchX;
			p[0][1] = (float)(j - Ny / 2) * pitchY;
			p[0][2] = data[j * (Nx+1) + i];
			//(i, j+1)
			p[1][0] = (float)(i - Nx / 2) * pitchX;
			p[1][1] = (float)(j+1 - Ny / 2) * pitchY;
			p[1][2] = data[(j+1) * (Nx+1) + i];

			//法線成分
			if(j == 0)
			{
				if(i == 0)
				{
					//(i+1, j)
					p[2][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i+1];
					//(i+1, j+1)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[3][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[4][0] = (float)(i - Nx / 2) * pitchX;
					p[4][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[4][2] = data[(j+2) * (Nx+1) + i];
					
					calcNormal(p[0],p[2],p[1],n1);
					a[0][0] = n1[0]; a[0][1] = n1[1]; a[0][2] = n1[2];
					calcNormal(p[0],p[3],p[1],n1);
					calcNormal(p[1],p[3],p[4],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}
				else if(i == Nx)
				{
					//(i-1, j)
					p[2][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i-1];
					//(i-1, j+1)
					p[3][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[3][2] = data[(j+1) * (Nx+1) + i-1];
					//(i, j+2)
					p[4][0] = (float)(i - Nx / 2) * pitchX;
					p[4][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[4][2] = data[(j+2) * (Nx+1) + i];
					
					calcNormal(p[0],p[1],p[2],n1);
					a[0][0] = n1[0]; a[0][1] = n1[1]; a[0][2] = n1[2];
					calcNormal(p[0],p[1],p[3],n1);
					calcNormal(p[1],p[4],p[3],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}
				else
				{
					//(i+1, j)
					p[2][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i+1];
					//(i-1, j)
					p[3][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					calcNormal(p[0],p[2],p[1],n1);
					calcNormal(p[0],p[1],p[3],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					//(i+1, j+1)
					p[5][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[5][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[5][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[6][0] = (float)(i - Nx / 2) * pitchX;
					p[6][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[6][2] = data[(j+2) * (Nx+1) + i];
					//(i-1, j+1)
					p[7][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[7][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[7][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[1],p[0],p[5],n1);
					calcNormal(p[1],p[5],p[6],n2);
					calcNormal(p[1],p[6],p[7],n3);
					calcNormal(p[1],p[7],p[0],n4);
					a[1][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[1][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[1][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;
				}

			}
			if(j == Ny-1)
			{
				if(i == 0)
				{
					//(i+1, j)
					p[2][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i+1];
					//(i, j-1)
					p[3][0] = (float)(i - Nx / 2) * pitchX;
					p[3][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[3][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j+1)
					p[4][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i+1];
					calcNormal(p[0],p[2],p[1],n1);
					calcNormal(p[0],p[3],p[2],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[0],p[4],p[1],n1);
					a[1][0] = n1[0]; a[1][1] = n1[1]; a[1][2] = n1[2];

				}
				if(i == Nx)
				{
					//(i-1, j)
					p[2][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i-1];
					//(i, j-1)
					p[3][0] = (float)(i - Nx / 2) * pitchX;
					p[3][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[3][2] = data[(j-1) * (Nx+1) + i];
					//(i-1, j+1)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[1],p[2],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[0],p[1],p[4],n1);
					a[1][0] = n1[0]; a[1][1] = n1[1]; a[1][2] = n1[2];
				}
				else
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					//(i-1, j)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j - Ny / 2) * pitchY;
					p[4][2] = data[j * (Nx+1) + i-1];
					
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[3],p[1],n2);
					calcNormal(p[0],p[1],p[4],n3);
					calcNormal(p[0],p[4],p[2],n4);
					a[0][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[0][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[0][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;

					//(i+1, j+1)
					p[5][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[5][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[5][2] = data[(j+1) * (Nx+1) + i+1];
					//(i-1, j+1)
					p[7][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[7][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[7][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[1],p[0],p[5],n1);
					calcNormal(p[1],p[7],p[0],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}

			}
			else
			{
				if(i == 0)
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					//(i+1, j+1)
					p[4][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[5][0] = (float)(i - Nx / 2) * pitchX;
					p[5][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[5][2] = data[(j+2) * (Nx+1) + i];
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[3],p[1],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[1],p[0],p[4],n1);
					calcNormal(p[1],p[4],p[5],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;

				}
				if(i == Nx)
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i-1, j)
					p[3][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i-1];
					//(i-1, j+1)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i-1];
					//(i, j+2)
					p[5][0] = (float)(i - Nx / 2) * pitchX;
					p[5][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[5][2] = data[(j+2) * (Nx+1) + i];
					calcNormal(p[0],p[3],p[2],n1);
					calcNormal(p[0],p[1],p[3],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[0],p[1],p[4],n1);
					calcNormal(p[4],p[1],p[5],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}
				else
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					//(i-1, j)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j - Ny / 2) * pitchY;
					p[4][2] = data[j * (Nx+1) + i-1];
					
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[3],p[1],n2);
					calcNormal(p[0],p[1],p[4],n3);
					calcNormal(p[0],p[4],p[2],n4);
					a[0][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[0][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[0][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;

					//(i+1, j+1)
					p[5][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[5][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[5][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[6][0] = (float)(i - Nx / 2) * pitchX;
					p[6][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[6][2] = data[(j+2) * (Nx+1) + i];
					//(i-1, j+1)
					p[7][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[7][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[7][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[1],p[0],p[5],n1);
					calcNormal(p[1],p[5],p[6],n2);
					calcNormal(p[1],p[6],p[7],n3);
					calcNormal(p[1],p[7],p[0],n4);
					a[1][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[1][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[1][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;
				}
			}
			glNormal3fv(a[1]);
			glVertex3fv(p[1]);
			glNormal3fv(a[0]);
			glVertex3fv(p[0]);
		}
		glEnd();

	}

	if(sideFlag == 1)//側面描画
	{
		//+x方向（i=Nx)
		glNormal3f(1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = 0; j <= Ny; j++)
		{
			//各頂点の座標(i, j)
			p[0][0] = (float)(Nx - Nx / 2) * pitchX;
			p[0][1] = (float)(j - Ny / 2) * pitchY;
			p[0][2] = data[j * (Nx+1) + Nx];
			
			p[1][0] = (float)(Nx - Nx / 2) * pitchX;
			p[1][1] = (float)(j - Ny / 2) * pitchY;
			p[1][2] = 0.0;
			glVertex3fv(p[0]);
			glVertex3fv(p[1]);
		}
		glEnd();
		//-x方向（i=0)
		glNormal3f(-1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = Ny; j >= 0; j--)
		{
			//各頂点の座標(i, j)
			p[0][0] = (float)(0 - Nx / 2) * pitchX;
			p[0][1] = (float)(j - Ny / 2) * pitchY;
			p[0][2] = data[j * (Nx+1)];
			
			p[1][0] = (float)(0 - Nx / 2) * pitchX;
			p[1][1] = (float)(j - Ny / 2) * pitchY;
			p[1][2] = 0.0;
			glVertex3fv(p[0]);
			glVertex3fv(p[1]);
		}
		glEnd();
		//+y方向（j=Ny)
		glNormal3f(0.0, 1.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(i = Nx; i >= 0; i--)
		{
			p[0][0] = (float)(i - Nx / 2) * pitchX;
			p[0][1] = (float)(Ny - Ny / 2) * pitchY;
			p[0][2] = data[Ny * (Nx+1) + i];
			
			p[1][0] = (float)(i - Nx / 2) * pitchX;
			p[1][1] = (float)(Ny - Ny / 2) * pitchY;
			p[1][2] = 0.0;
			glVertex3fv(p[0]);
			glVertex3fv(p[1]);
		}
		glEnd();
		//-y方向（j=0)
		glNormal3f(0.0, -1.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= Nx; i++)
		{
			p[0][0] = (float)(i - Nx / 2) * pitchX;
			p[0][1] = (float)( - Ny / 2) * pitchY;
			p[0][2] = data[i];
			
			p[1][0] = (float)(i - Nx / 2) * pitchX;
			p[1][1] = (float)( - Ny / 2) * pitchY;
			p[1][2] = 0.0;

			glVertex3fv(p[0]);
			glVertex3fv(p[1]);
		}
		glEnd();
	}
}
//-----------------------------------------------------------------------------
//四角形のメッシュ（x-y平面,中心が原点）
//ｘ軸方向，ｙ軸方向の幅を固定
void drawElevationC(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, int model, float* data)
{
	//全体の幅,長さsizeX, sizeY
	//sideFlag = 0:側面表示せず
	//sideFlag = 1:側面表示する
	//model = 0:Solid model
	//model = 1:Wireframe model

	const int NMAX = 110;
	int i, j, k;
	float p[8][3]; //頂点座標
	float a[2][3];//頂点の法線
	float rr[2], gg[2], bb[2];
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	if(Nx > NMAX) printf("NxがNMAXを超えています(drawElevation1) \n");
	if(Ny > NMAX) printf("NyがNMAXを超えています(drawElevation1) \n");

	//セルのサイズ
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;
	static float dif[4];

	for(j = 0; j < Ny; j++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= Nx; i++)
		{
			//各頂点の座標(i, j)
			p[0][0] = (float)(i - Nx / 2) * pitchX;
			p[0][1] = (float)(j - Ny / 2) * pitchY;
			p[0][2] = data[j * (Nx+1) + i];
			//(i, j+1)
			p[1][0] = (float)(i - Nx / 2) * pitchX;
			p[1][1] = (float)(j+1 - Ny / 2) * pitchY;
			p[1][2] = data[(j+1) * (Nx+1) + i];

			for(k = 0; k < 2; k++)
			{
				if(p[k][2] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * p[k][2]; bb[k] = 1.0;
				}
				else if(p[k][2] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - p[k][2]);
				}
				else if(p[k][2] < 0.75)
				{
					rr[k] = 4.0 * (p[k][2] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - p[k][2]) * 4.0; bb[k] = 0.0;
				}
			}

			//法線成分
			if(j == 0)
			{
				if(i == 0)
				{
					//(i+1, j)
					p[2][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i+1];
					//(i+1, j+1)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[3][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[4][0] = (float)(i - Nx / 2) * pitchX;
					p[4][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[4][2] = data[(j+2) * (Nx+1) + i];
					
					calcNormal(p[0],p[2],p[1],n1);
					a[0][0] = n1[0]; a[0][1] = n1[1]; a[0][2] = n1[2];
					calcNormal(p[0],p[3],p[1],n1);
					calcNormal(p[1],p[3],p[4],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}
				else if(i == Nx)
				{
					//(i-1, j)
					p[2][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i-1];
					//(i-1, j+1)
					p[3][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[3][2] = data[(j+1) * (Nx+1) + i-1];
					//(i, j+2)
					p[4][0] = (float)(i - Nx / 2) * pitchX;
					p[4][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[4][2] = data[(j+2) * (Nx+1) + i];
					
					calcNormal(p[0],p[1],p[2],n1);
					a[0][0] = n1[0]; a[0][1] = n1[1]; a[0][2] = n1[2];
					calcNormal(p[0],p[1],p[3],n1);
					calcNormal(p[1],p[4],p[3],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}
				else
				{
					//(i+1, j)
					p[2][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i+1];
					//(i-1, j)
					p[3][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					calcNormal(p[0],p[2],p[1],n1);
					calcNormal(p[0],p[1],p[3],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					//(i+1, j+1)
					p[5][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[5][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[5][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[6][0] = (float)(i - Nx / 2) * pitchX;
					p[6][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[6][2] = data[(j+2) * (Nx+1) + i];
					//(i-1, j+1)
					p[7][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[7][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[7][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[1],p[0],p[5],n1);
					calcNormal(p[1],p[5],p[6],n2);
					calcNormal(p[1],p[6],p[7],n3);
					calcNormal(p[1],p[7],p[0],n4);
					a[1][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[1][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[1][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;
				}

			}
			if(j == Ny-1)
			{
				if(i == 0)
				{
					//(i+1, j)
					p[2][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i+1];
					//(i, j-1)
					p[3][0] = (float)(i - Nx / 2) * pitchX;
					p[3][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[3][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j+1)
					p[4][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i+1];
					calcNormal(p[0],p[2],p[1],n1);
					calcNormal(p[0],p[3],p[2],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[0],p[4],p[1],n1);
					a[1][0] = n1[0]; a[1][1] = n1[1]; a[1][2] = n1[2];

				}
				if(i == Nx)
				{
					//(i-1, j)
					p[2][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[2][1] = (float)(j - Ny / 2) * pitchY;
					p[2][2] = data[j * (Nx+1) + i-1];
					//(i, j-1)
					p[3][0] = (float)(i - Nx / 2) * pitchX;
					p[3][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[3][2] = data[(j-1) * (Nx+1) + i];
					//(i-1, j+1)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[1],p[2],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[0],p[1],p[4],n1);
					a[1][0] = n1[0]; a[1][1] = n1[1]; a[1][2] = n1[2];
				}
				else
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					//(i-1, j)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j - Ny / 2) * pitchY;
					p[4][2] = data[j * (Nx+1) + i-1];
					
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[3],p[1],n2);
					calcNormal(p[0],p[1],p[4],n3);
					calcNormal(p[0],p[4],p[2],n4);
					a[0][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[0][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[0][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;

					//(i+1, j+1)
					p[5][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[5][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[5][2] = data[(j+1) * (Nx+1) + i+1];
					//(i-1, j+1)
					p[7][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[7][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[7][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[1],p[0],p[5],n1);
					calcNormal(p[1],p[7],p[0],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}

			}
			else
			{
				if(i == 0)
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					//(i+1, j+1)
					p[4][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[5][0] = (float)(i - Nx / 2) * pitchX;
					p[5][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[5][2] = data[(j+2) * (Nx+1) + i];
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[3],p[1],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[1],p[0],p[4],n1);
					calcNormal(p[1],p[4],p[5],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;

				}
				if(i == Nx)
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i-1, j)
					p[3][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i-1];
					//(i-1, j+1)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[4][2] = data[(j+1) * (Nx+1) + i-1];
					//(i, j+2)
					p[5][0] = (float)(i - Nx / 2) * pitchX;
					p[5][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[5][2] = data[(j+2) * (Nx+1) + i];
					calcNormal(p[0],p[3],p[2],n1);
					calcNormal(p[0],p[1],p[3],n2);
					a[0][0] = (n1[0] + n2[0]) / 2.0;
					a[0][1] = (n1[1] + n2[1]) / 2.0;
					a[0][2] = (n1[2] + n2[2]) / 2.0;
					calcNormal(p[0],p[1],p[4],n1);
					calcNormal(p[4],p[1],p[5],n2);
					a[1][0] = (n1[0] + n2[0]) / 2.0;
					a[1][1] = (n1[1] + n2[1]) / 2.0;
					a[1][2] = (n1[2] + n2[2]) / 2.0;
				}
				else
				{
					//(i, j-1)
					p[2][0] = (float)(i - Nx / 2) * pitchX;
					p[2][1] = (float)(j-1 - Ny / 2) * pitchY;
					p[2][2] = data[(j-1) * (Nx+1) + i];
					//(i+1, j)
					p[3][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[3][1] = (float)(j - Ny / 2) * pitchY;
					p[3][2] = data[j * (Nx+1) + i+1];
					//(i-1, j)
					p[4][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[4][1] = (float)(j - Ny / 2) * pitchY;
					p[4][2] = data[j * (Nx+1) + i-1];
					
					calcNormal(p[0],p[2],p[3],n1);
					calcNormal(p[0],p[3],p[1],n2);
					calcNormal(p[0],p[1],p[4],n3);
					calcNormal(p[0],p[4],p[2],n4);
					a[0][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[0][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[0][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;

					//(i+1, j+1)
					p[5][0] = (float)(i+1 - Nx / 2) * pitchX;
					p[5][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[5][2] = data[(j+1) * (Nx+1) + i+1];
					//(i, j+2)
					p[6][0] = (float)(i - Nx / 2) * pitchX;
					p[6][1] = (float)(j+2 - Ny / 2) * pitchY;
					p[6][2] = data[(j+2) * (Nx+1) + i];
					//(i-1, j+1)
					p[7][0] = (float)(i-1 - Nx / 2) * pitchX;
					p[7][1] = (float)(j+1 - Ny / 2) * pitchY;
					p[7][2] = data[(j+1) * (Nx+1) + i-1];
					calcNormal(p[1],p[0],p[5],n1);
					calcNormal(p[1],p[5],p[6],n2);
					calcNormal(p[1],p[6],p[7],n3);
					calcNormal(p[1],p[7],p[0],n4);
					a[1][0] = (n1[0] + n2[0] + n3[0] + n4[0]) / 4.0;
					a[1][1] = (n1[1] + n2[1] + n3[1] + n4[1]) / 4.0;
					a[1][2] = (n1[2] + n2[2] + n3[2] + n4[2]) / 4.0;
				}
			}
			dif[0] = rr[1], dif[1] = gg[1], dif[2] = bb[1], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glNormal3fv(a[1]);
			glVertex3fv(p[1]);
			dif[0] = rr[0], dif[1] = gg[0], dif[2] = bb[0], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glNormal3fv(a[0]);
			glVertex3fv(p[0]);
		}
		glEnd();

	}

	if(sideFlag == 1)//側面描画
	{
		//+x方向（i=Nx)
		glNormal3f(1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = 0; j <= Ny; j++)
		{
			//各頂点の座標(i, j)
			p[0][0] = (float)(Nx - Nx / 2) * pitchX;
			p[0][1] = (float)(j - Ny / 2) * pitchY;
			p[0][2] = data[j * (Nx+1) + Nx];
			
			p[1][0] = (float)(Nx - Nx / 2) * pitchX;
			p[1][1] = (float)(j - Ny / 2) * pitchY;
			p[1][2] = 0.0;

			for(k = 0; k < 2; k++)
			{
				if(p[k][2] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * p[k][2]; bb[k] = 1.0;
				}
				else if(p[k][2] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - p[k][2]);
				}
				else if(p[k][2] < 0.75)
				{
					rr[k] = 4.0 * (p[k][2] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - p[k][2]) * 4.0; bb[k] = 0.0;
				}
			}

			dif[0] = rr[0], dif[1] = gg[0], dif[2] = bb[0], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[0]);
			dif[0] = rr[1], dif[1] = gg[1], dif[2] = bb[1], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[1]);
		}
		glEnd();
		//-x方向（i=0)
		glNormal3f(-1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = Ny; j >= 0; j--)
		{
			//各頂点の座標(i, j)
			p[0][0] = (float)(0 - Nx / 2) * pitchX;
			p[0][1] = (float)(j - Ny / 2) * pitchY;
			p[0][2] = data[j * (Nx+1)];
			
			p[1][0] = (float)(0 - Nx / 2) * pitchX;
			p[1][1] = (float)(j - Ny / 2) * pitchY;
			p[1][2] = 0.0;

			for(k = 0; k < 2; k++)
			{
				if(p[k][2] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * p[k][2]; bb[k] = 1.0;
				}
				else if(p[k][2] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - p[k][2]);
				}
				else if(p[k][2] < 0.75)
				{
					rr[k] = 4.0 * (p[k][2] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - p[k][2]) * 4.0; bb[k] = 0.0;
				}
			}
			dif[0] = rr[0], dif[1] = gg[0], dif[2] = bb[0], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[0]);
			dif[0] = rr[1], dif[1] = gg[1], dif[2] = bb[1], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[1]);
		}
		glEnd();
		//+y方向（j=Ny)
		glNormal3f(0.0, 1.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(i = Nx; i >= 0; i--)
		{
			p[0][0] = (float)(i - Nx / 2) * pitchX;
			p[0][1] = (float)(Ny - Ny / 2) * pitchY;
			p[0][2] = data[Ny * (Nx+1) + i];
			
			p[1][0] = (float)(i - Nx / 2) * pitchX;
			p[1][1] = (float)(Ny - Ny / 2) * pitchY;
			p[1][2] = 0.0;

			for(k = 0; k < 2; k++)
			{
				if(p[k][2] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * p[k][2]; bb[k] = 1.0;
				}
				else if(p[k][2] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - p[k][2]);
				}
				else if(p[k][2] < 0.75)
				{
					rr[k] = 4.0 * (p[k][2] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - p[k][2]) * 4.0; bb[k] = 0.0;
				}
			}
			dif[0] = rr[0], dif[1] = gg[0], dif[2] = bb[0], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[0]);
			dif[0] = rr[1], dif[1] = gg[1], dif[2] = bb[1], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[1]);
		}
		glEnd();
		//-y方向（j=0)
		glNormal3f(0.0, -1.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= Nx; i++)
		{
			p[0][0] = (float)(i - Nx / 2) * pitchX;
			p[0][1] = (float)( - Ny / 2) * pitchY;
			p[0][2] = data[i];
			
			p[1][0] = (float)(i - Nx / 2) * pitchX;
			p[1][1] = (float)( - Ny / 2) * pitchY;
			p[1][2] = 0.0;

			for(k = 0; k < 2; k++)
			{
				if(p[k][2] < 0.25)
				{
					rr[k] = 0.0; gg[k] = 4.0 * p[k][2]; bb[k] = 1.0;
				}
				else if(p[k][2] < 0.5)
				{
					rr[k] = 0.0; gg[k] = 1.0; bb[k] = 4.0 * (0.5 - p[k][2]);
				}
				else if(p[k][2] < 0.75)
				{
					rr[k] = 4.0 * (p[k][2] - 0.5); gg[k] = 1.0; bb[k] = 0.0;
				}
				else
				{
					rr[k] = 1.0; gg[k] = (1.0 - p[k][2]) * 4.0; bb[k] = 0.0;
				}
			}
			dif[0] = rr[0], dif[1] = gg[0], dif[2] = bb[0], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[0]);
			dif[0] = rr[1], dif[1] = gg[1], dif[2] = bb[1], dif[3] = 1.0; 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
			glVertex3fv(p[1]);
		}
		glEnd();
	}
}



//----------------------------------------------------------------------
//２次元格子状オブジェクト
void drawGridSquare(int numRow, int numCol, float pd[][3])
{	//裏面も定義
	int i, j, np;
	float n1[3], n2[3], n3[3], n4[3];
	float a[101][101], b[101][101], c[101][101];

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
	//描画
	for(i = 0;i < numRow-1;i++)
	  for(j = 0;j < numCol-1;j++){
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
//-------------------------------------------------------------------
void drawFloorY(float widthX, float widthZ, int nx, int nz)
{
  int i, j;
  //Floor１枚当たりの幅
  float wX = widthX / (float)nx;
  float wZ = widthZ / (float)nz;

  float diffuse[][4] = {
//	{ 0.7, 0.7, 0.6, 1.0}, { 0.3f, 0.4, 0.4, 1.0} };
	{ 0.7, 0.7, 0.65, 1.0}, { 0.35, 0.4, 0.4, 1.0} };
  float ambient[] = { 0.2, 0.2, 0.2, 1.0};
  float specular[]= { 0.5, 0.5, 0.5, 1.0};
  glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
  glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
  glMaterialf(GL_FRONT,GL_SHININESS,100);

  glNormal3f(0.0, 1.0, 0.0);
  glPushMatrix();
  for (j = 0; j < nz; j++) 
  {
    float z1 = -widthZ / 2.0 + wZ * j; float z2 = z1 + wZ;
    for (i = 0; i < nx; i++) {
      float x1 = -widthX / 2.0 + wX * i; float x2 = x1 + wX;

      glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse[(i + j) & 1]);
	  glBegin(GL_QUADS);
      glVertex3f(x1, 0.0, z1);
      glVertex3f(x1, 0.0, z2);
      glVertex3f(x2, 0.0, z2);
      glVertex3f(x2, 0.0, z1);
	  glEnd();
    }
  }
  glPopMatrix();
}

void drawFloorZ(float widthX, float widthY, int nx, int ny)
{//ｚ軸：鉛直軸
  int i, j;
  //Floor１枚当たりの幅
  float wX = widthX / (float)nx;
  float wY = widthY / (float)ny;

  float diffuse[][4] = {
	{ 0.7, 0.7, 0.5, 1.0}, { 0.3, 0.3, 0.5, 1.0} };
//	{ 0.9, 1.0, 0.8, 1.0}, { 0.5, 0.7, 0.8, 1.0} };
  float ambient[] = { 0.5, 0.5, 0.5, 1.0};
  float specular[]= { 0.3, 0.3, 0.3, 1.0};

  //glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
  glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
  glMaterialf(GL_FRONT,GL_SHININESS,10);

  glNormal3d(0.0, 0.0, 1.0);
  glPushMatrix();
  for (j = 0; j < ny; j++) 
	{
    float y1 = -widthY / 2.0 + wY * j; float y2 = y1 + wY;
    for (i = 0; i < nx; i++) 
		{
      float x1 = -widthX / 2.0 + wX * i; float x2 = x1 + wX;

      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse[(i + j) & 1]);
			glBegin(GL_QUADS);
      glVertex3d(x1, y1, 0.0);
      glVertex3d(x2, y1, 0.0);
      glVertex3d(x2, y2, 0.0);
      glVertex3d(x1, y2, 0.0);
			glEnd();
    }
  }
  glPopMatrix();
}
//------------------------------------------------------------------------------------------------------
void drawGridZ(float widthX, float widthY, int nx, int ny)
{//ｚ軸：鉛直軸
  int i, j;
	float x1, y1, x2, y2;
  //Floor１枚当たりの幅
  float wX = widthX / (float)nx;
  float wY = widthY / (float)ny;

  float diffuse[] = {0.9, 0.6, 0.1, 1.0};
  float ambient[] = { 0.2, 0.2, 0.2, 1.0};
  float specular[]= { 0.1, 0.1, 0.1, 1.0};

  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
  glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
  glMaterialf(GL_FRONT,GL_SHININESS,10);

	glColor3f(0.4, 0.4, 0.5);
  glNormal3d(0.0, 0.0, 1.0);
  glPushMatrix();
  for (j = 0; j <= ny; j++) 
	{
    y1 = -widthY / 2.0 + wY * j;
    x1 = -widthX / 2.0; x2 = widthX / 2.0;

	glBegin(GL_LINES);
    glVertex3d(x1, y1, 0.0); glVertex3d(x2, y1, 0.0);
	glEnd();
  }
  for (i = 0; i <= nx; i++) 
	{
    x1 = -widthX / 2.0 + wX * i; 
    y1 = -widthY / 2.0; y2 = widthY / 2.0;

		glBegin(GL_LINES);
    glVertex3d(x1, y1, 0.0); glVertex3d(x1, y2, 0.0);
		glEnd();
    
  }

  glPopMatrix();
}
//----------------------------------------------------------------
void drawSpring(int Nm, int Ns, int Np, float radius, float ratio, float length)
{
	//Nm:主円周分割点数,Ns:断面円周分割点数,Np:ピッチ数
	//radius:主円周軸半径,ratio:断面半径/主円周軸半径,length:全長
	//バネの中間を原点
	int i, ii, j, jj, k, k1, k2;
	double phai, ph1, ph2;  //主円周分割点のx軸に対する偏角
	double theta, th1, th2; //断面円周分割点のx-z平面に対する偏角
	float p[21][21][101][3];//各点の座標
	float p1[3], p2[3], p3[3], p4[3];
	float rr[21], zz[21];
	float pitch, dp, hh;
	float r1, r2;

	r1 = radius; //主円周軸半径
	r2 = ratio * r1;//断面半径
	if( Nm > 20 ) Nm = 20;
	if( Ns > 20 ) Ns = 20;
	if( Np > 100 ) Np = 100;
	pitch = length / (float)Np;

	dp = (float)pitch / (float)Nm;

	//基本断面(x-z)の座標
	for(j = 0; j < Ns; j++)
	{	theta = M_PI - 2.0 * M_PI*(double)j/(double)Ns;
		rr[j] = r1 + r2 * (float)cos(theta); //原点からの距離
		zz[j] = r2 * (float)sin(theta);//ｚ
	}

	//他の断面の座標
	hh = 0;
	for(k = 0; k < Np; k++)
		for(i = 0; i < Nm; i++)
		{	phai = 2.0 * M_PI * (double)i/(double)Nm;
			for(j = 0; j < Ns; j++)
			{
				p[i][j][k][0] = rr[j] * (float)cos(phai); //x座標
				p[i][j][k][1] = rr[j] * (float)sin(phai); //y
				p[i][j][k][2] = zz[j] + hh - length / 2.0 ;              //z
			}
			hh += dp;//中心軸の高さがdpずつ増加
		}

	//最終端(k=Np-1,i=Nm)
	k = Np - 1; i = Nm;
	for(j = 0; j < Ns; j++){
		phai = 0.0;
		p[i][j][k][0] = rr[j] * (float)cos(phai); //x座標
		p[i][j][k][1] = rr[j] * (float)sin(phai); //y
		p[i][j][k][2] = zz[j] + hh - length / 2.0;            //z
	}

	//頂点列を定義し描画
	for(k = 0; k < Np; k++)
	for(i = 0; i < Nm; i++){
		ii = i+1;
		k1 = k; k2 = k;
		if(ii == Nm) {
			if(k < Np-1) { ii = 0; k2 = k + 1; }
		}
		ph1 = 2.0*M_PI*(double)i / (double)Nm;
		ph2 = 2.0*M_PI*(double)ii / (double)Nm;
		for(j = 0;j < Ns; j++)
		{
			jj = j+1;
			if(jj == Ns) jj = 0;
			th1 = M_PI - 2.0 * M_PI * (double)j / (double)Ns;
			th2 = M_PI - 2.0 * M_PI * (double)jj / (double)Ns;
			//面の頂点   x座標                       y座標                       z座標
			p1[0] = p[i][j][k1][0]   ; p1[1] = p[i][j][k1][1]   ; p1[2] = p[i][j][k1][2];
			p2[0] = p[i][jj][k1][0]  ; p2[1] = p[i][jj][k1][1]  ; p2[2] = p[i][jj][k1][2];
			p3[0] = p[ii][jj][k2][0] ; p3[1] = p[ii][jj][k2][1] ; p3[2] = p[ii][jj][k2][2];
			p4[0] = p[ii][j][k2][0]  ; p4[1] = p[ii][j][k2][1]  ; p4[2] = p[ii][j][k2][2];

			glBegin(GL_QUADS);
				glNormal3d(cos(th1)*cos(ph1),cos(th1)*sin(ph1),sin(th1));glVertex3fv(p1);
				glNormal3d(cos(th2)*cos(ph1),cos(th2)*sin(ph1),sin(th2));glVertex3fv(p2);
				glNormal3d(cos(th2)*cos(ph2),cos(th2)*sin(ph2),sin(th2));glVertex3fv(p3);
				glNormal3d(cos(th1)*cos(ph2),cos(th1)*sin(ph2),sin(th1));glVertex3fv(p4);
			glEnd();
		}
	}
	//始端（k=0)
	glBegin(GL_POLYGON);
		glNormal3d(0.0,-1.0,0.0);
		for(j = Ns-1; j >= 0; j--) glVertex3fv(p[0][j][0]);
	glEnd();
	//終端（k=Np)
	glBegin(GL_POLYGON);
		glNormal3d(0.0,1.0,0.0);
		for(j = 0; j < Ns; j++) glVertex3fv(p[Nm][j][Np-1]);
	glEnd();

}
//----------------------------------------------------------------------
//立方体格子状オブジェクト
void drawGridCube(int numRow, int numCol, int numStk, float p1[][3],
        float p2[][3], float p3[][3], float p4[][3], float p5[][3],
        float p6[][3])
{
    //すべて同じテクスチャー
    //上側
    drawGridSquare(numRow, numCol, p1);
    //正面
    drawGridSquare(numStk, numCol, p2);
    //右側
    drawGridSquare(numStk, numRow, p3);
    //左側
    drawGridSquare(numStk, numRow, p4);
    //裏側
    drawGridSquare(numStk, numCol, p5);
    //下側
    drawGridSquare(numRow, numCol, p6);
}
//----------------------------------------------------------------------------
//球格子状オブジェクト
void drawGridSphere(int Nxy, int Nz, float pd[][3])
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

//----------------------------------------------------------------------
//側面だけに円筒投影
void drawGridCylinder(int Nxy, int Nz, float p[][3])
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
