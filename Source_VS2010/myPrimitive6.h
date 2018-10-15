#include <stdio.h>

#define M_PI 3.14159265358979323846

//�@�������v�Z���[�`��
void calcNormal(float* p1,float* p2,float* p3,float* nn)
{
	Vector3D A = Vector3D(p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]);
	Vector3D B = Vector3D(p3[0] - p2[0], p3[1] - p2[1], p3[2] - p2[2]);
	Vector3D n = vCross(A , B);//�O��
	n.norm();
	nn[0] = n.x; nn[1] = n.y; nn[2] = n.z;
}
//------------------------------------------------------------------------------
void drawPlate()//x-y����
{//�ʂ̖@������������
	float p[4][3] = 
	{ { 0.5,-0.5, 0.0}, { 0.5, 0.5, 0.0}, {-0.5, 0.5, 0.0}, {-0.5,-0.5, 0.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0); //z�����̖@��
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}

//------------------------------------------------------------------------------
void drawPlateZ(float s)//x-y����
{//�ʂ̖@������������
	float p[4][3] = 
	{ { s/2.0,-s/2.0, 0.0}, { s/2.0, s/2.0, 0.0}, {-s/2.0, s/2.0, 0.0}, 
	  {-s/2.0,-s/2.0, 0.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0); //z�����̖@��
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateY(float s)//x-z����
{//�ʂ̖@��������y��
	float p[4][3] = 
	{ { s/2.0, 0.0, s/2.0}, { s/2.0, 0.0,-s/2.0}, {-s/2.0, 0.0,-s/2.0}, {-s/2.0, 0.0, s/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0); //y�����̖@��
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
/*
//------------------------------------------------------------------------------
void drawPlatePX(float sx, float sy, float sz)//y-z����
{//�ʂ̖@��������+x��
	float p[4][3] = 
	{ { sx, sy/2.0, sz/2.0}, { sx, -sy/2.0, sz/2.0}, {sx, -sy/2.0,-sz/2.0}, {sx, sy/2.0, -sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(1.0, 0.0, 0.0); //+x�����̖@��
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateMX(float sx, float sy, float sz)//y-z����
{//�ʂ̖@��������-x��
	float p[4][3] = 
	{ { sx, sy/2.0, sz/2.0}, { sx, -sy/2.0, sz/2.0}, {sx, -sy/2.0,-sz/2.0}, {sx, sy/2.0, -sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(-1.0, 0.0, 0.0); //-x�����̖@��
		glVertex3fv(p[3]);
		glVertex3fv(p[2]);
		glVertex3fv(p[1]);
		glVertex3fv(p[0]);
	glEnd();
}

//------------------------------------------------------------------------------
void drawPlatePY(float sx, float sy, float sz)//x-z����
{//�ʂ̖@��������+y��
	float p[4][3] = 
	{ { sx/2.0, sy, sz/2.0}, { sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy, sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0); //+y�����̖@��
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateMY(float sx, float sy, float sz)//x-z����
{//�ʂ̖@��������-y��
	float p[4][3] = 
	{ { sx/2.0, sy, sz/2.0}, { sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy,-sz/2.0}, {-sx/2.0, sy, sz/2.0}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, -1.0, 0.0); //-y�����̖@��
		glVertex3fv(p[3]);
		glVertex3fv(p[2]);
		glVertex3fv(p[1]);
		glVertex3fv(p[0]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlatePZ(float sx, float sy, float sz)//x-y����
{//�ʂ̖@��������+z��
	float p[4][3] = 
	{ { sx/2.0, sy/2.0, sz}, {-sx/2.0, sy/2.0, sz}, {-sx/2.0, -sy/2.0,sz}, {sx/2.0, -sy/2.0, sz}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0); //+z�����̖@��
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		glVertex3fv(p[3]);
	glEnd();
}
//------------------------------------------------------------------------------
void drawPlateMZ(float sx, float sy, float sz)//x-y����
{//�ʂ̖@��������-z��
	float p[4][3] = 
	{ { sx/2.0, sy/2.0, sz}, {-sx/2.0, sy/2.0, sz}, {-sx/2.0, -sy/2.0,sz}, {sx/2.0, -sy/2.0, sz}};

	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, -1.0); //+z�����̖@��
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
		glNormal3f(0.0f,0.0f,1.0f); //z����
		glVertex3fv(p[0]); glVertex3fv(p[1]);
		glVertex3fv(p[2]); glVertex3fv(p[3]);

		glNormal3f(1.0f,0.0f,0.0f); //x����(���ʁj
		glVertex3fv(p[0]); glVertex3fv(p[3]);
		glVertex3fv(p[7]); glVertex3fv(p[4]);

		glNormal3f(0.0f,1.0f,0.0f); //y����
		glVertex3fv(p[0]); glVertex3fv(p[4]);
		glVertex3fv(p[5]); glVertex3fv(p[1]);

	 	glNormal3f(-1.0f,0.0f,0.0f); //-x����
		glVertex3fv(p[1]); glVertex3fv(p[5]);
		glVertex3fv(p[6]); glVertex3fv(p[2]);

		glNormal3f(0.0f,-1.0f,0.0f); //-y����
		glVertex3fv(p[2]); glVertex3fv(p[6]);
		glVertex3fv(p[7]); glVertex3fv(p[3]);

		glNormal3f(0.0f,0.0f,-1.0f); //-z����
		glVertex3fv(p[4]); glVertex3fv(p[7]);
		glVertex3fv(p[6]); glVertex3fv(p[5]);
	glEnd();
}

void drawGridPlate(float sizeX, float sizeY, int nSliceX, int nSliceY)
{//xy���ʁC���S�͌��_
	int i, j;
	//double ;
	float p[2][3];
	float pitchX = sizeX / (float)nSliceX;
	float pitchY = sizeY / (float)nSliceY;

	for(j = 0; j < nSliceY; j++)
	{
		//���_z���W
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSliceX; i++)
		{

			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x���W
			p[0][1] = (float)(j+1) * pitchY - sizeY / 2.0;//y���W
			p[1][0] = (float)i * pitchX - sizeX / 2.0;//x���W
			p[1][1] = (float)j * pitchY - sizeY / 2.0;//y���W

			glNormal3f(0.0, 0.0, 1.0);//�@���x�N�g��
			glVertex3fv(p[0]);//���_���W
			glVertex3fv(p[1]);//���_���W			
		}
		glEnd();
	}
}

//-----------------------------------------------------------------
void drawGridLines(float sizeX, float sizeY, int nSliceX, int nSliceY)
{//xy���ʁC���S�͌��_(wireframe�p�j
	int i, j;
	//double ;
	float p[2][3];
	float pitchX = sizeX / (float)nSliceX;
	float pitchY = sizeY / (float)nSliceY;

	for(j = 0; j < nSliceY; j++)
	{
		//���_z���W
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_LINES);
		for(i = 0; i <= nSliceX; i++)
		{
			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x���W
			p[0][1] = (float)j * pitchY - sizeY / 2.0;//y���W
			p[1][0] = (float)i * pitchX - sizeX / 2.0;//x���W
			p[1][1] = (float)(j+1) * pitchY - sizeY / 2.0;//y���W

			glNormal3f(0.0, 0.0, 1.0);//�@���x�N�g��
			glVertex3fv(p[0]);//���_���W
			glVertex3fv(p[1]);//���_���W			
		}
		glEnd();
	}

	for(i = 0; i < nSliceX; i++)
	{
		//���_z���W
		p[0][2] = 0.0;
		p[1][2] = 0.0;
		glBegin(GL_LINES);
		for(j = 0; j <= nSliceY; j++)
		{
			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x���W
			p[0][1] = (float)j * pitchY - sizeY / 2.0;//y���W
			p[1][0] = (float)(i+1) * pitchX - sizeX / 2.0;//x���W
			p[1][1] = (float)j * pitchY - sizeY / 2.0;//y���W

			glNormal3f(0.0, 0.0, 1.0);//�@���x�N�g��
			glVertex3fv(p[0]);//���_���W
			glVertex3fv(p[1]);//���_���W			
		}
		glEnd();
	}
}
/*
//---------------------------------------------------------------------------
void drawGridPlateY(float sizeX, float sizeZ, int nSliceX, int nSliceZ)
{//xz���ʁC���S�͌��_(������������)
	int i, j;
	//double ;
	float p[2][3];
	float pitchX = sizeX / (float)nSliceX;
	float pitchZ = sizeZ / (float)nSliceZ;

	for(j = 0; j < nSliceZ; j++)
	{
		//���_y���W
		p[0][1] = 0.0;
		p[1][1] = 0.0;

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSliceX; i++)
		{

			p[0][0] = (float)i * pitchX - sizeX / 2.0;//x���W
			p[0][2] = (float)(j+1) * pitchZ - sizeZ / 2.0;//y���W
			p[1][0] = (float)i * pitchX - sizeX / 2.0;//x���W
			p[1][2] = (float)j * pitchZ - sizeZ / 2.0;//y���W

			glNormal3f(0.0, 1.0, 0.0);//�@���x�N�g��
			glVertex3fv(p[0]);//���_���W
			glVertex3fv(p[2]);//���_���W			
		}
		glEnd();
	}
}
*/

void drawPolarGridPlate(float radius1, float radius2, int nSliceX, int nSliceY)
{
	//�ɍ��W�̊i�q��~��
	//xy���ʁC���S�͌��_
	int i, j;
	
	float p[2][3];
	float xiMax = log(radius2 / radius1);//
	float dx = xiMax / (float)nSliceX;//���a��������
	float dy = 2.0 * M_PI / (float)nSliceY;//���ʊp��������

	float r;
	for(j = 0; j < nSliceY; j++)
	{
		//���_z���W
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSliceX; i++)
		{
			r = radius1 * exp((float)i * dx);
			p[0][0] = r * cos((float)j * dy);//x���W
			p[0][1] = r * sin((float)j * dy);//y���W
			p[1][0] = r * cos((float)(j+1) * dy);//x���W
			p[1][1] = r * sin((float)(j+1) * dy);;//y���W

			glNormal3f(0.0, 0.0, 1.0);//�@���x�N�g��
			glVertex3fv(p[0]);//���_���W
			glVertex3fv(p[1]);//���_���W			
		}
		glEnd();
	}
}

//-----------------------------------------------------------------
void drawPolarGridLines(float radius1, float radius2, int nSliceX, int nSliceY)
{//xy���ʁC���S�͌��_(wireframe�p�j
	int i, j;
	//double ;
	float p[2][3];
	float xiMax = log(radius2 / radius1);//
	float dx = xiMax / (float)nSliceX;//���a��������
	float dy = 2.0 * M_PI / (float)nSliceY;//���ʊp��������

	float r;

	for(j = 0; j < nSliceY; j++)
	{
		//���_z���W
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_LINES);
		for(i = 0; i <= nSliceX; i++)
		{
			r = radius1 * exp((float)i * dx);
			p[0][0] = r * cos((float)j * dy);//x���W
			p[0][1] = r * sin((float)j * dy);//y���W
			p[1][0] = r * cos((float)(j+1) * dy);//x���W
			p[1][1] = r * sin((float)(j+1) * dy);;//y���W

			glNormal3f(0.0, 0.0, 1.0);//�@���x�N�g��
			glVertex3fv(p[0]);//���_���W
			glVertex3fv(p[1]);//���_���W			
		}
		glEnd();
	}

	for(i = 0; i < nSliceX; i++)
	{
		//���_z���W
		p[0][2] = 0.0;
		p[1][2] = 0.0;

		glBegin(GL_LINES);
		for(j = 0; j < nSliceY; j++)
		{
			r = radius1 * exp((float)i * dx);
			p[0][0] = r * cos((float)j * dy);//x���W
			p[0][1] = r * sin((float)j * dy);//y���W
			r = radius1 * exp((float)(i+1) * dx);
			p[1][0] = r * cos((float)j * dy);//x���W
			p[1][1] = r * sin((float)j * dy);;//y���W

			glNormal3f(0.0, 0.0, 1.0);//�@���x�N�g��
			glVertex3fv(p[0]);//���_���W
			glVertex3fv(p[1]);//���_���W			
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
		//j=0�͖k�ɓ_(x=0, y=0, z=radius)
		//�V���p
		th0 = M_PI * (double)j / (double)nStack;
		th1 = M_PI * (double)(j+1) / (double)nStack;
		//x-y���ʂɓ��e�������a
		r0 = radius * sin(th0);
		r1 = radius * sin(th1);
		//���_z���W
		p[0][2] = radius * cos(th0);
		p[1][2] = radius * cos(th1);

		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			phi = 2.0 * M_PI * (double)i / (double)nSlice;
			//���_��xy���W(i=0��object����݂ĉE�[) 
			p[0][0] =   r0 * sin(phi);//x���W
			p[0][1] = - r0 * cos(phi);//y���W
			p[1][0] =   r1 * sin(phi);//x���W
			p[1][1] = - r1 * cos(phi);//y���W

			glNormal3dv(p[0]);//�@���x�N�g��
			glVertex3dv(p[0]);//���_���W

			glNormal3dv(p[1]);//�@���x�N�g��
			glVertex3dv(p[1]);//���_���W			
		}
		glEnd();
	}
}
//----------------------------------------------------------------------
void drawCylinder(float rBottom, float rTop, float height, int nSlice, int nStack)
{ //�������z��
	//�~��(rBottom=rTop))�A�~����A�~��(rTop = 0.0)
	//rBottom:���ꔼ�a, rTop:��ꔼ�a
	//nSlice--xy�f�ʕ�����
	//nStack---������������
	//���̂̒��S�͉���Ə��̒���

	int i, j;
	double x, y, z, z0, z1;
	double theta;
	double theta0 = 2.0*M_PI/(double)nSlice;
	//���(Top)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, height/2.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		x = (float)(rTop * cos(theta)); //x����
		y = (float)(rTop * sin(theta)); //������
		z = height/2.0;
		glVertex3f(x, y, z);
	}
	glEnd();

	//����(Bottom)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0, 0.0, -height/2.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)(nSlice - i);
		x = (float)(rBottom * cos(theta)); //x����
		y = (float)(rBottom * sin(theta)); //������
		z = -height/2.0;
		glVertex3f(x, y, z);
	}
	glEnd();

	double s, t0, t1, r0, r1, phi;
	double p[2][3], n[3];

	double rr = rBottom - rTop;
	double nz = rr / sqrt(rr*rr + height*height);
  double nxy = sqrt(1.0 - nz * nz);
	//����
	for(j = 0; j < nStack; j++)
	{
		//j=0�͏��(x=0, y=0, z=height/2)
		//2��t���W
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//��ʂ���̍���
		z0 = height * (1.0 - t0);
		z1 = height * (1.0 - t1);
		//���a
		r0 = rBottom + (rTop - rBottom) * z0 / height;
		r1 = rBottom + (rTop - rBottom) * z1 / height;

		//���_z���W
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s���W
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//���_��xy���W
			p[0][0] = r0 * cos(phi);//x���W
			p[0][1] = r0 * sin(phi);//y���W
			p[1][0] = r1 * cos(phi);//x���W
			p[1][1] = r1 * sin(phi);//y���W
			//�@���x�N�g��
			n[0] = nxy * cos(phi);
			n[1] = nxy * sin(phi);
			n[2] = nz;

			glNormal3dv(n);//�@���x�N�g��
			glVertex3dv(p[0]);//���_���W
			glVertex3dv(p[1]);//���_���W
		}
		glEnd();
	}
}
//----------------------------------------------------------------------
void drawCylinder(float rBottom, float rTop, float height, float thick, int nSlice, int nStack)
{ //���݂̂��钆��~��
	//�������z��
	//�~��(rBottom=rTop))�A�~����A�~��(rTop = 0.0)
	//rBottom:���ꔼ�a, rTop:��ꔼ�a�i�����j
	//nSlice--xy�f�ʕ�����
	//nStack---������������
	//���̂̒��S�͉���Ə��̒���

	int i, j;
	double x, y, z, z0, x1, y1,z1;
	double theta;
	double theta0 = 2.0*M_PI/(double)nSlice;
	//���(Top)
	glBegin(GL_QUAD_STRIP);
	glNormal3f(0.0, 0.0, 1.0);
	//glVertex3f(0.0, 0.0, height/2.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		x = (float)(rTop * cos(theta)); //x����(�����j
		y = (float)(rTop * sin(theta)); //y����(�����j
		x1 = (float)((rTop + thick) * cos(theta)); //x����(�O���j
		y1 = (float)((rTop + thick) * sin(theta)); //y����(�O���j
		z = height/2.0;
		glVertex3f(x, y, z);
		glVertex3f(x1, y1, z);
	}
	glEnd();

	//����(Bottom)
	glBegin(GL_QUAD_STRIP);
	glNormal3f(0.0, 0.0, -1.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)(nSlice - i);
		x = (float)(rBottom * cos(theta)); //x����(�����j
		y = (float)(rBottom * sin(theta)); //������(�����j
		x1 = (float)((rBottom + thick) * cos(theta)); //x����(�O���j
		y1 = (float)((rBottom + thick) * sin(theta)); //y����(�O���j
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
	//����(�O���j
	for(j = 0; j < nStack; j++)
	{
		//j=0�͏��(x=0, y=0, z=height/2)
		//2��t���W
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//��ʂ���̍���
		z0 = height * (1.0 - t0);
		z1 = height * (1.0 - t1);
		//���a
		r0 = rBottom + thick + (rTop - rBottom) * z0 / height;
		r1 = rBottom + thick + (rTop - rBottom) * z1 / height;

		//���_z���W
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s���W
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//���_��xy���W
			p[0][0] = r0 * cos(phi);//x���W
			p[0][1] = r0 * sin(phi);//y���W
			p[1][0] = r1 * cos(phi);//x���W
			p[1][1] = r1 * sin(phi);//y���W
			//�@���x�N�g��
			n[0] = nxy * cos(phi);
			n[1] = nxy * sin(phi);
			n[2] = nz;

			glNormal3dv(n);//�@���x�N�g��
			glVertex3dv(p[0]);//���_���W
			glVertex3dv(p[1]);//���_���W
		}
		glEnd();
	}
	//����(�����j
	for(j = 0; j < nStack; j++)
	{
		//j=0�͏��(x=0, y=0, z=height/2)
		//2��t���W
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//��ʂ���̍���
		z0 = height * t0;
		z1 = height * t1;
		//���a
		r0 = rBottom + (rTop - rBottom) * z0 / height;
		r1 = rBottom + (rTop - rBottom) * z1 / height;

		//���_z���W
		p[0][2] = z0 - height / 2.0;
		p[1][2] = z1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s���W
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//���_��xy���W
			p[0][0] = r0 * cos(phi);//x���W
			p[0][1] = r0 * sin(phi);//y���W
			p[1][0] = r1 * cos(phi);//x���W
			p[1][1] = r1 * sin(phi);//y���W
			//�@���x�N�g��
			n[0] = -nxy * cos(phi);
			n[1] = -nxy * sin(phi);
			n[2] = -nz;

			glNormal3dv(n);//�@���x�N�g��
			glVertex3dv(p[1]);//���_���W
			glVertex3dv(p[0]);//���_���W
		}
		glEnd();
	}
}

