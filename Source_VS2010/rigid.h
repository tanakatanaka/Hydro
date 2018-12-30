
//rigid.h
#include "rigid_parameters.h"

#define RIGID_NUM_MAX 20
#define MAX_REPRE 30//���ʑ̂̑�\�_�� 
#define MAX_VERTEX 100//���ʑ̂̒��_�� 
#define NON_COLLISION -999;

//int numRigid0 = 1;//���̐�
int numRigid = 1; 

//-------------------------------------------------------------------------
class CRigid{
public:
	//���ޕϐ�
	Kind kind;     //CRigid�̎��
	Color color;   //�F
	Vector3D vSize; //�T�C�Y
	Vector3D vPos;  //�ʒu
	Vector3D vVel;  //���x
	Vector3D vEuler;//�I�C���[�p

	Vector3D vP[MAX_VERTEX], vP0[MAX_VERTEX];//���ȊO�̒��_���W
	Vector3D vNormalFacet[6];//�����̖̂ʂ̖@���x�N�g��
	int nSlice;//�ȉ~��,�~��,�ȉ~���̌o�x����������
	int nStack;//�ȉ~��,�~��,�ȉ~���̍�������������
	int numVertex;//���ȊO�̑��ʑ̂̒��_��
	int numEdge;

	bool flagFixed;
	bool flagShadow;
  
	//���ފ֐�
	CRigid();  //�R���X�g���N�^
	~CRigid(); //�f�X�g���N�^
	void draw();

private:
	void setVertexOfCube();
	void setVertexOfSphere();
	void setVertexOfCylinder();
};
CRigid rigid[RIGID_NUM_MAX];

//--------------------------------------------------
CRigid::CRigid()
{
	kind = SPHERE;
	color = WHITE;
	vSize = Vector3D(1.0, 1.0, 1.0);
	vPos = Vector3D();
	vVel = Vector3D();
	vEuler = Vector3D();
	//�~��,�p�̂Ƃꂽ�~��(column),�p�̂Ƃꂽ������(box)
	nSlice = 16;//�����͏Փˌv�Z�ɕK�v
	nStack = 6;
	flagShadow = false;
	
}

//--------------------------------------------------
CRigid::~CRigid()
{
}
//---------------------------------------------------------
void CRigid::draw()
{
	//���_�̃}�e���A��
	static float specular[] = { 0.8, 0.8, 0.8, 1.0};
	//�e�̃}�e���A��
	static float diffuseS[] =  {0.2, 0.2, 0.2, 0.3};//�e�̊g�U��

	if(flagShadow)
	{
		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, diffuseS);
	}
	else
	{
		if(color == RED) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, red);
		if(color == GREEN) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, green);
		if(color == BLUE) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, blue);
		if(color == YELLOW) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, yellow);
		if(color == CYAN) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, cyan);
		if(color == MAGENTA) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, magenta);
		if(color == WHITE) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, white);
		if(color == GRAY) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, gray);
		if(color == DARK_RED) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, dark_red);
		if(color == DARK_GREEN) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, dark_green);
		if(color == DARK_BLUE) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, dark_blue);
		if(color == DARK_YELLOW) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, dark_yellow);
		if(color == DARK_CYAN) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, dark_cyan);
		if(color == DARK_MAGENTA) glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, dark_magenta);

		glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
		glMaterialf(GL_FRONT,GL_SHININESS, 100.0);
	}
	
	glPushMatrix();

	//���݈ʒu
	glTranslatef(vPos.x, vPos.y, vPos.z);//���s�ړ�
	glRotatef(vEuler.z, 0.0, 0.0, 1.0);//��]�p��deg
	glRotatef(vEuler.y, 0.0, 1.0, 0.0);//��]�p��deg
	glRotatef(vEuler.x, 1.0, 0.0, 0.0);//��]�p��deg

	//�X�P�[�����O
	glScalef(vSize.x, vSize.y, vSize.z);

	//��޼ު�Ă̕`��
	if(kind == SPHERE) glCallList(sphere);
	else if(kind == CUBE) glCallList(cube);
	else if(kind == CYLINDER) glCallList(cylinder);
	else if(kind == BOX) glCallList(box);
	else if(kind == COLUMN) glCallList(column);
	else if(kind == PLATE) glCallList(plate);
	glPopMatrix();
}

//-----------------------------------------------------------------------------
void CRigid::setVertexOfCube()
{
	vP0[0].x = vSize.x / 2.0; vP0[0].y = vSize.y / 2.0; vP0[0].z = vSize.z / 2.0;
	vP0[1].x =-vSize.x / 2.0; vP0[1].y = vSize.y / 2.0; vP0[1].z = vSize.z / 2.0;
	vP0[2].x =-vSize.x / 2.0; vP0[2].y =-vSize.y / 2.0; vP0[2].z = vSize.z / 2.0;
	vP0[3].x = vSize.x / 2.0; vP0[3].y =-vSize.y / 2.0; vP0[3].z = vSize.z / 2.0;
	vP0[4].x = vSize.x / 2.0; vP0[4].y = vSize.y / 2.0; vP0[4].z =-vSize.z / 2.0;
	vP0[5].x =-vSize.x / 2.0; vP0[5].y = vSize.y / 2.0; vP0[5].z =-vSize.z / 2.0;
	vP0[6].x =-vSize.x / 2.0; vP0[6].y =-vSize.y / 2.0; vP0[6].z =-vSize.z / 2.0;
	vP0[7].x = vSize.x / 2.0; vP0[7].y =-vSize.y / 2.0; vP0[7].z =-vSize.z / 2.0;
	numVertex = 8;
	numEdge = 12;
}
//-----------------------------------------------------------------------------
void CRigid::setVertexOfCylinder()
{
	int i;
	double phi, phi0;

	numVertex = 2 * nSlice + 2;
	if(numVertex > MAX_VERTEX) {
		printf("nSlice�����������Ă������� \n");
		return;
	}

	phi0 = 2.0 * M_PI / (double)nSlice;
	for(i = 0;i < nSlice;i++)
	{   
		phi = phi0*(double)i;
		vP0[i].x = 0.5 * cos(phi) * vSize.x; //����x����
		vP0[i].y = 0.5 * sin(phi) * vSize.y; //������
		vP0[i].z = 0.5 * vSize.z;            //������(����)
		vP0[i+nSlice].x = vP0[i].x;          //�����x����
		vP0[i+nSlice].y = vP0[i].y;          //������
		vP0[i+nSlice].z = - 0.5 * vSize.z;   //������
	}
	//���̒��S
	vP0[2*nSlice].x = 0.0;
	vP0[2*nSlice].y = 0.0;
	vP0[2*nSlice].z = 0.5 * vSize.z;
	//����̒��S
	vP0[2*nSlice+1].x = 0.0;
	vP0[2*nSlice+1].y = 0.0;
	vP0[2*nSlice+1].z = -0.5 * vSize.z;
}