//----------------------------------------------------------------------
void drawCylinderY(float rBottom, float rTop, float height, int nSlice, int nStack)
{ //�������Y��
	//�~��(rBottom=rTop))�A�~����A�~��(rTop = 0.0)
	//rBottom:���ꔼ�a, rTop:��ꔼ�a
	//nSlice--ZX�f�ʕ�����
	//nStack---Y����������
	//���̂̒��S�͉���Ə��̒���

	int i, j;
	double Z, X, Y, Y0, Y1;
	double theta;
	double theta0 = 2.0*M_PI/(double)nSlice;
	//���(Top)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, height/2.0, 0.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)i;
		Z = (float)(rTop * cos(theta)); //z����
		X = (float)(rTop * sin(theta)); //x����
		Y = height/2.0;
		glVertex3f(X, Y, Z);
	}
	glEnd();

	//����(Bottom)
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, -height/2.0, 0.0);
	for(i = 0; i <= nSlice; i++) 
	{ 
		theta = theta0 * (double)(nSlice - i);
		Z = (float)(rBottom * cos(theta)); //z����
		X = (float)(rBottom * sin(theta)); //x����
		Y = -height/2.0;
		glVertex3f(X, Y, Z);
	}
	glEnd();

	double s, t0, t1, r0, r1, phi;
	double p[2][3], n[3];

	double rr = rBottom - rTop;
	double nY = rr / sqrt(rr*rr + height*height);
  double nZX = sqrt(1.0 - nY * nY);
	//����
	for(j = 0; j < nStack; j++)
	{
		//j=0�͏��(Z=0, X=0, Y=height/2)
		//2��t���W
		t0 = (double)j / (double)nStack;
		t1 = (double)(j+1) / (double)nStack;
		//��ʂ���̍���
		Y0 = height * (1.0 - t0);
		Y1 = height * (1.0 - t1);
		//���a
		r0 = rBottom + (rTop - rBottom) * Y0 / height;
		r1 = rBottom + (rTop - rBottom) * Y1 / height;

		//���_Y���W
		p[0][1] = Y0 - height / 2.0;
		p[1][1] = Y1 - height / 2.0;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= nSlice; i++)
		{
			//s���W
			s = (double)i / (double)nSlice;
			phi = 2.0 * M_PI * s + M_PI / 6.0;
			//���_��ZX���W
			p[0][2] = r0 * cos(phi);//Z���W
			p[0][0] = r0 * sin(phi);//X���W
			p[1][2] = r1 * cos(phi);//Z���W
			p[1][0] = r1 * sin(phi);//X���W
			//�@���x�N�g��
			n[2] = nZX * cos(phi);
			n[0] = nZX * sin(phi);
			n[1] = nY;

			glNormal3dv(n);//�@���x�N�g��
			glVertex3dv(p[0]);//���_���W
			glVertex3dv(p[1]);//���_���W
		}
		glEnd();
	}
}

//-----------------------------------------------------------------------------------------

void drawTorus(float radius1, float radius2, int nSide, int nRing)
{	
	//radius1:�~�f�ʔ��a
	//radius2:�~�̒��S�����a
	//nSide:�~�f�ʂɂ�����\�ʕ����_��
	//nRing:�~�̕�����
	if(radius1 > radius2) { printf("radius1 < radius2�Ƃ��Ă������� \n "); return;}

	int i, j;
	double rr, zz;
	double phi0, phi1, theta;
	double p[2][3];

	for(i = 0; i < nRing; i++)
	{
		//i=0�͊�{�f��(x=radius2�𒆐S�Ƃ���~, y=0�j
		phi0 = 2.0 * M_PI * (double)i / (double)nRing;
		phi1 = 2.0 * M_PI * (double)(i+1) / (double)nRing;

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= nSide; j++)
		{
			theta = M_PI - 2.0 * M_PI * (double)j / (double)nSide;
			rr = radius2 + radius1 * cos(theta);//z������̋���
			zz = radius1 * sin(theta);
			//���_��xyz���W(j=0�����xy����)
			p[0][0] = rr * cos(phi0);//x���W
			p[0][1] = rr * sin(phi0);//y
			p[0][2] = zz;            //z
			p[1][0] = rr * cos(phi1);//x���W
			p[1][1] = rr * sin(phi1);//y
			p[1][2] = zz;            //z      

			glNormal3d(cos(theta)*cos(phi0),cos(theta)*sin(phi0),sin(theta));
			glVertex3dv(p[0]);//���_���W

			glNormal3d(cos(theta)*cos(phi1),cos(theta)*sin(phi1),sin(theta));
			glVertex3dv(p[1]);//���_���W
		}
		glEnd();
	}
}
//-----------------------------------------------------------------
void drawSuper(float r, int nSlice, int nStack, double eps1, double eps2)
{
	//�㉺�̒��S�����_
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
		                //theta��x-y���ʂ���̕Ίp�ƂȂ��Ă���

		if(theta >= 0.0) //�㔼��
		{
			if(theta == 0.0) z = 0.0;//pow(a,b)��a��a<=0.0�ŃG���[
			else z = pow(sin(fabs(theta)),eps1);//z
		}
		else  //������        
		{
			z = - pow(sin(fabs(theta)), eps1);
		}
		for (i = 0 ;i <= nSlice / 2;i++)
		{
			k1 = nSlice * j + i;// + nSlice / 8.0;//object���猩�č���
			k2 = nSlice * j + nSlice - i;// - nSlice / 8.0;//�E��
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

	//���ʂ̖@������
	for(i = 0;i < nSlice;i++)
	{
		ip = i+1;
		if(ip == nSlice) ip = 0;
		im = i-1;
		if(i == 0) im = nSlice-1;

		//�^��(Top)
		a[i][0] = 0.0f; b[i][0] = 0.0f; c[i][0] = 1.0f;
		//�^���iBottom)
		a[i][nStack] = 0.0f; b[i][nStack] = 0.0f; c[i][nStack] = -1.0f;

		for(j=1;j<nStack;j++)//�ׂ荇��4�̎O�p�`�̖@���x�N�g���𕽋ω�
		{
			np = j*nSlice+i;//���ړ_
			npL = j*nSlice+im;//����
			npR = j*nSlice+ip;//�E��
			npU = np-nSlice;//��
			npD = np+nSlice;//��
			if(j == 1) 
			{
				n1[0]=0.0f; n1[1]=0.0f; n1[2]=1.0f;//Top
				n2[0]=0.0f; n2[1]=0.0f; n2[2]=1.0f;//Top
				calcNormal(pd[np],pd[npL],pd[npD],n3);//�O���猩�č���
				calcNormal(pd[np],pd[npD],pd[npR],n4);//�E��
			}
			if(j == nStack-1)
			{
				calcNormal(pd[np],pd[npU],pd[npL],n1);//�O���猩�č���
				calcNormal(pd[np],pd[npR],pd[npU],n2);//�E��
				n3[0]=0.0f; n3[1]=0.0f; n3[2]=-1.0f;//Bottom
				n4[0]=0.0f; n4[1]=0.0f; n4[2]=-1.0f;//Bottom
			}
			else 
			{
				calcNormal(pd[np],pd[npU],pd[npL],n1);//�O���猩�č���
				calcNormal(pd[np],pd[npR],pd[npU],n2);//�E��
				calcNormal(pd[np],pd[npL],pd[npD],n3);//�O���猩�č���
				calcNormal(pd[np],pd[npD],pd[npR],n4);//�E��
			}
			a[i][j] = (float)((n1[0]+n2[0]+n3[0]+n4[0])/4.0f);//������
			b[i][j] = (float)((n1[1]+n2[1]+n3[1]+n4[1])/4.0f);//��
			c[i][j] = (float)((n1[2]+n2[2]+n3[2]+n4[2])/4.0f);//��
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
	//�㉺�̒��S�����_
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
		//�`��֐�
		if(z < 0.0) fz = (middle-1.0)*z + middle;
		else fz = (1.0-middle)*z + middle;

		for (i = 0 ;i <= nSlice/2 ;i++)
		{
			k1 = nSlice * j + i;//object���g���猩�č���(x > 0)
			k2 = nSlice * j + nSlice - i;//�E��(x < 0)
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

	//�ό`
	if(type == 0)
	{
		//�O���Fz��
		//z����](x>0�Ȃ琳��z����]�Cx<0�Ȃ畉��z����]�j
		for(j = jStart; j < nStack; j++)
		{
			for(i = 0; i <= nSlice/2; i++)
			{
				k1 = nSlice * j + i;//�������猩�č���(x > 0)
				k2 = nSlice * j + nSlice - i;//�E��(x < 0)
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
		//�O���Fz��
		//�㔼���㉺�ix����]�j
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
		//�O���Fz��
		//�㔼�����E�iy����]�j
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
	
	//���ʂ̖@������
	for(i = 0;i < nSlice;i++)
	{
 		ip = i+1;
		if(ip == nSlice) ip = 0;
		im = i-1;
		if(i == 0) im = nSlice-1;

		//�^��(Top)
		a[i][0] = 0.0; b[i][0] = 0.0; c[i][0] = 1.0;
		//�^���iBottom)
		a[i][nStack] = 0.0; b[i][nStack] = 0.0; c[i][nStack] = -1.0;

		for(j=1;j<nStack;j++)//�ׂ荇��4�̎O�p�`�̖@���x�N�g���𕽋ω�
		{
			np = j*nSlice+i;//���ړ_
			npL = j*nSlice+im;//����
			npR = j*nSlice+ip;//�E��
			npU = np-nSlice;//��
			npD = np+nSlice;//��
			if(j == 1) {
				n1[0]=0.0; n1[1]=0.0; n1[2]=1.0;//Top
				n2[0]=0.0; n2[1]=0.0; n2[2]=1.0;//Top
				calcNormal(pd[np],pd[npL],pd[npD],n3);//�O���猩�č���
				calcNormal(pd[np],pd[npD],pd[npR],n4);//�E��
			}
			if(j == nStack-1){
				calcNormal(pd[np],pd[npU],pd[npL],n1);//�O���猩�č���
				calcNormal(pd[np],pd[npR],pd[npU],n2);//�E��
				n3[0]=0.0; n3[1]=0.0; n3[2]=-1.0;//Bottom
				n4[0]=0.0; n4[1]=0.0; n4[2]=-1.0;//Bottom
			}
			else {
				calcNormal(pd[np],pd[npU],pd[npL],n1);//�O���猩�č���
				calcNormal(pd[np],pd[npR],pd[npU],n2);//�E��
				calcNormal(pd[np],pd[npL],pd[npD],n3);//�O���猩�č���
				calcNormal(pd[np],pd[npD],pd[npR],n4);//�E��
			}
			a[i][j] = (float)((n1[0]+n2[0]+n3[0]+n4[0])/4.0f);//������
			b[i][j] = (float)((n1[1]+n2[1]+n3[1]+n4[1])/4.0f);//��
			c[i][j] = (float)((n1[2]+n2[2]+n3[2]+n4[2])/4.0f);//��
		}
	}
	//�\��
	glBegin(GL_TRIANGLES);
	//���
	for(i = 0;i < nSlice;i++)
	{	ip = i+1;
		if(ip == nSlice) ip = 0;

			glNormal3f(a[i][0],b[i][0],c[i][0]);glVertex3fv(pd[i]);
			glNormal3f(a[i][1],b[i][1],c[i][1]);glVertex3fv(pd[nSlice+i]);
			glNormal3f(a[ip][1],b[ip][1],c[ip][1]);glVertex3fv(pd[nSlice+ip]);
	}
	//����
	j = nStack - 1;
	for(i = 0;i < nSlice;i++)
	{	ip = i+1;
		if(ip == nSlice) ip = 0;
			glNormal3f(a[i][j],b[i][j],c[i][j]);glVertex3fv(pd[j*nSlice+i]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);glVertex3fv(pd[(j+1)*nSlice+i]);
			glNormal3f(a[ip][j],b[ip][j],c[ip][j]);glVertex3fv(pd[j*nSlice+ip]);
	}
	glEnd();
	//����(4�p�`�p�b�`�j
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
//�l�p�`�̃��b�V���ix-y����,���S�����_�j
//���������C���������̕����Œ�
void drawElevation(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, float* data)
{
	//�S�̂̕�,����sizeX, sizeY
	//sideFlag = 0:���ʕ\������
	//sideFlag = 1:���ʕ\������

	const int NMAX = 110;
	int i, j;
	float p[NMAX][NMAX][3]; //���_���W
	float a[NMAX][NMAX], b[NMAX][NMAX], c[NMAX][NMAX];//���_�̖@��
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	if(Nx > NMAX) printf("Nx��NMAX�𒴂��Ă��܂�(drawElevation1) \n");
	if(Ny > NMAX) printf("Ny��NMAX�𒴂��Ă��܂�(drawElevation1) \n");

	//�Z���̃T�C�Y
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;

	//�e���_�̍��W
	for(j = 0; j <= Ny; j++){
		for(i = 0; i <= Nx; i++){
			p[i][j][0] = (float)(i - Nx / 2) * pitchX;
			p[i][j][1] = (float)(j - Ny / 2) * pitchY;
			p[i][j][2] = data[j * (Nx+1) + i];
		}
	}
	
	//�@������
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
					calcNormal(p[i][0],p[i][1],p[i-1][0],n1);//����
					calcNormal(p[i][0],p[i+1][0],p[i][1],n2);//�E��
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
					calcNormal(p[i][Ny],p[i-1][Ny],p[i][Ny-1],n1);//����
					calcNormal(p[i][Ny],p[i][Ny-1],p[i+1][Ny],n2);//�E��
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
			}
			else
			{
				if(i == 0) 
				{
					calcNormal(p[0][j],p[1][j],p[0][j+1],n1);//��
					calcNormal(p[0][j],p[0][j-1],p[0][1],n2);//��
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx][j],p[Nx][j+1],p[Nx-1][j],n1);//��
					calcNormal(p[Nx][j],p[Nx-1][j],p[Nx][j-1],n2);//��
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else 
				{//�㉺���E�S�̎O�p�`�̕���
					calcNormal(p[i][j],p[i][j+1],p[i-1][j],n1);//����
					calcNormal(p[i][j],p[i+1][j],p[i][j+1],n2);//�E��
					calcNormal(p[i][j],p[i-1][j],p[i][j-1],n3);//����
					calcNormal(p[i][j],p[i][j-1],p[i+1][j],n4);//�E��
					a[i][j] = (n1[0]+n2[0]+n3[0]+n4[0])/4.0f;
					b[i][j] = (n1[1]+n2[1]+n3[1]+n4[1])/4.0f;
					c[i][j] = (n1[2]+n2[2]+n3[2]+n4[2])/4.0f; }
			}
		}

	//�O�p�`�Ŗʂ��`
	glBegin(GL_TRIANGLES);
	for(j = 0;j < Ny;j++)
		for(i = 0;i < Nx;i++)
		{
			//�����̎O�p�`
			//�e���_�̖@������,���_���W��^����B
			glNormal3f(a[i][j],b[i][j],c[i][j]);//�@������
			glVertex3fv(p[i][j]);//��غ�݂̒��_���W�i�ȉ��������J��Ԃ��j
			glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
			glVertex3fv(p[i+1][j]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glVertex3fv(p[i][j+1]);
			//�E��̎O�p�`
			glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
			glVertex3fv(p[i+1][j]);
			glNormal3f(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glVertex3fv(p[i+1][j+1]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glVertex3fv(p[i][j+1]);
		}
	glEnd();

	if(sideFlag == 1)//���ʕ`��
	{
		glBegin(GL_QUADS);
		//+x�����ii=Nx)
		glNormal3f(1.0, 0.0, 0.0);
		for(j = 0; j < Ny; j++)
		{
			glVertex3f(p[Nx][j][0], p[Nx][j][1], 0.0f);
			glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], 0.0f);
			glVertex3f(p[Nx][j+1][0], p[Nx][j+1][1], p[Nx][j+1][2]);
			glVertex3f(p[Nx][j][0], p[Nx][j][1], p[Nx][j][2]);
		}
		//-x�����ii=0)
		glNormal3f(-1.0, 0.0, 0.0);
		for(j = 0; j < Ny; j++)
		{
			glVertex3f(p[0][j][0], p[0][j][1], 0.0f);
			glVertex3f(p[0][j][0], p[0][j][1], p[0][j][2]);
			glVertex3f(p[0][j+1][0], p[0][j+1][1], p[0][j+1][2]);
			glVertex3f(p[0][j+1][0], p[0][j+1][1], 0.0f);
		}
		//+y�����ij=Ny)
		glNormal3f(0.0, 1.0, 0.0);
		for(i = 0; i < Nx; i++)
		{
			glVertex3f(p[i][Ny][0], p[i][Ny][1], 0.0f);
			glVertex3f(p[i][Ny][0], p[i][Ny][1], p[i][Ny][2]);
			glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], p[i+1][Ny][2]);
			glVertex3f(p[i+1][Ny][0], p[i+1][Ny][1], 0.0f);
		}
		//-y�����ij=0)
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
//�l�p�`�̃��b�V���ix-y����,���S�����_�j
//���������C���������̕����Œ�
void drawElevationC(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, int model, float* data)
{
	//�S�̂̕�,����sizeX, sizeY
	//sideFlag = 0:���ʕ\������
	//sideFlag = 1:���ʕ\������
	//model = 0:Solid model
	//model = 1:Wireframe model

	const int NMAX = 110;
	int i, j;
	float p[NMAX][NMAX][3]; //���_���W
	float a[NMAX][NMAX], b[NMAX][NMAX], c[NMAX][NMAX];//���_�̖@��
	float rr[NMAX][NMAX], gg[NMAX][NMAX], bb[NMAX][NMAX];
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	if(Nx > NMAX) printf("Nx��NMAX�𒴂��Ă��܂�(drawElevation1) \n");
	if(Ny > NMAX) printf("Ny��NMAX�𒴂��Ă��܂�(drawElevation1) \n");

	//�Z���̃T�C�Y
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;

	//�e���_�̍��W,�F
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
	
	//�@������
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
					calcNormal(p[i][0],p[i][1],p[i-1][0],n1);//����
					calcNormal(p[i][0],p[i+1][0],p[i][1],n2);//�E��
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
					calcNormal(p[i][Ny],p[i-1][Ny],p[i][Ny-1],n1);//����
					calcNormal(p[i][Ny],p[i][Ny-1],p[i+1][Ny],n2);//�E��
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
			}
			else
			{
				if(i == 0) 
				{
					calcNormal(p[0][j],p[1][j],p[0][j+1],n1);//��
					calcNormal(p[0][j],p[0][j-1],p[0][1],n2);//��
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else if(i == Nx) 
				{
					calcNormal(p[Nx][j],p[Nx][j+1],p[Nx-1][j],n1);//��
					calcNormal(p[Nx][j],p[Nx-1][j],p[Nx][j-1],n2);//��
					a[i][j] = (n1[0]+n2[0])/2.0f;
					b[i][j] = (n1[1]+n2[1])/2.0f;
					c[i][j] = (n1[2]+n2[2])/2.0f; }
				else 
				{//�㉺���E�S�̎O�p�`�̕���
					calcNormal(p[i][j],p[i][j+1],p[i-1][j],n1);//����
					calcNormal(p[i][j],p[i+1][j],p[i][j+1],n2);//�E��
					calcNormal(p[i][j],p[i-1][j],p[i][j-1],n3);//����
					calcNormal(p[i][j],p[i][j-1],p[i+1][j],n4);//�E��
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
		//�O�p�`�Ŗʂ��`
		glBegin(GL_TRIANGLES);
		for(j = 0;j < Ny;j++)
			for(i = 0;i < Nx;i++)
			{
				//�����̎O�p�`
				//�e���_�̖@������,���_���W��^����B
				glNormal3f(a[i][j],b[i][j],c[i][j]);//�@������
				dif[0] = rr[i][j], dif[1] = gg[i][j], dif[2] = bb[i][j], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i][j]);//��غ�݂̒��_���W�i�ȉ��������J��Ԃ��j
				glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
				dif[0] = rr[i+1][j], dif[1] = gg[i+1][j], dif[2] = bb[i+1][j], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i+1][j]);
				glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
				dif[0] = rr[i][j+1], dif[1] = gg[i][j+1], dif[2] = bb[i][j+1], dif[4] = 1.0; 
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dif);
				glVertex3fv(p[i][j+1]);
				//�E��̎O�p�`
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

		if(sideFlag == 1)//���ʕ`��
		{
			glBegin(GL_QUADS);
			//+x�����ii=Nx)
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
			//-x�����ii=0)
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
			//+y�����ij=Ny)
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
			//-y�����ij=0)
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

	//�O�p�`�Ŗʂ��`
	glBegin(GL_TRIANGLES);
	for(j = 0;j < Ny;j++)
		for(i = 0;i < Nx;i++)
		{
			//�����̎O�p�`
			//�e���_�̖@������,���_���W��^����B
			glNormal3f(a[i][j],b[i][j],c[i][j]);//�@������
			glColor3f(rr[i][j], gg[i][j], bb[i][j]);
			glVertex3fv(p[i][j]);//��غ�݂̒��_���W�i�ȉ��������J��Ԃ��j
			glNormal3f(a[i+1][j],b[i+1][j],c[i+1][j]);
			glColor3f(rr[i+1][j], gg[i+1][j], bb[i+1][j]);
			glVertex3fv(p[i+1][j]);
			glNormal3f(a[i][j+1],b[i][j+1],c[i][j+1]);
			glColor3f(rr[i][j+1], gg[i][j+1], bb[i][j+1]);
			glVertex3fv(p[i][j+1]);
			//�E��̎O�p�`
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

	if(sideFlag == 1)//���ʕ`��
	{
		glBegin(GL_QUADS);
		//+x�����ii=Nx)
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
		//-x�����ii=0)
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
		//+y�����ij=Ny)
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
		//-y�����ij=0)
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
//�l�p�`�̃��b�V���ix-y����,���S�����_�j
//���������C���������̕����Œ�
void drawElevation(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, float* data)
{
	//�S�̂̕�,����sizeX, sizeY
	//sideFlag = 0:���ʕ\������
	//sideFlag = 1:���ʕ\������

	int i, j;
	float p[8][3];//���_���W
	float a[2][3];//���_�̖@��
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	//�Z���̃T�C�Y
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;

	for(j = 0; j < Ny; j++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= Nx; i++)
		{
			//�e���_�̍��W(i, j)
			p[0][0] = (float)(i - Nx / 2) * pitchX;
			p[0][1] = (float)(j - Ny / 2) * pitchY;
			p[0][2] = data[j * (Nx+1) + i];
			//(i, j+1)
			p[1][0] = (float)(i - Nx / 2) * pitchX;
			p[1][1] = (float)(j+1 - Ny / 2) * pitchY;
			p[1][2] = data[(j+1) * (Nx+1) + i];

			//�@������
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

	if(sideFlag == 1)//���ʕ`��
	{
		//+x�����ii=Nx)
		glNormal3f(1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = 0; j <= Ny; j++)
		{
			//�e���_�̍��W(i, j)
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
		//-x�����ii=0)
		glNormal3f(-1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = Ny; j >= 0; j--)
		{
			//�e���_�̍��W(i, j)
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
		//+y�����ij=Ny)
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
		//-y�����ij=0)
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
//�l�p�`�̃��b�V���ix-y����,���S�����_�j
//���������C���������̕����Œ�
void drawElevationC(int Nx, int Ny, float sizeX, float sizeY, int sideFlag, int model, float* data)
{
	//�S�̂̕�,����sizeX, sizeY
	//sideFlag = 0:���ʕ\������
	//sideFlag = 1:���ʕ\������
	//model = 0:Solid model
	//model = 1:Wireframe model

	const int NMAX = 110;
	int i, j, k;
	float p[8][3]; //���_���W
	float a[2][3];//���_�̖@��
	float rr[2], gg[2], bb[2];
	float pitchX, pitchY;
	float n1[3], n2[3], n3[3], n4[3];

	if(Nx > NMAX) printf("Nx��NMAX�𒴂��Ă��܂�(drawElevation1) \n");
	if(Ny > NMAX) printf("Ny��NMAX�𒴂��Ă��܂�(drawElevation1) \n");

	//�Z���̃T�C�Y
	pitchX = sizeX / (float)Nx;
	pitchY = sizeY / (float)Ny;
	static float dif[4];

	for(j = 0; j < Ny; j++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= Nx; i++)
		{
			//�e���_�̍��W(i, j)
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

			//�@������
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

	if(sideFlag == 1)//���ʕ`��
	{
		//+x�����ii=Nx)
		glNormal3f(1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = 0; j <= Ny; j++)
		{
			//�e���_�̍��W(i, j)
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
		//-x�����ii=0)
		glNormal3f(-1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLE_STRIP);
		for(j = Ny; j >= 0; j--)
		{
			//�e���_�̍��W(i, j)
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
		//+y�����ij=Ny)
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
		//-y�����ij=0)
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
//�Q�����i�q��I�u�W�F�N�g
void drawGridSquare(int numRow, int numCol, float pd[][3])
{	//���ʂ���`
	int i, j, np;
	float n1[3], n2[3], n3[3], n4[3];
	float a[101][101], b[101][101], c[101][101];

	//�@������
	for(i = 0;i < numRow;i++) //������
	  for(j = 0;j < numCol;j++)//�E����
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
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n1);//����
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//�E��
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
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//����
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//�E��
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
		}
		else
		{
			if(j == 0) {
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n1);//��
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n2);//��
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else if(j == numCol-1) {
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//��
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n2);//��
				a[i][j] = (n1[0]+n2[0])/2.0f;
				b[i][j] = (n1[1]+n2[1])/2.0f;
				c[i][j] = (n1[2]+n2[2])/2.0f; }
			else {//�㉺���E�S�̎O�p�`�̕���
				calcNormal(pd[np],pd[np-numCol],pd[np-1],n1);//����
				calcNormal(pd[np],pd[np+1],pd[np-numCol],n2);//�E��
				calcNormal(pd[np],pd[np-1],pd[np+numCol],n3);//����
				calcNormal(pd[np],pd[np+numCol],pd[np+1],n4);//�E��
				a[i][j] = (n1[0]+n2[0]+n3[0]+n4[0])/4.0f;
				b[i][j] = (n1[1]+n2[1]+n3[1]+n4[1])/4.0f;
				c[i][j] = (n1[2]+n2[2]+n3[2]+n4[2])/4.0f; }
		}
	}
	//�`��
	for(i = 0;i < numRow-1;i++)
	  for(j = 0;j < numCol-1;j++){
      np = i * numCol + j;
			glBegin(GL_TRIANGLES);
			//�����̎O�p�`
			//�e���_�̖@������,ø�������W,���_���W��^����B
			glNormal3d(a[i][j],b[i][j],c[i][j]);//�@������
			glVertex3fv(pd[np]);//��غ�݂̒��_���W�i�ȉ��������J��Ԃ��j
			glNormal3d(a[i+1][j],b[i+1][j],c[i+1][j]);
			glVertex3fv(pd[np+numCol]);
			glNormal3d(a[i+1][j+1],b[i+1][j+1],c[i+1][j+1]);
			glVertex3fv(pd[np+numCol+1]);
			//�E��̎O�p�`
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
  //Floor�P��������̕�
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
{//�����F������
  int i, j;
  //Floor�P��������̕�
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
{//�����F������
  int i, j;
	float x1, y1, x2, y2;
  //Floor�P��������̕�
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
	//Nm:��~�������_��,Ns:�f�ʉ~�������_��,Np:�s�b�`��
	//radius:��~�������a,ratio:�f�ʔ��a/��~�������a,length:�S��
	//�o�l�̒��Ԃ����_
	int i, ii, j, jj, k, k1, k2;
	double phai, ph1, ph2;  //��~�������_��x���ɑ΂���Ίp
	double theta, th1, th2; //�f�ʉ~�������_��x-z���ʂɑ΂���Ίp
	float p[21][21][101][3];//�e�_�̍��W
	float p1[3], p2[3], p3[3], p4[3];
	float rr[21], zz[21];
	float pitch, dp, hh;
	float r1, r2;

	r1 = radius; //��~�������a
	r2 = ratio * r1;//�f�ʔ��a
	if( Nm > 20 ) Nm = 20;
	if( Ns > 20 ) Ns = 20;
	if( Np > 100 ) Np = 100;
	pitch = length / (float)Np;

	dp = (float)pitch / (float)Nm;

	//��{�f��(x-z)�̍��W
	for(j = 0; j < Ns; j++)
	{	theta = M_PI - 2.0 * M_PI*(double)j/(double)Ns;
		rr[j] = r1 + r2 * (float)cos(theta); //���_����̋���
		zz[j] = r2 * (float)sin(theta);//��
	}

	//���̒f�ʂ̍��W
	hh = 0;
	for(k = 0; k < Np; k++)
		for(i = 0; i < Nm; i++)
		{	phai = 2.0 * M_PI * (double)i/(double)Nm;
			for(j = 0; j < Ns; j++)
			{
				p[i][j][k][0] = rr[j] * (float)cos(phai); //x���W
				p[i][j][k][1] = rr[j] * (float)sin(phai); //y
				p[i][j][k][2] = zz[j] + hh - length / 2.0 ;              //z
			}
			hh += dp;//���S���̍�����dp������
		}

	//�ŏI�[(k=Np-1,i=Nm)
	k = Np - 1; i = Nm;
	for(j = 0; j < Ns; j++){
		phai = 0.0;
		p[i][j][k][0] = rr[j] * (float)cos(phai); //x���W
		p[i][j][k][1] = rr[j] * (float)sin(phai); //y
		p[i][j][k][2] = zz[j] + hh - length / 2.0;            //z
	}

	//���_����`���`��
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
			//�ʂ̒��_   x���W                       y���W                       z���W
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
	//�n�[�ik=0)
	glBegin(GL_POLYGON);
		glNormal3d(0.0,-1.0,0.0);
		for(j = Ns-1; j >= 0; j--) glVertex3fv(p[0][j][0]);
	glEnd();
	//�I�[�ik=Np)
	glBegin(GL_POLYGON);
		glNormal3d(0.0,1.0,0.0);
		for(j = 0; j < Ns; j++) glVertex3fv(p[Nm][j][Np-1]);
	glEnd();

}
//----------------------------------------------------------------------
//�����̊i�q��I�u�W�F�N�g
void drawGridCube(int numRow, int numCol, int numStk, float p1[][3],
        float p2[][3], float p3[][3], float p4[][3], float p5[][3],
        float p6[][3])
{
    //���ׂē����e�N�X�`���[
    //�㑤
    drawGridSquare(numRow, numCol, p1);
    //����
    drawGridSquare(numStk, numCol, p2);
    //�E��
    drawGridSquare(numStk, numRow, p3);
    //����
    drawGridSquare(numStk, numRow, p4);
    //����
    drawGridSquare(numStk, numCol, p5);
    //����
    drawGridSquare(numRow, numCol, p6);
}
//----------------------------------------------------------------------------
//���i�q��I�u�W�F�N�g
void drawGridSphere(int Nxy, int Nz, float pd[][3])
{
	int i,j,ip,im,np,npL,npR,npU,npD;
	float a[11][11],b[11][11],c[11][11];//���_�̖@������
	float n[11][3];//���_�̖@������

    if(Nxy > 11) Nxy = 11;
    if(Nz > 11) Nz = 11;
    //Top
    //Top��numCol�̎O�p�`�̖@���x�N�g���𕽋ω�
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
    for(i = 1;i < Nxy; i++){ //Top���_�͂��ׂē���
        a[i][0] = a[0][0]; b[i][0] = b[0][0]; c[i][0] = c[0][0];
    }

	//���ʂ̖@������
    //�ׂ荇��4�̎O�p�`�̖@���x�N�g���𕽋ω�
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
		for(j = 1;j < Nz-1;j++)
		{
			np =  j * Nxy + i;//���ړ_
			npL = j * Nxy + im;//����
			npR = j * Nxy + ip;//�E��
			npU = np - Nxy;//��
			npD = np + Nxy;//��
            calcNormal(pd[np],pd[npU],pd[npL],n[0]);//�O���猩�č���
            calcNormal(pd[np],pd[npR],pd[npU],n[1]);//�E��
            calcNormal(pd[np],pd[npL],pd[npD],n[2]);//�O���猩�č���
            calcNormal(pd[np],pd[npD],pd[npR],n[3]);//�E��

			a[i][j] = (float)((n[0][0]+n[1][0]+n[2][0]+n[3][0])/4.0f);//������
			b[i][j] = (float)((n[0][1]+n[1][1]+n[2][1]+n[3][1])/4.0f);//��
			c[i][j] = (float)((n[0][2]+n[1][2]+n[2][2]+n[3][2])/4.0f);//��
		}
	}
    //Bottom
    //Bottom��numCol�̎O�p�`�̖@���x�N�g���𕽋ω�
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
    for(i = 1;i < Nxy; i++){ //Top���_�͂��ׂē���
        a[i][Nz-1] = a[0][Nz-1];
        b[i][Nz-1] = b[0][Nz-1];
        c[i][Nz-1] = c[0][Nz-1];
    }

	//�\��
	//����(4�p�`�p�b�`�j
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
//���ʂ����ɉ~�����e
void drawGridCylinder(int Nxy, int Nz, float p[][3])
{
    //p[0]�͏��̒��S�C�Ō��p[]�͉���̒��S
	int i,j,ip,im,np,npL,npR,npU,npD;
	float a[11][11],b[11][11],c[11][11];//���_�̖@������

	float n[11][3];//���_�̖@������

    if(Nxy > 10) Nxy = 10;
    if(Nz > 10) Nz = 10;

    //�S���_��
    int num = Nxy * Nz + 2;

    //Top
    //��ꒆ�S��Top��numCol�̎O�p�`�̖@���x�N�g���𕽋ω�
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
    //���̎��͂̒��_�ׂ͗荇��2�̎O�p�`�̖@���x�N�g���𕽋ω�
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
    //Top�̕\��
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
    //���ꒆ�S��Bottom��numCol�̎O�p�`�̖@���x�N�g���𕽋ω�
    a[0][0] = 0.0; b[0][0] = 0.0; c[0][0] = 0.0;
    //����̎��͂̒��_�ׂ͗荇��2�̎O�p�`�̖@���x�N�g���𕽋ω�
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
    //Bottom�̕\��
    for(i = 1; i <= Nxy; i++){
        ip = i + 1;
        if(ip == Nxy + 1) ip = 1;
		glBegin(GL_TRIANGLES);
			glNormal3f(a[0][0],b[0][0],c[0][0]);glVertex3fv(p[num-1]);
			glNormal3f(a[i][0],b[i][0],c[i][0]);glVertex3fv(p[num-1-i]);
			glNormal3f(a[ip][0],b[ip][0],c[ip][0]); glVertex3fv(p[num-1-ip]);
		glEnd();
	}

	//���ʂ̖@������
    //���̎��͒��_�̑��ʂɑ΂���@��������
    //�ׂ荇��2�̎O�p�`�̖@���x�N�g���𕽋ω�
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
        //j = 0;
        np = i+1;//���ړ_
        npL = im+1;//����
        npR = ip+1;//�E��
        npD = np + Nxy;//��
        calcNormal(p[np],p[npL],p[npD],n[0]);//�O���猩�č���
        calcNormal(p[np],p[npD],p[npR],n[1]);//�E��

        a[i][0] = (float)((n[0][0]+n[1][0])/2.0f);//������
        b[i][0] = (float)((n[0][1]+n[1][1])/2.0f);//��
        c[i][0] = (float)((n[0][2]+n[1][2])/2.0f);//��
	}

    //�����悤�ɉ���̒��_�ɑ΂���
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
        j = Nz - 1;
        np = j*Nxy+i+1;//���ړ_
        npL = j*Nxy+im+1;//����
        npR = j*Nxy+ip+1;//�E��
        npU = np - Nxy;//��
        calcNormal(p[np],p[npU],p[npL],n[0]);//�O���猩�č���
        calcNormal(p[np],p[npR],p[npU],n[1]);//�E��

        a[i][j] = (float)((n[0][0]+n[1][0])/2.0f);//������
        b[i][j] = (float)((n[0][1]+n[1][1])/2.0f);//��
        c[i][j] = (float)((n[0][2]+n[1][2])/2.0f);//��
	}

    //���̑��ʂ̒��_�ɑ΂��Ăׂ͗荇��4�̎O�p�`�̖@���x�N�g���𕽋ω�
	for(i = 0;i < Nxy;i++){
 		ip = i+1;
		if(ip == Nxy) ip = 0;
		im = i-1;
		if(i == 0) im = Nxy-1;
		for(j = 1;j < Nz-1;j++)
		{
			np =  j * Nxy + i + 1;//���ړ_
			npL = j * Nxy + im + 1;//����
			npR = j * Nxy + ip + 1;//�E��
			npU = np - Nxy;//��
			npD = np + Nxy;//��
            calcNormal(p[np],p[npU],p[npL],n[0]);//�O���猩�č���
            calcNormal(p[np],p[npR],p[npU],n[1]);//�E��
            calcNormal(p[np],p[npL],p[npD],n[2]);//�O���猩�č���
            calcNormal(p[np],p[npD],p[npR],n[3]);//�E��

			a[i][j] = (float)((n[0][0]+n[1][0]+n[2][0]+n[3][0])/4.0f);//������
			b[i][j] = (float)((n[0][1]+n[1][1]+n[2][1]+n[3][1])/4.0f);//��
			c[i][j] = (float)((n[0][2]+n[1][2]+n[2][2]+n[3][2])/4.0f);//��
		}
	}

	//����(4�p�`�p�b�`�j
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
